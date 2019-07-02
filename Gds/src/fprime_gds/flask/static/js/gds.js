/**
 * gds.js:
 *
 * JavaScript support file for the gds layer.
 */
import {Loader} from "./loader.js"
import {SimpleTemplate} from "./simple-templates.js";
let loader = new Loader();

/**
 * Setup the data bindings in order to map the loaded content to the GUI.
 */
function setupBindings() {
    // Setup a list of commands
    let cmds = [];
    let cmdSet = loader.endpoints["command-dict"]["data"];
    for (let cmd in cmdSet) {
        cmds.push(cmdSet[cmd]);
    }
    // Setup sata store, and templates
    let dataStore = {"commands": cmds, "activeCommand": cmds[14], "activeArgs": cmds[14].args};
    let evtTmpl = new SimpleTemplate("event-container", "event-list-template", dataStore);
    let chlTmpl = new SimpleTemplate("channel-container", "channel-table-template", dataStore);
    let cmdList = new SimpleTemplate("command-list-container", "command-list-template", dataStore);
    let cmdInpt = new SimpleTemplate("command-input-container", "command-input-template", dataStore);
    let cmdHist = new SimpleTemplate("command-hist-container", "command-hist-template", dataStore);


    loader.registerPoller("events",
        function(data) {
            dataStore["events"] = data["history"];
            evtTmpl.rerender()
        });
    loader.registerPoller("commands",
        function(data) {
            dataStore["commandHistory"] = data["history"];
            cmdHist.rerender()
        });
    loader.registerPoller("channels",
        function (data) {
            let chanData = data["history"];
            // Create unique set of data
            let chanSet = {};
            for (let i = 0; i < chanData.length; i++) {
                chanSet[chanData[i].id] = chanData[i];
            }
            let chanList = [];
            for (let chanItem in chanSet) {
                chanList.push(chanSet[chanItem]);
            }
            dataStore["channels"] = chanList;
            chlTmpl.rerender();
        });
    cmdList.rerender();
    cmdInpt.rerender();
}


// On ready when the document has been loaded
document.addEventListener("DOMContentLoaded", function(event) {
    loader.setup().then(setupBindings).catch(console.error);
});
