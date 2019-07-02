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
export class SimpleTemplate {
    /**
     * Creation of a template requires an element that is the container for the output html from the template. In
     * addition, an HTML element or string containing HTML is read in to provide the templated code, and (optionally)
     * a data object can be assigned for the templating.
     * @param element: element to write to, or ID of element to write to
     * @param template: HTML used as output.  DO NOT SUPPLY unsanitized user input.
     * @param data: data to bind to, or undefined if to be registered later.
     */
    constructor(element, template, data) {
        this.forreg = /for\s+([a-zA-Z0-9_]+)\s+in\s+([a-zA-Z0-9_]+)/;
        this.mustreg = /{{(.*)}}/;
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
        this._registerEvents();
        // Read the interHtml, if defined, otherwise treat as string
        if (!(template instanceof HTMLElement)) {
            template = document.getElementById(template);
        }
        this.template = template;
    }

    /**
     * Register the event handlers to handle several different events.
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
            let newNode = this._template(this.template, this.data);
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
     * @private
     */
    _template(tempNode, data) {
        let dataBind = null;
        let match = null;

        // Text nodes template the text and return the result.
        if (tempNode.nodeType == Node.COMMENT_NODE) {
            return document.createDocumentFragment();
        }
        else if (tempNode.nodeType == Node.TEXT_NODE) {
            return this._templateText(tempNode, data);
        }
        // Check if has data binding property. Only supports "for x in y"
        else if ((dataBind = tempNode.getAttribute("data-bind")) != null && (match = dataBind.match(this.forreg)) != null) {
            let itemList = data[match[2]];
            let fragment = document.createDocumentFragment();
            // Loop over all elements
            for (let i = 0; i < itemList.length; i++) {
                // Assign subdata and clone node
                let newData = {};
                newData[match[1]] = data[match[2]][i];
                let newNode = tempNode.cloneNode(false);
                // Loop over all children
                for (let j = 0; j < tempNode.childNodes.length; j++) {
                    newNode.appendChild(this._template(tempNode.childNodes[j], newData));
                }
                fragment.appendChild(newNode);
            }
            return fragment;
        }
        else {
            let newNode = tempNode.cloneNode(false);
            newNode.removeAttribute("hidden");
            // Loop over all children
            for (let j = 0; j < tempNode.childNodes.length; j++) {
                newNode.appendChild(this._template(tempNode.childNodes[j], data));
            }
            return newNode;
        }
    }
    /**
     * Templates text based on a mustashe style template. Everything inside the template is evalued and sub-in.
     * @param child: child node to clone
     * @param data: used as template arguments
     * @private
     */
    _templateText(child, data) {
        let content = child.textContent;
        let match = child.textContent.match(this.mustreg);
        if (match != null) {
            let argList = Object.keys(data);
            argList.push("return " + match[1] + ";");
            let jsEval = Function.apply(null, argList);
            let result = jsEval.apply(null, Object.values(data));
            content = content.replace(this.mustreg, result);
        }
        let clone = child.cloneNode(false);
        clone.textContent = content;
        return clone;
    }
}