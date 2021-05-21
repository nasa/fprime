####
#
####
import os
import flask_restful
import flask_restful.reqparse


class FlaskLogger(flask_restful.Resource):
    """
    Command dictionary endpoint. Will return dictionary when hit with a GET.
    """

    def __init__(self, logdir):
        """
        Constructor used to setup the log directory.

        :param logdir: log directory to search for logs
        """
        self.logdir = logdir

    def get(self):
        """
        Returns the logdir.
        """
        logs = {}
        listing = os.listdir(self.logdir)
        for path in [path for path in listing if path.endswith(".log")]:
            full_path = os.path.join(self.logdir, path)
            offset = 0
            with open(full_path) as file_handle:
                file_handle.seek(offset)
                logs[path] = file_handle.read()
        return logs
