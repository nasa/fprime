#!/usr/bin/env python3

# Convert output from Google's cpplint to the cppcheck XML format
# Reads from stdin and writes to stderr (to mimic cppcheck)
# https://stackoverflow.com/questions/14172232/how-to-make-cpplint-work-with-jenkins-warnings-plugin

import sys
import re
import xml.sax.saxutils


def cpplint_score_to_cppcheck_severity(err_score):
    if err_score in [1, 2]:
        return "style"
    if err_score in [3, 4]:
        return "warning"
    if err_score == 5:
        return "error"


def fmt_report_from_cpplint_to_cppcheck():
    sys.stderr.write("""<?xml version="1.0" encoding="UTF-8"?>\n""")
    sys.stderr.write("""<results version="2">\n""")
    sys.stderr.write("""<cppcheck version="1.90"/>\n""")
    sys.stderr.write("""<errors>\n""")

    compiled_regex = re.compile(
        "([^:]*):([0-9]*):  ([^\[]*)\[([^\]]*)\] \[([0-9]*)\].*"
    )

    for line in sys.stdin.readlines():
        matched_regex = compiled_regex.match(line.strip())
        if not matched_regex:
            continue

        matched_subgroups = matched_regex.groups()
        if len(matched_subgroups) != 5:
            continue

        file_name, err_line, raw_err_msg, err_label, err_score = matched_subgroups
        # prepare data to be used as an attribute value
        err_msg = xml.sax.saxutils.escape(raw_err_msg)
        err_msg = xml.sax.saxutils.quoteattr(err_msg)

        err_severity = cpplint_score_to_cppcheck_severity(int(err_score))

        if err_severity in ["warning", "error"]:
            sys.stderr.write(
                f"""<error id="{err_label}" severity="{err_severity}" msg={err_msg} verbose="">\n"""
            )
            sys.stderr.write(
                f"""<location file="{file_name}" line="{err_line}" column="0"/>\n"""
            )
            sys.stderr.write("""</error>\n""")

    sys.stderr.write("""</errors>\n""")
    sys.stderr.write("""</results>\n""")


if __name__ == "__main__":
    fmt_report_from_cpplint_to_cppcheck()
