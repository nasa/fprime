"use strict";

/*
 * get actual length of ANSI-formatted string
 */

var strip = require("./strip");

module.exports = function (str) { return strip(str).length; };
