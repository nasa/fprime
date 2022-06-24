"use strict";

var object        = require("es5-ext/object/valid-object")
  , stringifiable = require("es5-ext/object/validate-stringifiable-value")
  , forOf         = require("es6-iterator/for-of");

module.exports = function (text, style) {
	var result = "";
	text = stringifiable(text);
	object(style);
	forOf(text, function (char) { result += style[char] || char; });
	return result;
};
