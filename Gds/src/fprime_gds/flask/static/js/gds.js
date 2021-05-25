/**
 * gds.js:
 *
 * Entry-point for the JavaScript frontend tot he F prime GDS. This will start up all the services in JavaScript used to
 * display the GDS UI.
 *
 * @author mstarch
 */
import {_loader} from "./loader.js";
import {_uploader} from "./uploader.js";
import  {_datastore} from "./datastore.js";
import "../addons/enabled.js";
//Import all vue objects such that the components are defined
import "./vue-support/tabetc.js"


/**
 * Constructs the vue and registers all data polling functions used to pull data into the UI. These polls are attached
 * to the handling functions used to deal with this data. Sets up a tabbed view for GDS functions.
 *
 * Note: this function should be called as a result of the loader's setup function completing.
 */
function setupBindings() {
    //Startup the global datastore *before* setting-up the Vue application, as Vue needs a configured datastore
    _datastore.startup();
    new Vue({el: "#tabetc"});
}

/**
 * Main entry point of the GDS scripts.  This is loaded after all HTML content has been fetched, and will call the
 * loader's setup function, providing the "setupBindings" function as its callback.
 */
document.addEventListener("DOMContentLoaded", function(event) {
    _loader.setup().then(setupBindings).catch(console.error);
});
/**
 * Teardown best effort code.
 */
window.onbeforeunload = function (e) {
    _loader.destroy();
};
