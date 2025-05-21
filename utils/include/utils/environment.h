#pragma once

#include <string>

namespace redroid {

enum class Arch {
  Arm,
  Arm64,
  X86,
  X86_64,
};

std::string StringFromEnv(const std::string& varname,
                          const std::string& defval);

std::string HostArchStr();
Arch HostArch();
bool IsHostCompatible(Arch arch);

bool IsRunningInContainer();

}  // namespace redroid
