/*
 * Minimal filesystem tools
 */
#pragma once

#include <string>
#include <sys/stat.h>

namespace radl {

inline bool exists(const std::string& filename) noexcept {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

}  // namespace radl
