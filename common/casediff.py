#!/usr/bin/python3

import sys
import re
import argparse

CASE_PATTERN = re.compile(r'^Case #(\d+):\s*')
DEFAULT_EPSILON = 0
RED, GREEN, YELLOW, COLOR_RESET = '\033[1;31m', '\033[1;32m', '\033[1;34m', '\033[0m'


class Errors:
    def bad_first_line(file):
        print("{}: first line does not match case pattern".format(file))
        sys.exit(0)

    def file_not_found(txt):
        print("no such file: {}".format(txt))
        sys.exit(0)

    def bad_caseno(file, lineno, caseno, expected_caseno):
        print("{}:{}: expected case #{} but got #{}".format(
            file, lineno, expected_caseno, caseno))
        sys.exit(0)

    def different_casenum(a_file, a_casenos, b_file, b_casenos):
        print("different number of cases: {} ({}) vs {} ({})".format(
            a_casenos, a_file, b_casenos, b_file))
        sys.exit(0)

    def no_cases(file_a, file_b):
        print("no cases found in either {} or {}".format(file_a, file_b))
        sys.exit(0)


class WordSplitter:
    def __init__(self, line):
        self.words = re.split(r'(\S+)', line)
        self.i = -1
        self.n = len(self.words) // 2
        assert len(self.words) % 2 == 1

    def trailing_space(self):
        return self.words[-1]

    def __iter__(self):
        return self

    def __next__(self):
        self.i += 1
        if self.i >= self.n:
            self.i = -1
            raise StopIteration
        else:
            return (self.words[2 * self.i], self.words[2 * self.i + 1])


