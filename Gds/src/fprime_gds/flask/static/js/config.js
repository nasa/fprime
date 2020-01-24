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
    // Time in seconds to wait before reporting data flow error
    dataTimeout: 5,
    // Set the icon for the condition when there is data flowing
    dataSuccessIcon: "/img/success.svg",
    // Set the icon for the condition when there is a data-flow error
    dataErrorIcon: "/img/error.svg"
};
