#!/bin/sh -e

# Set up virtual environment
rm -rf fprime-venv
python3 -m venv ./fprime-venv
. fprime-venv/bin/activate

# Install F Prime Python packages
pip install --upgrade wheel setuptools pip
pip install ./Fw/Python
pip install ./Gds
