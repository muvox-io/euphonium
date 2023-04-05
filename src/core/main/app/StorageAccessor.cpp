#include "StorageAccessor.h"
#include <iostream>
#include <stdexcept>
#include "BellTar.h"

using namespace euph;

StorageAccessor::StorageAccessor()
    : bell::Task("storage_accessor", 12 * 1024, 0, 0, false) {
  this->requestSemaphore = std::make_unique<bell::WrappedSemaphore>(100);
  this->responseSemaphore = std::make_unique<bell::WrappedSemaphore>(100);

  this->currentOperation = Operation{
      .type = OperationType::READ,
      .format = OperationFormat::TEXT,
      .status = OperationStatus::SUCCESS,
      .path = nullptr,
      .dataBinary = {},
      .dataText = "",
      .dataCon = nullptr,
  };

  // Start the task
  this->startTask();
}

StorageAccessor::~StorageAccessor() {}

std::string StorageAccessor::readFile(std::string_view path) {
  std::string result;
  bool success = false;

  this->executeFromTask([&result, &path, &success]() {
    std::ifstream file(path.data(), std::ios::binary);

    if (!file.is_open()) {
      // Cannot open file
      return;
    }

    // read file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    result.resize(fileSize);

    // Read the file into string
    file.read(&result[0], fileSize);
    success = true;
  });

  if (success) {
    return result;
  } else {
    throw std::runtime_error("Failed to read file");
  }
}

void StorageAccessor::extractTar(std::string_view path, std::string_view dest) {
  this->currentOperation = Operation{
      .type = OperationType::EXTRACT,
      .format = OperationFormat::BINARY,
      .status = OperationStatus::PENDING,
      .path = (char*)path.data(),
      .dataBinary = {},
      .dataText = (char*)dest.data(),
      .dataCon = nullptr,
  };

  this->requestSemaphore->give();
  this->responseSemaphore->wait();

  if (this->currentOperation.status != OperationStatus::SUCCESS) {
    throw std::runtime_error("Failed to extract tar");
  }
}

void StorageAccessor::writeFile(std::string_view path, std::string_view body) {
  this->currentOperation = Operation{
      .type = OperationType::WRITE,
      .format = OperationFormat::TEXT,
      .status = OperationStatus::PENDING,
      .path = (char*)path.data(),
      .dataBinary = std::vector<uint8_t>(body.begin(), body.end()),
      .writeAppend = false,
  };

  this->requestSemaphore->give();
  this->responseSemaphore->wait();

  if (this->currentOperation.status != OperationStatus::SUCCESS) {
    throw std::runtime_error("Failed to write file at " + std::string(path));
  }
}

void StorageAccessor::writeFileBytes(std::string_view path,
                                     std::vector<uint8_t> bytes, bool append) {
  this->currentOperation = Operation{
      .type = OperationType::WRITE,
      .format = OperationFormat::BINARY,
      .status = OperationStatus::PENDING,
      .path = (char*)path.data(),
      .dataBinary = bytes,
      .writeAppend = append,
  };

  this->requestSemaphore->give();
  this->responseSemaphore->wait();

  if (this->currentOperation.status != OperationStatus::SUCCESS) {
    throw std::runtime_error("Failed to write file at " + std::string(path));
  }
}

std::vector<uint8_t> StorageAccessor::readFileBinary(std::string_view path) {
  std::vector<uint8_t> result;
  bool success = false;

  this->executeFromTask([&result, &path, &success]() {
    std::ifstream file(path.data(), std::ios::binary);

    if (!file.is_open()) {
      // Cannot open file
      return;
    }

    // read file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    result.resize(fileSize);

    // Read the file into string
    file.read((char*)result.data(), fileSize);
    success = true;
  });

  if (success) {
    return result;
  } else {
    throw std::runtime_error("Failed to read file");
  }
}

std::vector<std::string> StorageAccessor::listFiles(std::string_view path) {

  std::vector<std::string> result;
  bool success = false;

  this->executeFromTask([&result, &path, &success]() {
    std::ofstream file(path.data(), std::ios::binary);
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(path.data())) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir(dir)) != NULL) {
        result.push_back(ent->d_name);
      }
      closedir(dir);

      success = true;
    }
  });

  if (success) {
    return result;
  } else {
    throw std::runtime_error("Failed to list directories");
  }
}

void StorageAccessor::readFileToSocket(std::string_view path,
                                       struct mg_connection* conn) {
  this->currentOperation = Operation{
      .type = OperationType::READ,
      .format = OperationFormat::HTTP,
      .path = (char*)path.data(),
      .dataCon = conn,
  };

  this->requestSemaphore->give();
  this->responseSemaphore->wait();

  if (this->currentOperation.status == OperationStatus::FAILURE) {
    throw std::runtime_error("Failed to read file");
  }
}

void StorageAccessor::deleteFile(std::string_view path) {
  this->currentOperation = Operation{
      .type = OperationType::DELETE,
      .path = (char*)path.data(),
  };

  this->requestSemaphore->give();
  this->responseSemaphore->wait();

  if (this->currentOperation.status == OperationStatus::FAILURE) {
    throw std::runtime_error("Failed to delete file");
  }
}

bool StorageAccessor::strEndsWith(std::string const& fullString,
                                  std::string const& ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(),
                                    ending.length(), ending));
  } else {
    return false;
  }
}

void StorageAccessor::executeFromTask(std::function<void()> func) {
  std::scoped_lock lock(this->operationMutex);
  this->currentOperation = Operation{
      .type = OperationType::FUNC,
      .workFunc = func,
  };

  this->requestSemaphore->give();
  this->responseSemaphore->wait();
}

