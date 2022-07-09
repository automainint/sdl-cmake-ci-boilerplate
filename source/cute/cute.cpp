/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "cute.h"

namespace cute {
  using std::visit, std::decay_t, std::is_same_v, std::optional,
      std::nullopt, std::pmr::unsynchronized_pool_resource;

  unsynchronized_pool_resource memory_resource = {};

  auto state::put(entity_type entity) const noexcept -> state {
    auto s = state { *this };
    s.entities.emplace_back(entity);
    return s;
  }

  auto state::form_any(fn_form fn) const noexcept -> state {
    auto s = state { *this };
    s.forms.emplace_back(fn);
    return s;
  }

  auto state::fragment(coord2 position) const noexcept -> color4 {
    if (auto color = put(fragment_position { position })
                         .cycle()
                         .select_fragment(position);
        color)
      return *color;
    return {};
  }

  auto state::cycle() const noexcept -> state {
    auto s = state { *this };

    for (auto &fn : forms)
      for (auto &entity : entities)
        visit(
            [&](auto value) {
              if constexpr (!is_same_v<decay_t<decltype(value)>,
                                       noop>)
                s.entities.emplace_back(value);
            },
            fn(*this, entity));

    return s;
  }

  auto state::select_fragment(coord2 position) const noexcept
      -> optional<color4> {
    for (auto &entity : entities)
      if (auto color = visit(
              [&](auto value) -> optional<color4> {
                if constexpr (is_same_v<decay_t<decltype(value)>,
                                        render_fragment>)
                  if (value.position == position)
                    return value.color;
                return nullopt;
              },
              entity);
          color)
        return color;
    return nullopt;
  }
}
