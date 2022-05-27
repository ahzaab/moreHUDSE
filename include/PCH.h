#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>

#include <spdlog/sinks/basic_file_sink.h>

namespace WinAPI = SKSE::WinAPI;

#ifndef NDEBUG
#    include <iostream>
#    include <spdlog/sinks/msvc_sink.h>
#    include <spdlog/sinks/stdout_color_sinks.h>
#endif

#include "IForEachScriptObjectFunctor.h"

using namespace std::literals;
namespace logger = SKSE::log;

#define DLLEXPORT __declspec(dllexport)

#include "Version.h"
