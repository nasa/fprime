## Installing the linter through fprime-extras

The fprime-extras package includes a linter to assist with development of fprime applications.

To install fprime-extras, we must clone the repository and switch to the devel branch:

```
git clone https://github.com/SterlingPeet/python-fprime-extras.git
cd python-fprime-extras
git checkout devel
```

Then, install fprime-extras using pip:

```
pip install .
```

For the linter to work, it needs a location in the .cache directory to store the log files:

```
cd ~/.cache
mkdir fprime-extras
```

Now the linter can be used to check .xml files for potential errors.
In the directory of the .xml file we want to lint, we can use the linter with this command:

```
fprime-extras lint filename
```

The first time you run the linter, you may get an error message stating that there is no version.json file, but once the linter is run once, this file is created and this error should no longer appear.

To see more details from the lint report than what is provided in the terminal, you can view the log files at ~/.cache/fprime-extras/fprime-extras.log

To see all of the potential linting options, use:

```
fprime-extras lint -h
```

And to view the other capabilities of fprime-extras, use:

```
fprime-extras -h
```