/**
 * dashboard-row.js
 *
 * Contains a component that contains/marks a horizontally-grouped row of
 * components in the dashboard
 */

export const DashboardRow = Vue.component("dashboard-row", {
    template: `
        <div class="fp-dashboard-row">
            <slot></slot>
        </div>`
});
