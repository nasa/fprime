#!/usr/bin/env python3
"""At website runtime, git clone tutorials markdown files into fprime/docs/tutorials.
This was initially written for Zensical integration.
"""

from __future__ import annotations

import shutil
import subprocess
from pathlib import Path


def tutorials() -> None:
    # Get the tutorials directory (fprime/docs/tutorials/)
    script_dir = Path(__file__).parent

    # Define the repository URLs and their target directories
    repos = {
        "tutorials-hello-world": "https://github.com/fprime-community/fprime-tutorial-hello-world.git",
        "tutorials-led-blinker": "https://github.com/fprime-community/fprime-workshop-led-blinker.git",
        "tutorials-math-component": "https://github.com/fprime-community/fprime-tutorial-math-component.git",
        "tutorials-arduino-led-blinker": "https://github.com/fprime-community/fprime-tutorial-arduino-blinker.git",
    }

    for target_dir, repo_url in repos.items():
        target_path = script_dir / target_dir

        # Remove existing directory if it exists
        if target_path.exists():
            shutil.rmtree(target_path)

        # Clone the repository
        print(f"Cloning {repo_url} into {target_path}...")
        subprocess.run(["git", "clone", repo_url, str(target_path)], check=True)

        # Remove the .git directory to avoid nested repo issues with mike
        git_dir = target_path / ".git"
        if git_dir.exists():
            shutil.rmtree(git_dir)
            print(f"Removed .git directory from {target_path}")


if __name__ == "__main__":
    tutorials()
