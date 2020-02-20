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


year = read_year()
rnd = read_round()
name = read_name()
friendly = read_friendly()
competition = f"kickstart/{year}"
problem = f"{rnd}-{name}"
header = f"# Kickstart {year} - {friendly}\n\n## [{rnd} {friendly}]"
folder = f"problems/{competition}/{problem}"

os.makedirs(folder, exist_ok=True)

readme = open(f"{folder}/README.md", "w")
readme.write(header)
readme.close()

# Copy template
if template == "kickstart-cpp":
    shutil.copy("templates/kickstart-cpp/code.cpp", folder)
    shutil.copy("templates/kickstart-cpp/input.txt", folder)
    os.symlink("../../../../templates/kickstart-cpp/Makefile",
               f"{folder}/Makefile")
