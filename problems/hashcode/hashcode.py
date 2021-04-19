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
    year = tryinput("Year: ")
    if len(year) == 0:
        return datetime.now().year
    if not re.match("^20[0-9]{2}|tmp$", year):
        print(f"Bad input year: {year}")
        return read_year()
    return year


def read_name():
    name = tryinput("Name: ")
    if not re.match("^[a-zA-Z0-9-_!?#)(=~+\-*/.:,; ]+$", name):
        print(f"Bad input name name: {name}")
        return read_name()
    return name


year = read_year()
name = read_name()
foldername = re.sub('[^a-zA-Z0-9-\+]+', '', name.lower().replace(' ', '-'))

folder = f"{year}-{foldername}"
readme = f"""# Hashcode {year}

## {name}

Unattempted
"""

print(f"Problem folder: {folder}")

os.makedirs(folder, exist_ok=True)

readmefile = open(f"{folder}/README.md", "w")
readmefile.write(readme)
readmefile.close()

if template == "cpp":
    shutil.copy("templates/cpp/code.cpp", folder)
    shutil.copy("templates/cpp/run.sh", folder)
    os.symlink("../templates/cpp/Makefile", f"{folder}/Makefile")
    subprocess.call([
        "code",
        f"{folder}/README.md",
        f"{folder}/code.cpp",
        f"{folder}/run.sh",
    ])
