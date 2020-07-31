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
        },
        borderColor: {
            type: String,
            default: "transparent"
        }
    },
    template: `
        <fieldset class="col fp-dashboard-box" v-bind:style="{background: color, 'border-color': borderColor}">
            <legend class="fp-dashboard-box-title h5">{{ title }}</legend>
            <slot></slot>
        </fieldset>`
});
