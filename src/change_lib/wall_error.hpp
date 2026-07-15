#pragma once
#include <system_error>

namespace wallchanger {

enum class wall_errc : uint8_t {
  Success = 0,
  cache_does_not_exists = 1,
  cache_exists = 3,
  cache_frm_cache_to_same = 4,
  cache_elem_not_exists = 5,
  cache_name_item_name_same = 6,
};

std::error_code make_error_code(wall_errc);

} // namespace wallchanger
namespace std {
template <> struct is_error_code_enum<wallchanger::wall_errc> : true_type {};
} // namespace std
