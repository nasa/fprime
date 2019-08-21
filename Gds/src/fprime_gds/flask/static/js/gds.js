/**
 * gds.js:
 *
 * JavaScript support file for the gds layer.
 */
import {Loader} from "./loader.js"
import {ChannelView}  from "./vue-support/channel.js"
import {EventView}  from "./vue-support/event.js"
import {TabETCVue}  from "./vue-support/tabetc.js"

//import {SimpleTemplate} from "./simple-templates.js";
let loader = new Loader();

/**
 * Setup the data bindings in order to map the loaded content to the GUI.
 */
function setupBindings() {
    let tabView = new TabETCVue("#tabetc",
        loader.endpoints["command-dict"].data,
        loader.endpoints["channel-dict"].data,
        loader);
    loader.registerPoller("channels", function (data) {tabView.updateChannels(data["history"]);});
    loader.registerPoller("events", function (data) {tabView.updateEvents(data["history"]);});
    loader.registerPoller("commands", function (data) {tabView.updateCommandHistory(data["history"]);});
}

// On ready when the document has been loaded
document.addEventListener("DOMContentLoaded", function(event) {
    loader.setup().then(setupBindings).catch(console.error);
});
