#!/usr/bin/python

import re
import sys
import os
import shutil

template = sys.argv[1]


def read_difficulty():
    difficulty = input("Problem Difficulty?  ex: medium\n> ")
    if not re.match("[a-z]+", difficulty):
        print("Bad input difficulty: %s", difficulty)
        return read_difficulty()
    return difficulty


def read_name():
    name = input("Problem Name?  ex: le-problemo\n> ")
    if not re.match("[a-zA-Z0-9-]+", name):
        print("Bad input name: %s", name)
        return read_name()
    return name


def read_friendly():
    friendly = input("Problem Friendly Name?  ex: DD. Le Problemo\n> ")
    if not re.match("[a-zA-Z0-9-_!?#)(=~+\-*/.:,; ]", friendly):
        print("Bad input friendly name: %s", friendly)
        return read_friendly()
    return friendly


def read_link():
    link = input("Problem URL?\n> ")
    return link


difficulty = read_difficulty()
name = read_name()
friendly = read_friendly()
link = read_link()
competition = f"leet/{difficulty}"
problem = f"{name}"
header = f"# LeetCode - {difficulty}\n\n## [{friendly}]({link})"
folder = f"problems/{competition}/{problem}"

os.makedirs(folder, exist_ok=True)

readme = open(f"{folder}/README.md", "w")
readme.write(header)
readme.close()

# Copy template
if template == "leet-cpp":
    shutil.copy("templates/leet-cpp/code.cpp", folder)
    os.symlink("../../../../templates/leet-cpp/Makefile", f"{folder}/Makefile")
