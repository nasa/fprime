"""
Shared pytest fixtures for F' Svc integration tests.

This module provides common fixtures for setting up test files
that are required by multiple integration test suites.
"""

import shutil
import pytest
from pathlib import Path


@pytest.fixture(scope="session", autouse=True)
def setup_test_files():
    """
    Copy test files to /tmp/ for use by integration tests.

    Many integration tests (FileManager, FileDownlink, etc.) expect
    test files to exist in /tmp/ on the target filesystem.
    """
    # Source directory containing test files
    source_dir = Path(__file__).parent / "FileUplink" / "test" / "int"

    test_files = [
        "test_seq.seq",
        "test_seq_wait.seq",
        "1MiB.txt",
    ]

    # Copy files to /tmp/ if they don't exist or are different
    for filename in test_files:
        source = source_dir / filename
        dest = Path("/tmp") / filename

        if source.exists():
            try:
                shutil.copy2(source, dest)
                print(f"Copied {source} -> {dest}")
            except (IOError, PermissionError) as e:
                print(f"Warning: Could not copy {filename} to /tmp/: {e}")

    yield

    # Cleanup is optional - comment out if you want files to persist
    # for filename in test_files:
    #     dest = Path("/tmp") / filename
    #     if dest.exists():
    #         dest.unlink()
