// Violation of cpp/fprime/jpl-c/preprocessor-use-ifdef (JPL Rule 20):
// conditional compilation on a repository-defined macro, outside the OS
// platform layers and outside Fw/ FpConfig switches.
#define FEATURE_MODE 1

#if FEATURE_MODE
typedef unsigned int U32;
U32 featureValue = 1;
#endif
