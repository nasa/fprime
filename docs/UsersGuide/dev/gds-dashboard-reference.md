# GDS Dashboard Component Reference

The GDS dashboard is an XML specification for designing custom views into the GDS. This guide will walk through each
custom component that can be used in this specification. Properties are specified as XML attributes.  This specification
in reality is HTML and thus HTML tags may be used. However, for best results, the user should stick to these components
where possible.

In this guide:

- [Available Components and Properties](#available-components-and-properties)
- [Dashboard Layout Components](#dashboard-layout-components)
    - [dashboard-box](#dashboard-box)
    - [dashboard-row](#dashboard-row)
- [Functional Components](#functional-components)
    - [command-input](#command-input)
    - [command-history](#command-history)
    - [event-list](#event-list)
    - [channel-table](#channel-table)
- [Misc Components](#misc-components)
    - [logging](#logging)


## Available Components and Properties

The dashboard is composed of components and properties set on those components. This section will give a quick overview
of the properties and the components that use them.  Further explanation of the components is available below.

The following is the list of all properties defined by the XML dashboard specification. These properties have types that
help the user enter them and these types are implicit to the property:

| Property Type | Description | Example |
|---|---|---
| string  | text string                                        | title="My fancy title" |
| list    | Space separated list. Items may be quoted with (') | fields="Field1 'Field 2' 'Field 3'|
| keyword | Just a keyword, no value required                  | compact |

The following are the properties available, the type of the property from the above table, and a description of valid
values.

| Property | Description | Type | Valid values |
|---|---|---|---|
| title        | Title added as a label for box      | string  | any |
| color        | Background color applied to box     | string  | [Any CSS color](https://www.w3.org/wiki/CSS/Properties/color/keywords) |
| border-color | Color of border around the box      | string  | [Any CSS color](https://www.w3.org/wiki/CSS/Properties/color/keywords) |
| fields       | List of fields (columns) shown      | list    | any column field names |
| filter-text  | Text to filter displayed items      | string  | any |
| items-shown  | Names of items to restrict display  | list    | any |
| compact      | Use minimalist display of component | keyword | |

Finally, here is a list of available components and which properties they allow.

| Component | Function | Available F´ Fields |
|----|----|----|
| [<dashboard-row>...</dashboard-row>](#dashboard-row) | Horizontal container for other components | |
| [<dashboard-box>...</dashboard-box>](#dashboard-box) | Box container for other components        | title  |
| [<command-input/>](#command-input)     | Command builder and transmission component  | fields, filter-text, items-shown, compact |
| [<command-history/>](#command-history) | List of commands sent as tracked by the GDS | fields, filter-text, items-shown, compact |
| [<event-list/>](#event-list)       | Ordered list of events received by the GDS      | fields, filter-text, items-shown, compact |
| [<channel-table/>](#channel-table) | Latest channel readings as received by the GDS  | fields, filter-text, items-shown, compact |

## Dashboard Layout Components

There are dashboard layout components, which help users organize their dashboard. Apart from constraining the layout,
these components provide no additional functionality. To understand these components and their properties, there are
two notes to help clarify:

- Colors are specified as a [named CSS color](https://www.w3.org/wiki/CSS/Properties/color/keywords)
- All properties have defaults and are not required

### dashboard-box

The `dashboard-box` acts like a container where other elements are put inside, helping to visually group items together.
This allows the user to specify arbitrary blocks of components that may optionally define a title, background color, and
border color. An example is shown below.

```xml
<dashboard-box title="A Box!" color="gray" border-color="black">
    <!-- ...other content goes inside... -->
    <command-input/>
    ...
</dashboard-box>
```

`dashboard-box` supports the following properties:
- `title`: A text label to give the box, appearing at the top-left corner (defaults to nothing)
- `color`: The background color to give the box (defaults to transparent)
- `border-color`: The color to give the border of the box (defaults to transparent)

### dashboard-row

The `dashboard-row` is a component for horizontal grouping. This can be used to force components or boxes to align
horizontally across the screen. Without the component, groupings would be arrayed vertically. The container itself is
invisible.

```xml
<dashboard-row>
    <!-- ...other content goes inside... -->
    <dashboard-box>...</dashboard-box>
</dashboard-row>
```

The `dashboard-row` allows no properties.

## Functional Components

The F´ dashboard allows for several functional components as well. These components allow some access to the GDS data,
and through the GDS, access to the embedded system.  These components manipulate channels, events, and commands. File
handling components are currently not supported as they are better suited for a full view.

### command-input

`command-input` is used to build and send commands to the embedded system through the GDS. It consists of a command
drop-down list, argument input boxes, a clear, and a send button.  It is seen on the top of the `Commands` tab. The
input box is searchable. An example is shown below:

```xml
<command-input/>
```

`command-input` allows no properties.

### command-history

`command-history` is a list of commands and arguments sent to the embedded system. It is important to note that this is
tracked through the GDS, and these commands may have not been successfully received by the embedded system. This list
can be filtered for command types, and reduced to a set of fields to be displayed.  Use the [`event-list`](#event-list)
component with a filter for command severity to display commands as received by the embedded system.  An example of the
`command-history` component is below:

```xml
<command-history
    fields="'Command Time' 'Command Id' 'Command Mnemonic' 'Command Args'"
    filter-text="cmdDisp"
    items-shown="cmdDisp.CMD_NO_OP health.HLTH_CHNG_PING"
    compact/>
```

`command-history` supports the following properties:

-   `fields`: A list of columns to *show*. The supplied columns must exactly match column headers (default: show all)
-   `filter-text`: Initial filter text to filter items. Any items without string are hidden. (default: no filter)
-   `items-shown`: A list of command mnemonics to display. Command mnemonics must exactly match. (default: show all)
-   `compact`: Display using the compact version of this component.

### event-list

`event-list` displays a set of events as received by the GDS. This allows the user to monitor for either all commands, or for specific events. In addition, a filter may be used to look for certain severities. An example of the
`event-list` component is shown below:

```xml
<event-list
    fields="'Event Time' 'Event Id' 'Event Name' 'Event Severity' 'Event Description'"
    filter-text="WARNING_LO"
    items-shown=""
    compact/>
```

`event-list` supports the following properties:

-   `fields`: A list of columns to *show*. The supplied columns must exactly match column headers (default: show all)
-   `filter-text`: Initial filter text to filter items. Any items without string are hidden. Use this for filtering by
     severity or any arbitrary message text. (default: no filter)
-   `items-shown`: A list of event names to display. Event names must exactly match. (default: show all)
-   `compact`: Display using the compact version of this component.

### channel-table

`channel-table` displays the most recent reading of a channel's value. It provides a near real-time look at the channels
in the embedded system as received by the GDS. Often users use the `items-shown` property to view specific channels of
interest and the `fields` property can be used to remove undesired information like channel id.  An example is shown
below:

```xml
<channel-table
    fields="'Last Sample Time' 'Channel Id' 'Channel Name' 'Channel Value'"
    filter-text=""
    items-shown="cmdDisp.CommandsDispatched"
    compact/>
```

`channel-table` supports the following properties:

-   `fields`: A list of columns to *show*. The supplied columns must exactly match column headers (default: show all)
-   `filter-text`: Initial filter text to filter items. Any items without string are hidden. (default: no filter)
-   `items-shown`: A list of channel names to display. Channel names must exactly match. This is typically used to set
     a specific subset of channels to monitor. (default: show all)
-   `compact`: Display using the compact version of this component.

## Misc Components

This is a set of other components that are not typically used but are nonetheless available.

### logging

The logging component typically lives in the logging tab of the GDS.  It may be used to show logs in a dashboard, but
this feature is rarely used.

```xml
<logging/>
```

`logging` supports no properties.