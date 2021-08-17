# GDS Custom Add-Ons

The HTML GUI of the GDS is built around [Vue.js](https://vuejs.org/) components. Although fprime
defines a number of existing components that can be added to a custom dashboard, some users need to
build custom vue displays.  If the display is purely HTML it can be done directly in the dashboard
but should custom code be required, a Vue.js component needs to be created.  This can be done using
the add-on system.

The add-on system allows developers to create add-ons, register them, and then they are available
for use within the custom dashboards. This gives access to all of Vue.js features for custom
dashboards.

## Implementation of an Add-On

An addon for fprime just adds a custom JS file under the "flask/src" directory in the Gds system.
This JS file defines a Vue.js component in javascript with the HTML template and data top-level
items.


This file is typically placed in a folder named for the addon (e.g. `example`) and the JS file is
typically named `addon.js`. A quick example can be seen below.

```
Vue.component("example", {
    template: "<div> template {{ key1 }} goes here</div>",
    data: function() {"key1": "HTML"},
}
```

The fprime data structures (Channels, Events, etc.) can be tapped into using the datastore object
provided as a singleton from `datastore.js`. This singleton can be data-bound locally to receive
updates as new data arrives.  Sample data binding is shown below.


```
// Import the datastore
import {_datastore} from "../../js/datastore.js";

Vue.component("example", {
    // Add fprime Event count to our output template
    template: "<div>Event count: {{ events.length }}</div>",
    // Create local databinding from _datastore.events to events
    data: function() {"events": _datastore.events},
}
```

Finally, most add-ons will provide a dashboard showing off the addon and usage therein. A very basic
dashboard can be seen below. The name of the component becomes the HTML tag available in the
dashboard. The dashboard file should be an XML file and is usually placed in the same directory as
the above JS file.

```
<dashboard-box title="Example" border-color="black">
    <example></example>
</dashboard-box>
```

The last step is to import the addon in the `enabled.js` file found in the `addons/` directory. See below.

```
import "./example/addon.js"
```

## Vue.js Tips

Although this documentation is not intended to give a full account of how to use [Vue.js](https://vuejs.org/), it will provide a few tips.

1. Always define the HTML template with a single child element
2. `data` should be a function returning an object. This allows the component to be reused.
3. Choose a unique component name.
4. Vue.js usually runs in debug mode allowing for issues to appear in the JavaScript console.

Available data from `_datastore` is described in the following table

| Item | Type | Description |
|---|---|---|
| events    | list | List of events received |
| channels  | dict | Channel id to last received reading |
| upfiles   | list | List of uplinked file objects |
| downfiles | list | List of downlinked file objects |

A full example can be seen in the addons/image-display folder.
