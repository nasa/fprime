"""
base.py:

This file specifies the base-adapter for the F prime comm-layer. This class defines the basic methods needed to interact
with various wire formats as they are supported by the F prime comm-layer. This file defines a single abstract base
class representing the core features of the adapter class that must be implemented by every implementation of the
adapter for use with the comm-layer.

@author lestarch
"""
import abc


class BaseAdapter(abc.ABC):
    """
    Base adapter for adapting the communications layer. This essentially breaks down to providing the ability to read
    data from, and write to the necessary wire-format. The children of this class must at least implement the 'read' and
    'write' functions to ensure that data can be read and written. 'open' and 'close' are also provided as a helper to
    the subclass implementer to place resource initialization and release code, however; these implementations are
    defaulted not overridden.
    """

    def open(self):
        """Null default implementation """

    def close(self):
        """Null default implementation """

    @abc.abstractmethod
    def read(self, timeout=0.500):
        """
        Read from the interface. Must be overridden by the child adapter. Throw no fatal errors, reconnect instead. This
        call is expected to block waiting on incoming data.

        :param size: maximum size of data to read before breaking
        :param timeout: timeout for the block, default: 0.500 (500ms) as blocking w/o timeout may be uninterruptible
        :return: byte array of data, or b'' if no data was read
        """

    @abc.abstractmethod
    def write(self, frame):
        """
        Write to the interface. Must be overridden by the child adapter. Throw no fatal errors, reconnect instead.

        :param frame: framed data to uplink
        :return: True if data sent through adapter, False otherwise
        """

    @classmethod
    @abc.abstractmethod
    def get_arguments(cls):
        """
        Returns a set of arguments consumed by this adapter. This will be consumed by the CLI layer in order to provide
        command line arguments to the user. Note: these should be globally unique args, e.g. --ip-address

        :return: dictionary, keys of tuple of arg flags and value of list of other arguments to argparse's add_argument
        """

    @classmethod
    @abc.abstractmethod
    def check_arguments(cls, args):
        """
        Code that should check arguments of this adapter. If there is a problem with this code, then a "ValueError"
        should be raised describing the problem with these arguments.

        :param args: arguments as dictionary
        """

    @classmethod
    def get_adapters(cls):
        """
        Get all known adapters of this base class. These must be imported into the comm-layer to be available to the
        system, however; they only need to be imported. Once imported this function will find them and make them
        available to the comm-layer in the standard way.

        :return: list of all imported comm adapters.
        """
        adapter_map = {}
        for adapter in cls.__subclasses__():
            # Get two versions of names
            adapter_name = adapter.__module__
            adapter_short = adapter_name.split(".")[-1]
            # Check to use long or short name
            if adapter_short not in adapter_map:
                adapter_name = adapter_short
            adapter_map[adapter_name] = adapter
        return adapter_map

    @staticmethod
    def process_arguments(clazz, args):
        """
        Process arguments incoming from the command line and construct a dictionary of kwargs to supply to the chosen
        adapter at creation time. This will allow the user to choose any imported adapters at runtime.

        :param args: arguments to process
        :return: dictionary of constructor keyword arguments
        """
        kwargs = {}
        for value in clazz.get_arguments().values():
            kwargs[value["dest"]] = getattr(args, value["dest"])
        return kwargs

    @classmethod
    def construct_adapter(cls, adapter_name, args):
        """
        Constructs a new adapter, from the given adapter name and the given namespace of argument inputs. This is a
        wrapper of "get_adapters" and "process_arguments" to help build a new, fully configured, adapter. This is a
        factory method.

        :param adapter_name: name of the adapter to build
        :param args: namespace of arg value to help build an adapter
        :return: newly constructed adapter
        """
        adapter = cls.get_adapters()[adapter_name]
        return adapter(**cls.process_arguments(adapter, args))
