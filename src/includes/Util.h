#ifndef SRC_INCLUDES_UTIL_H_
#define SRC_INCLUDES_UTIL_H_

#include <cstddef>

namespace yql_model {
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
namespace Literals {
constexpr std::size_t operator"" _KiB(unsigned long long int val) {
  return 8 * 1024ULL * val;
}

constexpr std::size_t operator"" _MiB(unsigned long long int val) {
  return 1024_KiB * val;
}

constexpr std::size_t operator"" _GiB(unsigned long long int val) {
  return 1024_MiB * val;
}

constexpr std::size_t operator"" _TiB(unsigned long long int val) {
  return 1024_GiB * val;
}

constexpr std::size_t operator"" _KB(unsigned long long int val) {
  return 8 * 1000ULL * val;
}

constexpr std::size_t operator"" _MB(unsigned long long int val) {
  return 1000_KB * val;
}

constexpr std::size_t operator"" _GB(unsigned long long int val) {
  return 1000_MB * val;
}

constexpr std::size_t operator"" _TB(unsigned long long int val) {
  return 1000_GB * val;
}

constexpr std::size_t operator"" _Kb(unsigned long long int val) {
  return 1000ULL * val;
}

constexpr std::size_t operator"" _Mb(unsigned long long int val) {
  return 1000_KB * val;
}

constexpr std::size_t operator"" _Gb(unsigned long long int val) {
  return 1000_MB * val;
}

constexpr std::size_t operator"" _Tb(unsigned long long int val) {
  return 1000_GB * val;
}
}  // namespace Literals
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
// NOLINTEND(readability-magic-numbers)
}  // namespace yql_model

#endif  // SRC_INCLUDES_UTIL_H_
