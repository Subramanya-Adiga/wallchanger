#include "wall_error.hpp"

namespace {
struct wall_error_catagory : std::error_category {
  [[nodiscard]] const char *name() const noexcept override {
    return "wall_lib";
  }

  [[nodiscard]] std::string message(int err_val) const override {
    switch (static_cast<wallchanger::wall_errc>(err_val)) {

    case wallchanger::wall_errc::cache_does_not_exists:
      return "Cache Does Not Exists";
    case wallchanger::wall_errc::cache_exists:
      return "Cache Already Exists";
    case wallchanger::wall_errc::cache_frm_cache_to_same:
      return "Cache From And Cache To Names Cannot Be Same";
    default:
      return "(Unrecognized Error)";
    }
  }
};

const wall_error_catagory s_wall_err_cat{};
} // namespace

namespace wallchanger {
std::error_code make_error_code(wall_errc err) {
  return {static_cast<int>(err), s_wall_err_cat};
}
} // namespace wallchanger