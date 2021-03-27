import sys
import re


class Errors:
    def bad_caseno(filename, lineno, caseno, expected):
        print("{}:{}: expected case #{} but got #{}".format(
            filename, lineno, expected, caseno))
        sys.exit(0)

    def different_casenum(fileactual, acases, fileexpected, ecases):
        print("expected {} cases (in {}), but got {} in {}".format(
            ecases, fileexpected, acases, fileactual))
        sys.exit(0)

    def no_cases(program_txt, correct_txt):
        print("no cases found in either {} or {}".format(
            program_txt, correct_txt))
        sys.exit(0)

    def file_not_found(txt):
        print("no such file: {}".format(txt))
        sys.exit(0)


class Options:
    compare_header_only = False
    color_header = False

    def parse(argv):
        Options.compare_header_only = 0 < argv.count("--compare-header")
        Options.color_header = 0 < argv.count("--color-header")


class Diffline:
    def __init__(self, caseno, i, alineno, blineno, text_a, text_b, aline, bline):
        self.caseno = caseno
        self.index = i
        self.lineno = [(alineno + i) if alineno else 0, (blineno + i) if blineno else 0]
        self.text = (text_a, text_b)
        self.original = (aline, bline)
        self.width = (len(aline), len(bline))


def read_cases(filename):
    case_pattern = re.compile("^Case #(\d+):")
    caseno, case_body, case_lineno = 0, [], []

    try:
        with open(filename, "r") as file:
            for lineno, line in enumerate(file):
                if line[-1] == '\n':
                    line = line[:-1]
                match = case_pattern.match(line)
                if match:
                    caseno += 1
                    if int(match.group(1)) != caseno:
                        Errors.bad_caseno(filename, lineno, int(match.group(1)), caseno)
                    case_body.append([line])
                    case_lineno.append(lineno)
                else:
                    case_body[-1].append(line)
    except:
        Errors.file_not_found(filename)

    return case_body, case_lineno


def make_difflines(caseno, a, b, alineno, blineno):
    RED, GREEN, YELLOW, END = '\033[1;31m', '\033[1;32m', '\033[1;34m', '\033[0m'
    diffs = []

    for i, (aline, bline) in enumerate(zip(a, b)):
        if aline == bline:
            continue
        if i == 0 and not Options.color_header:
            diff = Diffline(caseno, i, alineno, blineno, aline, bline, aline, bline)
            diffs.append(diff)
            continue

        ok, text_a, text_b = True, "", ""

        for ca, cb in zip(aline, bline):
            if ca == cb and not ok:
                text_a += END
                text_b += END
            elif ca != cb and ok:
                text_a += YELLOW
                text_b += YELLOW
            text_a += ca
            text_b += cb
            ok = ca == cb
        if not ok:
            text_a += END
            text_b += END
        if len(aline) > len(bline):
            text_a += RED + aline[len(bline):] + END
        if len(aline) < len(bline):
            text_b += GREEN + bline[len(aline):] + END

        diff = Diffline(caseno, i, alineno, blineno, text_a, text_b, aline, bline)
        diffs.append(diff)

    for i in range(len(b), len(a)):
        aline = a[i]
        text_a = RED + aline + END
        diff = Diffline(caseno, i, alineno, 0, text_a, "", aline, "")
        diffs.append(diff)

    for i in range(len(a), len(b)):
        bline = b[i]
        text_b = GREEN + bline + END
        diff = Diffline(caseno, i, 0, blineno, "", text_b, "", bline)
        diffs.append(diff)

    return diffs


def print_diffs(diffs):
    if len(diffs) == 0:
        return

    lineno_align = [max(len(str(diff.lineno[s])) for diff in diffs) for s in (0, 1)]
    text_align = [max(diff.width[0] for diff in diffs), 0]
    pad = " " * (lineno_align[0] + 2 + text_align[0])

    def build(diff, s):
        spaces = text_align[s] - diff.width[s]
        return "{1:>{0}}: {3}{2}".format(lineno_align[s], diff.lineno[s],
                                         " " * spaces, diff.text[s])

    for diff in diffs:
        if diff.text[0] and diff.text[1]:
            print("{}      {}".format(build(diff, 0), build(diff, 1)))
        elif diff.text[1]:
            print("{}      {}".format(pad, build(diff, 1)))
        else:
            print("{}".format(build(diff, 0)))


def main():
    program_txt, correct_txt, rest_argv = sys.argv[1], sys.argv[2], sys.argv[3:]
    Options.parse(rest_argv)

    program_body, program_lineno = read_cases(program_txt)
    correct_body, correct_lineno = read_cases(correct_txt)
    casenos = len(correct_lineno)

    if len(program_lineno) != casenos:
        Errors.different_casenum(program_txt, len(program_lineno), correct_txt, casenos)
    if casenos == 0:
        Errors.no_cases(program_txt, correct_txt)

    def cases_match(i):
        if Options.compare_header_only:
            return program_body[i][0] == correct_body[i][0]
        else:
            return program_body[i] == correct_body[i]

    good, diffs = 0, []
    for i in range(casenos):
        if cases_match(i):
            good += 1
            continue
        diffs += make_difflines(i + 1, program_body[i], correct_body[i],
                                program_lineno[i], correct_lineno[i])

    print_diffs(diffs)
    print("Correct: {}/{} ({:.1f}%)".format(good, casenos, 100 * good / casenos))


if __name__ == "__main__":
    main()
