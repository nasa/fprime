/**
 * gds.js:
 *
 * Entry-point for the JavaScript frontend tot he F prime GDS. This will start up all the services in JavaScript used to
 * display the GDS UI.
 *
 * @author mstarch
 */
import {Loader} from "./loader.js"
import {Uploader} from "./uploader.js"
import {TabETCVue}  from "./vue-support/tabetc.js"

// Loader is used to run AJAX queries against a known endpoints. This interacts with the REST backend to load data
// from the remainder of the GDS.
let loader = new Loader();
// Uploader uses the loader object to handle the file uploading extensions used to get files onto the server in
// preparation for the file uplinker.
let uploader = new Uploader(loader);

/**
 * Constructs the vue and registers all data polling functions used to pull data into the UI. These polls are attached
 * to the handling functions used to deal with this data. Sets up a tabbed view for GDS functions.
 *
 * Note: this function should be called as a result of the loader's setup function completing.
 */
function setupBindings() {
    let tabView = new TabETCVue("#tabetc",
        loader.endpoints["command-dict"].data,
        loader.endpoints["channel-dict"].data,
        loader, uploader);
    // Register all pollers for data from the backend
    loader.registerPoller("channels", function (data) {tabView.updateChannels(data["history"]);});
    loader.registerPoller("events", function (data) {tabView.updateEvents(data["history"]);});
    loader.registerPoller("commands", function (data) {tabView.updateCommandHistory(data["history"]);});
    loader.registerPoller("logdata", function (data) {tabView.updateLogs(data);});
    loader.registerPoller("upfiles", function (data) {tabView.updateUpfiles(data["files"], data["running"]);});
    loader.registerPoller("downfiles", function (data) {tabView.updateDownfiles(data["files"]);});
}

/**
 * Main entry point of the GDS scripts.  This is loaded after all HTML content has been fetched, and will call the
 * loader's setup function, providing the "setupBindings" function as its callback.
 */
document.addEventListener("DOMContentLoaded", function(event) {
    loader.setup().then(setupBindings).catch(console.error);
});
/**
 * Teardown best effort code.
 */
window.onbeforeunload = function (e) {
    loader.destroy();
};
