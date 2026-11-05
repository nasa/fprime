// Violation of cpp/fprime/jpl-c/basic-int-types (JPL Rule 17):
// a declaration using a basic integral type instead of a sized typedef.
int badCounter = 5;

// A scalar char used as a small integer is also flagged (only plain-char
// C-strings/arrays are exempt).
char badSmallInt = 3;

// Compliant: a plain-char C-string is the language's string type.
const char* goodString = "ok";
