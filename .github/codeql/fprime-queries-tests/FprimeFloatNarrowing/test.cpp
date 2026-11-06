typedef float F32;
typedef double F64;

F64 compute(F64 x);

void use(F32 in, F64 wide) {
    F32 a = wide;                    // Violation of cpp/fprime/float-narrowing
    F32 b = compute(in);             // Violation of cpp/fprime/float-narrowing
    F32 c = static_cast<F32>(wide);  // Compliant: explicit cast
    F32 d = 1.0;                     // Compliant: literal
    F64 e = in;                      // Compliant: widening
    F32 f = in * 2.0f;               // Compliant: F32 arithmetic
    a = wide * in;                   // Violation of cpp/fprime/float-narrowing
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    (void)e;
    (void)f;
}
