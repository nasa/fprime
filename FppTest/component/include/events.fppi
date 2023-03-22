# ----------------------------------------------------------------------
# Events
# ----------------------------------------------------------------------

@ An activity high event with no params
event EventActivityHigh \
  severity activity high \
  format "Event Activity High occurred"

@ An activity low, throttled event with primitive params
event EventActivityLowThrottled(
  u32: U32, @< A U32
  f32: F32, @< An F32
  b: bool @< A boolean
) \
  severity activity low \
  id 0x10 \
  format "Event Activity Low occurred with arguments: {}, {}, {}" \
  throttle 5

@ A command event with string params
event EventCommand (
  str1: string, @< A string
  str2: string size 100 @< Another string
) \
  severity command \
  format "Event Command occurred with arguments: {}, {}"

@ A diagnostic event with enum params
event EventDiagnostic(
  e: E @< An enum
) \
  severity diagnostic \
  format "Event Diagnostic occurred with argument: {}"

@ A fatal, throttled event with array params
event EventFatalThrottled (
  a: A @< An array
) \
  severity fatal \
  format "Event Fatal occurred with argument: {}" \
  throttle 10

@ A warning high event with struct params
event EventWarningHigh (
  s: S @< A struct
) \
  severity warning high \
  id 0x20 \
  format "Event Warning High occurred with argument: {}"

@ A warning low, throttled event with no params
event EventWarningLowThrottled \
  severity warning low \
  format "Event Warning Low occurred" \
  throttle 10
