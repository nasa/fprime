# Creating a cookiecutter template

Create a directory to make your cookiecutter template in. This is where you will create your template 
files. For the full documentation, see [Cookiecutter Documentation](https://cookiecutter.readthedocs.io/en/1.7.2/index.html)

Note: fprime-util new only supports outside cookiecutters for components, not ports

## JSON file
Create a cookiecutter.json file in your cookiecutter directory. This is where you will specify the 
fields for your cookiecutter. Each line represents a different question the cookiecutter will ask 
the user.

Here is an example from the builtin cookiecutter:

```
{
    "component_name": "MyExample",
    "component_short_description": "Example Component for F Prime FSW framework.",
    "component_namespace": "Component namespace",
    "component_kind": ["active", "passive", "queued"],
    "commands": ["yes","no"],
    "parameters": ["yes","no"],
    "events": ["yes","no"],
    "telemetry": ["yes","no"]
}
```

The values in the brackets are the values that the user can choose from.

## Template files

To create the rest of the files in your template(*Ai.xml, CMakeLists.txt, etc.), just write these
files as your normally would, replacing the variables with `{{ cookiecutter.<value> }}` where you would
replace `<value>` with the value from the JSON file that you are using.

### Conditionals

You can also use conditionals to make your templates more adaptable.

Below is an example of how these could be used:

```
{% if cookiecutter.telemetry == "yes" %}
    <!-- Import telemetry port -->
    <import_port_type>Fw/Tlm/TlmPortAi.xml</import_port_type>
{% endif %}
```

Here, he file template will only include the telemetry port import if the user had selected yes
to the telemetry prompt

### Loops

You may also use loops to stamp out repeated elements Here is an example of this:

```    
{% for arg in arg_list %}
    <arg name="{{ arg[0] }}" type="{{ arg[1] }}">
        <comment>{{ arg[2] }}</comment>
    </arg>
{% endfor %}
```

Here, all of the arguments in the arg_list will be stamped out in the users file, one after
the other.

### Hooks

If you have code that you would like to run immediately before or after the template files
are stamped out, you can create post_gen_project.py and pre_gen_project.py files inside of a directory
titled `hooks` within your cookiecutter directory. The pre_gen_project.py will run right before 
your files are generated and the post_gen_project.py will run immediatly after the files are generated.

### Other Cookiecutter features

There are many other features that cookiecutter provides, available in the documentation here:

[Cookiecutter Documentation](https://cookiecutter.readthedocs.io/en/1.7.2/index.html)

## Using your own template

Once you have created your template, two ways to use it are locally, or through github.

### Using a GitHub repository

If you are using a GitHub repository, simply include the line:

```
component_cookiecutter: gh:repository_name
```

to the settings.ini file in your F' deployment.

For example, to import Sterling Pete's template from Georgia Tech, use the following line:

```
component_cookiecutter: gh:SterlingPeet/cookiecutter-fprime-component
```

### Using a local template

Similarly to using a GitHub repository, you can use a local template by using:

```
component_cookiecutter: gh:path_to_repository
```

## Features of fprime-util new --component

While using a different template will alter the generated files, there are some 
features that are present within the fprime-util new --component command that
will happen regardless of which template you use:

1. The component directory will be added to the deployment build system
2. The CMake chache will be refreshed to include the new component
3. The user will be given the option to automatically generate implementation and/or unit test files