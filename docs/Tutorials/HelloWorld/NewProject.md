# Getting Started: Creating an F´ Project 

This tutorial will walk new users through creating a new F´ project.

### Prerequisites:
- [F´ Installation Requirements](../../INSTALL.md#requirements)

### Tutorial Steps:
- [Bootstrapping F´](#bootstrapping-f)
- [Creating a New F´ Project](#creating-a-new-f-project)
- [Building the New F´ Project](#building-the-new-f-project)
- [Conclusion](#conclusion)

## Bootstrapping F´

An F´ [project](./Tutorial.md#project) ties to a specific versioned set of tools to work with F´.  In order to create
this project and install the correct version of tools, an initial bootstrap version of F´ tools must be installed. This
is accomplished with the following command:

```bash
pip install fprime-tools
```

## Creating a New F´ Project

Now that to tools are installed a new F´ project should be created. An F´ project internalizes the version of F´ that
the project will build upon and provides the user the basic setup for creating, building, and testing components.

In order to make a new project, run the following command and answer the questions as indicated below:

```bash
fprime-util new --project
```
This command will ask for some input. Respond with the following answers:
1. Project Name: MyProject
2. F´ Version: devel
3. Setup Tools In New Virtual Environment: yes

This will create a new F´ project structure in a folder called `MyProject`, use the `devel` branch of F´ as the basis
for the project, and set up the matching tools in a new Virtual Environment.

> Experienced F´ users may note that we have not yet created a deployment, but rather just the base project structure.

## Building the New F´ Project

The next step is to set up and build the newly created project. This will serve as a build environment for any newly
created components, and will build the F´ framework supplied components.

```bash
fprime-util generate
fprime-util build
```

> `fprime-util generate` sets up the build environment for a project/deployment. It only needs to be done once. At the
> end of this tutorial, a new deployment will be created and `fprime-util generate` will also be used then.

## Conclusion

A new project has been created with the name `MyProject` and has been placed in a new folder called in `MyProject` in
the current directory. It includes the initial build system setup, and F´ version. It is still empty in that the user
will still need to create components and deployments.

For the remainder of this Getting Started tutorial we should use the tools installed for our project and issue commands
within this new project's folder. Change into the project directory and load the newly install tools with:

```bash
cd MyProject
. venv/bin/activate
```

**Next:** [Getting Started: Creating an F´ Hello World Component](./HelloWorld.md)