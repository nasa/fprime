/**
 * logs.js:
 *
 * JavaScript support file for the gds layer.
 */
import {Loader} from "./loader.js"
import {LogView}  from "./vue-support/log.js"

//import {SimpleTemplate} from "./simple-templates.js";
let loader = new Loader();

/**
 * Setup the data bindings in order to map the loaded content to the GUI.
 */
function setupBindings() {
    let logview = new LogView("#logging-container");
    loader.registerPoller("logdata",
        function (data) {
            logview.updateLogs(data);
    });

}

// On ready when the document has been loaded
document.addEventListener("DOMContentLoaded", function(event) {
    loader.setup().then(setupBindings).catch(console.error);
});
