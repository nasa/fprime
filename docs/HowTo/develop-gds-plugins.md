# How-To: F´ GDS Plugin Development

This guide will walk through the process of developing GDS plugins.  GDS plugins allow users to add functionality to the
GDS in several ways. These include:

1. Selection plugins: add another choice for key GDS functionality
2. Functionality plugins: add functionality as an addition to the GDS.

This guide will walk through the development of a `framing`  implementation plugin and notes on start-up application
plugins.

## Contents

- [Plugin System Design](#plugin-system-design)
- [Developing a Plugin](#developing-a-plugin)
  - [Basic Plugin Skeleton](#basic-plugin-skeleton)
  - [Implementing Virtual Functions](#implementing-virtual-functions)
- [Distributing Plugins](#distributing-plugins)
- [Application Plugins](#application-plugins)
  - [Application Plugin Skeleton](#application-plugin-skeleton)
  - [Plugin Arguments](#plugin-arguments)
- [Conclusion](#conclusion)


## Plugin System Design

GDS plugins are built on top of the [pluggy](https://pluggy.readthedocs.io/en/stable/). This means that each implementor
of a GDS plugin must define a function for behavior and mark that function with an implementation decorator. GDS plugins
all define a registration function, which returns an implementation class for the given plugin category.

The GDS defines several categories of plugins that the user may implement. These categories and the plugin type of each
category is summarized in the table below.

| Category      | Type          | Description                                                              | Implementation Base Class                                                                                                         |
|---------------|---------------|--------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------|
| framing       | Selection     | Implement a framer/deframer pair to handle serialized data               | [FramerDeframer](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/common/communication/framing.py#L24)    |
| communication | Selection     | Implement a communication adapter for flight software communication      | [BaseAdapter](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/common/communication/adapters/base.py#L16) |
| gds-app       | Functionality | Implement a new GDS application isolated to a separate process           | [GdsApp](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/executables/apps.py#L76)                        |
| gds-function  | Functionality | (Advanced) Implement new GDS functionality with control over the process | [GdsFunction](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/executables/apps.py#L40)                   |


Plugins should define a function called `register_<category>_plugin` that return a concrete subclass of the category's
implementation base class from the above table.  These concrete classes may additionally define `get_arguments`,
`get_name`, and `check_arguments` functions used by the plugin system to provide and validate arguments from the CLI.

This guide will walk through the development of a framing plugin and compare that to a gds-app plugin.

## Developing a Plugin

The first step in developing a framing plugin is to determine the function that must be implemented and the class that
must be derived to develop the plugin. For the case of a `framing` plugin, the `register_framing_plugin` function
must be defined to return a concrete subclass of `FramerDeframer`. This information was found in the above table.

### Basic Plugin Skeleton

GDS plugins define a class that inherits from the implementation base class and implements all virtual functions. These
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

Each plugin implementation base class (e.g. `FramerDeframer`) has a set of virtual methods that plugin implementors
*must* implement  in order to support the plugin implementation.  These functions are marked as ` @abc.abstractmethod`
and can be found in the virtual class definition.

[FramerDeframer virtual functions](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/common/communication/framing.py#L29-L51)
consist of a `frame` and `deframe` method. Below the frame function adds the bytes `MY-PLUGIN` to the start of each
frame and strip the same bytes off the start of each frame. This is a trivial example of a start word.

**`src/my_plugin.py`:**
```python
from fprime_gds.common.communication.framing import FramerDeframer
from fprime_gds.plugin.definitions import gds_plugin_implementation

class MyPlugin(FramerDeframer):
    START_TOKEN = b"MY-PLUGIN"
    
    def frame(self, data):
        """ Frames data with 'MY-PLUGIN' start token """
        return self.START_TOKEN + data

    def deframe(self, data, no_copy=False):
        """ Deframe data with 'MY-PLUGIN' start token """
        discarded = b""
        data = data if no_copy else b"" + data # Copy data if no_copy
        
        # Deframing can deframe until data length isn't sufficent to provide start token
        while len(data) > len(self.START_TOKEN):
            # Starts with start word and a second start word found
            if data[:len(self.START_TOKEN)] == self.START_TOKEN and self.START_TOKEN in data[1:]:
                data = data[len(self.START_TOKEN):] # Remove initial start token
                # Return packet (data to next start token), unconsumed data, and discarded data
                return data[:data.index(self.START_TOKEN)], data[data.index(self.START_TOKEN):], discarded
            # Starts with start token, but beginning of next packet was not found
            elif data[:len(self.START_TOKEN)] == self.START_TOKEN:
                # Wait for new data
                break
            # Does not start with requested token throw away one byte and continue
            else:
                discarded += data[1]
                data[1:]
                continue
        # No packet found, all data unconsumed, and discarded
        return None, data, discarded
    
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

This is the basic implementation of a no-argument framing plugin. The above plugin tracks a single start `MY-PLUGIN`
string and deframes that as a packet.  Next, this guide will cover how to integrate this plugin via python packaging. 
Following that, plugin arguments will be covered.

## Distributing Plugins

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

## Application Plugins

Unlike the example `framing` plugin, application plugins run in addition to the GDS. These plugins can be used to start
new services that connect to the larger GDS network.  Application plugins will be used to show how to solicit arguments
from the command line.

Our plugin ill run python to print a message supplied via arguments. This is the equivalent to running the following
command line:
```bash
python -c "print('Hello World')"
```

### Application Plugin Skeleton

Here is the basic structure for a `gds-app` plugin.  It prints "Hello World".  gds-app plugins must implement the
function `get_process_invocation` that returns command line arguments to be run as a separate process using the
`subprocess` module.

```python
import sys
from fprime_gds.executables.apps import GdsApp

class MyApp(GdsApp):
    """ An app for the GDS """

    def get_process_invocation(self):
        """ Process invocation """
        return [sys.executable, "-c", "print('Hello World')"]

    @classmethod
    def get_name(cls):
        """ Get name """
        return "my-app"

    @classmethod
    @gds_plugin_implementation
    def register_gds_app_plugin(cls):
        """ Register a good plugin """
        return cls
```

### Plugin Arguments

Now is time to add in plugin arguments. This plugin will take one argument `--message` and will inject this message into
the printed message.  To do this we return the argument using the `get_arguments` class method.  Add this to your plugin
`MyApp` class:

```python
    @classmethod
    def get_arguments(cls):
      """ Get arguments """
      return {
        ("--message", ): {
          "type": str,
          "help": "Message to print",
          "required": True
        }
      }
```

`get_arguments` is a class method that returns a dictionary whose keys are tuples containing the flags, and whose value
is a dictionary of keyword arguments passed to
[`argparse.add_argument`](https://docs.python.org/3/library/argparse.html#the-add-argument-method).

Arguments are supplied to the plugin at instantiation via keyword arguments. Add the following to your `MyApp` class.

```python
    def __init__(self, message):
        """ Constructor """
        super().__init__()
        self.message = message
```

Change the `get_process_invocation` to use the new member variable.

```python
    def get_process_invocation(self):
        """ Process invocation """
        # Inject message into command line to print
        return [sys.executable, "-c", "print(f'{self.message}')"]
```

Finally, security-minded developers will notice there is an injection vulnerability above. This can be check using the
`check_arguments` class method.  This method should raise a `ValueError` or `TypeError` when an argument value is
malformed.  Add this function to `MyApp` fix the injection:

```python
    @classmethod
    def check_arguments(cls, message):
        """ Check arguments """
        if "'" in message or '\n' in message:
            raise ValueError("--message must not include ' nor a newline")
```

Now our plugin may be run.  The GDS will automatically solicit the message argument as seen should the user run
```bash
fprime-gds --help
```

The complete plugin would look like:

```python
import sys
from fprime_gds.executables.apps import GdsApp

class MyApp(GdsApp):
  """ An app for the GDS """

    def __init__(self, message):
        """ Constructor """
        super().__init__()
        self.message = message

    def get_process_invocation(self):
        """ Process invocation """
        # Inject message into command line to print
        return [sys.executable, "-c", "print(f'{self.message}')"]

    @classmethod
    def get_name(cls):
        """ Get name """
        return "my-app"
    
    @classmethod
    def get_arguments(cls):
        """ Get arguments """
        return {
            ("--message", ): {
                "type": str,
                "help": "Message to print",
                "required": True
            }
        }

    @classmethod
    def check_arguments(cls, message):
        """ Check arguments """
        if "'" in message or '\n' in message:
            raise ValueError("--message must not include ' nor a newline")
  
    @classmethod
    @gds_plugin_implementation
    def register_gds_app_plugin(cls):
        """ Register a good plugin """
        return cls
```

## Conclusion

This guide has covered how to develop GDS plugins, their design, and selection vs functionality plugins. You should now
be capable of writing plugins and handling arguments.