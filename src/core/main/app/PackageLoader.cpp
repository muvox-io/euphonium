#include "PackageLoader.h"

using namespace euph;

PackageLoader::PackageLoader(std::shared_ptr<euph::Context> ctx) {
  this->ctx = ctx;
}

void PackageLoader::loadValidPackages() {
  this->packages = std::vector<PackageInfo>();
  EUPH_LOG(info, TAG, "Loading packages");
  std::string path = fmt::format("{}/pkgs", ctx->rootPath);

  // iterate over all the files in the pkgs directory
  for (auto file : ctx->storage->listFiles(path)) {

    std::string manifestPath = fmt::format("{}/{}/manifest.json", path, file);

    try {
      std::string manifestContent = ctx->storage->readFile(manifestPath);

      nlohmann::json manifest = nlohmann::json::parse(manifestContent);

      // ensure that the manifest has all the required fields
      if (!manifest.contains("name") || !manifest.contains("version") ||
          !manifest.contains("description") || !manifest.contains("author") ||
          !manifest.contains("init_hook")) {
        EUPH_LOG(info, TAG, "Invalid manifest for package: %s", file.c_str());
        continue;
      }

      PackageInfo info;
      info.name = manifest["name"];
      info.version = manifest["version"];
      info.description = manifest["description"];
      info.author = manifest["author"];
      info.initHook = manifest["init_hook"];

      this->packages.push_back(info);
      EUPH_LOG(info, TAG, "Registered package: %s", info.name.c_str());
    } catch (...) {
      continue;
    }
  }
}

void PackageLoader::loadWithHook(const std::string& hook) {
  EUPH_LOG(info, TAG, "Loading packages with hook: %s", hook.c_str());
  for (auto pkg : this->packages) {
    if (pkg.initHook == hook) {
      EUPH_LOG(info, TAG, "Loading package: %s", pkg.name.c_str());
      std::string initPath =
          fmt::format("{}/pkgs/{}/init.be", ctx->rootPath, pkg.name);
      std::string initCode = ctx->storage->readFile(initPath);

      ctx->vm->execute_string(initCode, pkg.name);
    }
  }
}
