

#include "device_registry.h"

#include <android-base/logging.h>

#include "device_handler.h"

namespace redroid {

bool DeviceRegistry::RegisterDevice(
    const std::string& device_id,
    std::weak_ptr<DeviceHandler> device_handler) {
  if (devices_.count(device_id) > 0) {
    LOG(ERROR) << "Device '" << device_id << "' is already registered";
    return false;
  }

  devices_.try_emplace(device_id, device_handler);
  LOG(INFO) << "Registered device: '" << device_id << "'";
  return true;
}

void DeviceRegistry::UnRegisterDevice(const std::string& device_id) {
  auto record = devices_.find(device_id);
  if (record == devices_.end()) {
    LOG(WARNING) << "Requested to unregister an unkwnown device: '" << device_id
                 << "'";
    return;
  }
  devices_.erase(record);
  LOG(INFO) << "Unregistered device: '" << device_id << "'";
}

std::shared_ptr<DeviceHandler> DeviceRegistry::GetDevice(
    const std::string& device_id) {
  if (devices_.count(device_id) == 0) {
    LOG(INFO) << "Requested device (" << device_id << ") is not registered";
    return nullptr;
  }
  auto device_handler = devices_[device_id].lock();
  if (!device_handler) {
    LOG(WARNING) << "Destroyed device handler detected for device '"
                 << device_id << "'";
    UnRegisterDevice(device_id);
  }
  return device_handler;
}

std::vector<std::string> DeviceRegistry::ListDeviceIds() const {
  std::vector<std::string> ret;
  for (const auto& entry: devices_) {
    ret.push_back(entry.first);
  }
  return ret;
}

}  // namespace redroid
