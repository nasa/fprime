/**
 * simple-templates.js:
 *
 * Provides a very simple set of JavaScript utilities for UI development. At bare minimum this simple templates scheme
 * is used to provide the following features:
 *
 *   1. Templating, from a string of HTML or from an HTML element.
 *   2. Data-binding, simple two-way binding used to keep a view and data-modle in-sync.
 *
 * As of 2019, the JavaScript community has produced many frameworks for the purposes of building single-page apps, but
 * these frameworks often include a *huge* number of dependencies that come with security vulnerabilities, unanticipated
 * packages, and collisions. All of this results in "taking a tank to fetch your groceries" (Mark, 2019).  It is
 * overkill and represents a maintainability headache. Hench the creation of this file, no dependencies, simple, and
 * requiring no maintenance.
 *
 * @author mstarch
 */
let __SIMPLE_TEMPLATES_RERENDER_LIST = {};

/**
 * Rerenders attached templates that are affected by the given reference.
 * @param reference: reference to force rerender
 */
function __ST_rerenderChanged(reference) {
    let updates = __SIMPLE_TEMPLATES_RERENDER_LIST[reference];
    for (let key in ((typeof (updates) == "undefined") ? [] : updates)) {
        updates[key].rerender();
    }
}

/**
 * Register global update paths for simple templates.
 * @param template: template to rerender
 * @param reference: reference to trigger it
 * @private
 */
function __ST_registerReference(template, reference) {
    // Create a list on first call
    if (typeof (__SIMPLE_TEMPLATES_RERENDER_LIST[reference]) === "undefined") {
        __SIMPLE_TEMPLATES_RERENDER_LIST[reference] = [];
    }
    // Don't reregister
    if (__SIMPLE_TEMPLATES_RERENDER_LIST[reference].indexOf(template) == -1) {
        __SIMPLE_TEMPLATES_RERENDER_LIST[reference].push(template);
    }
}

export class SimpleTemplate {
    FOR_REGEX = /for\s+([a-zA-Z0-9_]+)\s+in(.*)/;
    TPL_REGEX = /{{(.*)}}/;

    /**
     * Creation of a template requires an element that is the container for the output html from the template. In
     * addition, an HTML element or string containing HTML is read in to provide the templated code, and (optionally)
     * a data object can be assigned for the templating.
     * @param element: element to write to, or ID of element to write to
     * @param template: HTML used as output.  DO NOT SUPPLY unsanitized user input.
     * @param data: data to bind to, or undefined if to be registered later.
     */
    constructor(element, template, data) {
        this.data = data;
        this.element = element;
        // Check if the element is undefined
        if (typeof(element) === "undefined") {
            throw TypeError("Templating element not supplied valid DOMElement or id of valid DOMElement");
        } else if (!(element instanceof HTMLElement)) {
            this.element = document.getElementById(element);
            if (this.element == null) {
                throw TypeError("Invalid element id supplied to template: " + element);
            }
        }
        // Read the interHtml, if defined, otherwise treat as string
        if (!(template instanceof HTMLElement)) {
            template = document.getElementById(template);
        }
        this.template = template;
    }

    /**
     * Registers events "change", "mousedown", and "click" to the given element. Wraps the given data in
     * @param element: element to register for events
     * @param data: data attribute to read for passing in data
     * @private
     */
    _registerEvents(element, data) {
        let _self = this;
        // Override defaults
        if (typeof(element) === "undefined") {
            element = this.element;
        }
        if (typeof(data) === "undefined") {
            data = this.data;
        }
        // Invent a closure around the above data
        let closureHandler = function () {
            _self.handleEvent()
        };

        // Events
        this.element.addEventListener("change", this, false);
        this.element.addEventListener("mousedown", this, false);
        this.element.addEventListener("click", this, false);
    }



