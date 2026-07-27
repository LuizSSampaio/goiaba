#include "GE/Logger.hpp"

#include <spdlog/spdlog.h>

#include <memory>
#include <string>

namespace {
std::shared_ptr<spdlog::logger> GetLogger(GE::Logger::Environment environment) {
    static std::shared_ptr<spdlog::logger> engine;
    static std::shared_ptr<spdlog::logger> game;

    if (environment == GE::Logger::Environment::Engine) {
        if (!engine) {
            engine = spdlog::get("engine");
        }
        return engine;
    }

    if (!game) {
        game = spdlog::get("game");
    }
    return game;
}
}  // namespace

namespace GE {
void Logger::Info(const std::string& message, Environment environment) {
    GetLogger(environment)->info(message);
}

void Logger::Warn(const std::string& message, Environment environment) {
    GetLogger(environment)->warn(message);
}

void Logger::Error(const std::string& message, Environment environment) {
    GetLogger(environment)->error(message);
}

void Logger::Critical(const std::string& message, Environment environment) {
    GetLogger(environment)->critical(message);
}

void Logger::Trace(const std::string& message, Environment environment) {
    GetLogger(environment)->trace(message);
}
}  // namespace GE
