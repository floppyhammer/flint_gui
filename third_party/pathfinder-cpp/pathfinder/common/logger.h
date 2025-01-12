#ifndef PATHFINDER_LOGGER_H
#define PATHFINDER_LOGGER_H

#include <iostream>
#include <unordered_map>
#include <memory>

#define PATHFINDER_DEFAULT_LOG_TAG "Pathfinder Default Logger"

#ifdef __ANDROID__
    #include <android/log.h>

    #define PATHFINDER_LOGV(tag, ...) __android_log_print(ANDROID_LOG_VERBOSE, tag, __VA_ARGS__)
    #define PATHFINDER_LOGD(tag, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, __VA_ARGS__)
    #define PATHFINDER_LOGI(tag, ...) __android_log_print(ANDROID_LOG_INFO, tag, __VA_ARGS__)
    #define PATHFINDER_LOGW(tag, ...) __android_log_print(ANDROID_LOG_WARN, tag, __VA_ARGS__)
    #define PATHFINDER_LOGE(tag, ...) __android_log_print(ANDROID_LOG_ERROR, tag, __VA_ARGS__)
#else
    #include <cstdio>

    #define RESET "\033[0m"
    #define RED "\033[31m"
    #define GREEN "\033[32m"
    #define YELLOW "\033[33m"

    #define PATHFINDER_LOGV(tag, ...) \
        printf("<%s>", tag);          \
        printf(__VA_ARGS__);          \
        printf("\n")
    #define PATHFINDER_LOGD(tag, ...) \
        printf("<%s>", tag);          \
        printf(__VA_ARGS__);          \
        printf("\n")
    #define PATHFINDER_LOGI(tag, ...) \
        printf("<%s>", tag);          \
        printf(__VA_ARGS__);          \
        printf("\n")
    #define PATHFINDER_LOGW(tag, ...) \
        printf(YELLOW "<%s>", tag);   \
        printf(__VA_ARGS__);          \
        printf("\n" RESET)
    #define PATHFINDER_LOGE(tag, ...) \
        printf(RED "<%s>", tag);      \
        printf(__VA_ARGS__);          \
        printf("\n" RESET)
#endif

namespace Pathfinder {

class Logger {
public:
    static Logger *get_singleton() {
        static Logger singleton;
        return &singleton;
    }

    enum class Level {
        Verbose = 0,
        Debug,
        Info,
        Warn,
        Error,
        Silence,
    } default_level_;

    std::unordered_map<std::string, Level> module_levels;

    static void set_default_level(Level level) {
        get_singleton()->default_level_ = level;
    }

    static void set_module_level(const std::string &module, Level level) {
        if (module.empty()) {
            return;
        }
        get_singleton()->module_levels[module] = level;
    }

    static Level get_module_level(const std::string &module) {
        if (module.empty()) {
            return get_singleton()->default_level_;
        }
        if (get_singleton()->module_levels.find(module) == get_singleton()->module_levels.end()) {
            return get_singleton()->default_level_;
        }
        return get_singleton()->module_levels.at(module);
    }

    static void verbose(const std::string &label, const std::string &module = "") {
        auto level = get_module_level(module);
        if (level <= Level::Verbose) {
            auto tag = (module.empty() ? PATHFINDER_DEFAULT_LOG_TAG : module).c_str();
            PATHFINDER_LOGV(tag, "[VERBOSE] %s", label.c_str());
        }
    }

    static void debug(const std::string &label, const std::string &module = "") {
        auto level = get_module_level(module);
        if (level <= Level::Debug) {
            auto tag = module.empty() ? PATHFINDER_DEFAULT_LOG_TAG : module.c_str();
            PATHFINDER_LOGD(tag, "[DEBUG] %s", label.c_str());
        }
    }

    static void info(const std::string &label, const std::string &module = "") {
        auto level = get_module_level(module);
        if (level <= Level::Info) {
            auto tag = module.empty() ? PATHFINDER_DEFAULT_LOG_TAG : module.c_str();
            PATHFINDER_LOGI(tag, "[INFO] %s", label.c_str());
        }
    }

    static void warn(const std::string &label, const std::string &module = "") {
        auto level = get_module_level(module);
        if (level <= Level::Warn) {
            auto tag = module.empty() ? PATHFINDER_DEFAULT_LOG_TAG : module.c_str();
            PATHFINDER_LOGI(tag, "[WARN] %s", label.c_str());
        }
    }

    static void error(const std::string &label, const std::string &module = "") {
        auto level = get_module_level(module);
        if (level <= Level::Error) {
            auto tag = module.empty() ? PATHFINDER_DEFAULT_LOG_TAG : module.c_str();
            PATHFINDER_LOGI(tag, "[ERROR] %s", label.c_str());
        }
    }

private:
    // So it can't be instantiated by outsiders.
    Logger() = default;

public:
    Logger(Logger const &) = delete;

    void operator=(Logger const &) = delete;
};

} // namespace Pathfinder

#endif // PATHFINDER_LOGGER_H
