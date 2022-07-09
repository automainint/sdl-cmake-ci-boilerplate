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

  extern std::pmr::synchronized_pool_resource memory_resource;

  struct color4 {
    value_type red   = 0;
    value_type green = 0;
    value_type blue  = 0;
    value_type alpha = 0;

    [[nodiscard]] auto operator<=>(color4 const &) const noexcept =
        default;
  };

  struct coord2i {
    ptrdiff_t x = 0;
    ptrdiff_t y = 0;

    [[nodiscard]] constexpr auto operator<=>(
        coord2i const &) const noexcept -> std::strong_ordering;
  };

  struct recti {
    ptrdiff_t x      = 0;
    ptrdiff_t y      = 0;
    ptrdiff_t width  = 0;
    ptrdiff_t height = 0;

    [[nodiscard]] auto operator<=>(recti const &) const noexcept =
        default;
  };

  struct fragment_in {
    recti area;

    [[nodiscard]] auto operator<=>(
        fragment_in const &) const noexcept = default;
  };

  struct fragment_out {
    coord2i position;
    color4  color;

    [[nodiscard]] auto operator<=>(
        fragment_out const &) const noexcept = default;
  };

  using primitive_type = std::variant<fragment_in, fragment_out>;

  struct state {
    using fn_form = std::function<std::pmr::vector<primitive_type>(
        state const &, primitive_type const &)>;

    std::pmr::vector<primitive_type> primitives =
        std::pmr::vector<primitive_type> { &memory_resource };

    std::pmr::vector<fn_form> forms = std::pmr::vector<fn_form> {
      &memory_resource
    };

    [[nodiscard]] auto put(primitive_type entity) const noexcept
        -> state;

    [[nodiscard]] auto form_any(fn_form fn) const noexcept -> state;

    [[nodiscard]] auto form(auto fn) const noexcept -> state;

    [[nodiscard]] auto cycle() const noexcept -> state;

    [[nodiscard]] auto fragment(coord2i position) const noexcept
        -> color4;

    [[nodiscard]] auto fragment(ptrdiff_t index) const noexcept
        -> color4;
  };
}

#include "cute.impl.h"

#endif