    /**
     * Element ID to rerender w.r.t. the driven data.
     * @param id: (optional) rerender this sub ID, or the current element if not supplied.
     */
    rerender(id) {
        // No ID values
        var data = this.data;
        var elem = this.element;
        // If ID is undefined, then
        if (typeof(id) !== "undefined") {
            data = this.data[id];
            elem = this.element.getElementById(id);
            elem.value = data;
        } else {
            // Create a new node that is our template
            let newNode = this._template(this.template, this.data, true);
            // With active children replace last always
            if (this.element.childNodes.length >= 1) {
                this.element.replaceChild(newNode, this.element.lastChild);
            }
            // Zero children, append
            else {
                this.element.appendChild(newNode);
            }
        }
    }

    /**
     * Handle the on-change event. This has incoming data binding.
     * @param event: on-change event
     */
    handleEvent(eventType, element, data) {
        //
        switch (eventType) {
            case "change":
                //this._update(event.currentTarget);
                break;
            case "click":
                break;
            default:
                throw Error("Unhandled event: " + eventType);
        }
    }

    /**
     * Register a click. When this element is clicked, the
     * @param element:
     * @param data:
     * @private
     */
    _registerClick(element, data) {
        let _self = this;
        let clickFun = function(event) {
            let key = element.getAttribute("click-action");
            let reference = _self.data[key];
            _self.data[key] = data;
            __ST_rerenderChanged(reference);
            return true;
        };
        element.addEventListener("click", clickFun, false);
    }


    /**
     * Update data from element change
     * @private
     */
    _update(element) {
        // Non-sub-element
        if (this.element == element) {
            this.data = element.value;
            rerender();
        }
        // Subdata, strip id
        else if (element.id in data) {
            data[element.id] = element.value;
            rerender(element.id);
        }
        // Data-binding fail
        else {
            throw Error("Unable to find data item for id: " + element.id);
        }
    }

    /**
     * Run the templating function.
     * @param tempNode: template node used as an input for templates.
     * @param data: data map to reference for elements. Will read directly from data bindings if not available.
     * @param top: top-level node, strip "hidden"
     * @private
     */
    _template(tempNode, data, top) {
        let dolly = null;
        top = (typeof(top) !== "undefined") ? top : false;
        // Comments essentially get stripped from templates
        if (tempNode.nodeType == Node.COMMENT_NODE) {
            return document.createDocumentFragment();
        }
        // Handle text nodes, as they are barren and without attributes
        else if (tempNode.nodeType == Node.TEXT_NODE) {
            let currentNode = tempNode.cloneNode(false);
            currentNode.textContent = this._templateText(currentNode.textContent, data);
            return currentNode;
        }
        // Multiply the node
        else {
            dolly = this._templateCloneMultiply(tempNode, data);
        }
        // Loop over all nodes, templating the attributes, and children nodes
        for (let i = 0; i < dolly.childNodes.length; i++) {
            let currentNode = dolly.childNodes[i];
            // Strip the hidden attribute from our template
            if (top) {
                currentNode.removeAttribute("hidden");
            }
            // Evaluate data based on item in list
            let newData = Object.assign({}, data);
            if (currentNode.hasAttribute("data-eval") && currentNode.hasAttribute("data-name")) {
                newData[currentNode.getAttribute("data-name")] = this._templateEvaluation(currentNode.getAttribute("data-eval"), newData);
            }
            // Data Bind elements update when changed
            if (currentNode.hasAttribute("data-bind") && top) {
                let reference = this._templateEvaluation(currentNode.getAttribute("data-bind"), newData);
                __ST_registerReference(this, reference);
            }
            if (currentNode.hasAttribute("click-action")) {
                let evalu = newData;
                // Get the attached data
                if (currentNode.hasAttribute("data-eval") && currentNode.hasAttribute("data-name")) {
                    evalu = this._templateEvaluation(currentNode.getAttribute("data-eval"), newData);
                }
                this._registerClick(currentNode, evalu);
            }
            // Template out attributes and child nodes
            this._templateAttributes(currentNode, newData);
            for (let j = 0; j < tempNode.childNodes.length; j++) {
                currentNode.appendChild(this._template(tempNode.childNodes[j], newData));
            }
        }
        return dolly;
    }

