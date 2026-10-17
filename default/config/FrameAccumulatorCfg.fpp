module Svc {
    module FrameAccumulatorCfg {
        @ Maximum CRC warning emissions in each throttle period; must be positive
        constant CrcFailureWarningThrottle = 1

        @ CRC warning throttle period in seconds; must be positive
        constant CrcFailureWarningPeriodSeconds = 60
    }
}
