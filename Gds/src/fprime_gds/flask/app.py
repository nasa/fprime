####
# app.py:
#
# This file sets up the flask app, and registers the endpoints for the individual APIs supplied by
# this framework.
#
####
import os
import logging
import flask
import flask_restful

# Import the Flask API implementations
import fprime_gds.flask.commands
import fprime_gds.flask.events
import fprime_gds.flask.channels
import fprime_gds.flask.logs
import fprime_gds.flask.json

# Import GDS layer items
import fprime_gds.common.pipeline.standard

# Update logging to avoid redundant messages
log = logging.getLogger('werkzeug')
log.setLevel(logging.WARN)

# Flask global objects
app = flask.Flask(__name__, static_url_path="")
# Configuration loading
app.config.from_object("fprime_gds.flask.default_settings")
if "FP_FLASK_SETTINGS" in os.environ:
    app.config.from_envvar("FP_FLASK_SETTINGS")
# JSON encoding seeting must come before restful
app.json_encoder = fprime_gds.flask.json.GDSJsonEncoder
app.config['RESTFUL_JSON'] = {'cls': app.json_encoder}

api = flask_restful.Api(app)

# Middleware to python data pipeline
pipeline = fprime_gds.common.pipeline.standard.StandardPipeline()
pipeline.setup(app.config["GDS_CONFIG"], app.config["DICTIONARY"], logging_prefix=app.config["LOG_DIR"])
app.logger.info("Connected to GDS at: {}:{}".format(app.config["ADDRESS"], app.config["PORT"]))
pipeline.connect(app.config["ADDRESS"], app.config["PORT"])


# Application routes
api.add_resource(fprime_gds.flask.commands.CommandDictionary, "/dictionary/commands",
                 resource_class_args=[pipeline.dictionaries.command_name])
api.add_resource(fprime_gds.flask.commands.CommandHistory, "/commands",
                 resource_class_args=[pipeline.histories.commands])
api.add_resource(fprime_gds.flask.commands.Command, "/commands/<command>",
                 resource_class_args=[pipeline])
api.add_resource(fprime_gds.flask.events.EventDictionary, "/dictionary/events",
                 resource_class_args=[pipeline.dictionaries.event_id])
api.add_resource(fprime_gds.flask.events.EventHistory, "/events",
                 resource_class_args=[pipeline.histories.events])
api.add_resource(fprime_gds.flask.channels.ChannelDictionary, "/dictionary/channels",
                 resource_class_args=[pipeline.dictionaries.channel_id])
api.add_resource(fprime_gds.flask.channels.ChannelHistory, "/channels",
                 resource_class_args=[pipeline.histories.channels])
# Optionally serve log files
if app.config["SERVE_LOGS"]:
    api.add_resource(fprime_gds.flask.logs.FlaskLogger, "/logdata", resource_class_args=[app.config["LOG_DIR"]])

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
if __name__ == '__main__':
    app.run(debug=True)

