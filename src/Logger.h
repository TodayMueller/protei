#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

class Logger {
public:
    Logger(const std::string& configFileName);

    std::shared_ptr<spdlog::logger> getLogger() const;

private:
    std::shared_ptr<spdlog::logger> initLogger(const std::string& logLevelStr);

    std::shared_ptr<spdlog::logger> logger;
};