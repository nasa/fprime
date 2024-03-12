# How-To: F´ GDS Plugin Development

This guide will walk through the process of developing GDS plugins.  GDS plugins allow users to select an implementation
for key functions within the GDS. These selections allow users to tailor the behavior of the GDS without affecting the
top-level flow of data throughout the system.

## Plugin System Design

GDS plugins are built on top of the [pluggy](https://pluggy.readthedocs.io/en/stable/). This means that each implementor
of a GDS plugin must define a function for behavior and mark that function with an implementation decorator.

The GDS defines several categories of plugins that the user may implement:

1. Framing plugins: used to change the framing of uplink and downlink packets
2. Communication plugins: used to change the interface for reading bytes from the spacecraft

Plugins should define a function called `register_<category>_plugin` that would return a concrete subclass of the
category's virtual-class that performs the work of the plugin.  These classes may define `get_arguments`, `get_name`,
and `check_arguments` functions used by the plugin system to provide and validate arguments from the CLI.

This guide will walk through the development of a framing plugin.

## Developing a Framing Plugin

The first step in developing a framing plugin is to determine the function that must be implemented and the class that
must be derived to develop the plugin. This information is available in
[`fprime_gds.plugin.definitions`](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/plugin/definitions.py).

For the case of a `framing` plugin, the `register_framing_plugin` function contains the needed information: our function
must be named `register_framing_plugin` and return a concrete subclass of `FramerDeframer`.

```python
@gds_plugin_specification
def register_framing_plugin() -> Type["FramerDeframer"]:
```

### Basic Plugin Skeleton

Most GDS plugins define a class that inherits from the necessary virtual-class and implements  virtual functions. These
classes also define a properly decorated class method for the registration function, and may define the other class
methods used for CLI interaction.

A basic framing plugin skeleton would thus look like:
**`src/my_plugin.py`:**
```python
from fprime_gds.common.communication.framing import FramerDeframer
from fprime_gds.plugin.definitions import gds_plugin_implementation

class MyPlugin(FramerDeframer):
    # TODO: implement virtual functions
    @classmethod
    def get_name(cls):
        """ Name of this implementation provided to CLI """
        return "my-plugin"

    @classmethod
    def get_arguments(cls):
        """ Arguments to request from the CLI """
        return {}

    @classmethod
    def check_arguments(cls):
        """ Check arguments from the CLI """
        pass

    @classmethod
    @gds_plugin_implementation
    def register_framing_plugin(cls):
        """ Register the MyPlugin plugin """
        return cls
```

### Implementing Virtual Functions

Each plugin virtual-class (e.g. `FramerDeframer`) has a set of virtual methods that plugin implementors *must* implement
in order to support the plugin implementation.  These functions are marked as ` @abc.abstractmethod` and can be found in
the virtual class definition.

[FramerDeframer virtual functions](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/common/communication/framing.py#L29-L51)
consist of a `frame` and `deframe` method. This guide will use no-op functions.

**`src/my_plugin.py`:**
```python
from fprime_gds.common.communication.framing import FramerDeframer
from fprime_gds.plugin.definitions import gds_plugin_implementation

class MyPlugin(FramerDeframer):

    def frame(self, data):
        """ Frames data by no-op """
        return b"" + data # Forces a copy

    def deframe(self, data, no_copy=False):
        """ Deframe data by no-op """
        if no_copy:
            return data
        return b"" + data # Forces a copy
        
    @classmethod
    def get_name(cls):
        """ Name of this implementation provided to CLI """
        return "my-plugin"

    @classmethod
    def get_arguments(cls):
        """ Arguments to request from the CLI """
        return {}

    @classmethod
    def check_arguments(cls):
        """ Check arguments from the CLI """
        pass

    @classmethod
    @gds_plugin_implementation
    def register_framing_plugin(cls):
        """ Register the MyPlugin plugin """
        return cls
```

This is the basic implementation of a no-argument framing plugin.  Next, this guide will cover how to integrate this
plugin via python packaging.  Then arguments will be covered.

### Distributing Plugins

Plugins are supplied as python packages with an entrypoint used to load the plugin. In the root of the package a basic
python package will need to be configured. This consists of two files: pyproject.toml representing the package, and
setup.py for backwards compatibility.

The project structure should look like:
```
    src/my_plugin.py
    pyproject.toml
    setup.py
```

A sample pyproject.toml file would look like:

```toml
[build-system]
requires = ["setuptools", "setuptools_scm[toml]>=6.2", "wheel"]
build-backend = "setuptools.build_meta"

[project]
name = "fprime-gds-my-plugin"
dynamic = ["version"]
dependencies = [
    "pluggy>=1.3.0",
    "fprime-gds>=3.4.4"
]

[project.entry_points."fprime_gds"]
my_plugin = "my_plugin:MyPlugin"

[tool.setuptools_scm]
```

A sample `setup.py` would look like:
```python
from setuptools import setup

# Configuration is in pyproject.toml
setup()
```