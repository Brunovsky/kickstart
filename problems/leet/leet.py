#!/usr/bin/python

import re
import sys
import os
import shutil
import subprocess
import signal

template = sys.argv[1]
signal.signal(signal.SIGINT, lambda sig, frame: sys.exit(0))
signal.signal(signal.SIGTERM, lambda sig, frame: sys.exit(0))


def tryinput(str):
    try:
        return input(str).strip()
    except EOFError as e:
        sys.exit(0)


def read_difficulty():
    difficulty = tryinput("Difficulty: ")
    if not re.match("^easy|medium|hard$", difficulty, flags=re.IGNORECASE):
        print(f"Bad input difficulty: {difficulty}")
        return read_difficulty()
    return difficulty.lower()


def read_number():
    number = tryinput("Number: ")
    if not re.match("^[0-9]+$", number):
        print(f"Bad input number: {number}")
        return read_number()
    return number


def read_name():
    name = tryinput("Name: ")
    if not re.match("^[a-zA-Z0-9-_!?#)(=~+\-*/.:,; ]+$", name):
        print(f"Bad input name name: {name}")
        return read_name()
    return name


def read_link(foldername):
    default = f"https://leetcode.com/problems/{foldername}"
    link = tryinput("URL: ")
    return link if link else default


difficulty = read_difficulty()
number = read_number()
name = read_name()
foldername = name.lower().replace(' ', '-')
link = read_link(foldername)

folder = f"{number}-{foldername}"
readme = f"""# LeetCode - {difficulty}

## [{number}. {name}]({link})

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
    subprocess.call(["code", f"{folder}/README.md", f"{folder}/code.cpp"])

if template == "shell":
    shutil.copy("templates/shell/code.sh", folder)
    os.symlink("../templates/shell/Makefile", f"{folder}/Makefile")
    subprocess.call([
        "code",
        f"{folder}/README.md",
        f"{folder}/code.sh",
    ])

if template == "js":
    shutil.copy("templates/js/code.js", folder)
    os.symlink("../templates/js/Makefile", f"{folder}/Makefile")
    subprocess.call(["code", f"{folder}/README.md", f"{folder}/code.js"])
