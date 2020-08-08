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
    rnd = tryinput("Problem Round?  ex: QR\n> ")
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


def read_points():
    pts = tryinput(f"Problem points?  ex: 10pts, 12pts\n> ")
    if not re.match("^(?:[0-9]{1,2}pts,? ?){1,3}$", pts):
        print(f"Bad input points: {pts}")
        return read_points()
    return pts


def read_link():
    link = tryinput("Problem URL?  (warning: not sanitized)\n> ")
    return link


year = read_year()
rnd = read_round()
name = read_name()
friendly = read_friendly()
pts = read_points()
link = read_link()

folder = f"{year}/{rnd}-{name}"
readme = f"""# CodeJam {year} - {friendly}

## [{friendly} ({pts})]({link})

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

if template == "cpp-interactive":
    shutil.copy("templates/cpp-interactive/code.cpp", folder)
    shutil.copy("templates/cpp-interactive/run.sh", folder)
    shutil.copy("templates/cpp-interactive/interactive_runner.py", folder)
    os.symlink("../../templates/cpp-interactive/Makefile",
               f"{folder}/Makefile")
    print("Don't forget to add the testing tool (testing_tool.py)")
    subprocess.call(["code",
                     f"{folder}/README.md",
                     f"{folder}/code.cpp",
                     f"{folder}/run.sh"
                     ])
