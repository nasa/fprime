/**
 * vue-support/tabetc.js:
 *
 * A tabbed routed view into all three aspects of F´ (events 'e', channels/telemetry 't', commands 'c') that allows the
 * user to switch between the various parts of the F´ system using a set of tab buttons.
 *
 * @author mstarch
 */
import {ChannelMixins} from "./channel.js";
import {CommandMixins} from "./command.js";
import {EventMixins} from "./event.js";

Vue.component("tabbed-etc", {
    template: "#tabetc-template",
    props:["commands", "loader", "cmdhist", "events", "channels"],
    data: function () {
        return {"currentTab": "Commanding", "tabs": ["Commanding", "Events", "Channels"]}
    }
});

/**
 * TabETC:
 *
 * Class implementing the vue-items used to do a tabbed version of the F´  setup.
 */
export class TabETCVue {
    constructor(element, commands, loader) {
        //Mixin functions for each of the components
        Object.assign(TabETCVue.prototype, CommandMixins);
        Object.assign(TabETCVue.prototype, EventMixins);
        Object.assign(TabETCVue.prototype, ChannelMixins);

        let data = {
            ...this.setupCommands(commands, loader),
            ...this.setupEvents(),
            ...this.setupChannels()
        };
        // Create a vue object
        this.vue = new Vue({
            el: element,
            data: data
        });
        // Initialize the commands portion
        this.initializeCommands();
    }


}