/**
 * dashboard-box.js
 *
 * Contains a component for grouping together a set of dashboard items under a
 * given title/background color
 */

export const DashboardBox = Vue.component("dashboard-box", {
    props: {
        title: {
            type: String,
            default: ""
        },
        color: {
            type: String,
            default: "transparent"
        }
    },
    // TODO: Figure out better way to handle styles here, e.g. through classes?
    template: `<fieldset v-bind:style="{background: color}">
        <legend style="background: black; color: white;">{{ title }}</legend>
        <slot></slot>
    </fieldset>`
});
