# Responsible AI-Assisted Development for F Prime

F Prime (F') is a flight software framework used on multiple NASA missions.

## Gate Model for Port Contract Verification

Before running fprime-util generate, verify with a gate model:

1. Every input port has a matching handler implementation
2. Every command has a matching cmdHandler case
3. Every telemetry channel has a defined update point
4. No blocking calls in port handlers (real-time constraint)
5. State transitions complete -- no undefined states

## The Key Principle

Same as formal proof verification: a model can declare correctness
while producing an incorrect artifact. The gate model with the
F Prime spec loaded catches the contradiction before it reaches hardware.
