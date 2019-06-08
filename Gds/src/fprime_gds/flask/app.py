####
# app.py:
#
# This file sets up the flask app, and registers the endpoints for the individual APIs supplied by
# this framework.
#
####
import flask
import flask_restful

# Import the Flask API implementations
import fprime_gds.flask.commands
import fprime_gds.flask.events
import fprime_gds.flask.telemetry

# Import GDS layer items
import fprime_gds.common.pipeline

# Flask global objects
app = flask.Flask(__name__)
api = flask_restful.Api(app)

# Middleware to python data pipeline
pipeline = fprime_gds.common.pipeline.StandardPipeline()
pipeline.setup(None, None)

# Application routes
api.add_resource(fprime_gds.flask.commands.CommandHistory, "/commands",
                 resource_class_args=[pipeline.get_command_history(), pipeline.get_command_dictionary()])
api.add_resource(fprime_gds.flask.commands.Command, "/commands/<command>")
api.add_resource(fprime_gds.flask.events.EventHistory, "/events",
                 resource_class_args=[pipeline.get_event_history(), pipeline.get_event_dictionary()])
api.add_resource(fprime_gds.flask.telemetry.ChannelHistory, "/channels",
                 resource_class_args=[pipeline.get_channel_history(), pipeline.get_channel_dictionary()])


# When running from the command line, this will allow the flask development server to launch
if __name__ == '__main__':
    app.run(debug=True)

