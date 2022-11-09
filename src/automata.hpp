#ifndef AUTOMATA_HW2_AUTOMATA_HPP_
#define AUTOMATA_HW2_AUTOMATA_HPP_

#include <cassert>
#include <string>
#include <utility>
#include <vector>

#include "fixedmap.hpp"

namespace athw2 {
namespace detail {
  template <class VT>
  class TransitionImpl {
  private:
    fixed_map<int, VT> table_;

  public:
    TransitionImpl(const fixed_map<int, VT> &table): table_(table) { }

    TransitionImpl(fixed_map<int, VT> &&table) noexcept
      : table_(std::move(table)) { }

    bool has_next(int key) const {
      return table_.contains(key);
    }

    const VT &operator()(int key) const {
      return table_[key];
    }
  };
} // namespace detail

using StackTransition =
  detail::TransitionImpl<std::pair<int, std::vector<int>>>;
using ParseTable = fixed_map<int, StackTransition>;

using StringTransition = detail::TransitionImpl<int>;

template <class T>
class transparent_stack {
private:
  std::vector<T> data_;

public:
  transparent_stack() = default;

  transparent_stack(const std::vector<T> &data): data_(data) { }

  transparent_stack(std::vector<T> &&data) noexcept: data_(std::move(data)) { }

  bool empty() const {
    return data_.empty();
  }

  T &top() {
    return data_.back();
  }

  const T &top() const {
    return data_.back();
  }

  void pop() {
    data_.pop_back();
  }

  void push(const T &val) {
    data_.push_back(val);
  }

  void push(T &&val) noexcept {
    data_.push_back(std::move(val));
  }

  auto begin() {
    return data_.rbegin();
  }

  auto end() {
    return data_.rend();
  }

  auto begin() const {
    return data_.rbegin();
  }

  auto end() const {
    return data_.rend();
  }
};

/* Detriministic Pushdown Automata */
template <class Lexer>
class DPA: public Lexer {
private:
  StringTransition str_trs_;
  ParseTable table_;
  int start_state_;
  int end_state_;
  int toplevel_;

public:
  DPA(StringTransition &&str_trs, ParseTable &&table, int start, int end,
      int toplevel)
  noexcept
    : str_trs_(std::move(str_trs)), table_(std::move(table)),
      start_state_(start), end_state_(end), toplevel_(toplevel) { }

  std::vector<std::string> parse(const std::string &str) const;

private:
  int string_transition(int state, char a) const;
  int stack_transition(int state, char a, transparent_stack<int> &stack,
                       std::vector<std::string> &msg,
                       std::string &parsed) const;

  int tokenize(const char a) const {
    return Lexer::tokenize(a);
  }

  bool is_terminal(const int A) const {
    return Lexer::is_terminal(A);
  }

  std::string getrepr(const int A, const char a = '\0') const {
    return Lexer::getrepr(A, a);
  }
};

template <class Lexer>
std::vector<std::string> DPA<Lexer>::parse(const std::string &str) const {
  transparent_stack<int> stack({ toplevel_ });
  std::vector<std::string> msg { getrepr(toplevel_) };
  std::string parsed;

  int state = start_state_;
  const char *cptr = str.c_str();
  for (int i = 0; i <= str.size(); ++i) {
    const char a = cptr[i];
    int temp = string_transition(state, a);
    if (temp < 0)
      goto reject;

    state = stack_transition(temp, a, stack, msg, parsed);
  }

  // null character for end of string
  state = stack_transition(state, '\0', stack, msg, parsed);
  if (!stack.empty() || state != end_state_)
    goto reject;

  return msg;

reject:
  return { "reject" };
}

template <class Lexer>
int DPA<Lexer>::string_transition(const int state, const char a) const {
  if (state != start_state_)
    return -1;

  const int tok = tokenize(a);
  return str_trs_.has_next(tok) ? str_trs_(tok) : -1;
}

template <class Lexer>
int DPA<Lexer>::stack_transition(int state, const char a,
                                 transparent_stack<int> &stack,
                                 std::vector<std::string> &msg,
                                 std::string &parsed) const {
  while (!stack.empty()) {
    const int A = stack.top();
    // Stop processing, move to the next character
    if (!table_.contains(state))
      break;

    const StackTransition &trs = table_[state];
    // Stop processing, move to the next character
    if (!trs.has_next(A))
      break;

    stack.pop();
    const auto &entry = trs(A);
    state = entry.first;

    // Already parsed, return
    if (is_terminal(A)) {
      parsed += a;
      break;
    }

    for (auto it = entry.second.rbegin(); it != entry.second.rend(); ++it)
      stack.push(*it);

    std::string &repr = msg.emplace_back(parsed);
    for (auto e: stack)
      repr += getrepr(e, a);
  }

  return state;
}
} // namespace athw2

#endif /* AUTOMATA_HW2_AUTOMATA_HPP_ */
