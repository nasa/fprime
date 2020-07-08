/**
 * dashboard-row.js
 *
 * Contains a component that contains/marks a horizontally-grouped row of
 * components in the dashboard
 */

export const DashboardRow = Vue.component("dashboard-row", {
    // TODO: Figure out better way to handle styles here, e.g. through classes?
    template: `<div style="display: flex; flex-wrap: nowrap; width: 100%;">
        <slot></slot>
    </div>`
});
