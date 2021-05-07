#!/usr/bin/python3

import re
import sys
import os
import shutil
import subprocess
import signal
from datetime import datetime

template = sys.argv[1]
signal.signal(signal.SIGINT, lambda sig, frame: sys.exit(0))
signal.signal(signal.SIGTERM, lambda sig, frame: sys.exit(0))


def tryinput(str):
    try:
        return input(str).strip()
    except EOFError as e:
        sys.exit(0)


def read_name():
    name = tryinput("Name: ")
    if not re.match("^[a-zA-Z0-9-_!?#)(=~+\-*/.:,; ]+$", name):
        print(f"Bad input name name: {name}")
        return read_name()
    return name


name = read_name()
foldername = re.sub('[^a-zA-Z0-9-\+]+', '', name.lower().replace(' ', '-'))

folder = f"{foldername}"
print(f"Problem folder: {folder}")

os.makedirs(folder, exist_ok=True)

if template == "cpp":
    shutil.copy("templates/cpp/code.cpp", folder)
    shutil.copy("templates/cpp/input.txt", folder)
    os.symlink("../templates/cpp/Makefile", f"{folder}/Makefile")
    subprocess.call(["code", f"{folder}/code.cpp", f"{folder}/input.txt"])

if template == "cpp-interactive":
    shutil.copy("templates/cpp-interactive/code.cpp", folder)
    shutil.copy("templates/cpp-interactive/run.sh", folder)
    os.symlink("../../templates/cpp-interactive/Makefile",
               f"{folder}/Makefile")
    os.symlink("../../templates/cpp-interactive/interactive_runner.py",
               f"{folder}/interactive_runner.py")
    print("Don't forget to add the testing tool (testing_tool.py)")
    subprocess.call(["code", f"{folder}/code.cpp", f"{folder}/run.sh"])
