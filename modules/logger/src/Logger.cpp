#include "GE/Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

#include "spdlog/common.h"
#include "spdlog/logger.h"

namespace {
std::shared_ptr<spdlog::logger> GetLogger(GE::Logger::Environment environment) {
    return spdlog::get(environment == GE::Logger::Environment::Engine ? "engine"
                                                                      : "game");
}
}  // namespace

namespace GE {
void Logger::Init() {
    if (spdlog::get("engine")) {
        return;
    }

    auto engineSink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    auto gameSink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();

    auto engine = std::make_shared<spdlog::logger>("engine", engineSink);
    auto game = std::make_shared<spdlog::logger>("game", gameSink);

    engine->set_level(spdlog::level::trace);
    game->set_level(spdlog::level::trace);

    spdlog::register_logger(engine);
    spdlog::register_logger(game);
}

void Logger::Shutdown() {
    spdlog::drop("engine");
    spdlog::drop("game");
    spdlog::shutdown();
}

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
