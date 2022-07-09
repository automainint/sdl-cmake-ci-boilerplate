/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "cute.h"

#include <algorithm>

namespace cute {
  using std::visit, std::decay_t, std::is_same_v, std::optional,
      std::nullopt, std::pmr::synchronized_pool_resource, std::sort,
      std::is_sorted, std::merge, std::back_inserter,
      std::pmr::vector, std::lower_bound;

  synchronized_pool_resource memory_resource = {};

  auto state::put(primitive_type entity) const noexcept -> state {
    auto s = state { *this };
    s.primitives.emplace_back(entity);
    return s;
  }

  auto state::form_any(fn_form fn) const noexcept -> state {
    auto s = state { *this };
    s.forms.emplace_back(fn);
    return s;
  }

  auto state::cycle() const noexcept -> state {
    auto s = state { .forms = forms };

    for (auto &fn : forms)
      for (auto &entity : primitives) {
        auto v = fn(*this, entity);
        if (v.size() < 16)
          for (auto &x : v)
            s.primitives.emplace(lower_bound(s.primitives.begin(),
                                             s.primitives.end(), x),
                                 x);
        else {
          auto merged = vector<primitive_type> { &memory_resource };
          merged.reserve(s.primitives.size() + v.size());
          if (!is_sorted(v.begin(), v.end()))
            sort(v.begin(), v.end());
          merge(s.primitives.begin(), s.primitives.end(), v.begin(),
                v.end(), back_inserter(merged));
          s.primitives = std::move(merged);
        }
      }

    return s;
  }

  auto state::fragment(coord2i position) const noexcept -> color4 {
    auto i = lower_bound(
        primitives.begin(), primitives.end(),
        primitive_type { fragment_out { .position = position } });
    if (i == primitives.end())
      return {};
    return visit(
        [](auto const &f) -> color4 {
          if constexpr (is_same_v<decay_t<decltype(f)>, fragment_out>)
            return f.color;
          return {};
        },
        *i);
  }

  auto state::fragment(ptrdiff_t index) const noexcept -> color4 {
    if (index < 0 || index >= primitives.size())
      return {};
    return visit(
        [](auto const &f) -> color4 {
          if constexpr (is_same_v<decay_t<decltype(f)>, fragment_out>)
            return f.color;
          return {};
        },
        primitives[index]);
  }
}
