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


def read_round():
    rnd = tryinput("Round: ")
    if not re.match("^[a-zA-Z1-9-\+]{1,10}$", rnd):
        print(f"Bad input round: {rnd}")
        return read_round()
    return rnd.upper()


def read_name():
    name = tryinput("Name: ")
    if not re.match("^[a-zA-Z0-9-_!?#)(=~+\-*/.:,; ]+$", name):
        print(f"Bad input name name: {name}")
        return read_name()
    return name


rnd = read_round()
name = read_name()
foldername = re.sub('[^a-zA-Z0-9-\+]+', '', name.lower().replace(' ', '-'))

folder = f"{rnd}-{foldername}"
readme = f"""# Codeforces {rnd} - {name}

Unattempted
"""

print(f"Problem folder: {folder}")

os.makedirs(folder, exist_ok=True)

readmefile = open(f"{folder}/README.md", "w")
readmefile.write(readme)
readmefile.close()

if template == "cpp":
    shutil.copy("templates/cpp/code.cpp", folder)
    shutil.copy("templates/cpp/input.txt", folder)
    os.symlink("../templates/cpp/Makefile", f"{folder}/Makefile")
    subprocess.call([
        "code", f"{folder}/README.md", f"{folder}/code.cpp",
        f"{folder}/input.txt"
    ])
