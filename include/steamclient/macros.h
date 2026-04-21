/// @file macros.h
/// File logging macros for use within SteamClient.
/// Only enabled if STEAMCLIENT_USE_SPDLOG is defined.

#pragma once

#if defined(STEAMCLIENT_USE_SPDLOG)

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <filesystem>

#define STEAMCLIENT_FILE_LOGGER() \
    static auto logger = spdlog::stdout_color_mt(std::filesystem::path(__FILE__).filename().string())

#define STEAMCLIENT_LOG_INFO(...)  logger->info(__VA_ARGS__)
#define STEAMCLIENT_LOG_WARN(...)  logger->warn(__VA_ARGS__)
#define STEAMCLIENT_LOG_ERROR(...) logger->error(__VA_ARGS__)

#else

#define STEAMCLIENT_FILE_LOGGER()
#define STEAMCLIENT_LOG_INFO(...)
#define STEAMCLIENT_LOG_WARN(...)
#define STEAMCLIENT_LOG_ERROR(...)

#endif