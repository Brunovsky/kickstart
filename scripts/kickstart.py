#!/usr/bin/python

import re
import sys
import os
import shutil

template = sys.argv[1]


def read_year():
    year = input("Problem Year?  ex: 2020\n> ")
    if not re.match("20[0-9]{2}", year):
        print("Bad input year: %s", year)
        return read_year()
    return year


def read_round():
    rnd = input("Problem Round?  ex: B\n> ")
    if not re.match("[A-Z]", rnd):
        print("Bad input round: %s", rnd)
        return read_round()
    return rnd


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


def read_points(index):
    pts = input(f"Problem points #{index}?  ex: 10  (as in 10pts)\n> ")
    if not re.match("[0-9]{1,2}", pts):
        print("Bad input points: %s", pts)
        return read_points()
    return pts


def read_link():
    link = input("Problem URL?  (warning: not sanitized)\n> ")
    return link


year = read_year()
rnd = read_round()
name = read_name()
friendly = read_friendly()
pts1 = read_points(1)
pts2 = read_points(2)
link = read_link()
competition = f"kickstart/{year}"
problem = f"{rnd}-{name}"
readme = f"""# Kickstart {year} - {friendly}

## [{friendly} ({pts1}pts, {pts2}pts)]({link})

Unattempted

* Time: 0 hours
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
    shutil.copy("templates/kickstart-cpp/code.cpp", folder)
    shutil.copy("templates/kickstart-cpp/input.txt", folder)
    os.symlink("../../../../templates/kickstart-cpp/Makefile",
               f"{folder}/Makefile")
