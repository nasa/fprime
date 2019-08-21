import {filter} from "./utils.js";

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
        }
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
            if (typeof (this.itemToColumns) === "function") {
                return this.itemToColumns(this.item);
            }
            throw Error("Failed to define required 'itemToColumns' function on fp-table")
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
        }
    },
    // Required data items (unique for each table instance)
    data: function() {return {matching: [], editing: false, view: []}},
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
         * Clears everything from a view.  Everything will be displated.
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
         * Calculates a list of displayed items.
         */
        filteredItems: function() {
            // Map loacal methods into the closure provided to the filtering function
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