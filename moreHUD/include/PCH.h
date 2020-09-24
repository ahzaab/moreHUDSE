#pragma once
#include "RE/Skyrim.h"
#include <xbyak/xbyak.h>  // must be between these two
#include "SKSE/SKSE.h"

#include "SKSE/Logger.h"

#include <spdlog/sinks/basic_file_sink.h>

#ifndef NDEBUG
#include <iostream>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#endif

using namespace std::literals;
namespace logger = SKSE::log;

template <typename T1, typename T2>
inline T2* dyna_cast(T1* base)
{
    auto asForm = static_cast<T1*>(base);
    auto ret = (asForm)->As<targetType>();
    return ret;
}
#define DYNAMIC_CAST(base, srcType, targetType) (dyna_cast<srcType, targetType>(base))


