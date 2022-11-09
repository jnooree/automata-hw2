#ifndef AUTOMATA_HW2_FIXEDSET_HPP_
#define AUTOMATA_HW2_FIXEDSET_HPP_

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <vector>

namespace athw2 {
template <class KT, class VT>
class fixed_map {
public:
  using key_type = KT;
  using value_type = VT;

private:
  std::vector<int> map_;
  std::vector<value_type> vals_;

public:
  fixed_map() = default;

  explicit fixed_map(const int capacity): map_(capacity, -1) { }

  fixed_map(const int capacity,
            std::initializer_list<std::pair<key_type, value_type>> li)
    : fixed_map(capacity) {
    vals_.reserve(li.size());
    for (const auto &p: li)
      insert(p.first, p.second);
  }

  bool contains(const key_type key) const {
    assert(key >= 0 && key < capacity());
    return map_[key] >= 0;
  }

  bool insert(const key_type key, const value_type &val) {
    assert(key >= 0 && key < capacity());
    if (contains(key))
      return false;

    map_[key] = vals_.size();
    vals_.push_back(val);
    return true;
  }

  bool insert(const key_type key, value_type &&val) noexcept {
    assert(key >= 0 && key < capacity());
    if (contains(key))
      return false;

    map_[key] = vals_.size();
    vals_.push_back(std::move(val));
    return true;
  }

  value_type &operator[](key_type key) {
    return vals_[map_[key]];
  }

  const value_type &operator[](key_type key) const {
    return vals_[map_[key]];
  }

  int capacity() const {
    return map_.size();
  }

  void clear() {
    std::fill(map_.begin(), map_.end(), -1);
    vals_.clear();
  }
};
} // namespace athw2

#endif /* AUTOMATA_HW2_FIXEDSET_HPP_ */
