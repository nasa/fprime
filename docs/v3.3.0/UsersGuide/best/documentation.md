
# Documentation of F´ Projects

Most projects need to generate documentation on the software. F´ uses [Doxygen](https://www.doxygen.nl/index.html) to
generate project documentation and projects using F´ may use Doxygen to generate their project documents. Doxygen was
chosen because it can run automatically on F´ markdown and C++ documents to generate a web-compatible (HTML) version of
the documentation.

The document will describe the basic usage of Doxygen for F´ projects and will also discuss some key settings that
projects may wish to change. The output of Doxygen will be a set of HTML files that allow inspection of the project
markdowns and C++ code. F´ Doxygen documentation may be viewed to see what the output will look like and is available at
[https://nasa.github.io/fprime/UsersGuide/api/c++/html/index.html](https://nasa.github.io/fprime/UsersGuide/api/c++/html/index.html).

## Basic Doxygen Usage

Although a full understanding of Doxygen is outside of the scope of this document, we will attempt to discuss the basic
usage of doxygen as it pertains to F´. Doxygen requires two things - a working directory and a configuration file. It
will recurse through the working directory, generating documentation using the settings set in the configuration.

If the user wishes to include autocoded parts of F´ in the documentation, make sure to build your F´ project before
running Doxygen. Make sure to delete or exclude (see EXCLUDE settings below) any non-native builds before running.

The working directory should be the root of the project that is to be documented.  A configuration file can be generated
or the sample can be used/modified. To run Doxygen through the GUI, select the working directory and a configuration
file and then run generation.  Alternatively, the `doxygen` command line utility can be run.  Change to the working
directory and supply the path to the configuration file. More detail can be found on the
 [Doxygen](https://www.doxygen.nl/index.html) website.

A sample configuration file can be seen at
[https://github.com/nasa/fprime/blob/devel/docs/doxygen/Doxyfile](https://github.com/nasa/fprime/blob/devel/docs/doxygen/Doxyfile).
This file can be used to run documentation of a project.  Note: the output directory inside F´ will need to be changed
basic configuration settings are described below.

Once generated, load `index.html` in the output directory using a web browser to view the documentation.

## Basic Doxygen Configuration

There are certain settings that users should change in the above configuration sample, and likely these settings should
be changed when starting from scratch.

**Note:** The sample file and templated Doxygen configuration give a detailed explanation of each Doxygen setting.

### Key Settings

These settings are essential to use Doxygen to generate documentation.

| Setting | Description | Example |
|---|---|---|
| OUTPUT_DIRECTORY | Directory relative to working directory to write out HTML. | ./docs |
| STRIP_FROM_PATH | Paths to strip out of documentation. Add your project's build cache to this. | ./MyProject/build-fprime-automatic-native/ |
| STRIP_FROM_INC_PATH | Paths to strip from autocoder include files. Add your project's build cache to this. | ./MyProject/build-fprime-automatic-native/ |
| USE_MDFILE_AS_MAINPAGE | Set a MD file to be the landing page documentation | MyProject/README.md |
| MARKDOWN_SUPPORT | Pull in markdown files | YES |
| RECURSIVE | Run doxygen recursively | YES |
| EXCLUDE | Paths to exclude. Used to not generate fprime Doxygen inside project documentation. | ./fprime |
| EXCLUDE_PATTERNS | Wildcard patterns to not include | MyProject/test* |

### Project Descriptions

There are several settings that describe the basic project - these are listed in the table below: (a project should
change these settings)

| Setting | Description |
|---|---|
| PROJECT_NAME | Name of the project displayed prominently in the documentation |
| PROJECT_NUMBER | Project version number |
| PROJECT_BRIEF  | Brief description of the project |
| PROJECT_LOGO | 200x50 or smaller logo for the project. Leave blank for no logo |

## GitHub Pages Notes

There is a small problem with the Doxygen files being generated and then subsequently hosted on GitHub pages. This is because Doxygen converts the leading / of markdown paths to an _ which then is ignored by Jekyll, a static site generator used by gh-pages.

Possible fixes:

1. Projects can set no-jekyll on their GitHub pages.  This will prevent converting MD to web pages, but the HTML output of Doxygen will be hosted as is. This is a good choice for projects that just want to host Doxygen output.

2. Rename underscore files. This is necessary when Jekyll needs to preprocess MD files. A sample renaming script can
be found next to the Doxygen sample configuration. Mileage may vary.