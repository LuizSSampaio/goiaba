#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/spdlog.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "GE/Logger.hpp"

using GE::Logger;

struct GlobalTestSetup {
    std::ostringstream engine_oss;
    std::ostringstream game_oss;

    GlobalTestSetup() {
        auto engine_sink =
            std::make_shared<spdlog::sinks::ostream_sink_mt>(engine_oss);
        auto game_sink =
            std::make_shared<spdlog::sinks::ostream_sink_mt>(game_oss);

        auto engine_l = std::make_shared<spdlog::logger>("engine", engine_sink);
        auto game_l = std::make_shared<spdlog::logger>("game", game_sink);
        engine_l->set_pattern("%v");
        engine_l->set_level(spdlog::level::trace);
        game_l->set_pattern("%v");
        game_l->set_level(spdlog::level::trace);

        spdlog::register_logger(engine_l);
        spdlog::register_logger(game_l);
    }

    void reset() {
        engine_oss.str("");
        engine_oss.clear();
        game_oss.str("");
        game_oss.clear();
    }
};
static GlobalTestSetup g_setup;

TEST_CASE("Logger is not default-constructible") {
    static_assert(!std::is_default_constructible_v<GE::Logger>);
    CHECK(true);
}

TEST_CASE("Logger::Info") {
    SUBCASE("Game environment (default)") {
        g_setup.reset();
        Logger::Info("hello");
        CHECK(g_setup.game_oss.str() == "hello\n");
        CHECK(g_setup.engine_oss.str().empty());
    }
    SUBCASE("Engine environment") {
        g_setup.reset();
        Logger::Info("hello", Logger::Environment::Engine);
        CHECK(g_setup.engine_oss.str() == "hello\n");
        CHECK(g_setup.game_oss.str().empty());
    }
    SUBCASE("Empty message") {
        g_setup.reset();
        Logger::Info("");
        CHECK(g_setup.game_oss.str() == "\n");
    }
    SUBCASE("Long message") {
        g_setup.reset();
        std::string msg(10240, 'x');
        Logger::Info(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
    SUBCASE("Special characters") {
        g_setup.reset();
        std::string msg = R"(!@#$%^&*()_+-=[]{}|;':",./<>?)";
        Logger::Info(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
    SUBCASE("Unicode") {
        g_setup.reset();
        std::string msg = "café 🎮 你好";
        Logger::Info(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
}

TEST_CASE("Logger::Warn") {
    SUBCASE("Game environment (default)") {
        g_setup.reset();
        Logger::Warn("warning");
        CHECK(g_setup.game_oss.str() == "warning\n");
        CHECK(g_setup.engine_oss.str().empty());
    }
    SUBCASE("Engine environment") {
        g_setup.reset();
        Logger::Warn("warning", Logger::Environment::Engine);
        CHECK(g_setup.engine_oss.str() == "warning\n");
        CHECK(g_setup.game_oss.str().empty());
    }
    SUBCASE("Empty message") {
        g_setup.reset();
        Logger::Warn("");
        CHECK(g_setup.game_oss.str() == "\n");
    }
    SUBCASE("Long message") {
        g_setup.reset();
        std::string msg(10240, 'x');
        Logger::Warn(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
    SUBCASE("Special characters") {
        g_setup.reset();
        std::string msg = R"(!@#$%^&*()_+-=[]{}|;':",./<>?)";
        Logger::Warn(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
    SUBCASE("Unicode") {
        g_setup.reset();
        std::string msg = "café 🎮 你好";
        Logger::Warn(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
}

TEST_CASE("Logger::Error") {
    SUBCASE("Game environment (default)") {
        g_setup.reset();
        Logger::Error("error");
        CHECK(g_setup.game_oss.str() == "error\n");
        CHECK(g_setup.engine_oss.str().empty());
    }
    SUBCASE("Engine environment") {
        g_setup.reset();
        Logger::Error("error", Logger::Environment::Engine);
        CHECK(g_setup.engine_oss.str() == "error\n");
        CHECK(g_setup.game_oss.str().empty());
    }
    SUBCASE("Empty message") {
        g_setup.reset();
        Logger::Error("");
        CHECK(g_setup.game_oss.str() == "\n");
    }
    SUBCASE("Long message") {
        g_setup.reset();
        std::string msg(10240, 'x');
        Logger::Error(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
    SUBCASE("Special characters") {
        g_setup.reset();
        std::string msg = R"(!@#$%^&*()_+-=[]{}|;':",./<>?)";
        Logger::Error(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
    SUBCASE("Unicode") {
        g_setup.reset();
        std::string msg = "café 🎮 你好";
        Logger::Error(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
}

TEST_CASE("Logger::Critical") {
    SUBCASE("Game environment (default)") {
        g_setup.reset();
        Logger::Critical("critical");
        CHECK(g_setup.game_oss.str() == "critical\n");
        CHECK(g_setup.engine_oss.str().empty());
    }
    SUBCASE("Engine environment") {
        g_setup.reset();
        Logger::Critical("critical", Logger::Environment::Engine);
        CHECK(g_setup.engine_oss.str() == "critical\n");
        CHECK(g_setup.game_oss.str().empty());
    }
    SUBCASE("Empty message") {
        g_setup.reset();
        Logger::Critical("");
        CHECK(g_setup.game_oss.str() == "\n");
    }
    SUBCASE("Long message") {
        g_setup.reset();
        std::string msg(10240, 'x');
        Logger::Critical(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
    SUBCASE("Special characters") {
        g_setup.reset();
        std::string msg = R"(!@#$%^&*()_+-=[]{}|;':",./<>?)";
        Logger::Critical(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
    SUBCASE("Unicode") {
        g_setup.reset();
        std::string msg = "café 🎮 你好";
        Logger::Critical(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
}

TEST_CASE("Logger::Trace") {
    SUBCASE("Game environment (default)") {
        g_setup.reset();
        Logger::Trace("trace");
        CHECK(g_setup.game_oss.str() == "trace\n");
        CHECK(g_setup.engine_oss.str().empty());
    }
    SUBCASE("Engine environment") {
        g_setup.reset();
        Logger::Trace("trace", Logger::Environment::Engine);
        CHECK(g_setup.engine_oss.str() == "trace\n");
        CHECK(g_setup.game_oss.str().empty());
    }
    SUBCASE("Empty message") {
        g_setup.reset();
        Logger::Trace("");
        CHECK(g_setup.game_oss.str() == "\n");
    }
    SUBCASE("Long message") {
        g_setup.reset();
        std::string msg(10240, 'x');
        Logger::Trace(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
    SUBCASE("Special characters") {
        g_setup.reset();
        std::string msg = R"(!@#$%^&*()_+-=[]{}|;':",./<>?)";
        Logger::Trace(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
    SUBCASE("Unicode") {
        g_setup.reset();
        std::string msg = "café 🎮 你好";
        Logger::Trace(msg);
        CHECK(g_setup.game_oss.str() == msg + "\n");
    }
}

TEST_CASE("Cross-environment isolation") {
    g_setup.reset();
    Logger::Info("engine-msg", Logger::Environment::Engine);
    Logger::Info("game-msg", Logger::Environment::Game);
    CHECK(g_setup.engine_oss.str() == "engine-msg\n");
    CHECK(g_setup.game_oss.str() == "game-msg\n");
}

TEST_CASE("Multiple sequential messages") {
    g_setup.reset();
    Logger::Info("first");
    Logger::Info("second");
    Logger::Info("third");
    CHECK(g_setup.game_oss.str() == "first\nsecond\nthird\n");
}
