####
# app.py:
#
# This file sets up the flask app, and registers the endpoints for the individual APIs supplied by
# this framework.
#
####
import os
import flask
import flask_restful

# Import the Flask API implementations
import fprime_gds.flask.commands
import fprime_gds.flask.events
import fprime_gds.flask.channels
import fprime_gds.flask.json

# Import GDS layer items
import fprime_gds.common.pipeline.standard

# Flask global objects
app = flask.Flask(__name__)
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
pipeline.setup(app.config["GDS_CONFIG"], app.config["DICTIONARY"])

# Application routes
api.add_resource(fprime_gds.flask.commands.CommandHistory, "/commands",
                 resource_class_args=[pipeline.get_command_history(), pipeline.get_command_dictionary()])
api.add_resource(fprime_gds.flask.commands.Command, "/commands/<command>")
api.add_resource(fprime_gds.flask.events.EventHistory, "/events",
                 resource_class_args=[pipeline.get_event_history(), pipeline.get_event_dictionary()])
api.add_resource(fprime_gds.flask.channels.ChannelHistory, "/channels",
                 resource_class_args=[pipeline.get_channel_history(), pipeline.get_channel_dictionary()])


# When running from the command line, this will allow the flask development server to launch
if __name__ == '__main__':
    app.run(debug=True)

