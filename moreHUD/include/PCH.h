#pragma once
#include "RE/Skyrim.h"
#include "SKSE/Logger.h"

#include <spdlog/sinks/basic_file_sink.h>

#ifndef NDEBUG
#include <iostream>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#endif

using namespace std::literals;
namespace logger = SKSE::log;


#define DYNAMIC_CAST(base, srcType, targetType)((base)->As<targetType>())
