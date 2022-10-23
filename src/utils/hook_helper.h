#pragma once
#include <dlfcn.h>
#include <dobby.h>
#include <spdlog/spdlog.h>

#define INSTALL_HOOK(lib, name, fn_ret_t, fn_args_t...)                                                                    \
    fn_ret_t (*orig_##name)(fn_args_t);                                                                                    \
    fn_ret_t fake_##name(fn_args_t);                                                                                       \
    static void install_hook_##name() {                                                                                    \
        void *sym_addr = DobbySymbolResolver(lib, #name);                                                                  \
        hook_function(sym_addr, (dobby_dummy_func_t)fake_##name, (dobby_dummy_func_t*)&orig_##name);                       \
    }                                                                                                                      \
    fn_ret_t fake_##name(fn_args_t)

#define INSTALL_HOOK_NO_LIB(name, fn_ret_t, fn_args_t...)                                                                  \
    INSTALL_HOOK(nullptr, name, fn_ret_t, fn_args_t)

#define TO_ABS_VOIDP(map, address)                                                                                         \
    reinterpret_cast<void*>(map.startAddress + address)

inline int hook_function(void* original, dobby_dummy_func_t replace, dobby_dummy_func_t* backup)
{
    Dl_info info;
    if (dladdr(original, &info)) {
        spdlog::info("Hooking {} ({}) from {} ({})",
                     info.dli_sname ? info.dli_sname : "(unknown symbol)", info.dli_saddr,
                     info.dli_fname ? info.dli_fname : "(unknown file)", info.dli_fbase);
    }

    return DobbyHook(original, replace, backup);
}

inline int unhook_function(void* original)
{
    Dl_info info;
    if (dladdr(original, &info)) {
        spdlog::info("Unhooking {} ({}) from {} ({})",
                     info.dli_sname ? info.dli_sname : "(unknown symbol)", info.dli_saddr,
                     info.dli_fname ? info.dli_fname : "(unknown file)", info.dli_fbase);
    }

    return DobbyDestroy(original);
}
