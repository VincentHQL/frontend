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

namespace redroid {
namespace {

}  // namespace

ConfigFragment::~ConfigFragment() = default;

static constexpr char kFragments[] = "fragments";
bool RedroidConfig::LoadFragment(ConfigFragment& fragment) const {
  if (!dictionary_->isMember(kFragments)) {
    LOG(ERROR) << "Fragments member was missing";
    return false;
  }
  const Json::Value& json_fragments = (*dictionary_)[kFragments];
  if (!json_fragments.isMember(fragment.Name())) {
    LOG(ERROR) << "Could not find a fragment called " << fragment.Name();
    return false;
  }
  return fragment.Deserialize(json_fragments[fragment.Name()]);
}

bool RedroidConfig::SaveFragment(const ConfigFragment& fragment) {
  Json::Value& json_fragments = (*dictionary_)[kFragments];
  if (json_fragments.isMember(fragment.Name())) {
    LOG(ERROR) << "Already have a fragment called " << fragment.Name();
    return false;
  }
  json_fragments[fragment.Name()] = fragment.Serialize();
  return true;
}

static constexpr char kDisplayConfigs[] = "display_configs";
static constexpr char kXRes[] = "x_res";
static constexpr char kYRes[] = "y_res";
static constexpr char kDpi[] = "dpi";
static constexpr char kRefreshRateHz[] = "refresh_rate_hz";
std::vector<RedroidConfig::DisplayConfig>
RedroidConfig::display_configs() const {
  std::vector<DisplayConfig> display_configs;
  for (auto& display_config_json : (*dictionary_)[kDisplayConfigs]) {
    DisplayConfig display_config = {};
    display_config.width = display_config_json[kXRes].asInt();
    display_config.height = display_config_json[kYRes].asInt();
    display_config.dpi = display_config_json[kDpi].asInt();
    display_config.refresh_rate_hz =
        display_config_json[kRefreshRateHz].asInt();
    display_configs.emplace_back(std::move(display_config));
  }
  return display_configs;
}

void RedroidConfig::set_display_configs(
    const std::vector<DisplayConfig>& display_configs) {
  Json::Value display_configs_json(Json::arrayValue);

  for (const DisplayConfig& display_configs : display_configs) {
    Json::Value display_config_json(Json::objectValue);
    display_config_json[kXRes] = display_configs.width;
    display_config_json[kYRes] = display_configs.height;
    display_config_json[kDpi] = display_configs.dpi;
    display_config_json[kRefreshRateHz] = display_configs.refresh_rate_hz;
    display_configs_json.append(display_config_json);
  }

  (*dictionary_)[kDisplayConfigs] = display_configs_json;
}


/*static*/ RedroidConfig* RedroidConfig::BuildConfigImpl(const std::string& path) {
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

/*static*/ std::unique_ptr<const RedroidConfig>
RedroidConfig::GetFromFile(const std::string& path) {
  return std::unique_ptr<const RedroidConfig>(BuildConfigImpl(path));
}

// Creates the (initially empty) config object and populates it with values from
// the config file if the CUTTLEFISH_CONFIG_FILE env variable is present.
// Returns nullptr if there was an error loading from file
/*static*/ const RedroidConfig* RedroidConfig::Get() {
  auto config_file_path = "/data/stream/";
  static std::shared_ptr<RedroidConfig> config(
      BuildConfigImpl(config_file_path));
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
