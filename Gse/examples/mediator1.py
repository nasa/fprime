#!/usr/bin/env python3
# Copyright 2012-13 trac Ltd. All rights reserved.
# This program or module is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version. It is provided for
# educational purposes and is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.

import collections


def main():
    form = Form()
    test_user_interaction_with(form)


class Form:

    def __init__(self):
        self.create_widgets()
        self.create_mediator()


    def create_widgets(self):
        self.nameText = Text()
        self.emailText = Text()
        self.okButton = Button("OK")
        self.cancelButton = Button("Cancel")


    def create_mediator(self):
        self.mediator = Mediator(((self.nameText, self.update_ui),
                (self.emailText, self.update_ui),
                (self.okButton, self.clicked),
                (self.cancelButton, self.clicked)))
        self.update_ui()


    def update_ui(self, widget=None):
        self.okButton.enabled = (bool(self.nameText.text) and
                                 bool(self.emailText.text))


    def clicked(self, widget):
        if widget == self.okButton:
            print("OK")
        elif widget == self.cancelButton:
            print("Cancel")


class Mediator:

    def __init__(self, widgetCallablePairs):
        self.callablesForWidget = collections.defaultdict(list)
        for widget, caller in widgetCallablePairs:
            self.callablesForWidget[widget].append(caller)
            widget.mediator = self


    def on_change(self, widget):
        callables = self.callablesForWidget.get(widget)
        if callables is not None:
            for caller in callables:
                caller(widget)
        else:
            raise AttributeError("No on_change() method registered for {}"
                    .format(widget))


class Mediated(object):

    def __init__(self):
        self.mediator = None


    def on_change(self):
        if self.mediator is not None:
            self.mediator.on_change(self)


class Button(Mediated):

    def __init__(self, text=""):
        super(Button,self).__init__()
        self.enabled = True
        self.text = text
    

    def click(self):
        if self.enabled:
            self.on_change()


    def __str__(self):
        return "Button({!r}) {}".format(self.text,
                "enabled" if self.enabled else "disabled")


class Text(Mediated):

    def __init__(self, text=""):
        super(Text,self).__init__()
        self.__text = text
    

    @property
    def text(self):
        return self.__text


    @text.setter
    def text(self, text):
        if self.text != text:
            self.__text = text
            self.on_change()


    def __str__(self):
        return "Text({!r})".format(self.text)


def test_user_interaction_with(form):
    form.okButton.click()           # Ignored because it is disabled
    print(form.okButton.enabled)    # False
    form.nameText.text = "Fred"
    print(form.okButton.enabled)    # False
    form.emailText.text = "fred@bloggers.com"
    print(form.okButton.enabled)    # True
    form.okButton.click()           # OK
    form.emailText.text = ""
    print(form.okButton.enabled)    # False
    form.cancelButton.click()       # Cancel


if __name__ == "__main__":
    main()
