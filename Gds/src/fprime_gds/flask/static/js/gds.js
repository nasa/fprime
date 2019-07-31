/**
 * gds.js:
 *
 * JavaScript support file for the gds layer.
 */
import {Loader} from "./loader.js"
import {ChannelView}  from "./vue-support/channel.js"
import {EventView}  from "./vue-support/event.js"
import {CommandView}  from "./vue-support/command.js"

//import {SimpleTemplate} from "./simple-templates.js";
let loader = new Loader();

/**
 * Setup the data bindings in order to map the loaded content to the GUI.
 */
function setupBindings() {
    //Setup a channel view, in this case the one supported by Vue.js from vue-support package
    let channelView = new ChannelView("#channels");
    loader.registerPoller("channels", function (data) {channelView.updateChannels(data["history"]);});

    //Setup an event view, in this case the one supported by Vue.js from vue-support package
    let eventView = new EventView("#events");
    loader.registerPoller("events", function (data) {eventView.updateEvents(data["history"]);});


    let commandView = new CommandView("#commands", loader.endpoints["command-dict"].data, loader);
}


// On ready when the document has been loaded
document.addEventListener("DOMContentLoaded", function(event) {
    loader.setup().then(setupBindings).catch(console.error);
});
