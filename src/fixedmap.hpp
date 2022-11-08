#ifndef AUTOMATA_HW2_FIXEDSET_HPP_
#define AUTOMATA_HW2_FIXEDSET_HPP_

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <vector>

namespace athw2 {
template <class KT, class VT>
class iterable_fixed_map;

template <class KT, class VT>
class fixed_map_iterator {
public:
  using key_type = KT;

  using value_type = VT;
  using reference = value_type &;
  using pointer = value_type *;
  using difference_type = int;
  using iterator_category = std::forward_iterator_tag;

private:
  using key_iterator = typename std::vector<key_type>::const_iterator;

  using non_const_value_type = std::remove_const_t<value_type>;
  using value_container = std::vector<non_const_value_type>;
  using values_pointer =
    const std::conditional_t<std::is_const_v<value_type>, const value_container,
                             value_container> *;

  const std::vector<int> *map_;
  values_pointer vals_;
  key_iterator iter_;

  fixed_map_iterator(const std::vector<int> *map, values_pointer *vals,
                     key_iterator iter)
    : map_(map), vals_(vals), iter_(iter) { }

  template <typename = std::enable_if_t<std::is_const_v<value_type>>>
  fixed_map_iterator(
    const fixed_map_iterator<KT, std::remove_const_t<value_type>> &other)
    : map_(other.map_), vals_(other.vals_), iter_(other.iter_) { }

public:
  fixed_map_iterator() = default;

  fixed_map_iterator &operator++() {
    ++iter_;
    return *this;
  }

  fixed_map_iterator operator++(int) {
    fixed_map_iterator ret = *this;
    ++(*this);
    return ret;
  }

  bool operator==(const fixed_map_iterator &rhs) const {
    return iter_ == rhs.iter_;
  }

  bool operator!=(const fixed_map_iterator &rhs) const {
    return !(*this == rhs);
  }

  reference &operator*() const {
    return vals_[map_[*iter_]];
  }

  const key_type &key() const {
    return *iter_;
  }

  friend class iterable_fixed_map<KT, const value_type>;
  friend class iterable_fixed_map<KT, non_const_value_type>;
};

template <class KT, class VT>
class iterable_fixed_map {
public:
  using key_type = KT;
  using value_type = VT;

  using iterator = fixed_map_iterator<key_type, value_type>;
  using const_iterator = fixed_map_iterator<key_type, const value_type>;

private:
  std::vector<int> map_;
  std::vector<key_type> keys_;
  std::vector<value_type> vals_;

public:
  iterable_fixed_map() = default;

  explicit iterable_fixed_map(const int capacity): map_(capacity, -1) { }

  iterable_fixed_map(const int capacity,
                     std::initializer_list<std::pair<key_type, value_type>> li)
    : iterable_fixed_map(capacity) {
    keys_.reserve(li.size());
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
    keys_.push_back(key);
    vals_.push_back(val);
    return true;
  }

  bool insert(const key_type key, value_type &&val) noexcept {
    assert(key >= 0 && key < capacity());
    if (contains(key))
      return false;

    map_[key] = vals_.size();
    keys_.push_back(key);
    vals_.push_back(std::move(val));
    return true;
  }

  value_type &operator[](key_type key) {
    return vals_[map_[key]];
  }

  const value_type &operator[](key_type key) const {
    return vals_[map_[key]];
  }

  int size() const {
    return keys_.size();
  }

  int capacity() const {
    return map_.size();
  }

  bool empty() const {
    return keys_.empty();
  }

  void clear() {
    if (size() > capacity() >> 1)
      std::fill(map_.begin(), map_.end(), -1);
    else
      for (const auto v: keys_)
        map_[v] = -1;

    keys_.clear();
    vals_.clear();
  }

  iterator begin() {
    return iterator(&map_, &vals_, keys_.begin());
  }

  iterator end() {
    return iterator(&map_, &vals_, keys_.end());
  }

  const_iterator begin() const {
    return const_iterator(&map_, &vals_, keys_.begin());
  }

  const_iterator end() const {
    return const_iterator(&map_, &vals_, keys_.end());
  }
};
} // namespace athw2

#endif /* AUTOMATA_HW2_FIXEDSET_HPP_ */
