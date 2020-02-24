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

def read_number():
    number = input("Problem Number?  ex: 123\n> ")
    if not re.match("[0-9]+", number):
        print("Bad input number: %s", number)
        return read_number()
    return number


def read_name():
    name = input("Problem Name?  ex: le-problemo\n> ")
    if not re.match("[a-zA-Z0-9-]+", name):
        print("Bad input name: %s", name)
        return read_name()
    return name


def read_friendly():
    friendly = input("Problem Friendly Name?  ex: Le Problemo\n> ")
    if not re.match("[a-zA-Z0-9-_!?#)(=~+\-*/.:,; ]", friendly):
        print("Bad input friendly name: %s", friendly)
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
competition = f"leet/{difficulty}"
problem = f"{number}-{name}"
header = f"# LeetCode - {difficulty}\n\n## [{number}. {friendly}]({link})"
folder = f"problems/{competition}/{problem}"

print(f"Problem folder: {folder}")

os.makedirs(folder, exist_ok=True)

readme = open(f"{folder}/README.md", "w")
readme.write(header)
readme.close()

# Copy template
if template == "leet-cpp":
    shutil.copy("templates/leet-cpp/code.cpp", folder)
    os.symlink("../../../../templates/leet-cpp/Makefile", f"{folder}/Makefile")
