####
# commands.py:
#
# This file provides the API for the commands Gds interface for use with the Gds Flask server. This
# API should provide the following HTML API behaviors:
#
#  GET /commands: list all commandsi history available to the GUI. Note: this also provides a full
#                 command listing.
#  PUT /commands/<command>: issue a command through the GDS
#      Data: {
#                "key": "0xfeedcafe", # A key preventing accidental issuing of a command
#                "args": {
#                            <arg-key>:<arg-value>,
#                             ...
#                             ...
#                        }
#             }
#
#
# Note: for commands, these are not true "REST" objects, and thus this is a bit of a stretch to use
#       a restful interface here. It is done this way to be in-tandem with the events and telemetry
#       APIs for maintainability.
####
import flask_restful
import flask_restful.reqparse

import fprime.common.models.serialize.type_exceptions
import fprime_gds.common.data_types.cmd_data


class CommandDictionary(flask_restful.Resource):
    """
    Command dictionary endpoint. Will return dictionary when hit with a GET.
    """

    def __init__(self, dictionary):
        """
        Constructor used to setup for dictionary.
        """
        self.dictionary = dictionary

    def get(self):
        """
        Returns the dictionary object
        """
        return self.dictionary


class CommandHistory(flask_restful.Resource):
    """
    Command history returning both the full list of available commands and the global history of all
    of these commands that have run.
    """

    def __init__(self, history):
        """
        Constructor: setup the parser for incoming command runs

        :param history: history object holding commands
        """
        self.parser = flask_restful.reqparse.RequestParser()
        self.parser.add_argument(
            "session", required=True, help="Session key for fetching data."
        )
        self.history = history

    def get(self):
        """
        Return the command history object
        """
        args = self.parser.parse_args()
        return_set = {"history": self.history.retrieve(start=args.get("session"))}
        self.history.clear()
        return return_set

    def delete(self):
        """
        Delete the event history for a given session. This keeps the data all clear like.
        """
        args = self.parser.parse_args()
        self.history.clear(start=args.get("session"))


class Command(flask_restful.Resource):
    """
    Command object used to send commands into the GDS.
    """

    def __init__(self, sender):
        """
        Constructor: setup the parser for incoming command runs
        """
        self.parser = flask_restful.reqparse.RequestParser()
        self.parser.add_argument(
            "key", required=True, help="Protection key. Must be: 0xfeedcafe."
        )
        self.parser.add_argument(
            "arguments", action="append", help="Argument list to pass to command."
        )
        self.sender = sender

    def put(self, command):
        """
        Receive a command run request.
        @param command: command identification
        """
        args = self.parser.parse_args()
        key = args.get("key", None)
        arg_list = args.get("arguments", [])
        # Error checking
        if key is None or int(key, 0) != 0xFEEDCAFE:
            flask_restful.abort(
                403,
                message="{} is invalid command key. Supply 0xfeedcafe to run command.".format(
                    key
                ),
            )
        if arg_list is None:
            arg_list = []
        try:
            self.sender.send_command(command, arg_list)
        except fprime.common.models.serialize.type_exceptions.NotInitializedException:
            flask_restful.abort(403, message="Did not supply all required arguments.")
        except fprime_gds.common.data_types.cmd_data.CommandArgumentException as exc:
            flask_restful.abort(403, message=str(exc))
        #        except fprime_gds.common.data_types.cmd_data.CommandArgumentsException as exc:
        #            flask_restful.abort(403, message="Argument errors occurred", errors=exc.errors)
        return {"message": "success"}
