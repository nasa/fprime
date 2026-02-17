"""Guard against reintroducing deprecated setuptools resource APIs."""

import subprocess
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]


def test_no_deprecated_setuptools_resource_api_usage():
    """Fail when deprecated setuptools resource APIs appear in tracked Python files."""
    deprecated_api = "_".join(["pkg", "resources"])
    result = subprocess.run(
        ["git", "grep", "-n", deprecated_api, "--", "*.py"],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )

    assert result.returncode in (0, 1), f"git grep failed:\n{result.stderr}"
    assert result.returncode == 1, (
        "Found deprecated setuptools resource API usage in Python files:\n"
        f"{result.stdout}"
    )