void StorageAccessor::runTask() {
  size_t fileSize = 0;
  size_t mgBytesLeft = 0;
  auto mgBuffer = std::vector<uint8_t>(HTTP_CHUNK_SIZE);

  while (true) {
    this->requestSemaphore->wait();

    this->currentOperation.status = OperationStatus::PENDING;

    if (this->currentOperation.type == OperationType::READ) {
      std::string filePath = this->currentOperation.path;
      std::ifstream file(filePath, std::ios::binary);

      // read file size
      file.seekg(0, std::ios::end);
      fileSize = file.tellg();
      file.seekg(0, std::ios::beg);

      // Make sure to try a gzipped version if file does not exist
      if (!file.is_open()) {
        filePath = filePath + ".gz";
        file = std::ifstream(filePath, std::ios::binary);
        EUPH_LOG(info, TASK, "File %s not found, trying to use gz alternative",
                 this->currentOperation.path);

        // read file size
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
      }

      // check if file exists
      if (file.is_open()) {
        switch (this->currentOperation.format) {
          // Handle reads to string
          case OperationFormat::TEXT: {
            this->currentOperation.dataText = std::string(fileSize, ' ');
            file.read(&currentOperation.dataText[0], fileSize);
            this->currentOperation.status = OperationStatus::SUCCESS;
            break;
          }

          // Handle reads to binary
          case OperationFormat::BINARY: {
            this->currentOperation.dataBinary = std::vector<uint8_t>(fileSize);
            file.read((char*)&currentOperation.dataBinary[0], fileSize);
            this->currentOperation.status = OperationStatus::SUCCESS;
            break;
          }

          // Handle reads to socket
          case OperationFormat::HTTP: {
            this->currentOperation.status = OperationStatus::SUCCESS;
            mgBytesLeft = fileSize;

            std::string contentType = "text/plain";
            std::string extraHeaders = "";

            // Handle gzipped files
            if (strEndsWith(filePath, ".gz")) {
              extraHeaders = "Content-Encoding: gzip\r\n";
              filePath = filePath.substr(0, filePath.size() - 3);
            }

            // Deduce content-type
            if (strEndsWith(filePath, ".html")) {
              contentType = "text/html";
            } else if (strEndsWith(filePath, ".js")) {
              contentType = "application/javascript";
            } else if (strEndsWith(filePath, ".css")) {
              contentType = "text/css";
            }

            mg_printf(this->currentOperation.dataCon,
                      "HTTP/1.1 %d OK\r\nContent-Type: "
                      "%s\r\n%sContent-Length: %d\r\nConnection: close\r\n\r\n",
                      200, contentType.c_str(), extraHeaders.c_str(),
                      (int)fileSize);

            // Write the requested file to the socket, chunk by chunk
            while (mgBytesLeft > 0) {
              size_t toRead =
                  mgBytesLeft > HTTP_CHUNK_SIZE ? HTTP_CHUNK_SIZE : mgBytesLeft;

              // read file to temporary buffer
              file.read((char*)mgBuffer.data(), toRead);

              size_t toReadBytes = file.gcount();

              // write to socket
              size_t writtenBytes = mg_write(this->currentOperation.dataCon,
                                             mgBuffer.data(), toReadBytes);

              if (writtenBytes < 0) {
                // Error writing to socket
                mgBytesLeft = 0;
                this->currentOperation.status = OperationStatus::FAILURE;
              } else {
                // Update bytes left
                mgBytesLeft -= writtenBytes;
              }
            }
            break;
          }
        }
      } else {
        // File does not exist
        this->currentOperation.status = OperationStatus::FAILURE;
      }
    }

    if (this->currentOperation.type == OperationType::FUNC) {
      this->currentOperation.workFunc();
      this->currentOperation.status = OperationStatus::SUCCESS;
    }

    if (this->currentOperation.type == OperationType::EXTRACT) {
      std::string filePath = this->currentOperation.path;
      std::ifstream file(filePath, std::ios::binary);

      if (!file.is_open()) {
        // Archive does not exist
        this->currentOperation.status = OperationStatus::FAILURE;
      } else {
        bell::BellTar::reader tarReader(file);
        tarReader.extract_all_files(this->currentOperation.dataText);

        this->currentOperation.status = OperationStatus::SUCCESS;
      }
    }

    if (this->currentOperation.type == OperationType::WRITE) {
      std::ofstream file(this->currentOperation.path,
                         currentOperation.writeAppend
                             ? (std::ios::binary | std::ios::app)
                             : (std::ios::binary));

      // if file doesnt exist, create it
      if (!file.is_open()) {
        file.open(this->currentOperation.path, std::ios::out);
      }

      // check if file exists
      if (file.is_open()) {
        file.write((char*)this->currentOperation.dataBinary.data(),
                   this->currentOperation.dataBinary.size());

        this->currentOperation.status = OperationStatus::SUCCESS;
      } else {
        // File does not exist
        this->currentOperation.status = OperationStatus::FAILURE;
      }
    }

    if (this->currentOperation.type == OperationType::LIST_DIRS) {
      this->currentOperation.dataPaths = std::vector<std::string>();

      std::ofstream file(this->currentOperation.path, std::ios::binary);
      DIR* dir;
      struct dirent* ent;
      if ((dir = opendir(this->currentOperation.path)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
          this->currentOperation.dataPaths.push_back(ent->d_name);
        }
        closedir(dir);

        this->currentOperation.status = OperationStatus::SUCCESS;
      } else {

        this->currentOperation.status = OperationStatus::FAILURE;
      }
    }

    if(this->currentOperation.type == OperationType::DELETE) {
      if (std::remove(this->currentOperation.path) != 0) {
        this->currentOperation.status = OperationStatus::FAILURE;
      } else {
        this->currentOperation.status = OperationStatus::SUCCESS;
      }
    }

    this->responseSemaphore->give();
  }
}
