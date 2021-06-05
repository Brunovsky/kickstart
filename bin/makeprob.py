#!/usr/bin/python3

import re
import sys
import os
import shutil
import subprocess
import signal
from pathlib import Path


def tryinput(str):
    try:
        return input(str).strip()
    except EOFError:
        sys.exit(0)


def read_which():
    valid = ["cpp", "debug", "int", "debug-int", ""]
    names = ["cpp", "debug", "int", "debug-int", "cpp"]
    which = tryinput(f"Type ({','.join(valid)}): ")
    if valid.count(which) == 0:
        print(f"Bad type name: {which}")
        return read_which()
    return names[valid.index(which)]


def read_name():
    name = tryinput("Name: ")
    if not re.match("^[a-zA-Z0-9-_!?#)(=~+\-*/.:,; ]+$", name):
        print(f"Bad input name: {name}")
        return read_name()
    foldername = re.sub('[^a-zA-Z0-9-\+]+', '', name.lower().replace(' ', '-'))
    return name, foldername


def get_repo_root():
    if Path.exists(Path(".git/")):
        return "."
    dir = ".."
    while not Path.exists(Path(f"{dir}/.git/")):
        dir = f"../{dir}"
        if Path.resolve(Path(dir), strict=True) == "/":
            print("Could not find repo root", file=sys.stderr)
            exit(2)
    return dir


def makeprob():
    which = read_which()
    name, folder = read_name()
    print(f"Problem folder: {folder}")

    root = get_repo_root()
    os.makedirs(folder, exist_ok=False)

    if which == "debug-cpp" or which == "cpp":
        shutil.copy(f"{root}/templates/{which}/code.cpp", folder)
        shutil.copy(f"{root}/templates/{which}/input.txt", folder)
        os.symlink(f"../{root}/common/cxx.make", f"{folder}/Makefile")
        subprocess.call(["code", f"{folder}/code.cpp", f"{folder}/input.txt"])
        return

    if which == "debug-int" or which == "int":
        shutil.copy(f"{root}/templates/{which}/code.cpp", folder)
        os.symlink(f"../{root}/common/cxx.make", f"{folder}/Makefile")
        os.symlink(f"../{root}/common/interactive_runner.py",
                   f"{folder}/interactive_runner.py")
        subprocess.call(["code", f"{folder}/code.cpp", f"{folder}/judge.py"])
        return

    assert False


if __name__ == "__main__":
    signal.signal(signal.SIGINT, lambda sig, frame: sys.exit(0))
    signal.signal(signal.SIGTERM, lambda sig, frame: sys.exit(0))
    makeprob()
