/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef CUTE_CUTE_IMPL_H
#define CUTE_CUTE_IMPL_H

namespace cute {
  inline auto state::form(auto fn) const noexcept -> state {
    return form_any([fn](state const       &self,
                         entity_type const &e) -> entity_type {
      return std::visit(
          [&](auto value) -> entity_type { return fn(self, value); },
          e);
    });
  }
}

#endif
