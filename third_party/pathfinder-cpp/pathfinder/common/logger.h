#ifndef PATHFINDER_LOGGER_H
#define PATHFINDER_LOGGER_H

#include <format>
#include <iostream>
#include <memory>
#include <unordered_map>

#define PATHFINDER_DEFAULT_LOG_TAG "Pathfinder Default Logger"

namespace Pathfinder {

template <class... Args>
decltype(auto) LOGV(std::string tag, const std::string_view message, Args... format_items) {
    std::string output = std::vformat(message, std::make_format_args(format_items...));
#ifdef __ANDROID__
    __android_log_write(ANDROID_LOG_VERBOSE, tag, output.c_str());
#else
    std::cout << std::format("<{}>", tag) << output << std::endl;
#endif
}

template <class... Args>
decltype(auto) LOGD(std::string tag, const std::string_view message, Args... format_items) {
    std::string output = std::vformat(message, std::make_format_args(format_items...));
#ifdef __ANDROID__
    __android_log_write(ANDROID_LOG_DEBUG, tag, output.c_str());
#else
    std::cout << std::format("<{}>", tag) << output << std::endl;
#endif
}

template <class... Args>
decltype(auto) LOGI(std::string tag, const std::string_view message, Args... format_items) {
    std::string output = std::vformat(message, std::make_format_args(format_items...));
#ifdef __ANDROID__
    __android_log_write(ANDROID_LOG_INFO, tag, output.c_str());
#else
    std::cout << std::format("<{}>", tag) << output << std::endl;
#endif
}

template <class... Args>
decltype(auto) LOGW(std::string tag, const std::string_view message, Args... format_items) {
    std::string output = std::vformat(message, std::make_format_args(format_items...));
#ifdef __ANDROID__
    __android_log_write(ANDROID_LOG_WARN, tag, output.c_str());
#else
    std::cout << std::format("<{}>", tag) << output << std::endl;
#endif
}

template <class... Args>
decltype(auto) LOGE(std::string tag, const std::string_view message, Args... format_items) {
    std::string output = std::vformat(message, std::make_format_args(format_items...));
#ifdef __ANDROID__
    __android_log_write(ANDROID_LOG_WARN, tag, output.c_str());
#else
    std::cerr << std::format("<{}>", tag) << output << std::endl;
#endif
}

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
    } default_level_ = Level::Warn;

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
            LOGV(tag, "[VERBOSE] {%s}{}", label.c_str());
        }
    }

    static void debug(const std::string &label, const std::string &module = "") {
        auto level = get_module_level(module);
        if (level <= Level::Debug) {
            auto tag = module.empty() ? PATHFINDER_DEFAULT_LOG_TAG : module.c_str();
            LOGD(tag, "[DEBUG] {}", label.c_str());
        }
    }

    static void info(const std::string &label, const std::string &module = "") {
        auto level = get_module_level(module);
        if (level <= Level::Info) {
            auto tag = module.empty() ? PATHFINDER_DEFAULT_LOG_TAG : module.c_str();
            LOGI(tag, "[INFO] {}", label.c_str());
        }
    }

    static void warn(const std::string &label, const std::string &module = "") {
        auto level = get_module_level(module);
        if (level <= Level::Warn) {
            auto tag = module.empty() ? PATHFINDER_DEFAULT_LOG_TAG : module.c_str();
            LOGI(tag, "[WARN] {}", label.c_str());
        }
    }

    static void error(const std::string &label, const std::string &module = "") {
        auto level = get_module_level(module);
        if (level <= Level::Error) {
            auto tag = module.empty() ? PATHFINDER_DEFAULT_LOG_TAG : module.c_str();
            LOGE(tag, "[ERROR] {}", label.c_str());
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
