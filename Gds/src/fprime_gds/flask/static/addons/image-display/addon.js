/**
 * ImageDisplay: an example component addon.
 *
 * This file defines the "image-display" component used to display the last downlinked image in the
 * view. Custom components are designed using Vue.js and typically inline the template.
 */
import {_datastore} from "../../js/datastore.js";

Vue.component("image-display", {
    // Note: inlined template, defining exactly 1 top level element.
    template:
        "<div><img :src='lastImage' alt='No images downlinked' /></div>",
    // Note: choose and name your data items here
    data: function() {
        return {"downlinked": _datastore.downfiles};
    },
    computed: {
        /**
         * Compute the last image downlinked for use within the above source. Returns just the file
         * name for use with the above image source
         */
        lastImage: function() {
            let regex = /.*[\\\/]/;
            let exts = [".png", ".jpg"];
            let images = this.downlinked.filter(item => {
                    for (let i = 0; i < exts.length; i++) {
                        if (item.destination.endsWith(exts[i])) {
                            return true;
                        }
                    }
                    return false;
                }
            );
            // Return with nothing
            if (images.length == 0) {
                return "";
            }
            let last = images[images.length - 1].destination.replace(regex, "");
            return "/download/files/" + last;
         }
    }
}
);
