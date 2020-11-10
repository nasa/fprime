/**
 * uploader.js:
 *
 * Contains the necessary classes to wrap the Loader object (loader.js) and allow it to upload files to the flask
 * backend. This requires a special endpoint, and a special upload call that can set the destination and construct the
 * file upload packet.
 *
 * The primary class Uploader is designed as a singleton and should be used and exported once.
 * @author mstarch
 */
import {_loader} from "./loader.js";

export class Uploader {
    /**
     * Setup the endpoint, and wrap a supplied Loader object.
     */
    constructor() {
        this.endpoint = "/upload/files";
    }
    /**
     * Takes in a list of files that have been selected by the files input type, and a destination (on the embedded
     * system) to post to. Then it will set the destination on the server, and upload each file to the rest backend.
     * This will trigger uplinking of the supplied files to the supplied destination.
     * @param files: files to uplink
     * @param destination: destination (on embedded system) to uplink to
     * @return {Promise} what to do when the download is done and the uplinking is started
     */
    upload(files, destination) {
        return new Promise((success, error) => {
            _loader.load("/upload/destination", "PUT", {"destination": destination}).then( () => {
                let data = new FormData();
                while(0 < files.length) {
                    let file = files.shift();
                    data.append(file.file.name, file.file);
                }
                _loader.load(this.endpoint, "POST", data, false).then(success).catch(error);
            });
        });
    }
    /**
     * Pause the uplinker. This will send the command to the backend to pause.
     * @return {Promise<any> | number | Promise<boolean> | void | boolean}
     */
    pause() {
        return _loader.load("/upload/files", "PUT", {"action": "pause-all"});
    }
    /**
     * Unpause the uplinker. This will send the command to the backend to unpause.
     * @return {Promise<any> | number | Promise<boolean> | void | boolean}
     */
    unpause() {
        return _loader.load("/upload/files", "PUT", {"action": "unpause-all"});
    }
    /**
     * Send a command to the server to command a specific file. This  allows files to be canceled and/or removed from
     * the uplink queue.
     * @param file: file to operate on (source of uplinking file)
     * @param action: action to perform "Cancel" or "Remove"
     * @return {Promise<any> | number | Promise<boolean> | void | boolean}
     */
    command(file, action) {
        return _loader.load("/upload/files", "PUT", {"source": file, "action": action});
    }
};
export let _uploader = new Uploader();
