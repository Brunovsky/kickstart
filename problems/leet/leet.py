#!/usr/bin/python

import re
import sys
import os
import shutil
import subprocess
import signal

template = sys.argv[1]


def signal_handler(sig, frame):
    sys.exit(0)


signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)


def tryinput(str):
    try:
        return input(str)
    except EOFError as e:
        sys.exit(0)


def read_difficulty():
    difficulty = tryinput("Problem Difficulty?  ex: medium\n> ")
    if not re.match("easy|medium|hard", difficulty):
        print(f"Bad input difficulty: {difficulty}")
        return read_difficulty()
    return difficulty


def read_number():
    number = tryinput("Problem Number?  ex: 123\n> ")
    if not re.match("[0-9]+", number):
        print(f"Bad input number: {number}")
        return read_number()
    return number


def read_name():
    name = tryinput("Problem Name?  ex: le-problemo\n> ")
    if not re.match("[a-zA-Z0-9-]+", name):
        print(f"Bad input name: {name}")
        return read_name()
    return name


def read_friendly():
    friendly = tryinput("Problem Friendly Name?  ex: Le Problemo\n> ")
    if not re.match("[a-zA-Z0-9-_!?#)(=~+\-*/.:,; ]", friendly):
        print(f"Bad input friendly name: {friendly}")
        return read_friendly()
    return friendly


def read_link(name):
    default = f"https://leetcode.com/problems/{name}"
    link = tryinput(f"Problem URL?  ({default})\n> ")
    return link if link else default


difficulty = read_difficulty()
number = read_number()
name = read_name()
friendly = read_friendly()
link = read_link(name)

folder = f"{number}-{name}"
readme = f"""# LeetCode - {difficulty}

## [{number}. {friendly}]({link})

Unattempted

* Complexity: -
* Memory: -
"""

print(f"Problem folder: {folder}")

os.makedirs(folder, exist_ok=True)

readmefile = open(f"{folder}/README.md", "w")
readmefile.write(readme)
readmefile.close()

if template == "cpp":
    shutil.copy("templates/cpp/code.cpp", folder)
    os.symlink("../templates/cpp/Makefile", f"{folder}/Makefile")
    subprocess.call(["code",
                     f"{folder}/code.cpp"
                     ])

if template == "shell":
    shutil.copy("templates/shell/code.sh", folder)
    os.symlink("../templates/shell/Makefile", f"{folder}/Makefile")
    subprocess.call(["code",
                     f"{folder}/code.sh"
                     ])

if template == "js":
    shutil.copy("templates/js/code.js", folder)
    os.symlink("../templates/js/Makefile", f"{folder}/Makefile")
    subprocess.call(["code",
                     f"{folder}/code.js"
                     ])
