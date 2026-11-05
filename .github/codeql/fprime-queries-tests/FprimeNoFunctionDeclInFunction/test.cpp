// Compliant: declared at namespace scope.
int helper(int value);

// Violation of cpp/fprime/no-function-decl-in-function: function declared
// inside another function's body.
int outer(int value) {
    int nested(int inner);
    return nested(value);
}

// Compliant: uses the namespace-scope declaration.
int caller(int value) {
    return helper(value);
}
