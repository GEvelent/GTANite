#include <chrono>
#include <thread>
#include <dobby.h>
#include <KittyMemory.h>
#include <jni.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/android_sink.h>

#include "utils/gtasa.h"

eGTASAVersion g_version = UNKNOWN;

void check_version(const KittyMemory::ProcMap& map)
{
    std::uint8_t buf[4];
    KittyMemory::memRead(buf, reinterpret_cast<void*>(map.startAddress + 0x29DE6A), 4);

    spdlog::debug("0x{:X}, 0x{:X}, 0x{:X}, 0x{:X}", buf[0], buf[1], buf[2], buf[3]);

#if defined(__aarch64__)
    if (buf[0] == 0x00 && buf[1] == 0xAA && buf[2] == 0x74 && buf[3] == 0x96) {
        spdlog::info("GTA: SA v2.10 64-bit detected.");
        g_version = V21064;
    }
    else {
        spdlog::info("GTA: SA v2.10 32-bit detected.");
        spdlog::error("Unsupported GTA: SA version.");
        g_version = V21032;
    }
#elif defined(__arm__)
    if (buf[0] == 0xBD && buf[1] == 0xE8 && buf[2] == 0x00 && buf[3] == 0x0B) {
        spdlog::info("GTA: SA v2.00 detected.");
        g_version = V200;
    }
    else {
        spdlog::info("GTA: SA v1.08 detected.");
        spdlog::error("Unsupported GTA: SA version.");
        g_version = V108;
    }
#endif
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    try {
        auto android_logger{ spdlog::android_logger_mt("android", "GTANite") };
        android_logger->set_level(spdlog::level::debug);
        android_logger->set_pattern("[%n] [%^%l%$] %v");
        spdlog::set_default_logger(android_logger);
    }
    catch (const spdlog::spdlog_ex& ex) {
        __android_log_print(ANDROID_LOG_ERROR, "GTANite", "Log initialization failed: %s", ex.what());
        return JNI_VERSION_1_6;
    }

    spdlog::info("GTANite starting.. Build time: " __DATE__ " " __TIME__);

    // Set Dobby Hook log level.
    log_set_level(0);

    KittyMemory::ProcMap map{};
    while (true) {
        map = KittyMemory::getLibraryBaseMap("libGTASA.so");
        if (!map.isValid()) {
            std::this_thread::sleep_for(std::chrono::milliseconds{ 32 });
            continue;
        }

        spdlog::debug("libGTASA.so start address: 0x{:X}", map.startAddress);
        break;
    }

    check_version(map);
    if (g_version == V21032 || g_version == V108) {
        return JNI_VERSION_1_6;
    }

    (void)vm; (void)reserved;
    return JNI_VERSION_1_6;
}