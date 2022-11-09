#include <iostream>
#include <string>

#include "automata.hpp"

namespace athw2 {
namespace {
  enum State {
    kQ,    /* 0 */
    kQId,  /* 1 */
    kQPM,  /* 2 */
    kQMD,  /* 3 */
    kQLp,  /* 4 */
    kQRp,  /* 5 */
    kQEos, /* 6 */
  };

  enum StringAlphabet {
    kId,  /* 0 */
    kPM,  /* 1 */
    kMD,  /* 2 */
    kLp,  /* 3 */
    kRp,  /* 4 */
    kEos, /* 5 */
  };

  enum StackAlphabet {
    kStackId,  /* 0 */
    kStackPM,  /* 1 */
    kStackMD,  /* 2 */
    kStackLp,  /* 3 */
    kStackRp,  /* 4 */
    kStackEos, /* 5 */
    kStackE,   /* 6 */
    kStackEp,  /* 7 */
    kStackT,   /* 8 */
    kStackTp,  /* 9 */
    kStackF,   /* 10 */
    kStackA,   /* 11 */
  };

  constexpr int kStateCount = kQEos + 1, kStringAlphabetCount = kEos + 1,
                kStackAlphabetCount = kStackA + 1;

  StringTransition build_str_trs() {
    // clang-format off
    StringTransition q({ kStringAlphabetCount, {
      { kId,  kQId  },
      { kPM,  kQPM  },
      { kMD,  kQMD  },
      { kLp,  kQLp  },
      { kRp,  kQRp  },
      { kEos, kQEos },
    } });
    // clang-format on
    return q;
  }

  ParseTable build_table() {
    // clang-format off
    StackTransition q_id({ kStackAlphabetCount, {
      { kStackId, { kQ,   { } } },
      { kStackE,  { kQId, { kStackT, kStackEp } } },
      { kStackT,  { kQId, { kStackF, kStackTp } } },
      { kStackF,  { kQId, { kStackA } } },
      { kStackA,  { kQId, { kStackId } } },
    } });
    StackTransition q_pm({ kStackAlphabetCount, {
      { kStackPM, { kQ,   { } } },
      { kStackEp, { kQPM, { kPM, kStackT, kStackEp } } },
      { kStackTp, { kQPM, { } } },
    } });
    StackTransition q_md({ kStackAlphabetCount, {
      { kStackMD, { kQ,   { } } },
      { kStackTp, { kQMD, { kMD, kStackF, kStackTp } } },
    } });
    StackTransition q_lp({ kStackAlphabetCount, {
      { kStackLp, { kQ,   { } } },
      { kStackE,  { kQLp, { kStackT, kStackEp } } },
      { kStackT,  { kQLp, { kStackF, kStackTp } } },
      { kStackF,  { kQLp, { kStackLp, kStackE, kStackRp } } },
    } });
    StackTransition q_rp({ kStackAlphabetCount, {
      { kStackRp, { kQ, { } } },
      { kStackEp, { kQRp, { } } },
      { kStackTp, { kQRp, { } } },
    } });
    StackTransition q_eof({ kStackAlphabetCount, {
      { kStackEp, { kQEos, { } } },
      { kStackTp, { kQEos, { } } },
    } });

    ParseTable t_q({ kStateCount, {
      { kQId, std::move(q_id) },
      { kQPM, std::move(q_pm) },
      { kQMD, std::move(q_md) },
      { kQLp, std::move(q_lp) },
      { kQRp, std::move(q_rp) },
      { kQEos, std::move(q_eof) },
    } });
    // clang-format on

    return t_q;
  }

  // NOLINTBEGIN(clang-diagnostic-unused-member-function)
  struct Lexer {
    static int tokenize(const char a) {
      switch (a) {
      case '(':
        return kLp;
      case ')':
        return kRp;
      case '+':
      case '-':
        return kPM;
      case '*':
      case '/':
        return kMD;
      case '\0':
        return kEos;
      default:
        return kId;
      }
    }

    static bool is_terminal(const int A) {
      return A < kStackE;
    }

    static std::string getrepr(const int A, const char a) {
      switch (A) {
      case kStackLp:
        return "(";
      case kStackRp:
        return ")";
      case kStackE:
        return "E";
      case kStackEp:
        return "E'";
      case kStackT:
        return "T";
      case kStackTp:
        return "T'";
      case kStackF:
        return "F";
      case kStackA:
        return "A";
      default:
        // kStackId, kStackPM, kStackMD, kStackEos
        return { a };
      }
    }
  };
  // NOLINTEND(clang-diagnostic-unused-member-function)

  void stacktrace(std::ostream &os, const std::vector<std::string> &msg) {
    if (msg.empty())
      return;

    auto it = msg.begin();
    os << *it << '\n';
    for (; ++it != msg.end();)
      os << "=> " << *it << '\n';
  }
} // namespace
} // namespace athw2

int main() {
  std::string input;
  std::cin >> input;

  athw2::DPA<athw2::Lexer> dpa(athw2::build_str_trs(), athw2::build_table(),
                               athw2::kQ, athw2::kQEos, athw2::kStackE);
  const auto result = dpa.parse(input);

  athw2::stacktrace(std::cout, result);
  return 0;
}
