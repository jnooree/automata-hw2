#!/usr/bin/env python3
import sys
import random
from pathlib import Path
from collections import defaultdict
from typing import TypeVar, Generic, Iterator

_T = TypeVar("_T")


class Stack(Generic[_T]):
    def __init__(self, *initial: _T):
        super().__init__()
        self._data = list(initial)

    def push(self, item: _T):
        self._data.append(item)

    def peek(self) -> _T:
        return self._data[-1]

    def pop(self) -> _T:
        return self._data.pop()

    def clear(self):
        self._data.clear()

    def __len__(self) -> int:
        return len(self._data)

    def __iter__(self) -> Iterator[_T]:
        return reversed(self._data)


class TopdownGenerator:
    def __init__(self) -> None:
        super().__init__()
        self.stack = Stack[str]()
        self._dispatcher = defaultdict(lambda: self._terminal, {
            "E": self._E,
            "E'": self._Ep,
            "T": self._T,
            "T'": self._Tp,
            "F": self._F,
            "A": self._A,
        })

    def on_begin(self):
        pass

    def on_step(self, expr: str):
        pass

    def generate(self):
        self.stack.clear()
        self.stack.push("E")

        expr = ""
        self.on_begin()
        while self.stack:
            expr += self.dispatch()
            self.on_step(expr)
        return expr

    def dispatch(self):
        stackvar = self.stack.peek()
        terminal, stack_next = self._dispatcher[stackvar]()
        self.stack.pop()
        for v in reversed(stack_next):
            self.stack.push(v)
        return terminal

    def _E(self):
        return "", ["T", "E'"]

    def _Ep(self):
        stack_choices = [["T", "E'"], []]
        term_choices = ["+", "-"]
        stack_next = random.choice(stack_choices)
        terminal = random.choice(term_choices) if stack_next else ""
        return terminal, stack_next

    def _T(self):
        return "", ["F", "T'"]

    def _Tp(self):
        stack_choices = [["F", "T'"], []]
        term_choices = ["*", "/"]
        stack_next = random.choice(stack_choices)
        terminal = random.choice(term_choices) if stack_next else ""
        return terminal, stack_next

    def _F(self):
        stack_choices: list[tuple] = [("(", ["E", ")"]), ("", ["A"])]
        ret, = random.choices(stack_choices, weights=[1, 5])
        return ret

    def _A(self):
        choices = ["a", "b", "c", "d", "x", "y", "z",
                   "2", "3", "4", "5", "6", "7", "8", "9"]
        return random.choice(choices), []

    def _terminal(self):
        return self.stack.peek(), []


class TracingTopdownGenerator(TopdownGenerator):
    def __init__(self):
        super().__init__()
        self.stacktrace = []

    def on_begin(self):
        self.stacktrace = ["".join(self.stack)]

    def on_step(self, expr: str):
        # Skip duplicate stacktrace
        if not self.stack or self.stack.peek() != ")":
            self.stacktrace.append(expr + "".join(self.stack))

    def generate(self):
        return super().generate(), self.stacktrace


def generate_positive(total: int = 100):
    generator = TracingTopdownGenerator()
    examples = []
    while len(examples) < total:
        examples += [result for _ in range(total)
                     if len((result := generator.generate())[0]) > 10]
    return examples[:total]


def _remove_random(expr: str) -> str:
    pos = random.randrange(len(expr))
    return expr[:pos] + expr[pos + 1:]


def generate_negative(total: int = 100):
    generator = TopdownGenerator()
    examples = []
    while len(examples) < total:
        examples += [(_remove_random(result), ["reject"]) for _ in range(total)
                     if len(result := generator.generate()) > 10]
    return examples[:total]


def main():
    out_dir = Path(sys.argv[1])
    out_dir.mkdir(parents=True)

    def write_examples(i, expr, stacktrace):
        prefix = f"{i:03d}"
        inp = out_dir / f"{prefix}.q"
        out = out_dir / f"{prefix}.a"

        inp.write_text(expr + "\n")
        out.write_text("\n=> ".join(stacktrace) + "\n")

    i = 0
    for i, (expr, stacktrace) in enumerate(generate_positive()):
        write_examples(i, expr, stacktrace)
    for i, (expr, stacktrace) in enumerate(generate_negative(), i + 1):
        write_examples(i, expr, stacktrace)


if __name__ == "__main__":
    main()
