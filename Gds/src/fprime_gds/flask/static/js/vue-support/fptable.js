import {filter, toArrayIfString} from "./utils.js";

/**
 * fp-row:
 *
 * Component representing a single row of the table. This allows independent styling and computed functions applied to
 * the row. Will be composed (en masse) into the full fp-table. It has several properties that are required to be
 * supplied:
 *
 * -item: a single item object to bind into the row
 * -itemToColumns: a function harvesting out data from an item into the columns
 */
Vue.component("fp-row", {
    template: "#fp-row-template",
    props: {
        /**
         * item:
         *
         * 'item' will be automatically bound to each item in the 'items' list of the consuming table. It is the loop
         * variable, and will be passed into the 'itemToColumns' function to produce columns.
         */
        item: Object,
        /**
         * itemToColumns:
         *
         * 'itemToColumns' will be bound to a function taking one item from the parent fp-table object. See fp-table.
         */
        itemToColumns: Function,
        /**
         * rowStyle:
         *
         * 'rowStyle' should be bound to a string of static style for the given row, or a Function taking a single item
         * as input and used to calculate that style. If a Function is used, it should return a string. The String or
         * return value must be compatible with the HTML class attribute. This will be passed to the child fp-row.
         *
         * See: fp-table
         */
        rowStyle: [String, Function],
        /**
         * 'editing':
         *
         * Has the parent entered editing mode to select rows for views.
         */
        editing: Boolean,
        /**
         * inView:
         *
         * A boolean showing if this row is in the current view.
         */
        inView: {
            type: Boolean,
            default: true
        },
        /**
         * Action to perform when the clicked row has been clicked.
         */
        clickAction: {
            type: Function,
            default: (item) => {},
        },
        /**
         * An array of indices that are visible.
         */
        visible: {
            type: Array,
            default: null
        },
    },
    methods: {
        /**
         * Function handling inputs on child and emitting them back to the parent to select this row as selected. This
         * allows the parent to track the selected list.
         * @param event: event kicking off this change
         */
        onInput: function(event) {
            this.$emit("row-checked", {"child": this.item, "value": event.target.checked});
        }
    },
    computed: {
        /**
         * Calls the itemToColumns function to return the columns Array. This is *required* and will raise an error if
         * the itemToColumns variable has not been bound to.
         */
        calculatedColumns: function () {
            if (typeof (this.itemToColumns) !== "function") {
                throw Error("Failed to define required 'itemToColumns' function on fp-table")
            }
            return this.itemToColumns(this.item).filter((item, index) => this.visible == null || this.visible.indexOf(index) != -1);
        },
        /**
         * Calculates the style of the row based on a given item. This is optional and will not raise an error if the
         * rowStyle function has not been bound to.
         */
        calculateStyle: function () {
            if (typeof (this.rowStyle) === "function") {
                return this.rowStyle(this.item);
            }
            return this.rowStyle;
        }
    }
});

/**
 * file-row:
 *
 * File row Vue component. This is used to display rows that represent file data in the GDS. This includes sources,
 * destinations, and progress bars.
 */
Vue.component("file-row", {
    template: "#file-row-template",
    props: {
        /**
         * item:
         *
         * 'item' will be automatically bound to each item in the 'items' list of the consuming table. It is the loop
         * variable, and will be passed into the 'itemToColumns' function to produce columns.
         */
        item: Object,
        /**
         * itemToColumns:
         *
         * 'itemToColumns' will be bound to a function taking one item from the parent fp-table object. See fp-table.
         */
        itemToColumns: Function,
    },
    methods: {
        /**
         * Performs an action on a given file. This allows for the "Remove" and "Cancel" actions to be sent via the
         * uploader to control the file's state on the server. If the file is being curated in JavaScript before uplink
         * was started, then it will just be removed locally.
         * @param event: button event to look at for the action.
         */
        fileAction(event) {
            let action = event.currentTarget.innerText;
            let uplinkvue = this.$parent.$parent;
            let index = uplinkvue.selected.indexOf(this.item);
            // Local javascript file, removeit from the selected (curation) list
            if (action == "Remove" && index != -1) {
                uplinkvue.selected.splice(index, 1);
            } else {
                uplinkvue.uploader.command(this.item.source, action);
            }
        }
    },
    computed: {
        /**
         * Calculates the basename of the given file. This is used for cases where the REST interface needs to be
         * supplied a filename in the URL i.e. GET /files/filename1.  This strip all characters up to the last / or \.
         * @return {string} full path to file (source, destination)
         */
        basename() {
            let regex = /.*[\\\/]/;
            return this.item.destination.replace(regex, "")
        }
    }
});

