"use strict";

var d = require("d");

Object.defineProperties(exports, {
	width: d.gs("ce", function () { return process.stdout.columns || 0; }),
	height: d.gs("ce", function () { return process.stdout.rows || 0; })
});
