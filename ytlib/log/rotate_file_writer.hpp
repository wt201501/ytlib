#pragma once

#include "log.hpp"
#include "log_exports.h"

namespace ytlib {

LOG_API LogWriter GetRotateFileWriter(const std::map<std::string, std::string>& cfg);

}  // namespace ytlib
