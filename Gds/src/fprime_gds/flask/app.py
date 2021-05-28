####
# app.py:
#
# This file sets up the flask app, and registers the endpoints for the individual APIs supplied by
# this framework.
#
####
import logging
import os
import sys

import flask
import flask_restful
from fprime_gds.flask import flask_uploads

import fprime_gds.flask.channels

# Import the Flask API implementations
import fprime_gds.flask.commands
import fprime_gds.flask.events
import fprime_gds.flask.json
import fprime_gds.flask.logs
import fprime_gds.flask.updown

from . import components

# Update logging to avoid redundant messages
logger = logging.getLogger("werkzeug")
logger.setLevel(logging.WARN)
logger = logging.getLogger("downlink")
logger.setLevel(logging.INFO)


def construct_app():
    """
    Constructs the Flask app by taking the following steps:

    1. Setup and configure the app
    2. Setup JSON encoding for Flask and flask_restful to handle F prime types natively
    3. Setup standard pipeline used throughout the system
    4. Create Restful API for registering flask items
    5. Setup flask_uploads settings
    6. Register all restful endpoints

    :return: setup app
    """
    app = flask.Flask(__name__, static_url_path="")
    app.config.from_object("fprime_gds.flask.default_settings")
    # Override defaults from python files specified in 'FP_FLASK_SETTINGS'
    if "FP_FLASK_SETTINGS" in os.environ:
        app.config.from_envvar("FP_FLASK_SETTINGS")

    # JSON encoding setting must come before restful
    app.json_encoder = fprime_gds.flask.json.GDSJsonEncoder
    app.config["RESTFUL_JSON"] = {"cls": app.json_encoder}
    # Standard pipeline creation
    pipeline = components.setup_pipelined_components(
        app.debug,
        app.logger,
        app.config["GDS_CONFIG"],
        app.config["DICTIONARY"],
        app.config["DOWNLINK_DIR"],
        app.config["LOG_DIR"],
        app.config["ADDRESS"],
        app.config["PORT"],
    )
    # Restful API registration
    api = flask_restful.Api(app)
    # File upload configuration, 1 set for everything
    uplink_set = flask_uploads.UploadSet("uplink", flask_uploads.ALL)
    flask_uploads.configure_uploads(app, [uplink_set])

    # Application routes
    api.add_resource(
        fprime_gds.flask.commands.CommandDictionary,
        "/dictionary/commands",
        resource_class_args=[pipeline.dictionaries.command_name],
    )
    api.add_resource(
        fprime_gds.flask.commands.CommandHistory,
        "/commands",
        resource_class_args=[pipeline.histories.commands],
    )
    api.add_resource(
        fprime_gds.flask.commands.Command,
        "/commands/<command>",
        resource_class_args=[pipeline],
    )
    api.add_resource(
        fprime_gds.flask.events.EventDictionary,
        "/dictionary/events",
        resource_class_args=[pipeline.dictionaries.event_id],
    )
    api.add_resource(
        fprime_gds.flask.events.EventHistory,
        "/events",
        resource_class_args=[pipeline.histories.events],
    )
    api.add_resource(
        fprime_gds.flask.channels.ChannelDictionary,
        "/dictionary/channels",
        resource_class_args=[pipeline.dictionaries.channel_id],
    )
    api.add_resource(
        fprime_gds.flask.channels.ChannelHistory,
        "/channels",
        resource_class_args=[pipeline.histories.channels],
    )
    api.add_resource(
        fprime_gds.flask.updown.Destination,
        "/upload/destination",
        resource_class_args=[pipeline.files.uplinker],
    )
    api.add_resource(
        fprime_gds.flask.updown.FileUploads,
        "/upload/files",
        resource_class_args=[pipeline.files.uplinker, uplink_set],
    )
    api.add_resource(
        fprime_gds.flask.updown.FileDownload,
        "/download/files",
        "/download/files/<string:source>",
        resource_class_args=[pipeline.files.downlinker],
    )
    # Optionally serve log files
    if app.config["SERVE_LOGS"]:
        api.add_resource(
            fprime_gds.flask.logs.FlaskLogger,
            "/logdata",
            resource_class_args=[app.config["LOG_DIR"]],
        )
    return app, api


try:
    app, _ = construct_app()
except Exception as exc:
    print("[ERROR] {}".format(exc), file=sys.stderr)
    sys.exit(1)



@app.route("/js/<path:path>")
def files_serve(path):
    """
    A function used to serve the JS files needed for the GUI layers.

    :param path: path to the file (in terms of web browser)
    """
    return flask.send_from_directory("static/js", path)


@app.route("/")
def index():
    """
    A function used to serve the JS files needed for the GUI layers.
    """
    return flask.send_from_directory("static", "index.html")


@app.route("/logs")
def log():
    """
    A function used to serve the JS files needed for the GUI layers.
    """
    return flask.send_from_directory("static", "logs.html")


# When running from the command line, this will allow the flask development server to launch
if __name__ == "__main__":
    app.run()
