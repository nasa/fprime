# F´ Python Guidelines

This guide describes the guidelines for developing Python code to be used with F´. This is done when contributing to the
F´ Python code and is recommended for project Python code as well.

## Required Development Installs

Setup the virtual environment per the install guide using the small additions below to get the packages F´ uses to
facilitate Python development.

```bash
pip install -e fprime/Fw/Python[dev]
pip install -e fprime/Gds/
```

After you have installed the python packages you need to set up pre-commit hooks using the following command. This
ensures that for each commit, checks are performed before the developer commits the code.

```bash
pre-commit install
```

## Unit Tests

Probably one of the most important parts of developing code is to ensure Python unit tests pass. We use pytest:

```bash
cd fprime/Fw/Python/
pytest
```

```bash
cd fprime/Gds/
pytest
```

## When and How to Format Your Code

Everyone has their own habits and style when writing code. We are asking you to change your habits, but this is made
easy through the use of a code formatter. This ensures your code is consistent with F´ so that reviewers and
collaborators can more quickly comprehend what the code is doing. This can easily be done executing the `black`
formatter right before you commit and push your code.

Here is how to run black:

```bash
# format a single file
black example-file.py
# format an entire folder and subfolders
black path/to/folder/
```

### Git Pre-commit hooks

If you followed the Development Install Instructions above you will have automatically set up pre-commit hooks.
These will run automatically whenever you commit your code. Currently, the only pre-commit hook we have is `black`.
Black automatically formats your python code so that it is consistent with the rest of the F´ python code.

```bash
$ git commit -m "Add new python code"
Format Python Code (black)...............................................Passed
[master 3415f7dd] Add new python code
 1 file changed, 30 insertions(+)
```

If black finds unformatted code the commit will fail, meaning that you will have to stage the formatted changes and
run the commit again. If for some reason black's formatting introduces a bug into your code you can optionally skip the
pre-commit hook by modifying the 'SKIP' environment variable in bash. For example:

```bash
SKIP=black git commit -m "foo"
```

However, this should only be used in an emergency. If SKIP is used for all of your commits it will dramatically
reduce code quality over time.

### Why did we choose black

Black is an opinionated formatter that will format your code without any configuration or input from the programmer.
Why did we choose an “opinionated” formatter? In short, convention is so much more efficient than configuration.  With
convention, everyone just does what is dictated (in this case from the tool) and there is no room for choice, which
causes entropy in the project (e.g. one developer chooses spaces, another chooses tabs, and chaos results). As entropy
increases, the code is harder to understand with automated tooling stifled, and development becomes more difficult. Hence,
projects select coding styles, standards, best practices, etc. as a way to reduce entropy. By accepting an established
opinion, we can gain the advantages of a well-ordered project via a tool.  Assuming the opinions of the tool are not too
onerous, we’ll have a better project as a result.  Be like the Borg and just assimilate...

## Static Analysis

While almost all projects require unit tests, not enough of them consistently use static analysis to improve code quality.
With a few simple commands, static analysis will drastically reduce time wasted with simple errors or issues and increase
the lifecycle of your code.

F uses Pylama to perform static analysis on our python code. Pylama contains pylint, pyflakes, pycodestyle(pep8),
mccabe, pydocstyle, radon, gjslint, eradicate, and Mypy. Combining the output from the tools that you want into a single
file.

```bash
# How to run pylama
pylama -o path/to/fprime/pylama/setup.cfg path/to/directory/you/want/to/analyze/
# the '-o' specifies the configuration file
```

Pylama can be configured within the setup.cfg file. This file can also go in the directory you are analyzing if you want
a different configuration for the python files in that directory. Inside the configuration file, you can configure both
pylama as a whole and each of the tools individually.

The recommended F´ pylama config is at: Fw/Python/pylama.cfg