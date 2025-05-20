

#pragma once

#include <cinttypes>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <json/json.h>

namespace redroid {

class DeviceHandler;

class DeviceRegistry {
 public:
  bool RegisterDevice(const std::string& device_id,
                      std::weak_ptr<DeviceHandler> device_handler);
  void UnRegisterDevice(const std::string& device_id);

  std::shared_ptr<DeviceHandler> GetDevice(const std::string& device_id);

  std::vector<std::string> ListDeviceIds() const;

 private:
  std::map<std::string, std::weak_ptr<DeviceHandler>> devices_;
};

}  // namespace redroid
