/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef CUTE_CUTE_IMPL_H
#define CUTE_CUTE_IMPL_H

namespace cute {
  constexpr auto coord2i::operator<=>(
      coord2i const &other) const noexcept {
    using std::strong_ordering;
    if (y < other.y || (y == other.y && x < other.x))
      return strong_ordering::less;
    if (y > other.y || (y == other.y && x > other.x))
      return strong_ordering::greater;
    return strong_ordering::equivalent;
  }

  inline auto state::form(auto fn) const noexcept -> state {
    return form_any([fn](state const &self, primitive_type const &e)
                        -> std::pmr::vector<primitive_type> {
      return std::visit(
          [&](auto value) -> std::pmr::vector<primitive_type> {
            return fn(self, value);
          },
          e);
    });
  }
}

#endif
