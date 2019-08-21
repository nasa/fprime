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

/**
 * tabbed-ect:
 *
 * This component sets up a tabbed view for Events, Channels (Telemetry), and Commands. This is a composite of the
 * component-views that were composed into a tabbed-view.
 */
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
 * Class implementing the vue-items used to do a tabbed version of the F´ setup.
 */
export class TabETCVue {
    /**
     * Constructs the tabbed element from the input variables.
     * @param element: HTML element ID to render to
     * @param commands: commands list to render in a drop down
     * @param channels: channel templates list
     * @param loader: loader used to handel F prime REST
     */
    constructor(element, commands, channels, loader) {
        //Mixin functions for each of the components
        Object.assign(TabETCVue.prototype, CommandMixins);
        Object.assign(TabETCVue.prototype, EventMixins);
        Object.assign(TabETCVue.prototype, ChannelMixins);

        let data = {
            ...this.setupCommands(commands, loader),
            ...this.setupEvents(),
            ...this.setupChannels(channels)
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