#pragma once

#include <string>

namespace GE {
class Logger {
public:
    Logger() = delete;

    enum Environment { Engine, Game };

    static void Info(const std::string& message,
                     Environment environment = Environment::Game);
    static void Warn(const std::string& message,
                     Environment environment = Environment::Game);
    static void Error(const std::string& message,
                      Environment environment = Environment::Game);
    static void Critical(const std::string& message,
                         Environment environment = Environment::Game);
    static void Trace(const std::string& message,
                      Environment environment = Environment::Game);
};
}  // namespace GE
