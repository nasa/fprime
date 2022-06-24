"use strict";

var d = require("d");

module.exports = Object.defineProperties(require("./bare"), {
	windowSize: d(require("./window-size")),
	erase: d(require("./erase")),
	move: d(require("./move")),
	beep: d(require("./beep")),
	columns: d(require("./columns")),
	strip: d(require("./strip")),
	getStrippedLength: d(require("./get-stripped-length")),
	slice: d(require("./slice")),
	throbber: d(require("./throbber")),
	reset: d(require("./reset")),
	art: d(require("./art"))
});
