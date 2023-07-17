#pragma once

#include <dirent.h>
#include <fstream>
#include <istream>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

// For mg_connection
#include "BellTask.h"
#include "WrappedSemaphore.h"
#include "civetweb.h"

#include "EuphLogger.h"

/**
 * @brief StorageAccessor is a class that provides access to the filesystem.
 *
 * The main reason for abstracting this into a separate interface, 
 * is to make sure that ESP32 calls all FS operation from one task with stack in the internal ram.
 * This is done to prevent PSRAM memory corruption.
 */
namespace euph {
class StorageAccessor : public bell::Task {
 public:
  // Indicates the type of the operation
  enum class OperationType { READ, WRITE, LIST_DIRS, EXTRACT, FUNC, DELETE };

  // Indicates the format of the data
  enum class OperationFormat {
    BINARY,
    TEXT,
    HTTP
  };  // HTTP is being used for direct writes to a socket

  // Indicates the status of the operation
  enum class OperationStatus { PENDING, SUCCESS, FAILURE };

  struct Operation {
    OperationType type;
    OperationFormat format;
    OperationStatus status;

    char* path;

    std::vector<uint8_t> dataBinary;
    std::string dataText;
    std::vector<std::string> dataPaths;
    struct mg_connection* dataCon;
    std::function<void()> workFunc;
    bool writeAppend;
  };

  StorageAccessor();
  ~StorageAccessor();

  /**
   * @brief Reads a file from the filesystem and returns it as a string
   * 
   * @param path Path to the file
   * @return std::string 
   */
  std::string readFile(std::string_view path);

  /**
   * @brief Reads a file from the filesystem and returns it as a string
   * 
   * @param path Path to the file
   * @param body String to write to the file
   */
  void writeFile(std::string_view path, std::string_view body);

  /**
   * @brief Writes a vector of bytes to a file
   * 
   * @param path Path to the file
   * @param body Vector of bytes to write to the file
   * @param append If true, the data will be appended to the file. Otherwise, the file will be overwritten.
   */
  void writeFileBytes(std::string_view path, std::vector<uint8_t> body,
                      bool append = false);

  /**
   * @brief Extracts a tar archive from @param path to @param dest
   * 
   * @param path Path to the archive
   * @param dest Path to the destination directory
   */
  void extractTar(std::string_view path, std::string_view dest);

  /**
   * @brief Reads a file from the filesystem and returns it as a vector of bytes
   * 
   * @param path Path to the file
   * @return std::vector<uint8_t> 
   */
  std::vector<uint8_t> readFileBinary(std::string_view path);

  /**
   * @brief Executes a function from the context of this task, allowing it to access the filesystem.
   * 
   * @param func Function to execute
   */
  void executeFromTask(std::function<void()> func);

  /**
   * @brief Reads a file from the filesystem and writes it to a socket. Used in HTTP Server.
   * 
   * @param path Path to the file
   * @param conn Pointer to the socket
   */
  void readFileToSocket(std::string_view path, struct mg_connection* conn);

  /**
   * @brief Returns a list of files in a directory
   *
   * While StorageAccessor is not explicitly meant to be used for FS abstraction,
   * it is still useful to have a function that returns a list of files in a directory.
   * 
   * @param path Path to the directory
   * @return std::vector<std::string> list of files
   */
  std::vector<std::string> listFiles(std::string_view path);


  void deleteFile(std::string_view path);

  /**
   * @brief Utility function to check if a string ends with another string
   * 
   * @param fullString 
   * @param ending 
   * @return true 
   * @return false 
   */
  bool strEndsWith(std::string const& fullString, std::string const& ending);
 private:
  const size_t HTTP_CHUNK_SIZE = 1024;

  // Current operation
  Operation currentOperation;


  // Semaphores used to synchronize the task
  std::unique_ptr<bell::WrappedSemaphore> requestSemaphore;
  std::unique_ptr<bell::WrappedSemaphore> responseSemaphore;
  std::mutex operationMutex;

  void runTask() override;
};
}  // namespace euph
