/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../cute/cute.h"
#include <catch2/catch.hpp>
#include <cmath>

namespace cute::test {
  using std::is_same_v, std::decay_t, std::monostate,
      std::pmr::vector;

  TEST_CASE("create state") {
    std::ignore = state {};
  }

  TEST_CASE("black pixels by default") {
    REQUIRE(state {}.fragment({ .x = 0, .y = 0 }) ==
            color4 { .red = 0, .green = 0, .blue = 0, .alpha = 0 });
  }

  TEST_CASE("custom fragment converter") {
    REQUIRE(
        state {}
            .form([](state const &,
                     auto in) -> vector<primitive_type> {
              if constexpr (is_same_v<decay_t<decltype(in)>,
                                      fragment_in>)
                return { fragment_out {
                    .position = { .x = in.area.x, .y = in.area.y },
                    .color    = { .red   = .2f,
                                  .green = in.area.x * .01f,
                                  .blue  = in.area.y * .01f,
                                  .alpha = .7f } } };
              return {};
            })
            .put(fragment_in { .area = { .x      = 10,
                                         .y      = 20,
                                         .width  = 1,
                                         .height = 1 } })
            .cycle()
            .fragment({ .x = 10, .y = 20 }) ==
        color4 { .red   = .2f,
                 .green = 10 * .01f,
                 .blue  = 20 * .01f,
                 .alpha = .7f });
  }

  TEST_CASE("ignored primitives will be lost") {
    REQUIRE(state {}.put(fragment_in {}).cycle().primitives.empty());
  }

  TEST_CASE("fragment rect") {
    REQUIRE(
        state {}
            .form([](state const &,
                     auto in) -> std::pmr::vector<primitive_type> {
              if constexpr (is_same_v<decay_t<decltype(in)>,
                                      fragment_in>) {
                auto v = vector<primitive_type> { &memory_resource };

                for (auto j = in.area.y;
                     j < in.area.y + in.area.height; ++j)
                  for (auto i = in.area.x;
                       i < in.area.x + in.area.width; ++i)
                    v.emplace_back(
                        fragment_out { .position = { i, j },
                                       .color    = { .red   = .1f,
                                                     .green = .2f,
                                                     .blue  = .3f,
                                                     .alpha = .4f } });

                return v;
              }
              return {};
            })
            .put(fragment_in { .area = { .x      = 0,
                                         .y      = 0,
                                         .width  = 10,
                                         .height = 10 } })
            .cycle()
            .fragment({ .x = 5, .y = 5 }) ==
        color4 {
            .red = .1f, .green = .2f, .blue = .3f, .alpha = .4f });
  }

  TEST_CASE("fragment by index") {
    REQUIRE(
        state {}
            .form([](state const &,
                     auto in) -> std::pmr::vector<primitive_type> {
              if constexpr (is_same_v<decay_t<decltype(in)>,
                                      fragment_in>) {
                auto v = vector<primitive_type> { &memory_resource };

                for (auto j = in.area.y;
                     j < in.area.y + in.area.height; ++j)
                  for (auto i = in.area.x;
                       i < in.area.x + in.area.width; ++i)
                    v.emplace_back(fragment_out {
                        .position = { i, j },
                        .color    = { .red   = static_cast<float>(i),
                                      .green = static_cast<float>(j),
                                      .blue  = 0,
                                      .alpha = 0 } });

                return v;
              }
              return {};
            })
            .put(fragment_in { .area = { .x      = 0,
                                         .y      = 0,
                                         .width  = 10,
                                         .height = 10 } })
            .cycle()
            .fragment(56) ==
        color4 { .red = 6.f, .green = 5.f, .blue = 0, .alpha = 0 });
  }
}
