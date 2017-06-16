#!/usr/bin/env python3
# Copyright 2012-13 Qtrac Ltd. All rights reserved.
# This program or module is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version. It is provided for
# educational purposes and is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.

import datetime
import itertools
import sys
import time


class Observed(object):

    def __init__(self):
        self.__observers = set()


    def observers_add(self, observer, *observers):
        for observer in itertools.chain((observer,), observers):
            self.__observers.add(observer)
            observer.update(self)


    def observer_discard(self, observer):
        self.__observers.discard(observer)


    def observers_notify(self):
        for observer in self.__observers:
            observer.update(self)


class SliderModel(Observed):

    def __init__(self, minimum, value, maximum):
        super(SliderModel, self).__init__()
        # These must exist before using their property setters
        self.__minimum = self.__value = self.__maximum = None
        self.minimum = minimum
        self.value = value
        self.maximum = maximum


    @property
    def value(self):
        return self.__value


    @value.setter
    def value(self, value):
        if self.__value != value:
            self.__value = value
            self.observers_notify()


if __name__ == "__main__":
    pass
