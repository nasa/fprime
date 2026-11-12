#define BUFFER_SIZE 128            // Violation of cpp/fprime/define-constant
#define TIMEOUT_MS (500)           // Violation of cpp/fprime/define-constant
#define MASK 0xFFu                 // Violation of cpp/fprime/define-constant
#define SCALE 1.5e3f               // Violation of cpp/fprime/define-constant
#define NEG (-1)                   // Violation of cpp/fprime/define-constant
#define ENABLED                    // Compliant: flag macro, no value
#define SQUARE(x) ((x) * (x))      // Compliant: function-like macro
#define NAME "widget"              // Compliant: string
#define DERIVED (BUFFER_SIZE * 2)  // Compliant: expression of other macros

typedef unsigned int U32;
static const U32 GOOD_SIZE = 128;  // Compliant: typed constant
enum { GOOD_ENUM = 4 };            // Compliant: enum constant
