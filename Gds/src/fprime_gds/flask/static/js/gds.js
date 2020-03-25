/**
 * gds.js:
 *
 * JavaScript support file for the gds layer.
 */
import {Loader} from "./loader.js"
import {Uploader} from "./uploader.js"
import {TabETCVue}  from "./vue-support/tabetc.js"

//import {SimpleTemplate} from "./simple-templates.js";
let loader = new Loader();
let uploader = new Uploader(loader);

/**
 * Setup the data bindings in order to map the loaded content to the GUI.
 */
function setupBindings() {
    let tabView = new TabETCVue("#tabetc",
        loader.endpoints["command-dict"].data,
        loader.endpoints["channel-dict"].data,
        loader, uploader);
    loader.registerPoller("channels", function (data) {tabView.updateChannels(data["history"]);});
    loader.registerPoller("events", function (data) {tabView.updateEvents(data["history"]);});
    loader.registerPoller("commands", function (data) {tabView.updateCommandHistory(data["history"]);});
    loader.registerPoller("logdata", function (data) {tabView.updateLogs(data);});
    loader.registerPoller("upfiles", function (data) {tabView.updateUpfiles(data["files"]);});
    loader.registerPoller("downfiles", function (data) {tabView.updateDownfiles(data["files"]);});
}

// On ready when the document has been loaded
document.addEventListener("DOMContentLoaded", function(event) {
    loader.setup().then(setupBindings).catch(console.error);
});
