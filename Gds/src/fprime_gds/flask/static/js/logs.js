/**
 * logs.js:
 *
 * JavaScript support file for the log page of the GDS layer. This supports the display of the Log files found on the
 * GDS backend.
 */
import {Loader} from "./loader.js"
import {LogView}  from "./vue-support/log.js"

let loader = new Loader();

/**
 * Constructs the vue and registers all data polling functions used to pull data into the UI. These polls are attached
 * to the handling functions used to deal with this data. This particular function sets up a LogView.
 *
 * Note: this function should be called as a result of the loader's setup function completing.
 */
function setupBindings() {
    let logview = new LogView("#logging-container");
    loader.registerPoller("logdata",
        function (data) {
            logview.updateLogs(data);
    });

}

/**
 * Main entry point of the GDS scripts.  This is loaded after all HTML content has been fetched, and will call the
 * loader's setup function, providing the "setupBindings" function as its callback.
 */
document.addEventListener("DOMContentLoaded", function(event) {
    loader.setup().then(setupBindings).catch(console.error);
});
