#ifndef CHESSY_BINARY_H_
#define CHESSY_BINARY_H_

#include <cstdint>

namespace chessy {

template<char... digits>
struct conv2bin;

template<char high, char... digits>
struct conv2bin<high, digits...> {
  static_assert(high == '0' || high == '1', "invalid binary");
  static uint64_t const value = ((high - '0') * (1 << sizeof...(digits)) +
                                 conv2bin<digits...>::value);
};

template<char high>
struct conv2bin<high> {
  static_assert(high == '0' || high == '1', "invalid binary");
  static uint64_t const value = (high - '0');
};

template<char... digits>
constexpr uint64_t operator ""_bin() {
  return conv2bin<digits...>::value;
}

constexpr uint64_t R8(uint64_t r0, uint64_t r1, uint64_t r2, uint64_t r3,
                      uint64_t r4, uint64_t r5, uint64_t r6, uint64_t r7) {
  return ((r0 << 0 * 8) | (r1 << 1 * 8) | (r2 << 2 * 8) | (r3 << 3 * 8) |
          (r4 << 4 * 8) | (r5 << 5 * 8) | (r6 << 6 * 8) | (r7 << 7 * 8));
}

}  // namespace chessy

#endif  // CHESSY_BINARY_H_
