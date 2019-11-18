/**
 * vue-support/event.js:
 *
 * Event listing support for F´ that sets up the Vue.js components used to display events. These components allow the
 * user to render events. This file also provides EventMixins, which are the core functions needed to convert events to
 * something Vue.js can display. These should be mixed with any F´ objects wrapping Vue.js component creation.
 *
 * @author mstarch
 */
import {filter, timeToString} from "./utils.js";
/**
 * events-list:
 *
 * Renders lists as a colorized table. This is a thin-wrapper to pass events to the fp-table component. It supplies the
 * needed method to configure fp-table to render events.
 */
Vue.component("event-list", {
    props:["events"],
    template: "#event-list-template",
    methods: {
        /**
         * Takes in a given event item, and harvests out the column values for display in the fp-table.
         * @param item: event object to harvest
         * @return {[string, *, *, void | string, *]}
         */
        columnify: function (item) {
            return [timeToString(item.time), "0x" + item.id.toString(16), item.template.full_name,
                item.template.severity.value.replace("Severity.", ""), item.display_text];
        },
        /**
         * Use the row's values and bounds to colorize the row. This function will color red and yellow items using
         * the boot-strap "warning" and "danger" calls.
         * @param item: item passed in with which to calculate style
         * @return {string}: style-class to use
         */
        style: function (item) {
            let severity = {
                "Severity.FATAL":      "fp-color-fatal",
                "Severity.WARNING_HI": "fp-color-warn-hi",
                "Severity.WARNING_LO": "fp-color-warn-lo",
                "Severity.ACTIVITY_HI": "fp-color-act-hi",
                "Severity.ACTIVITY_LO": "fp-color-act-lo",
                "Severity.COMMAND":     "fp-color-command",
                "Severity.DIAGNOSTIC":  ""
            }
            return severity[item.template.severity.value];
        },
        /**
         * Take the given item and converting it to a unique key by merging the id and time together with a prefix
         * indicating the type of the item. Also strip spaces.
         * @param item: item to convert
         * @return {string} unique key
         */
        keyify(item) {
            return "evt-" + item.id + "-" + item.time.seconds + "-"+ item.time.microseconds;
        }
    }
});
/**
 * EventMixins:
 *
 * This set of functions should be mixed in as member functions to the F´ wrappers around the above Vue.js component.
 * These provide the functions required to update events on the fly.
 *
 * Note: to mixin these functions: Object.assign(EventMixins)
 */
export let EventMixins = {
    /**
     * Update the list of events with the supplied new list of events.
     * @param newEvents: new full list of events to render
     */
    updateEvents(newEvents) {
        this.vue.events.push(...newEvents);
    },
    /**
     * Sets up the needed event data items.
     * @return [] an empty list to fill with events
     */
    setupEvents() {
        return {"events": []};
    }
};

/**
 * EventView:
 *
 * A wrapper for the event-list viewable. This is stand-alone and could be used anywhere that an events list is needed.
 * It will setup the Vue.js component and mixin the above needed functions.
 *
 * @author mstarch
 */
export class EventView {
    /**
     * Creates a ChannelView that delegates to a Vue.js view.
     * @param elemid: HTML ID of the element to render to
     */
    constructor(elemid) {
        Object.assign(EventView.prototype, EventMixins);
        this.vue = new Vue({
            el: elemid,
            data: this.setupEvents()
        });
    }

}
