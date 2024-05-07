#include "UpdateFilesystemServiceJob.h"
#include <fmt/core.h>
#include <filesystem>
#include <fstream>
#include "BellTar.h"
#include "ServiceTask.h"

using namespace euph;

UpdateFilesystemServiceJob::UpdateFilesystemServiceJob(std::string archivePath)
    : archivePath(archivePath) {}

std::string UpdateFilesystemServiceJob::jobTypeName() {
  return "UpdateFilesystemServiceJob";
}

void UpdateFilesystemServiceJob::run(std::shared_ptr<euph::Context> ctx) {
  // First determine the size of the file
  std::filesystem::path path(archivePath);
  if (!std::filesystem::exists(path)) {
    throw ServiceJobFailedException(
        fmt::format("Archive at '{}' does not exist.", archivePath));
  }

  size_t fileSize = std::filesystem::file_size(path);
  size_t progressReportStep = fileSize / 100;  // Report progress every 1%

  // Open the file
  std::ifstream fileStream(archivePath, std::ios::in | std::ios::binary);

  if (!fileStream.is_open()) {
    throw ServiceJobFailedException(
        fmt::format("Failed to open archive at '{}'.", archivePath));
  }
  bell::BellTar::reader tarArchive(fileStream);
  // Remove directory if it exists
  tarArchive.extract_all_files(ctx->rootPath, [&](size_t bytesRead) {
    this->reportProgress(ctx, (float)bytesRead / (float)fileSize);
  });

  // We are done
}
