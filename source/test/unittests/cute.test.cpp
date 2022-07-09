/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../cute/cute.h"
#include <catch2/catch.hpp>

namespace cute::test {
  using std::is_same_v, std::decay_t, std::monostate;

  TEST_CASE("create state") {
    std::ignore = state {};
  }

  TEST_CASE("black pixels by default") {
    REQUIRE(state {}.fragment({ .x = 0, .y = 0 }) ==
            color4 { .red = 0, .green = 0, .blue = 0, .alpha = 0 });
  }

  TEST_CASE("custom fragment converter") {
    REQUIRE(state {}
                .form([](state const &, auto in) -> entity_type {
                  if constexpr (is_same_v<decay_t<decltype(in)>,
                                          fragment_position>)
                    return render_fragment { .position = in,
                                             .color    = {
                                                    .red   = .2f,
                                                    .green = in.x * .01f,
                                                    .blue  = in.y * .01f,
                                                    .alpha = .7f } };
                  return noop {};
                })
                .fragment({ .x = 10, .y = 20 }) ==
            color4 { .red   = .2f,
                     .green = 10 * .01f,
                     .blue  = 20 * .01f,
                     .alpha = .7f });
  }
}