def parse_args():
    parser = argparse.ArgumentParser(
        description='Compare two kickstart/codejam output casefiles, side by side',
        epilog='Exit status is always 0 unless there is trouble.\n'
               'By default each case should match numerically.\n'
               'There must be an equal number of cases, ordered 1 to N.\n'
               'Diffs are coloured by default.',
        allow_abbrev=False,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    files = parser.add_argument_group('Input files')
    files.add_argument('program_txt',
                       nargs='?', default='program.txt',
                       help='(LHS) Casefile with tentative answers')
    files.add_argument('correct_txt',
                       nargs='?', default='correct.txt',
                       help='(RHS) Casefile with presumably correct answers')

    comp = parser.add_argument_group('Comparison')
    comp.add_argument('-s', '--equal-header',
                      action='store_true', default=False,
                      help='Answers considered equal if headers match')
    comp.add_argument('--equal-body',
                      action='store_false', dest='equal_header',
                      help='Answers considered equal if headers and entire case bodies match')

    comp.add_argument('--exact',
                      action='store_const', const='exact', dest='method', default='numeric',
                      help='Compare lines exactly, byte by byte')
    comp.add_argument('--token', '--tokenize',
                      action='store_const', const='token', dest='method',
                      help='Compare lines token by token, textually (byte by byte)')
    comp.add_argument('--numeric',
                      action='store_const', const='numeric', dest='method',
                      help='Compare lines token by token, numerically if possible')
    comp.add_argument('-e', '--epsilon',
                      type=float, default=DEFAULT_EPSILON,
                      help='Maximum absolute relative error for numerical comparisons')

    fmt = parser.add_argument_group('Formatting')
    fmt.add_argument('-c', '--color-header',
                     action='store_true', default=True,
                     help='Color case header diffs')
    fmt.add_argument('-b', '--color-body',
                     action='store_true', default=True,
                     help='Color case body diffs')
    fmt.add_argument('--no-color-header',
                     action='store_false', dest='color_header')
    fmt.add_argument('--no-color-body',
                     action='store_false', dest='color_body')

    args = parser.parse_args()
    args.exact = args.method == 'exact'
    args.tokenize = args.method == 'token'
    args.numeric = args.method == 'numeric'
    return args


def read_cases(filename):
    caseno, headers, bodies, linenos = 0, [], [], []

    try:
        with open(filename, "r") as file:
            for lineno, line in enumerate(file):
                line = line[:-1] if len(line) > 0 and line[-1] == '\n' else line
                match = CASE_PATTERN.match(line)
                if match:
                    caseno += 1
                    actual_caseno = int(match.group(1))
                    if actual_caseno != caseno:
                        Errors.bad_caseno(filename, lineno, actual_caseno, caseno)
                    prefix_len = len(match[0])
                    bodies.append([line])
                    headers.append(line[prefix_len:])
                    linenos.append(lineno + 1)
                elif caseno == 0:
                    Errors.bad_first_line(filename, line)
                else:
                    bodies[-1].append(line)
    except:
        Errors.file_not_found(filename)

    return [(headers[i], bodies[i], linenos[i]) for i in range(caseno)]


def is_num(s):
    try:
        x = float(s)
        return True
    except:
        return False


def equal_numeric(a, b, epsilon=0):
    return a == b or is_num(a) and is_num(b) and abs(float(a) - float(b)) <= epsilon


def equal_lines_word(aline, bline):
    a_words, b_words = WordSplitter(aline), WordSplitter(bline)
    return a_words.n == b_words.n and \
        all(a[1] == b[1] for a, b in zip(a_words, b_words))


def equal_lines_numeric(aline, bline, epsilon=0):
    a_words, b_words = WordSplitter(aline), WordSplitter(bline)
    return a_words.n == b_words.n and \
        all(equal_numeric(a[1], b[1], epsilon) for a, b in zip(a_words, b_words))


def equal_lines(aline, bline, opts):
    if opts.numeric:
        return equal_lines_numeric(aline, bline, opts.epsilon)
    elif opts.tokenize:
        return equal_lines_word(aline, bline)
    else:
        aline == bline


def format_difflines_text(aline, bline):
    ok, a_diff, b_diff = True, "", ""
    for a_char, b_char in zip(aline, bline):
        if a_char == b_char and not ok:
            a_diff += COLOR_RESET
            b_diff += COLOR_RESET
        elif a_char != b_char and ok:
            a_diff += YELLOW
            b_diff += YELLOW
        a_diff += a_char
        b_diff += b_char
        ok = a_char == b_char
    if not ok:
        a_diff += COLOR_RESET
        b_diff += COLOR_RESET
    if len(aline) > len(bline):
        a_diff += RED + aline[len(bline):] + COLOR_RESET
    if len(aline) < len(bline):
        b_diff += GREEN + bline[len(aline):] + COLOR_RESET
    return a_diff, b_diff


def format_difflines_word(aline, bline):
    a_diff, b_diff = "", ""
    a_words, b_words = WordSplitter(aline), WordSplitter(bline)
    for (a_space, a_word), (b_space, b_word) in zip(a_words, b_words):
        if a_word == b_word:
            a_diff += a_space + a_word
            b_diff += b_space + b_word
        else:
            a_wordiff, b_wordiff = format_difflines_text(a_word, b_word)
            a_diff += a_space + a_wordiff
            b_diff += b_space + b_wordiff
    if a_words.n > b_words.n:
        for (a_space, a_word) in a_words:
            a_diff += a_space + RED + a_word + COLOR_RESET
    elif a_words.n < b_words.n:
        for (b_space, b_word) in b_words:
            b_diff += b_space + GREEN + b_word + COLOR_RESET
    a_diff += a_words.trailing_space()
    b_diff += b_words.trailing_space()
    return a_diff, b_diff


def format_difflines_numeric(aline, bline, epsilon):
    a_diff, b_diff = "", ""
    a_words, b_words = WordSplitter(aline), WordSplitter(bline)
    for (a_space, a_word), (b_space, b_word) in zip(a_words, b_words):
        if equal_numeric(a_word, b_word, epsilon):
            a_diff += a_space + a_word
            b_diff += b_space + b_word
        elif is_num(a_word) or is_num(b_word):
            a_diff += a_space + YELLOW + a_word + COLOR_RESET
            b_diff += b_space + YELLOW + b_word + COLOR_RESET
        else:
            a_wordiff, b_wordiff = format_difflines_text(a_word, b_word)
            a_diff += a_space + a_wordiff
            b_diff += b_space + b_wordiff
    if a_words.n > b_words.n:
        for (a_space, a_word) in a_words:
            a_diff += a_space + RED + a_word + COLOR_RESET
    elif a_words.n < b_words.n:
        for (b_space, b_word) in b_words:
            b_diff += b_space + GREEN + b_word + COLOR_RESET
    a_diff += a_words.trailing_space()
    b_diff += b_words.trailing_space()
    return a_diff, b_diff


def format_diffline(aline, bline, is_header, opts):
    if is_header and opts.color_header or not is_header and opts.color_body:
        if opts.numeric:
            return format_difflines_numeric(aline, bline, opts.epsilon)
        elif opts.tokenize:
            return format_difflines_word(aline, bline)
        else:
            return format_difflines_text(aline, bline)
    else:
        return aline, bline


def format_extraline(line, is_header, color, opts):
    if is_header and opts.color_header or not is_header and opts.color_body:
        return color + line + COLOR_RESET
    else:
        return line


class Diffline:
    def __init__(self, caseno, i, linenos, lines, diffs):
        self.caseno = caseno
        self.index = i
        self.lineno = [(n + i) if n >= 0 else 0 for n in linenos]
        self.line = lines
        self.diff = diffs
        self.width = (len(lines[0]), len(lines[1]))


def make_diff(caseno, a, b, opts):
    a_header, alines, alineno = a
    b_header, blines, blineno = b

    if opts.equal_header and equal_lines(a_header, b_header, opts) or \
            not opts.equal_header and len(alines) == len(blines) and \
            all(equal_lines(aline, bline, opts) for aline, bline in zip(alines, blines)):
        return []

    diffs = [Diffline(caseno, i, (alineno, blineno), (aline, bline),
                      format_diffline(aline, bline, i == 0, opts))
             for i, (aline, bline) in enumerate(zip(alines, blines))]

    if len(alines) > len(blines):
        diffs += [Diffline(caseno, i + len(blines), (alineno, -1), (aline, ""),
                           (format_extraline(aline, False, RED, opts), ""))
                  for i, aline in enumerate(alines[len(blines):])]
    elif len(alines) < len(blines):
        diffs += [Diffline(caseno, i + len(alines), (-1, blineno), ("", bline),
                           ("", format_extraline(bline, False, GREEN, opts)))
                  for i, bline in enumerate(blines[len(alines):])]

    return diffs


def print_difflines(diffs):
    if len(diffs) == 0:
        return

    lineno_align = [max(len(str(diff.lineno[s])) for diff in diffs) for s in range(2)]
    text_align = [max(diff.width[0] for diff in diffs), 0]
    pad = " " * (lineno_align[0] + 2 + text_align[0])

    def build(diff, s):
        spaces = text_align[s] - diff.width[s]
        return "{1:>{0}}: {3}{2}".format(lineno_align[s], diff.lineno[s],
                                         " " * spaces, diff.diff[s])

    for diff in diffs:
        if diff.width[0] and diff.width[1]:
            print("{}      {}".format(build(diff, 0), build(diff, 1)))
        elif diff.width[1]:
            print("{}      {}".format(pad, build(diff, 1)))
        else:
            print("{}".format(build(diff, 0)))


def run_casediff():
    opts = parse_args()
    a_file, b_file = opts.program_txt, opts.correct_txt
    a_cases, b_cases = read_cases(a_file), read_cases(b_file)
    a_casenos, b_casenos = len(a_cases), len(b_cases)

    if a_casenos != b_casenos:
        Errors.different_casenum(a_file, a_casenos, b_file, b_casenos)
    if a_casenos == 0:
        Errors.no_cases(a_file, b_file)

    good, diffs = 0, []
    for i, (a, b) in enumerate(zip(a_cases, b_cases)):
        case_diffs = make_diff(i + 1, a, b, opts)
        good += 1 if len(case_diffs) == 0 else 0
        diffs += case_diffs

    percentage = 100 * good / a_casenos
    print_difflines(diffs)
    print("Correct: {}/{} ({:.1f}%)".format(good, a_casenos, percentage))


if __name__ == "__main__":
    run_casediff()
