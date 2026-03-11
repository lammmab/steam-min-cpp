#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <filesystem>

#define FILE_LOGGER() \
    static auto logger = spdlog::stdout_color_mt(std::filesystem::path(__FILE__).filename().string())
