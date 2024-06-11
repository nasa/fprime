#!/usr/bin/env python3

# Convert output from Google's cpplint to the cppcheck XML format
# Reads from stdin and writes to stderr (to mimic cppcheck)
# https://stackoverflow.com/questions/14172232/how-to-make-cpplint-work-with-jenkins-warnings-plugin

import re
import sys
import xml.sax.saxutils

OUTPUT_FILE_XML_HEADER = """<?xml version="1.0" encoding="UTF-8"?>
<results version="2">
<cppcheck version="1.90"/>
<errors>
"""

OUTPUT_FILE_XML_FOOTER = """</errors>
</results>
"""

CPPLINT_ERR_REGEX = "([^:]*):([0-9]*):  ([^\[]*)\[([^\]]*)\] \[([0-9]*)\].*"
CPPLINT_ERR_REGEX_NB_INFO = 5


def cpplint_score_to_cppcheck_severity(err_score: int) -> str:
    if err_score in {1, 2}:
        return "style"
    if err_score in {3, 4}:
        return "warning"
    return "error" if err_score == 5 else ""


def write_if_relevant_error(
    file_name: str, err_severity: str, err_label: str, err_msg: str, err_line: str
) -> None:
    if err_severity in {"warning", "error"}:
        sys.stderr.write(
            f"""<error id="{err_label}" severity="{err_severity}" msg={err_msg} verbose="">\n"""
        )
        sys.stderr.write(
            f"""<location file="{file_name}" line="{err_line}" column="0"/>\n"""
        )
        sys.stderr.write("""</error>\n""")


def fmt_report_from_cpplint_to_cppcheck() -> None:
    sys.stderr.write(OUTPUT_FILE_XML_HEADER)
    compiled_regex = re.compile(CPPLINT_ERR_REGEX)

    for line in sys.stdin.readlines():
        matched_regex = compiled_regex.match(line.strip())
        if not matched_regex:
            continue

        matched_subgroups = matched_regex.groups()
        if len(matched_subgroups) != CPPLINT_ERR_REGEX_NB_INFO:
            continue

        file_name, err_line, raw_err_msg, err_label, err_score = matched_subgroups
        # Prepare the data to be used as attribute values
        err_msg = xml.sax.saxutils.escape(raw_err_msg)
        err_msg = xml.sax.saxutils.quoteattr(err_msg)

        err_severity = cpplint_score_to_cppcheck_severity(int(err_score))

        write_if_relevant_error(file_name, err_severity, err_label, err_msg, err_line)

    sys.stderr.write(OUTPUT_FILE_XML_FOOTER)


if __name__ == "__main__":
    fmt_report_from_cpplint_to_cppcheck()
