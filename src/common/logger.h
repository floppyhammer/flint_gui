#ifndef FLINT_LOGGER_H
#define FLINT_LOGGER_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>

#define FLINT_LOG_TAG "Flint"
#ifdef __ANDROID__
    #include <android/log.h>
    #define FLINT_LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, FLINT_LOG_TAG, __VA_ARGS__)
    #define FLINT_LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, FLINT_LOG_TAG, __VA_ARGS__)
    #define FLINT_LOGI(...) __android_log_print(ANDROID_LOG_INFO, FLINT_LOG_TAG, __VA_ARGS__)
    #define FLINT_LOGW(...) __android_log_print(ANDROID_LOG_WARN, FLINT_LOG_TAG, __VA_ARGS__)
    #define FLINT_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, FLINT_LOG_TAG, __VA_ARGS__)
#else
    #define FLINT_LOGV(...)            \
        printf("<%s>", FLINT_LOG_TAG); \
        printf(__VA_ARGS__);           \
        printf("\n")
    #define FLINT_LOGD(...)            \
        printf("<%s>", FLINT_LOG_TAG); \
        printf(__VA_ARGS__);           \
        printf("\n")
    #define FLINT_LOGI(...)            \
        printf("<%s>", FLINT_LOG_TAG); \
        printf(__VA_ARGS__);           \
        printf("\n")
    #define FLINT_LOGW(...)            \
        printf("<%s>", FLINT_LOG_TAG); \
        printf(__VA_ARGS__);           \
        printf("\n")
    #define FLINT_LOGE(...)            \
        printf("<%s>", FLINT_LOG_TAG); \
        printf(__VA_ARGS__);           \
        printf("\n")
#endif

namespace Flint::Utils {

class Logger {
public:
    static Logger *get_singleton() {
        static Logger singleton;
        return &singleton;
    }

    enum class Level {
        VERBOSE = 0,
        DEBUG,
        INFO,
        WARN,
        ERROR,
    } level = Level::INFO;

    static void set_level(Level p_level) {
        get_singleton()->level = p_level;
    }

    static void verbose(const std::string &label, const std::string &module = "") {
        if (get_singleton()->level <= Level::VERBOSE) {
            system("Color 07");
            FLINT_LOGV("[VERBOSE][%s] %s", (module.empty() ? "default" : module).c_str(), label.c_str());
        }
    }

    static void debug(const std::string &label, const std::string &module = "") {
        if (get_singleton()->level <= Level::DEBUG) {
            system("Color 07");
            FLINT_LOGD("[DEBUG][%s] %s", (module.empty() ? "default" : module).c_str(), label.c_str());
        }
    }

    static void info(const std::string &label, const std::string &module = "") {
        if (get_singleton()->level <= Level::INFO) {
            system("Color 07");
            FLINT_LOGI("[INFO][%s] %s", (module.empty() ? "default" : module).c_str(), label.c_str());
        }
    }

    static void warn(const std::string &label, const std::string &module = "") {
        if (get_singleton()->level <= Level::WARN) {
            system("Color 06");
            FLINT_LOGW("[WARN][%s] %s", (module.empty() ? "default" : module).c_str(), label.c_str());
        }
    }

    static void error(const std::string &label, const std::string &module = "") {
        if (get_singleton()->level <= Level::ERROR) {
            system("Color 04");
            FLINT_LOGE("[ERROR][%s] %s", (module.empty() ? "default" : module).c_str(), label.c_str());
        }
    }

private:
    // So we can't be instantiated by outsiders.
    Logger() = default;

public:
    Logger(Logger const &) = delete;

    void operator=(Logger const &) = delete;
};

} // namespace Flint::Utils

#endif // FLINT_LOGGER_H
