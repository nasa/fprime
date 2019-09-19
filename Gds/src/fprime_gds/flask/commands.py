####
# commands.py:
#
# This file provides the API for the commands Gds interface for use with the Gds Flask server. This
# API should provide the following HTML API behaviors:
#
#  GET /commands: list all commandsi history available to the GUI. Note: this also prvides a full
#                 command listing.
#  PUT /commands/<command>: issue a command through the GDS 
#      Data: {
#                "key": "0xfeedcafe", # A key preventing accedential issuing of a command
#                "args": {
#                            <arg-key>:<arg-value>,
#                             ...
#                             ...
#                        }
#             }
#  
#  
# Note: for commands, these are not true "REST" objects, and thus this is a bit of a stretch to use
#       a restful interface here. It it done this way to be in-tandum with the events and telemetry
#       APIs for maintainability.
####
import flask_restful
import flask_restful.reqparse
import fprime.common.models.serialize.type_exceptions

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
    Command history reurning both the full list of available commands and the global history of all
    of these commands that have run.
    """
    def __init__(self, history):
        """
        Constructor: setup the parser for incoming command runs
        :param history: history object holding commands
        :param dictionary: dictionary holding command list
        """
        self.parser = flask_restful.reqparse.RequestParser()
        self.parser.add_argument("start-time")
        self.history = history

    def get(self):
        """
        Return the command history object
        """
        args = self.parser.parse_args()
        return_set = {"history": self.history.retrieve_new()}
        return return_set


class Command(flask_restful.Resource):
    """
    Command object used to send commands into the GDS.
    """
    def __init__(self, sender):
        """
        Constructor: setup the parser for incoming command runs
        """
        self.parser = flask_restful.reqparse.RequestParser()
        self.parser.add_argument("key", required=True, help="Protection key. Must be: 0xfeedcafe.")
        self.parser.add_argument("arguments", action="append", help="Argument list to pass to command.")
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
        if key is None or int(key, 0) != 0xfeedcafe:
            flask_restful.abort(403, message="{} is invalid command key. Supply 0xfeedcafe to run command.".format(key))
        if arg_list is None:
            arg_list = []
        try:
            cmd_name_dict = self.sender.get_command_name_dictionary()
            self.sender.send_command(cmd_name_dict[command].opcode, arg_list)
        except fprime.common.models.serialize.type_exceptions.NotInitializedException:
            flask_restful.abort(403, message="Did not supply all required arguments.")
        return {"message": "success"}

