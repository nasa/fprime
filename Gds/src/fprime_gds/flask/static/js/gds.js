/**
 * gds.js:
 *
 * JavaScript support file for the gds layer.
 */
import {Loader} from "./loader.js"
let loader = new Loader();

// On ready when the document has been loaded
document.addEventListener("DOMContentLoaded", function(event) {
    loader.setup().then(
        function() {
            alert("Worked!!!!!!");
        }
    ).catch(
        function(e) {
            alert("Failed:" + e)
        }
    );
});
