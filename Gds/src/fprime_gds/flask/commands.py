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


class CommandHistory(flask_restful.Resource):
    """
    Command history reurning both the full list of available commands and the global history of all
    of these commands that have run.
    """
    def __init__(self, history, dictionary):
        """
        Constructor: setup the parser for incoming command runs
        :param history: history object holding commands
        :param dictionary: dictionary holding command list
        """
        self.parser = flask_restful.reqparse.RequestParser()
        self.parser.add_argument("start-time")
        self.history = history
        self.dictionary = dictionary

    def get(self):
        """
        Return the command history object
        """
        args = self.parser.parse_args()
        return {"history": self.history.retrieve(args.get("starttime", None)), "dictionary": self.dictionary}


class Command(flask_restful.Resource):
    """
    Command object used to send commands into the GDS.
    """
    def __init__(self):
        """
        Constructor: setup the parser for incoming command runs
        """
        self.parser = flask_restful.reqparse.RequestParser()
        self.parser.add_argument("key")
        self.parser.add_Argument("args")

    def put(self, command):
        """
        Receive a command run request.
        @param command: command identification
        """
        args = self.parser.parse_args()
        if int(args.key) != 0xfeedcafe:
            flask_restful.abort(403, "{0} is invalid command key. Supply 0xfeedcafe to run command")
        #TODO: call into GDS here

