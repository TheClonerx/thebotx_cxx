#pragma once
#include <string>
#include <typeinfo>

#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#define HAS_CXXABI_H
#endif

namespace tcx {
inline std::string unmangled_name(std::type_info const& type)
{
#ifdef HAS_CXXABI_H
    char const* name = type.name();
    int status;
    std::unique_ptr<char, void (*)(void*)> res {
        abi::__cxa_demangle(name, nullptr, nullptr, &status),
        std::free
    };
    return (status == 0) ? res.get() : name;
#else
    return type.name();
#endif
}
} // namespace tcx
