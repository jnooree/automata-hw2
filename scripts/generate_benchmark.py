#!/usr/bin/env python3
import sys
from pathlib import Path

from generate_test import TracingTopdownGenerator


def main():
    out_dir = Path(sys.argv[1])
    out_dir.mkdir(parents=True)

    generator = TracingTopdownGenerator()
    while len((e := generator.generate())[0]) < 3000:
        print("Skip", len(e[0]))

    out_dir.joinpath("positive.q").write_text(e[0] + "\n")
    out_dir.joinpath("positive.a").write_text("\n=> ".join(e[1]) + "\n")

    out_dir.joinpath("negative.q").write_text(e[0][:-1] + "\n")
    out_dir.joinpath("negative.a").write_text("reject\n")


if __name__ == "__main__":
    main()
