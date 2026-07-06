# Svc::EventSeverityFilter Utility

## 1. Introduction

`Svc::EventSeverityFilter` is a shared utility class that provides per-severity event filtering. It is used by `Svc::ActiveTextLogger`, `Svc::PassiveConsoleTextLogger`, and `Svc::EventManager` to implement configurable severity-based event filtering with a common implementation.

## 2. Design

### 2.1 Public API

```cpp
class EventSeverityFilter {
    static constexpr FwSizeType NUM_FILTER_LEVELS = 6;

    void setFilter(Fw::LogSeverity severity, bool enabled);
    bool isFiltered(Fw::LogSeverity severity) const;
    bool isEnabled(Fw::LogSeverity severity) const;
    static Fw::Success fromIndex(FwSizeType index, Fw::LogSeverity& severity);
};
```

- `setFilter()` — Enables or disables filtering for a given severity. Calls with `FATAL` or invalid severities are silently ignored.
- `isFiltered()` — Returns `true` if events at the given severity should be dropped. FATAL and unknown severities always return `false` (never filtered).
- `isEnabled()` — Returns `true` if events at the given severity are enabled (pass through). Inverse of filtering state; FATAL always returns `true`.
- `fromIndex()` — Maps a numeric index (0–5) to a `Fw::LogSeverity` value. Returns `Fw::Success::SUCCESS` on valid indices, `Fw::Success::FAILURE` otherwise. Used by `EventManager` to convert `FilterSeverity` command enum values to `LogSeverity`.

### 2.2 Severity Ordering

The canonical index-to-severity mapping (`SEVERITY_ORDER`) is:

Index | Severity
----- | --------
0 | WARNING_HI
1 | WARNING_LO
2 | COMMAND
3 | ACTIVITY_HI
4 | ACTIVITY_LO
5 | DIAGNOSTIC

This ordering matches the `EventManager::FilterSeverity` FPP enum. The `EventManager` uses `static_assert` to verify this correspondence at compile time.

### 2.3 FATAL Invariant

FATAL events are architecturally protected from filtering. The class has no filter state for FATAL — `setFilter(FATAL, ...)` is a no-op, `isFiltered(FATAL)` always returns `false`, and `isEnabled(FATAL)` always returns `true`.

## 3. Unit Testing

Five unit tests verify the utility's behavior: default state, per-severity disable, FATAL-never-filtered invariant, re-enable after disable, and disable-all.

To run: `fprime-util check` in `Svc/Types/EventSeverityFilter`.
