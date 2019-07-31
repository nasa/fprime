import {filter, timeToString} from "./utils.js";

/**
 * channel-row:
 *
 * A vue component designed to render exactly on row of the channel lising. It includes both the massage functions used
 * to transform data for display purposes (e.g. getChanTime) and also the colorization code used to colorize these lists
 * based on the channel's bounds.
 */
Vue.component("event-item", {
    props:["event"],
    template: "#event-item-template",
    computed: {
        /**
         * Use the row's values and bounds to colorize the row. This function will color red and yellow items using
         * the boot-strap "warning" and "danger" calls.
         * @return {string}: style-class to use
         */
        calculateItemStyle: function () {
            let severity = {
                "Severity.FATAL":      "list-group-item-danger",
                "Severity.WARNING_HI": "list-group-item-warning",
                "Severity.WARNING_LO": "list-group-item-warning",
                "Severity.ACTIVITY_HI": "list-group-item-success",
                "Severity.ACTIVITY_LO": "list-group-item-dark",
                "Severity.COMMAND":     "list-group-item-primary",
                "Severity.DIAGNOSTIC":  ""
            }
            return "list-group-item list-group-item-action flex-column align-items-start " +
                severity[this.event.template.severity.value];
        },
        /**
         * Produces the channel's time in the form of a string.
         * @return {string} seconds.microseconds
         */
        calculateEventTime: function() {return timeToString(this.event.time)}
    }
});


/**
 * events-list:
 *
 *
 */
Vue.component("event-list", {
    props:["events"],
    data: function() {return {"matching": ""}},
    template: "#event-list-template",
    computed: {
        calculateFilteredEvents: function() {
            return filter(this.events, this.matching,
                function (event)
                {
                    return "0x" + event.id.toString(16) + event.template.name + event.args;
                });
            }
    }
});

/**
 * EventView:
 *
 * A wrapper for the event-list viewable. This particular instance is supported by Vue.js in order to render the
 * events as a list of bootstrap list items.
 *
 * @author mstarch
 */
export class EventView {
    /**
     * Creates a ChannelView that delegates to a Vue.js view.
     * @param elemid: HTML ID of the element to render to
     */
    constructor(elemid) {
        this.vue = new Vue({
            el: elemid,
            data: {
                events: [],
            }
        });
    }
    /**
     * Update the list of events with the supplied new list of events.
     * @param newEvents: new full list of events to render
     */
    updateEvents(newEvents) {
        //TODO: make this efficient by appending to vue's data only
        this.vue.events = newEvents;
    }
}
