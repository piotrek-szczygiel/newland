#!/usr/bin/env python3
import errno
import os
import pathlib
import subprocess
import sys


def collect_files(roots, extensions):
    result = []
    for root in roots:
        for (directory, _, files) in os.walk(root):
            for f in files:
                path = os.path.join(directory, f)
                if pathlib.Path(path).suffix in extensions:
                    result.append(path)

    return result


def format(file):
    format_base = ["clang-format", "-i"]
    command = format_base + [f]
    print("Formatting", f)
    subprocess.run(command)


if __name__ == "__main__":
    extensions = [".cpp", ".hpp"]

    try:
        if len(sys.argv) > 1:
            for f in sys.argv[1:]:
                format(f)
        else:
            for f in collect_files(["newland"], extensions):
                format(f)
    except OSError as e:
        if e.errno == errno.ENOENT:
            print("Error: clang-format is not installed!")
            print("Install from package manager")
            print("Download from https://llvm.org/builds/ on Windows")

    print("Formatting CMakeLists.txt")
    try:
        subprocess.run(
            [
                "cmake-format",
                "-i",
                "--line-width",
                "120",
                "--tab-size",
                "4",
                "CMakeLists.txt",
            ]
        )
    except OSError as e:
        if e.errno == errno.ENOENT:
            print("Error: cmake-format is not installed!")
            print("Install with: pip install cmake-format")
