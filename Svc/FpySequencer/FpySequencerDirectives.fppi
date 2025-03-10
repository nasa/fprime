struct WaitRelDirective {
    duration: Fw.TimeInterval
}

struct WaitAbsDirective {
    wakeupTime: Fw.Time
}

internal port directive_waitRel(directive: WaitRelDirective)

internal port directive_waitAbs(directive: WaitAbsDirective)