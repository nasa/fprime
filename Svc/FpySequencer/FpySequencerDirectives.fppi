struct WaitRelDirective {
    duration: Fw.TimeInterval
}

struct WaitAbsDirective {
    wakeupTime: Fw.Time
}

internal port directive_waitRel(directive: WaitRelDirective) priority 6 assert

internal port directive_waitAbs(directive: WaitAbsDirective) priority 6 assert