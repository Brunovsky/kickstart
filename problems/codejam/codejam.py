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


def read_year():
    year = tryinput("Year: ")
    if len(year) == 0:
        return datetime.now().year
    if not re.match("^20[0-9]{2}|tmp$", year):
        print(f"Bad input year: {year}")
        return read_year()
    return year


def read_round():
    rnd = tryinput("Round: ")
    if not re.match("^[a-zA-Z1-9]{1,2}$", rnd):
        print(f"Bad input round: {rnd}")
        return read_round()
    return rnd.upper()


def read_folder():
    folder = tryinput("Folder: ")
    if not re.match("^[a-zA-Z0-9-\+]+$", folder):
        print(f"Bad input folder: {folder}")
        return read_folder()
    return folder


def read_name():
    name = tryinput("Name: ")
    if not re.match("^[a-zA-Z0-9-_!?#)(=~+\-*/.:,; ]+$", name):
        print(f"Bad input name name: {name}")
        return read_name()
    return name


def read_points(index, need=False):
    pts = tryinput(f"Points #{index}: ")
    if not need and len(pts) == 0:
        return ""
    if not re.match("^[0-9]+$", pts):
        print(f"Bad input points: {pts}")
        return read_points(index)
    return pts


def read_all_points():
    s, i = f"{read_points(1, True)}pts", 2
    pts = read_points(i)
    while len(pts) > 0:
        s, i, pts = s + f", {pts}pts", i + 1, read_points(i + 1)
    return s


def read_link():
    link = tryinput("URL: ")
    return link


year = read_year()
rnd = read_round()
foldername = read_folder()
name = read_name()
pts = read_all_points()
link = read_link()

folder = f"{year}/{rnd}-{foldername}"
readme = f"""# CodeJam {year} - {name}

## [{name} ({pts})]({link})

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
    os.symlink("../../templates/cpp/Makefile", f"{folder}/Makefile")
    subprocess.call([
        "code", f"{folder}/README.md", f"{folder}/code.cpp",
        f"{folder}/input.txt"
    ])

if template == "cpp-interactive":
    shutil.copy("templates/cpp-interactive/code.cpp", folder)
    shutil.copy("templates/cpp-interactive/run.sh", folder)
    os.symlink("../../templates/cpp-interactive/Makefile",
               f"{folder}/Makefile")
    os.symlink("../../templates/cpp-interactive/interactive_runner.py",
               f"{folder}/interactive_runner.py")
    print("Don't forget to add the testing tool (testing_tool.py)")
    subprocess.call([
        "code", f"{folder}/README.md", f"{folder}/code.cpp", f"{folder}/run.sh"
    ])