/**
 * fp-table:
 *
 * F´ table used to render F´ objects in a table format. This provides for filtering and rendering capabilities in a
 * tabular format. Several items are required to use the fp table:
 *
 * - headerColumns: list of header column values
 * - items: items bound to the table.  Each will be made 1 row.
 * -itemToColumns: function to convert an item object to a list of row values. Passed to fp-row.
 * -itemToUnique: function to supply a unique key from an item
 * -rowStyle: (optional) a static string, or function used to generate a string passed to the HTML "class" attribute
 */
Vue.component("fp-table", {
    template: "#fp-table-template",
    //Properties used by fp-table
    props: {
        /**
         * initialFields:
         *
         * 'initialFields' is an accept-list of fields (columns) to display. Use null if all fields should be displayed. If not
         * supplied default is null such that all fields will be printed.
         */
        initialFields: {
            type: [Array, String],
            default: null
        },
        /**
         * headerColumns:
         *
         * 'headerColumns' should be bound to an Array of static column header values or a Function that will be used to
         * generate those header values on the fly. Functions should not expect to receive any input arguments.
         */
        headerColumns: [Array, Function],
        /**
         * items:
         *
         * 'items' should be bound to an Array of objects. Each object will be passed to an individual row. These items
         * must be compatible with the 'itemToColumns' property.
         */
        items: Array,
        /**
         * itemToColumns:
         *
         * 'itemToColumns' should be bound to a function taking one item from the above array as input, and returning
         * a Array of column values for display. This will be performed in the row sub-component.
         */
        itemToColumns: Function,
        /**
         * itemToUnique:
         *
         * 'itemToUnique' should be bound to a function that takes in an individual data item, and returns a unique key
         * for the purposes of setting the v-key attribute.
         */
        itemToUnique: Function,
        /**
         * rowStyle:
         *
         * 'rowStyle' should be bound to a string of static style for the given row, or a Function taking a single item
         * as input and used to calculate that style. If a Function is used, it should return a string. The String or
         * return value must be compatible with the HTML class attribute. This will be passed to the child fp-row.
         */
        rowStyle: [String, Function],
        /**
         * supportViews:
         *
         * 'supportViews' allows the user to support views into the data. This allows the rows of the table to be
         * permanently subset to show only certain items matching the view. This subsetting (unlike filtering) is an
         * exact match on a supplied column.
         *
         * default: false
         */
        supportViews: {
            type: Boolean,
            default: false
        },
        /**
         * compact:
         *
         * 'compact' allows the user to hide filters/buttons/headers/etc. to
         * only show the table itself for a cleaner view
         *
         * default: false
         */
        compact: {
            type: Boolean,
            default: false
        },
        /**
         * itemToViewName:
         *
         * 'itemToViewName' provides a function that converts from an item to a view's name. This allows the views to
         * be saved cleanly in human readable text. The default grabs the item's id, as all things have ids.
         */
        itemToViewName: {
            type: Function,
            default: function(item) {
                return item.id;
            }
        },
        /**
         * itemHide:
         *
         * 'itemHide' provides a function for hiding objects when no in in-view mode and editing mode. This function can
         * take the item and return true or false to hide this item.
         */
        itemHide: {
            type: Function,
            default: function () {
                return false;
            }
        },
        /**
         * Display template to use for the row. Override with fp-row with something else for uplink/downlink file
         * displays.
         */
        displayTemplate: {
            default: "fp-row"
        },
        /**
         * Action to perform when the clicked row has been clicked.
         */
        clickAction: {
            type: Function,
            default: (item) => {},
        },
        /**
         * Action to perform when the clear rows button has been clicked.
         */
        clearRows: {
            type: Function,
            default: null
        },
        /**
         * The search text to initialize the filter with (defaults to nothing)
         */
        filterText: {
            type: String,
            default: ""
        },
        /**
         * The initial views to show in this table, if views are enabled
         * (defaults to empty list)
         */
        initialViews: {
            type: [Array, String],
            default: []
        }
    },
    // Required data items (unique for each table instance)
    data: function() {
        return {
            matching: (this.filterText) ? [this.filterText] : [],
            editing: false,
            // use Vue.util.extend to copy by data, not by reference
            view: Vue.util.extend([], toArrayIfString(this.initialViews)),
            fields: Vue.util.extend([], toArrayIfString(this.initialFields))
        }
    },
    methods: {
        /**
         * Process the checked-child message. This should add or remove names from the view.
         * @param message: message to process
         */
        checkedChild: function(message) {
            let vname = this.itemToViewName(message.child);
            if (message.value && this.view.indexOf(vname) == -1) {
                this.view.push(vname);
            } else if (!message.value && this.view.indexOf(vname) != -1) {
                this.view.splice(this.view.indexOf(vname), 1);
            }
            this.$refs.allbox.indeterminate = true;
        },
        /**
         * Clears everything from a view.  Everything will be displayed.
         */
        clearView: function() {
            this.view = [];
        },
        /**
         * Saving the view removes the user from edit mode. Thus allowing them to see the refined view.
         */
        saveView: function() {
            this.editing = false;
        },
        /**
         * Puts the user into view-edit mode. Thus allowing the user to set channels.
         */
        editViews: function () {
            this.editing = true;
        },
        /**
         * Adds everything into the view
         * @param e: event to use
         */
        checkAll: function(e) {
            let state = e.target.checked;
            let itemToColumns = this.itemToColumns;
            let filtered = filter(this.items, this.matching,
                function(item) {
                    return itemToColumns(item).join(" ");
                });
            for (let i = 0; i < filtered.length; i++) {
                this.checkedChild({child: filtered[i], value: state});
            }
            // Clear intermediate state
            this.$refs.allbox.indeterminate = false;
        },
        /**
         * Determines if the current item is in the current view
         * @return {boolean}
         */
        inView(item) {
            return !this.supportViews || this.view.indexOf(this.itemToViewName(item)) != -1;
        },
        /**
         * Function used to read a file an input it as a view.
         * @param event: event to represent a file load
         */
        readFile(event) {
            // Check file size and read
            let file = event.target.files[0];
            let _self = this;
            if (file.size < 50 * 1024) {
                let filer = new FileReader();
                filer.readAsText(file);
                filer.onload = function (something) {
                    if (FileReader.DONE == filer.DONE) {
                        let splits = filer.result.split(/\s/);
                        for (let i = 0; i < splits.length; i++) {
                            splits[i] = splits[i].trim();
                        }
                        _self.view = splits;
                    }
                };
            } else {
                console.error("[ERROR] File too big. Try again champ");
            }
        }
    },
    // Computed items
    computed: {
        /**
         * visibleIndices:
         *
         * Computes the visible indices from the fields to show. This allows the sub row to restrict the columns based
         * on what header fields are used.
         *
         * @return {null|Uint8Array}
         */
        visibleIndices: function() {
            if (this.fields == null || this.fields.length == 0) {
                return null;
            }
            return this.fields.map(field => this.headerColumns.indexOf(field)).filter(index => index != -1);
        },
        /**
         * calculatedHeaderColumns:
         *
         * Computes the visible headers based on the fields to show. This allows this element to shrink to a specified
         * set of columns.
         *
         * @return {null|Uint8Array}
         */
        calculatedHeaderColumns: function() {
            // Check for null full-display
            if (this.fields == null || this.fields.length == 0) {
                return this.headerColumns;
            }
            return this.fields.filter(field => this.headerColumns.indexOf(field) != -1);
        },
        /**
         * Calculates a list of displayed items.
         */
        filteredItems: function() {
            // Map local methods into the closure provided to the filtering function
            let itemToColumns = this.itemToColumns;
            // Pre-filter step removes non-viewable items
            let items = [];
            for (let i = 0; i < this.items.length; i++) {
                let item = this.items[i];
                // Visible if editing
                if (this.editing) {
                    items.push(item);
                }
                // Visible if in no views selected and not hidden
                else if (this.view.length == 0 && !this.itemHide(item)) {
                    items.push(item);
                }
                // Visible if not hidden and not supporting views
                else if (!this.supportViews && !this.itemHide(item)) {
                    items.push(item);
                }
                // Visible if in the view, always
                else if (this.view.indexOf(this.itemToViewName(item)) != -1) {
                    items.push(item);
                }
            }
            // Now filter items based on removable filters
            return filter(items, this.matching,
                function(item) {
                    return itemToColumns(item).join(" ");
                });
        },
        /**
         * Generates an href that can be used to download a file.  Keep the download up-to-date with the view.
         * @return {string} href string bound to href attribute
         */
        viewHref: function() {
            return 'data:text/plain;charset=utf-8,' + encodeURIComponent(this.view.join("\n"));
        }
    },
    // Makes the table sortable after creation using post creation updates
    updated: function() {
        /**
         * A function that registers a next-tick callback to make tables sortable. This allows us to manually convert
         * a table to a sorttable after data is rerendered.
         */
        this.$nextTick(function() {
            // Check existence of the third party library
            if (typeof(sorttable) !== "undefined") {
                let tables = this.$el.getElementsByTagName("table");
                for (let i = 0; i < tables.length; i++) {
                    sorttable.makeSortable(tables[i]);
                }
            } else {
                console.warn("sortable.js not found, not attempting to sort tables");
            }
        })
    }
});
