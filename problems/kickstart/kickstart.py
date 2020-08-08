#!/usr/bin/python3

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
        return input(str).strip()
    except EOFError as e:
        sys.exit(0)


def read_year():
    year = tryinput("Problem Year?  ex: 2020\n> ")
    if not re.match("^20[0-9]{2}$", year):
        print(f"Bad input year: {year}")
        return read_year()
    return year


def read_round():
    rnd = tryinput("Problem Round?  ex: B\n> ")
    if not re.match("^[A-Z1-9]{1,2}$", rnd):
        print(f"Bad input round: {rnd}")
        return read_round()
    return rnd


def read_name():
    name = tryinput("Problem Name?  ex: le-problemo\n> ")
    if not re.match("^[a-zA-Z0-9-]+$", name):
        print(f"Bad input name: {name}")
        return read_name()
    return name


def read_friendly():
    friendly = tryinput("Problem Friendly Name?  ex: Le Problemo\n> ")
    if not re.match("^[a-zA-Z0-9-_!?#)(=~+\-*/.:,; ]+$", friendly):
        print(f"Bad input friendly name: {friendly}")
        return read_friendly()
    return friendly


def read_points(index):
    pts = tryinput(f"Problem points #{index}?  ex: 10  (as in 10pts)\n> ")
    if not re.match("^[0-9]{1,2}$", pts):
        print(f"Bad input points: {pts}")
        return read_points(index)
    return pts


def read_link():
    link = tryinput("Problem URL?  (warning: not sanitized)\n> ")
    return link


year = read_year()
rnd = read_round()
name = read_name()
friendly = read_friendly()
pts1 = read_points(1)
pts2 = read_points(2)
link = read_link()

folder = f"{year}/{rnd}-{name}"
readme = f"""# Kickstart {year} - {friendly}

## [{friendly} ({pts1}pts, {pts2}pts)]({link})

Unattempted

* Time: 0 hours
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
    shutil.copy("templates/cpp/input.txt", folder)
    os.symlink("../../templates/cpp/Makefile",
               f"{folder}/Makefile")
    subprocess.call(["code",
                     f"{folder}/README.md",
                     f"{folder}/code.cpp",
                     f"{folder}/input.txt"
                     ])
