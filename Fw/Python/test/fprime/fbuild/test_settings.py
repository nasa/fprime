"""
(test) fprime.fbuild.settings:

Tests the F prime settings module.
@author joshuaa
"""

import pytest
import os
from pathlib import Path

from fprime.fbuild.settings import IniSettings

LOCAL_PATH = Path(__file__).parent


def full_path(path):
    path = LOCAL_PATH / Path(path)
    return path.resolve()


def test_settings():
    test_cases = [
        {
            "file": "nonexistent.ini",
            "expected": {
                "framework_path": full_path("../../../../.."),
                "install_dest": full_path("settings-data/build-artifacts"),
            },
        },
        {
            "file": "settings-empty.ini",
            "expected": {
                "settings_file": full_path("settings-data/settings-empty.ini"),
                "default_toolchain": "native",
                "framework_path": full_path("../../../../.."),
                "install_dest": full_path("settings-data/build-artifacts"),
                "library_locations": [],
                "environment_file": full_path("settings-data/settings-empty.ini"),
                "environment": {},
            },
        },
        {
            "file": "settings-custom-install.ini",
            "expected": {
                "settings_file": full_path("settings-data/settings-custom-install.ini"),
                "default_toolchain": "native",
                "framework_path": full_path("../../../../.."),
                "install_dest": full_path("test"),
                "library_locations": [],
                "environment_file": full_path(
                    "settings-data/settings-custom-install.ini"
                ),
                "environment": {},
            },
        },
    ]

    for case in test_cases:
        fp = full_path("settings-data/" + case["file"])
        print(fp)
        results = IniSettings.load(fp, LOCAL_PATH)
        assert case["expected"] == results, "{}: Expected {}, got {}".format(
            fp, case["expected"], results
        )
