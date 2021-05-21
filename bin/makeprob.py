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
    except EOFError as e:
        sys.exit(0)


def read_which():
    which = tryinput("Type (gcpp,gint,cpp,int): ")
    if ["gcpp", "gint", "cpp", "int"].count(which) == 0:
        print(f"Bad type name: {which}")
        return read_which()
    return which


def read_name():
    name = tryinput("Name: ")
    if not re.match("^[a-zA-Z0-9-_!?#)(=~+\-*/.:,; ]+$", name):
        print(f"Bad input name: {name}")
        return read_name()
    foldername = re.sub('[^a-zA-Z0-9-\+]+', '', name.lower().replace(' ', '-'))
    return foldername


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
    folder = read_name()
    print(f"Problem folder: {folder}")

    root = get_repo_root()
    os.makedirs(folder, exist_ok=False)

    if which == "gcpp":
        shutil.copy(f"{root}/templates/google-cpp/code.cpp", folder)
        shutil.copy(f"{root}/templates/google-cpp/input.txt", folder)
        os.symlink(f"../{root}/common/cxx.make", f"{folder}/Makefile")
        subprocess.call(["code", f"{folder}/code.cpp", f"{folder}/input.txt"])
        return

    if which == "cpp":
        shutil.copy(f"{root}/templates/cpp/code.cpp", folder)
        shutil.copy(f"{root}/templates/cpp/input.txt", folder)
        os.symlink(f"../{root}/common/cxx.make", f"{folder}/Makefile")
        subprocess.call(["code", f"{folder}/code.cpp", f"{folder}/input.txt"])
        return

    if which == "gint":
        shutil.copy(f"{root}/templates/google-int/code.cpp", folder)
        os.symlink(f"../{root}/common/cxx.make", f"{folder}/Makefile")
        os.symlink(f"../{root}/common/interactive_runner.py",
                   f"{folder}/interactive_runner.py")
        subprocess.call(["code", f"{folder}/code.cpp", f"{folder}/judge.py"])
        return

    if which == "int":
        shutil.copy(f"{root}/templates/int/code.cpp", folder)
        os.symlink(f"../{root}/common/cxx.make", f"{folder}/Makefile")
        os.symlink(f"../{root}/common/interactive_runner.py",
                   f"{folder}/interactive_runner.py")
        subprocess.call(["code", f"{folder}/code.cpp", f"{folder}/judge.cpp"])
        return

    assert False


if __name__ == "__main__":
    signal.signal(signal.SIGINT, lambda sig, frame: sys.exit(0))
    signal.signal(signal.SIGTERM, lambda sig, frame: sys.exit(0))
    makeprob()
