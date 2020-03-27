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
import {LogMixins} from "./log.js";
import {config} from "../config.js"
/**
 * tabbed-ect:
 *
 * This component sets up a tabbed view for Events, Channels (Telemetry), and Commands. This is a composite of the
 * component-views that were composed into a tabbed-view.
 */
Vue.component("tabbed-etc", {
    template: "#tabetc-template",
    props:["commands", "loader", "cmdhist", "events", "channels", "logs", "eventsActive", "channelsActive"],
    data: function () {
        let hash = window.location.hash.replace("#", "");
        return {
            "currentTab": (hash == "")? "Commanding" : hash,
            "tabs": ["Commanding", "Events", "Channels", "Logs"],
            "config": config
        }
    },
    methods: {
        /**
         * Route the tab-change and place it in the Window's location
         * @param tab: tab to route to. No need for the #
         */
        route: function (tab) {
            window.location.hash = tab;
            this.currentTab = tab;
        },
        /**
         * Spawns a new window when the new window button is clicked.
         */
        spawn: function () {
            window.open(window.location);
        }
    },
    computed: {
        noneActive: function () {
            return !(this.eventsActive || this.channelsActive);
        }

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
        Object.assign(TabETCVue.prototype, LogMixins);

        let data = {
            ...this.setupCommands(commands, loader),
            ...this.setupEvents(),
            ...this.setupChannels(channels),
            ...this.setupLogs()
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