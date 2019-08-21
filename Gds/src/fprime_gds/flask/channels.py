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


class ChannelDictionary(flask_restful.Resource):
    """
    Channel dictionary endpoint. Will return dictionary when hit with a GET.
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


class ChannelHistory(flask_restful.Resource):
    """
    Endpoint to return telemetry history data with optional time argument.
    """
    def __init__(self, history):
        """
        Constructor used to setup time argument to this history.
        :param history: history object holding channel
        :param dictionary: dictionary holding channel list
        """
        self.parser = flask_restful.reqparse.RequestParser()
        self.parser.add_argument("starttime")
        self.history = history

    def get(self):
        """
        Return the telemetry history object
        """
        args = self.parser.parse_args()
        return {"history": self.history.retrieve_new()}
