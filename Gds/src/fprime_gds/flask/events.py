####
# events.py:
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


class EventHistory(flask_restful.Resource):
    """
    Endpoint to return event history data with optional time argument.
    """
    def __init__(self, history, dictionary):
        """
        Constructor used to setup time argument to this history.
        :param history: history object holding events
        :param dictionary: dictionary holding events list
        """
        self.parser = flask_restful.reqparse.RequestParser()
        self.parser.add_argument("start-time")
        self.history = history
        self.dictionary = dictionary

    def get(self):
        """
        Return the event history object
        """
        args = self.parser.parse_args()
        return {"history": self.history.retrieve(args.starttime), "dictionary": self.dictionary}
