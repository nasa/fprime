"use strict";

module.exports = function () {
	// Borrowed from ansi-regex package
	// https://github.com/chalk/ansi-regex/blob/a28b8e7ee67aa9996ba44bf123f0436eea62d285/index.js

	return new RegExp(
		"[\\u001B\\u009B][[\\]()#;?]" +
			"*(?:(?:(?:(?:;[-a-zA-Z\\d\\/#&.:=?%@~_]+)*|[a-zA-Z\\d]" +
			"+(?:;[-a-zA-Z\\d\\/#&.:=?%@~_]*)*)?\\u0007)" +
			"|" +
			"(?:(?:\\d{1,4}(?:;\\d{0,4})*)?[\\dA-PR-TZcf-ntqry=><~]))",
		"g"
	);
};
