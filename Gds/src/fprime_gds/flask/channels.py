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
import types
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
        self.parser.add_argument(
            "session", required=True, help="Session key for fetching data."
        )
        self.history = history

    def get(self):
        """
        Return the telemetry history object
        """
        args = self.parser.parse_args()
        new_chans = self.history.retrieve(session=args.get("session"))
        self.history.clear()
        for chan in new_chans:
            # Add the 'display_text' to the event, along with a getter
            if chan.template.fmt_str is not None:
                setattr(
                    chan, "display_text", chan.template.fmt_str % (chan.val_obj.val)
                )
                func = lambda this: this.display_text
                setattr(chan, "get_display_text", types.MethodType(func, chan))
        return {"history": new_chans}

    def delete(self):
        """
        Delete the event history for a given session. This keeps the data all clear like.
        """
        args = self.parser.parse_args()
        self.history.clear(session=args.get("session"))
