####
# channels.py:
#
# This file captures the HTML endpoint for the events list. This file contains one endpoint which allows for listing
# events after a certain time-stamp.
#
#  GET /events: list events
#      Input Data: {
#                      "start-time": "YYYY-MM-DDTHH:MM:SS.sss" #Start time for event listing
#                  }
####
import flask_restful
import flask_restful.reqparse


class ChannelHistory(flask_restful.Resource):
    """
    Endpoint to return telemetry history data with optional time argument.
    """
    def __init__(self, history, dictionary):
        """
        Constructor used to setup time argument to this history.
        :param history: history object holding channel
        :param dictionary: dictionary holding channel list
        """
        self.parser = flask_restful.reqparse.RequestParser()
        self.parser.add_argument("starttime")
        self.history = history
        self.dictionary = dictionary

    def get(self):
        """
        Return the telemetry history object
        """
        args = self.parser.parse_args()
        return {"history": self.history.retrieve(args.starttime), "dictionary": self.dictionary}
