"""
flask/updown.py:

A simple service that handles file uploads and downloads. This allowed the REST api to show the status of file uplinks
and downlinks. In addition, an uplink destination directory is exposed for the UI to set where new uploads should be
uplinked to.

@author mstarch
"""
import os

import flask
import flask_restful


class Destination(flask_restful.Resource):
    """
    A data model for the current location of the destination of uplinked files.
    """

    def __init__(self, uplinker):
        """
        Constructor: setup the parser for incoming destination arguments
        """
        self.parser = flask_restful.reqparse.RequestParser()
        self.parser.add_argument(
            "destination", required=True, help="Destination to place uploaded files"
        )
        self.uplinker = uplinker

    def get(self):
        """
        Gets the current destination

        :return: current destination
        """
        return {"destination": self.uplinker.destination_dir}

    def put(self):
        """
        Updates the current destination directory
        """
        args = self.parser.parse_args()
        destination = args.get("destination", None)
        self.uplinker.destination_dir = destination
        return {"message": "success"}


class FileUploads(flask_restful.Resource):
    """
    A data model for the current uplinking file set.
    """

    def __init__(self, uplinker, uplink_set):
        """
        Constructor: setup the uplinker and argument parsing
        """
        self.uplinker = uplinker
        self.uplink_set = uplink_set
        self.parser = flask_restful.reqparse.RequestParser()
        self.parser.add_argument(
            "action", required=True, help="Action to take against files"
        )
        self.parser.add_argument(
            "source", required=False, default=None, help="File on which to act file"
        )

    def get(self):
        """
        Gets the current set of files

        :return: current uplinking files
        """
        return {
            "files": self.uplinker.current_files(),
            "running": self.uplinker.is_running(),
        }

    def put(self):
        """
        Handles an update to an existing source file.  Source and action are expected parameters to be supplied. If
        source is None, then "pause-all" or "unpause-all" should be supplied to globally pause the uplinker.
        """
        args = self.parser.parse_args()
        action = args.get("action", None)
        source = args.get("source", None)
        if action == "Remove" or action == "Cancel" and source is not None:
            self.uplinker.cancel_remove(source)
        elif action == "pause-all":
            self.uplinker.pause()
        elif action == "unpause-all":
            self.uplinker.unpause()

    def post(self):
        """
        Adds file(s) to be uplinked by enqueuing each into the uplinker.
        """
        successful = []
        failed = []
        for key, file in flask.request.files.items():
            try:
                filename = self.uplink_set.save(file)
                flask.current_app.logger.info(
                    "Received file. Saved to: {}".format(filename)
                )
                self.uplinker.enqueue(
                    os.path.join(self.uplink_set.config.destination, filename)
                )
                successful.append(key)
            except Exception as exc:
                flask.current_app.logger.warning(
                    "Failed to save file {} with error: {}".format(key, exc)
                )
                failed.append(key)
        return {"successful": successful, "failed": failed}


class FileDownload(flask_restful.Resource):
    """  """

    def __init__(self, downlinker):
        """
        Constructor: setup the downlinker
        """
        self.downlinker = downlinker

    def get(self, source=None):
        """
        Gets the current downlinking files

        :return: current downlinking files
        """
        # Serve the  source if asked for, otherwise list all files
        if source is not None:
            return flask.send_from_directory(
                self.downlinker.directory, os.path.basename(source), as_attachment=True
            )
        else:
            return {"files": self.downlinker.current_files()}