    /**
     * Clone/multiply nodes with conditionals and repeats thrown in.
     * @param tempNode: template node used for templating
     * @param data: data to use when multiplying and running conditionals
     * @return {DocumentFragment}: the newly cloned node(s)
     * @private
     */
    _templateCloneMultiply(tempNode, data) {
        let match = null;
        let repeatAttr = null;

        let fragment = document.createDocumentFragment();
        // First process repeats in order to get a set of
        if ((repeatAttr = tempNode.getAttribute("repeat")) != null && (match = repeatAttr.match(this.FOR_REGEX))) {
            let itemList = this._templateEvaluation(match[2], data);
            // Loop over all elements in the repeat list, clone the template node, and add clones to list
            for (let key in itemList) {
                // Subset data and assign it to the node
                let newData = Object.assign({}, data);
                newData[match[1]] = itemList[key];
                // Short circuit if any "if" clause would fail
                if (tempNode.hasAttribute("if") &&
                    !this._templateEvaluation(tempNode.getAttribute("if"), newData)) {
                    continue;
                }
                // Clone the node and add it to the result set
                let newNode = tempNode.cloneNode(false);
                fragment.appendChild(newNode);
                newNode.setAttribute("data-name", match[1]);
                newNode.setAttribute("data-eval", match[2]+"["+ key + "]");
            }
        }
        // Otherwise add a single clone of the template given the "if" evaluates properly
        else if (!tempNode.hasAttribute("if") ||
                 this._templateEvaluation(tempNode.getAttribute("if"), data)) {
            let newNode = tempNode.cloneNode(false);
            fragment.appendChild(newNode);
        }
        return fragment;
    }

    /**
     * Evaluate the expression inside a mustache teamplate producing a JS result.
     * @param matchedText: text matched inside template
     * @param data: data to put into the evaluation
     * @return {result of evaluation|null}
     * @private
     */
    _templateEvaluation(matchedText, data) {
        try {
            let argList = Object.keys(data); // Get all keys from data and prepare them as names
            argList.push("return " + matchedText + ";"); // Add an argument that is the evaluation itself
            let jsEval = Function.apply(null, argList); // Create the function object
            return jsEval.apply(null, Object.values(data)); // Run the evaluation, and produce result
        } catch (err) {
            console.error("Invalid template evaluation: '" + matchedText + "' caused error: " + err);
        }
        return null;
    }

    /**
     * Takes a string and replaces the mustache templates within it to the equivalent evaluation of the expression
     * inside the template. For example: "owl-{{ 1 }}" becomes "owl-1" and "owl-{{ 15.toString(16) }}" becomes owl-f.
     * @param text: text searched for the replacement.
     * @param data: used as template arguments
     * @private
     */
    _templateText(text, data) {
        let match = text.match(this.TPL_REGEX);
        if (match != null) {
            let result = this._templateEvaluation(match[1], data);
            if (result != null) {
                text = text.replace(this.TPL_REGEX, result);
            }
        }
        return text;
    }

    /**
     * Loops over a node's attributes and replaces all the mustache templates with the evaluation of the inside of that
     * template. This only updates attributes, it does not clone nor creat new nodes.
     * @param newNode: node to update attributes
     * @param data: data to use when templating
     * @private
     */
    _templateAttributes(newNode, data) {
        // Loop over all attributes, only if an attribute exists
        if (newNode.hasAttributes()) {
            for (let i = 0; i < newNode.attributes.length; i++) {
                newNode.setAttribute(newNode.attributes[i].name, this._templateText(newNode.attributes[i].value, data));
            }
        }
        return newNode;
    }
}