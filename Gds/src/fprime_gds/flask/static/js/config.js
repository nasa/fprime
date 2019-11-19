/**
 * config.js:
 *
 * Configuration for the F´ GDS. This allows projects to quickly set properties that run the system.
 */
export let config = {
    // Allows projects to brand the UI
    projectName: "F´ Infrastructure",
    // Logo of the project. Will be grey when timed-out, otherwise will be full-color
    logo: "https://www.nasa.gov/sites/all/themes/custom/nasatwo/images/nasa-logo.svg",
    // Time in seconds to wait before reporting events missing status
    eventsTimeout: 60,
    // Time in seconds to wait before reporting channels missing status
    channelsTimeout: 2
};