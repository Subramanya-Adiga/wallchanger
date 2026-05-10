#pragma once
#include "../../defines.hpp"

struct linux_error_code{
    explicit linux_error_code(u32 err)noexcept:error(err){}
    u32 error;
};

namespace std {
template <> struct is_error_code_enum<linux_error_code> : std::true_type {};
} // namespace std

std::error_code make_error_code(const linux_error_code &e);

std::error_code make_linux_error_code();
