#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <mutex>
#include <string_view>
#include <vector>
#include <istream>

// For mg_connection
#include "BellTask.h"
#include "WrappedSemaphore.h"
#include "civetweb.h"

#include "EuphLogger.h"

/**
 * @brief StorageAccessor is a class that provides access to the filesystem.
 *
 * The main reason for abstracting this into a separate interface, is to make sure that ESP32 calls all FS operation from one task with stack in the internal ram.
 * This is done to prevent PSRAM memory corruption.
 */
namespace euph {
class StorageAccessor : public bell::Task {
 public:
  // Indicates the type of the operation
  enum class OperationType { READ, WRITE };

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
    struct mg_connection* dataCon;
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
   * @brief Reads a file from the filesystem and returns it as a vector of bytes
   * 
   * @param path Path to the file
   * @return std::vector<uint8_t> 
   */
  std::vector<uint8_t> readFileBinary(std::string_view path);

  /**
   * @brief Reads a file from the filesystem and writes it to a socket. Used in HTTP Server.
   * 
   * @param path Path to the file
   * @param conn Pointer to the socket
   */
  void readFileToSocket(std::string_view path, struct mg_connection* conn);

 private:
  const size_t HTTP_CHUNK_SIZE = 1024;
  
  // Current operation
  Operation currentOperation;

  // Semaphores used to synchronize the task
  std::unique_ptr<bell::WrappedSemaphore> requestSemaphore;
  std::unique_ptr<bell::WrappedSemaphore> responseSemaphore;

  // Mutex used to synchronize the task
  std::mutex operationLock;

  void runTask() override;
};
}  // namespace euph