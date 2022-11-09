#!/usr/bin/env python3
import sys
import random
from pathlib import Path
from collections import defaultdict
from typing import TypeVar, Generic, Iterator

_T = TypeVar("_T")
MAX = 3000


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
        self._done = False
        self._dispatcher = defaultdict(
            lambda: self._terminal, self._dispatch_variables)

    def on_begin(self):
        self._done = False

    def on_step(self, expr: str):
        if len(expr) >= MAX:
            self._done = True

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

    def _terminal(self):
        return self.stack.peek(), []

    @property
    def _dispatch_variables(self):
        return {
            "E": self._E,
            "E'": self._Ep,
            "T": self._T,
            "T'": self._Tp,
            "F": self._F,
            "A": self._A,
        }

    @staticmethod
    def _E():
        return "", ["T", "E'"]

    def _Ep(self):
        stack_choices = [["T", "E'"], []]
        if self._done:
            return "", stack_choices[-1]

        term_choices = ["+", "-"]
        stack_next = random.choice(stack_choices)
        terminal = random.choice(term_choices) if stack_next else ""
        return terminal, stack_next

    @staticmethod
    def _T():
        return "", ["F", "T'"]

    def _Tp(self):
        stack_choices = [["F", "T'"], []]
        if self._done:
            return "", stack_choices[-1]

        term_choices = ["*", "/"]
        stack_next = random.choice(stack_choices)
        terminal = random.choice(term_choices) if stack_next else ""
        return terminal, stack_next

    def _F(self):
        stack_choices: list[tuple] = [("(", ["E", ")"]), ("", ["A"])]
        if self._done:
            return stack_choices[-1]
        ret, = random.choices(stack_choices, weights=[1, 2])
        return ret

    @staticmethod
    def _A():
        choices = ["a", "b", "c", "d", "x", "y", "z",
                   "2", "3", "4", "5", "6", "7", "8", "9"]
        return random.choice(choices), []


class TracingTopdownGenerator(TopdownGenerator):
    def __init__(self):
        super().__init__()
        self.stacktrace = []

    def on_begin(self):
        super().on_begin()
        self.stacktrace = ["".join(self.stack)]

    def on_step(self, expr: str):
        super().on_step(expr)
        # Skip duplicate stacktrace
        if not self.stack or self.stack.peek() != ")":
            self.stacktrace.append(expr + "".join(self.stack))

    def generate(self):
        return super().generate(), self.stacktrace


def generate_positive(total: int):
    generator = TracingTopdownGenerator()
    examples = []
    while len(examples) < total:
        examples += [result for _ in range(total)
                     if len((result := generator.generate())[0]) > 10]
    return examples[:total]


def remove_random(expr: str) -> str:
    pos = random.randrange(len(expr))
    return expr[:pos] + expr[pos + 1:]


def generate_negative(total: int):
    generator = TopdownGenerator()
    examples = []
    while len(examples) < total:
        examples += [(remove_random(result), ["reject"]) for _ in range(total)
                     if len(result := generator.generate()) > 10]
    return examples[:total]


def main():
    out_dir = Path(sys.argv[1])
    out_dir.mkdir(parents=True)

    total = 100

    def write_examples(i, expr, stacktrace):
        prefix = f"{i:03d}"
        inp = out_dir / f"{prefix}.q"
        out = out_dir / f"{prefix}.a"

        inp.write_text(expr + "\n")
        out.write_text("\n=> ".join(stacktrace) + "\n")

    i = 0
    for i, (expr, stacktrace) in enumerate(generate_positive(total)):
        write_examples(i, expr, stacktrace)
    for i, (expr, stacktrace) in enumerate(generate_negative(total), i + 1):
        write_examples(i, expr, stacktrace)


if __name__ == "__main__":
    main()
