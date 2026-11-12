typedef float F32;
typedef double F64;
typedef unsigned int U32;

bool isNan(F64 x) {
    return x != x;  // Compliant: NaN idiom
}

bool compare(F32 a, F32 b, U32 c, U32 d) {
    bool r = (c == d);  // Compliant: integer equality
    if (a == b) {       // Violation of cpp/fprime/float-equality
        r = true;
    }
    if (a != 0.0f) {  // Violation of cpp/fprime/float-equality
        r = false;
    }
    if (c == 1.5) {  // Violation of cpp/fprime/float-equality (c converted to double)
        r = false;
    }
    return r;
}
