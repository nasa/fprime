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
import types

import flask_restful
import flask_restful.reqparse
from fprime_gds.common.utils.string_util import format_string


class EventDictionary(flask_restful.Resource):
    """
    Event dictionary endpoint. Will return dictionary when hit with a GET.
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


class EventHistory(flask_restful.Resource):
    """
    Endpoint to return event history data with optional time argument.
    """

    def __init__(self, history):
        """
        Constructor used to setup time argument to this history.

        :param history: history object holding events
        """
        self.parser = flask_restful.reqparse.RequestParser()
        self.parser.add_argument(
            "session", required=True, help="Session key for fetching data."
        )
        self.history = history

    def get(self):
        """
        Return the event history object
        """
        args = self.parser.parse_args()
        new_events = self.history.retrieve(args.get("session"))
        self.history.clear()
        for event in new_events:
            # Add the 'display_text' to the event, along with a getter
            setattr(
                event,
                "display_text",
                format_string(event.template.format_str, tuple([arg.val for arg in event.args])),
            )

            def func(this):
                return this.display_text
            setattr(event, "get_display_text", types.MethodType(func, event))
        return {"history": new_events}

    def delete(self):
        """
        Delete the event history for a given session. This keeps the data all clear like.
        """
        args = self.parser.parse_args()
        self.history.clear(start=args.get("session"))
