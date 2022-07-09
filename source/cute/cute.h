/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef CUTE_CUTE_H
#define CUTE_CUTE_H

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory_resource>
#include <optional>
#include <variant>

namespace cute {
  using time_type  = int64_t;
  using value_type = float;

  extern std::pmr::unsynchronized_pool_resource memory_resource;

  struct noop { };

  struct color4 {
    value_type red   = 0;
    value_type green = 0;
    value_type blue  = 0;
    value_type alpha = 0;

    [[nodiscard]] auto operator<=>(color4 const &) const noexcept =
        default;
  };

  struct coord2 {
    value_type x = 0;
    value_type y = 0;

    [[nodiscard]] auto operator<=>(coord2 const &) const noexcept =
        default;
  };

  struct fragment_position : coord2 { };

  struct render_fragment {
    coord2 position;
    color4 color;
  };

  using entity_type =
      std::variant<noop, fragment_position, render_fragment>;

  struct state {
    using fn_form = std::function<entity_type(state const &,
                                              entity_type const &)>;

    std::pmr::vector<entity_type> entities =
        std::pmr::vector<entity_type> { &memory_resource };
    std::pmr::vector<fn_form> forms = std::pmr::vector<fn_form> {
      &memory_resource
    };

    [[nodiscard]] auto put(entity_type entity) const noexcept
        -> state;

    [[nodiscard]] auto form_any(fn_form fn) const noexcept -> state;

    [[nodiscard]] auto form(auto fn) const noexcept -> state;

    [[nodiscard]] auto fragment(coord2 position) const noexcept
        -> color4;

    [[nodiscard]] auto cycle() const noexcept -> state;

    [[nodiscard]] auto select_fragment(coord2 position) const noexcept
        -> std::optional<color4>;
  };
}

#include "cute.impl.h"

#endif
