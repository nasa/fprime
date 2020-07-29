# The Discerning User's Guide to the F´ GDS CLI

![](img/gds_cli_user_guide_cover.png)

This guide will give you a quick introduction to what the F´ GDS CLI is and how you can use it, with a more detailed reference provided further down. If you're interested in developing for this tool instead, please see the [developer's CLI guide](../../Gds/docs/GDS_CLI_DEV_GUIDE.md).

-   [What is this?](#what-is-this)
-   [Getting Started](#getting-started)
    -   [Installing the Tools](#installing-the-tools)
    -   [Troubleshooting](#troubleshooting)
    -   [The Basics](#the-basics)
    -   [Why You Need a Dictionary](#why-you-need-a-dictionary)
    -   [Using the Tools](#using-the-tools)
        -   [Receiving Commands](#receiving-commands)
        -   [Sending Commands](#sending-commands)
        -   [Is the GDS Running?](#is-the-gds-running)
-   [Command Details](#command-details)
    -   [channels](#channels)
    -   [command-send](#command-send)
    -   [events](#events)

## What is this?

The GDS CLI provides a command-line interface for working with F´s ground data system, as an alternative to using the browser-based GUI. While graphical interfaces are often convenient, a command-line interface is still handy to have for situations like...

-   ...working remotely on a server, where setting up windows can be a hassle
-   ...analyzing data with existing command-line tools
-   ...automating certain tasks by writing scripts
-   ...pacifying hardcore command-line users in your office
-   ...and many, many more I'm sure you'll think of!

In particular, the CLI tools for the GDS let you do 3 essential operations from the comfort of your own shell:

-   Send commands to the spacecraft/F´ instance
-   View events that are happening on the spacecraft
-   Receive telemetry data from the spacecraft

Let's see how you can use these for yourself!

## Getting Started

### Installing the Tools

1.  Make sure you have [F´ installed correctly](https://github.com/nasa/fprime/blob/master/docs/INSTALL.md).
2.  Activate your virtual environment for F´
3.  (optional) [Set up tab completion](https://github.com/nasa/fprime/blob/devel/docs/gds/AUTOCOMPLETE.md) for these tools.
4.  Run `fprime-cli -h` in your terminal as a test; if you see a help message pop up, you're done!

### Troubleshooting

If the tools aren't quite working yet, first, double-check that you [have indeed installed F´ correctly](https://github.com/nasa/fprime/blob/master/docs/INSTALL.md#checking-your-f-installation). If you're sure your F´ installation isn't the issue, check the following:

-   TODO: Write this list out

If all else fails, ask a question by posting an [issue on GitHub](https://github.com/nasa/fprime/issues/new) or a [message on our Google Group](https://groups.google.com/forum/#!forum/fprime-community) describing your problem, and someone from the F´ team will try to help you out. Before posting, double-check if someone has had a similar issue before - their solution might work for you, too.

### The Basics

All of the GDS CLI commands are run through the `fprime-cli` tool; if you run that all by itself, you should see a help message like this:

```
$ fprime-cli
usage: fprime-cli [-h] [-V] {channels,command-send,events} ...

provides utilities for interacting with the F´ Ground Data System (GDS)

positional arguments:
  {channels,command-send,events}

optional arguments:
  -h, --help            show this help message and exit
  -V, --version         show program's version number and exit
```

We can see there are 3 "positional arguments" that we can provide here, corresponding to the 3 commands we can run:

-   `channels`, which lets us receive the values of open telemetry channels on the spacecraft
-   `command-send`, which lets us give certain commands to the spacecraft
-   `events`, which lets us receive events the spacecraft is recording

Like other command line tools such as git or npm, you can use each command by running `fprime-cli [COMMAND NAME]`, e.g. `fprime-cli events` or `fprime-cli channels`. To learn more about each one, you can include the `-h` or `--help` flag when you call them to print out their help messages (e.g. `fprime-cli events --help`).

### Why You Need a Dictionary

As mentioned, these CLI commands let you interact with the GDS through events and commands and telemetry channels that it already knows about - but how does the CLI tool know which commands and other items are available? Through a *project dictionary file*.

Every F´ project deployment will have a `*Dictionary.xml` file that's created when the project is built (in the example `Ref` project, for instance, it'll be the `Ref/Top/RefTopologyAppDictionary.xml` file), which lists all the items we're interested in. By reading this file, the CLI tool knows what to look for when it's reading or sending data to the GDS. If it doesn't know where the dictionary is, then the CLI tool can't do much beyond printing out help messages about all the cool things you *could* be doing if you had a dictionary.

When you run one of the CLI commands, the tool will automatically look for files ending in `Dictionary.xml` in your current working directory and use the first one it can find. If it can't find any dictionaries, it will print out an error message like this:

```
fprime-cli: error: No valid project dictionary found
```

Running commands from your project folder (e.g. inside `/Ref`) should work most of the time, but if you want to run commands from a different folder, or the tool is accidentally using the wrong dictionary, you can specify the dictionary file exactly using the `--dictionary` option with the file's path (e.g. `fprime-cli command-send --dictionary Ref/Top/RefTopologyAppDictionary.xml`).

### Using the Tools

The `fprime-cli` commands can roughly be grouped into 2 categories:

-   Ones that receive GDS data (`channels`, `events`), and
-   Ones that send data to the GDS (`command-send`)

#### Receiving Commands

`channels` and `events`, in the first category, are two very similar commands; when they're run, they'll respectively listen for any telemetry channel data or logged events coming from the GDS and print out all the data they receive to the console until the user stops them with `CTRL+C`. Since this can be quite a lot of data, and you'll often only care about a few particular things, there are several options to filter which items you want to see:

-   `-i`/`--ids` will only look for events/data with the numeric IDs or opcodes you give; for instance, `fprime-cli events --ids 1 2 3` will only display events with ID numbers 1, 2, or 3
-   `-c`/`--components` will only look for items generated by one of the components you list; for instance, `fprime-cli channels --components pingRcvr` will only display data sent from `pingRcvr`
-   `s`/`--search` will only show items whose printed strings contain the (case-sensitive) search term you provide; for instance, `fprime-cli events --search WARNING_LO` will only display `WARNING_LO` events, while the `-s FirstPacketSent` option will only show events of the `FirstPacketSent` type
    -   Note that unlike the other two options, `--search` will only take a *single* search string

These filters can be combined together to further narrow your search (e.g. `fprime-cli channels -c pingRcvr -s WARNING_LO` will only show `WARNING_LO` events from `pingRcvr`).

If you want to view a full list of all the events or channels available, you can use the `-l`/`--list` flag on a command to list everything in the dictionary it's interested in (e.g. `fprime-cli events -l` will list all known events for the project) - and if that's too much information for you, guess what? The same filtering options we just talked about work on `--list`'s output, too!

If you ever need the full programmatic guts of a response, you can also pass in the `-j/--json` flag to print out the full data objects being received in JSON format. While this will include some redundant information, it's guaranteed to show you all the information available and can potentially be passed as data to other programs.

#### Sending Commands

In the second category is currently just the `command-send` command, which - to no one's surprise - you use to send commands through the GDS to the spacecraft. You can send commands by giving the *full name* of the command in `COMPONENT.NAME` format (e.g. `health.HLTH_CHNG_PING`); if the command needs any arguments provided to it, you can pass them in as a space-separated list using the `-args`/`--arguments` option. To send a command that doesn't need arguments like `cmdDisp.CMD_NO_OP`, you would just run the following:

```
$ fprime-cli command-send cmdDisp.CMD_NO_OP
```

To send one that *does* need arguments like `health.HLTH_CHNG_PING`, though, you'd run something like...

```
$ fprime-cli command-send health.HLTH_CHNG_PING --arguments eventLogger 50 20
```

...to pass 3 arguments to the command.

> If you need to pass in a string with spaces as an argument, just enclose it in quotes `"like this"`

Similarly to the other CLI tools, you can run `fprime-cli command-send -l` to get a list of the available commands you can send and the arguments they take - and like before, you can still use all the filtering options we've talked about to search for particular commands. If you have tab completion enabled, you can also double-tap tab to view a list of potential commands based on what you've typed so far.

#### Is the GDS Running?

Finally, it's important to remember that these commands all directly interact with the GDS, and therefore **the GDS needs to be running to use these commands** - with the exceptions of viewing their help messages and listing available items with `-l`. By default, these commands will try to connect to the GDS over the default port (50050) and IP address (`127.0.0.1`); if you're not running the GDS there, you can specify a different port or IP with the `-p/--port` and `-ip/--ip-address` options, respectively.

--------------------------------------------------------------------------------

## Command Details

Below should be a print out of each command's help menu, with a more thorough explanation of what each one does.

### `channels`

Help Message:

```
usage: fprime-cli channels [-h] [-d DICTIONARY] [-ip IP] [-p PORT]
                           [-t SECONDS] [-l] [-i ID [ID ...]]
                           [-c COMP [COMP ...]] [-s STRING] [-j]

print out new telemetry data that has been received from the F Prime instance,
sorted by timestamp

optional arguments:
  -h, --help            show this help message and exit
  -d DICTIONARY, --dictionary DICTIONARY
                        path from the current working directory to the
                        "<project name>Dictionary.xml" file for the project
                        you're using the API with; if unused, tries to search
                        the current working directory for such a file
  -ip IP, --ip-address IP
                        connect to the GDS server using the given IP or
                        hostname (default=127.0.0.1 (i.e. localhost))
  -p PORT, --port PORT  connect to the GDS server using the given port number
                        (default=50050)
  -t SECONDS, --timeout SECONDS
                        wait at most SECONDS seconds for a single new message,
                        then exit (defaults to listening until the user exits
                        via CTRL+C, and logging all messages)
  -l, --list            list all possible channel types the current F Prime
                        instance could produce, based on the channels
                        dictionary, sorted by channel type ID
  -i ID [ID ...], --ids ID [ID ...]
                        only show channels matching the given type ID(s) "ID";
                        can provide multiple IDs to show all given types
  -c COMP [COMP ...], --components COMP [COMP ...]
                        only show channels from the given component name
                        "COMP"; can provide multiple components to show
                        channels from all components given
  -s STRING, --search STRING
                        only show channels whose name or output string exactly
                        matches or contains the entire given string "STRING"
  -j, --json            return the JSON response of the API call, with
                        channels filtered based on other flags provided
```

### `command-send`

Help Message:

```
usage: fprime-cli command-send [-h] [-d DICTIONARY] [-ip IP] [-p PORT]
                               [-args [ARGUMENTS [ARGUMENTS ...]]] [-l]
                               [-i ID [ID ...]] [-c COMP [COMP ...]]
                               [-s STRING] [-j]
                               [command-name]

sends the given command to the spacecraft via the GDS

positional arguments:
  command-name          the full name of the command you want to execute in
                        "<component>.<name>" form

optional arguments:
  -h, --help            show this help message and exit
  -d DICTIONARY, --dictionary DICTIONARY
                        path from the current working directory to the
                        "<project name>Dictionary.xml" file for the project
                        you're using the API with; if unused, tries to search
                        the current working directory for such a file
  -ip IP, --ip-address IP
                        connect to the GDS server using the given IP or
                        hostname (default=127.0.0.1 (i.e. localhost))
  -p PORT, --port PORT  connect to the GDS server using the given port number
                        (default=50050)
  -args [ARGUMENTS [ARGUMENTS ...]], --arguments [ARGUMENTS [ARGUMENTS ...]]
                        provide a space-separated set of arguments to the
                        command being sent
  -l, --list            list all possible command types the current F Prime
                        instance could produce, based on the commands
                        dictionary, sorted by command type ID
  -i ID [ID ...], --ids ID [ID ...]
                        only show commands matching the given type ID(s) "ID";
                        can provide multiple IDs to show all given types
  -c COMP [COMP ...], --components COMP [COMP ...]
                        only show commands from the given component name
                        "COMP"; can provide multiple components to show
                        commands from all components given
  -s STRING, --search STRING
                        only show commands whose name or output string exactly
                        matches or contains the entire given string "STRING"
  -j, --json            return the JSON response of the API call, with
                        commands filtered based on other flags provided
```

### `events`

Help message:

```
usage: fprime-cli events [-h] [-d DICTIONARY] [-ip IP] [-p PORT] [-t SECONDS]
                         [-l] [-i ID [ID ...]] [-c COMP [COMP ...]]
                         [-s STRING] [-j]

print out new events that have occurred on the F Prime instance, sorted by
timestamp

optional arguments:
  -h, --help            show this help message and exit
  -d DICTIONARY, --dictionary DICTIONARY
                        path from the current working directory to the
                        "<project name>Dictionary.xml" file for the project
                        you're using the API with; if unused, tries to search
                        the current working directory for such a file
  -ip IP, --ip-address IP
                        connect to the GDS server using the given IP or
                        hostname (default=127.0.0.1 (i.e. localhost))
  -p PORT, --port PORT  connect to the GDS server using the given port number
                        (default=50050)
  -t SECONDS, --timeout SECONDS
                        wait at most SECONDS seconds for a single new message,
                        then exit (defaults to listening until the user exits
                        via CTRL+C, and logging all messages)
  -l, --list            list all possible event types the current F Prime
                        instance could produce, based on the events
                        dictionary, sorted by event type ID
  -i ID [ID ...], --ids ID [ID ...]
                        only show events matching the given type ID(s) "ID";
                        can provide multiple IDs to show all given types
  -c COMP [COMP ...], --components COMP [COMP ...]
                        only show events from the given component name "COMP";
                        can provide multiple components to show events from
                        all components given
  -s STRING, --search STRING
                        only show events whose name or output string exactly
                        matches or contains the entire given string "STRING"
  -j, --json            return the JSON response of the API call, with events
                        filtered based on other flags provided
```
