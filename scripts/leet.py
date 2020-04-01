#!/usr/bin/python

import re
import sys
import os
import shutil
import subprocess

template = sys.argv[1]


def read_difficulty():
    difficulty = input("Problem Difficulty?  ex: medium\n> ")
    if not re.match("[a-z]+", difficulty):
        print("Bad input difficulty: {difficulty}")
        return read_difficulty()
    return difficulty


def read_number():
    number = input("Problem Number?  ex: 123\n> ")
    if not re.match("[0-9]+", number):
        print("Bad input number: {number}")
        return read_number()
    return number


def read_name():
    name = input("Problem Name?  ex: le-problemo\n> ")
    if not re.match("[a-zA-Z0-9-]+", name):
        print("Bad input name: {name}")
        return read_name()
    return name


def read_friendly():
    friendly = input("Problem Friendly Name?  ex: Le Problemo\n> ")
    if not re.match("[a-zA-Z0-9-_!?#)(=~+\-*/.:,; ]", friendly):
        print("Bad input friendly name: {friendly}")
        return read_friendly()
    return friendly


def read_link():
    link = input("Problem URL?  (warning: not sanitized)\n> ")
    return link


difficulty = read_difficulty()
number = read_number()
name = read_name()
friendly = read_friendly()
link = read_link()
competition = "leet"
problem = f"{number}-{name}"
readme = f"""# LeetCode - {difficulty}

## [{number}. {friendly}]({link})

Unattempted

* Complexity: -
* Memory: -
"""
folder = f"problems/{competition}/{problem}"

print(f"Problem folder: {folder}")

os.makedirs(folder, exist_ok=True)

readmefile = open(f"{folder}/README.md", "w")
readmefile.write(readme)
readmefile.close()

# Copy template
if template == "cpp":
    shutil.copy("templates/leet-cpp/code.cpp", folder)
    os.symlink("../../../templates/leet-cpp/Makefile", f"{folder}/Makefile")

if template == "shell":
    shutil.copy("templates/leet-shell/code.sh", folder)
    os.symlink("../../../templates/leet-shell/Makefile", f"{folder}/Makefile")

subprocess.call(["code", "--reuse-window", folder])
