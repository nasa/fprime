/**
 * datastore.js:
 *
 * Creates a datastore object that handles the storing of the various data items in the system. It then allows for
 * accessing those data items by the components in this system.
 *
 *  @author mstarch
 */
import {config} from "./config.js";
import {_loader} from "./loader.js";

/**
 * DataStore:
 *
 * Storage class for holding the one copy of the data. This is meant to be a *singleton* that distributes the known data
 * and thus only the single instance should be used and exported from this file.  This will wrap the loader for
 * automating the polling of the data.
 *
 * This will support the following types of data:
 *
 * - Commands
 * - Events
 * - Channels
 */
export class DataStore {

    constructor() {
        this.loggers = [];
        // Activity timeout for checking spacecraft health and "the orb" (ours, not Keynes')
        this.active = [false, false];
        this.active_timeout = null;

        // Data stores used to store all data supplied to the system
        this.events = [];
        this.command_history = [];
        this.channels = {};
        this.commands = {};
        this.logs ={"": ""};

        // File data stores used for file handling
        this.downfiles = [];
        this.upfiles = [];
        this.uploading = false;
    }

    startup() {
        let channel_dict = _loader.endpoints["channel-dict"].data;
        // Setup channels object in preparation for updates
        let channels = {};
        for (let key in channel_dict) {
            channels[key] = {id: key, template: channel_dict[key], val: null, time: null};
        }
        this.channels = channels; // Forces new channel map into Vue
        this.commands = _loader.endpoints["command-dict"].data;
        // Clear the commands dictionary for setup
        for (let command in this.commands) {
            command = this.commands[command];
            for (let i = 0; i < command.args.length; i++) {
                command.args[i].error = "";
                if (command.args[i].type == "Enum") {
                    command.args[i].value = command.args[i].possible[0];
                }
            }
        }
        // Register callbacks for loading all the data types
        _loader.registerPoller("channels", this.updateChannels.bind(this));
        _loader.registerPoller("events", this.updateEvents.bind(this));
        _loader.registerPoller("commands", this.updateCommandHistory.bind(this));
        _loader.registerPoller("logdata", this.updateLogs.bind(this));
        _loader.registerPoller("upfiles", this.updateUpfiles.bind(this));
        _loader.registerPoller("downfiles", this.updateDownfiles.bind(this));
    }

    registerLogHandler(item) {
        this.loggers.push(item);
        return this.logs;
    }

    registerActiveHandler(item) {
        this.actives.push(item);
        return [false, false];
    }

    updateCommandHistory(data) {
        this.command_history.push(...data["history"]);
    }

    updateChannels(data) {
        let new_channels = data["history"];
        // Loop over all dictionaries, and merge to the last reading
        for (let i = 0; i < new_channels.length; i++) {
            let channel = new_channels[i];
            let id = channel.id;
            this.channels[id] = channel;
        }
        this.updateActivity(new_channels, 0);
    }

    updateEvents(data) {
        let new_events = data["history"];
        this.events.push(...new_events);
        this.updateActivity(new_events, 1);
    }

    updateLogs(log_data) {
        this.logs = log_data;
        for (let i = 0; i < this.loggers.length; i++) {
            this.loggers[i].update();
        }
    }

    updateUpfiles(data) {
        let files = data["files"];
        this.upfiles.splice(0, this.upfiles.length, ...files);
        this.uploading = data["running"];
    }

    updateDownfiles(data) {
        let files = data["files"];
        this.downfiles.splice(0, this.downfiles.length, ...files);
    }

    updateActivity(new_items, index) {
        let timeout = config.dataTimeout * 1000;
        // Set active items, and register a timeout to turn it off again
        if (new_items.length > 0) {
            let _self = this;
            this.active.splice(index, 1, true);
            clearTimeout(this.active_timeout);
            this.active_timeout = setTimeout(() => _self.active.splice(index, 1, false), timeout);
        }
    }
};


// Exports the datastore
export let _datastore = new DataStore();
