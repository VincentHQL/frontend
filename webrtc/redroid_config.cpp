#include "redroid_config.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <string>
#include <time.h>

#include <android-base/strings.h>
#include <android-base/logging.h>
#include <json/json.h>

#include "utils/files.h"
namespace redroid {

/*static*/ 
RedroidConfig* RedroidConfig::BuildConfigImpl(const std::string& path) {
  auto ret = new RedroidConfig();
  if (ret) {
    auto loaded = ret->LoadFromFile(path.c_str());
    if (!loaded) {
      delete ret;
      return nullptr;
    }
  }
  return ret;
}

/*static*/ 
std::unique_ptr<const RedroidConfig>
RedroidConfig::GetFromFile(const std::string& path) {
  return std::unique_ptr<const RedroidConfig>(BuildConfigImpl(path));
}

/*static*/ 
const RedroidConfig* RedroidConfig::Get() {
  auto config_file_path = "/data/stream/";
  static std::shared_ptr<RedroidConfig> config(BuildConfigImpl(config_file_path));
  return config.get();
}


RedroidConfig::RedroidConfig() : dictionary_(new Json::Value()) {}

RedroidConfig::~RedroidConfig() = default;

RedroidConfig::RedroidConfig(RedroidConfig&&) = default;

RedroidConfig& RedroidConfig::operator=(RedroidConfig&&) = default;

bool RedroidConfig::LoadFromFile(const char* file) {
  auto real_file_path = AbsolutePath(file);
  if (real_file_path.empty()) {
    LOG(ERROR) << "Could not get real path for file " << file;
    return false;
  }
  Json::CharReaderBuilder builder;
  std::ifstream ifs(real_file_path);
  std::string errorMessage;
  if (!Json::parseFromStream(builder, ifs, dictionary_.get(), &errorMessage)) {
    LOG(ERROR) << "Could not read config file " << file << ": " << errorMessage;
    return false;
  }
  return true;
}



}  // namespace redroid
