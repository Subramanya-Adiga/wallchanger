#include "common.hpp"
#include <linux/errno.h>
#include <linux/string.h>
#include <system_error>

namespace {
class linux_error_catagory : public std::error_category {
public:
  [[nodiscard]] const char *name() const noexcept override {
    return "LinuxError";
  }
  [[nodiscard]] std::string message(int c) const override {
    const char *err = nullptr;

    locale_t loc = uselocale((locale_t)0);
    locale_t copy = loc;

    if (copy == LC_GLOBAL_LOCALE) {
      copy = duplocale(copy);
    }

    if (copy != (locale_t)nullptr) {
      err = strerror_l(c, copy);
    }

    if (loc == LC_GLOBAL_LOCALE) {
      freelocale(copy);
    }

    if (err == nullptr) {
      return "Unknown Error";
    }
    return err;
  }
};

const linux_error_catagory linux_error_cat{};

} // namespace

std::error_code make_error_code(const linux_error_code &e) {
  return {static_cast<int>(e.error), linux_error_cat};
}

std::error_code make_linux_error_code() {
  return make_error_code(linux_error_code{errno});
}
