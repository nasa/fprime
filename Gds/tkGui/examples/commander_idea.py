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
    cmd = Commander()
    test_user_interaction_with(cmd)


class Commander:

    def __init__(self):
        self.create_widgets()
        self.create_mediator()


    def create_widgets(self):
        self.cmd1Button = Button()
        self.cmd2Button = Button()
        self.cmd3Button = Button()
        self.cmd4Button = Button()


    def create_mediator(self):
        self.mediator = Mediator(((self.cmd1Button, self.clicked),
                                  (self.cmd2Button, self.clicked),
                                  (self.cmd3Button, self.clicked),
                                  (self.cmd4Button, self.clicked)))

    def register(self, widget, caller):
        self.mediator.register(widget, caller)
        return widget
    
    def unregister(self, widget):
        self.mediator.unregister(widget)


    def clicked(self, widget, cmd_obj):
        print "Executing...."
        if widget == self.cmd1Button:
            print("cmd1")
        elif widget == self.cmd2Button:
            print("cmd2")
        elif widget == self.cmd3Button:
            print("cmd3")
        elif widget == self.cmd4Button:
            print("cmd4")
        else:
            print widget


class Mediator(object):

    def __init__(self, widgetCallablePairs):
        self.callablesForWidget = collections.defaultdict(list)
        for widget, caller in widgetCallablePairs:
            self.callablesForWidget[widget].append(caller)
            widget.mediator = self


    def register(self, widget, caller):
        """
        Register a new input object with caller.
        """
        self.callablesForWidget[widget].append(caller)
        widget.mediator = self
        return widget

        
    def unregister(self, widget):
        """
        Remove widget from dictionary.
        """
        if widget in self.callablesForWidget: self.callablesForWidget[widget]


    def on_change(self, widget, cmd_obj):
        callables = self.callablesForWidget.get(widget)
        if callables is not None:
            for caller in callables:
                caller(widget, cmd_obj)
        else:
            raise AttributeError("No on_change() method registered for {}"
                    .format(widget))

#
# adding the on_change call.
def mediated(Class):
    setattr(Class, "mediator", None)
    def on_change(self, cmd_obj):
        print "mediator on_change"
        if self.mediator is not None:
            self.mediator.on_change(self, cmd_obj)
    setattr(Class, "on_change", on_change)
    return Class


@mediated
class Button(object):

    def __init__(self, text=""):
        #super().__init__()
        self.enabled = True
        self.text = text
    

    def click(self, cmd_obj):
        if self.enabled:
            self.on_change(cmd_obj)


    def __str__(self):
        return "Button({!r}) {}".format(self.text,
                "enabled" if self.enabled else "disabled")


def test_user_interaction_with(cmd):
    cmd.cmd1Button.click("cmd1")
    cmd.cmd3Button.click("cmd2")
    cmd.cmd2Button.click("cmd3")
    w =  Button("Registered cmd")
    #
    cmd.register(w, cmd.clicked)
    w.click("New registered widget cmd")
    cmd.cmd4Button.click("cmd4")
    #
    w2 = Button("Registered cmd 2")
    cmd.register(w2, cmd.clicked)
    cmd.cmd2Button.click("cmd2 again")
    w2.click("new command")
    #
    # Remove command object
    cmd.unregister(w)
    #
    w.click("Execute fails!!!")
    #
    cmd.cmd1Button.click("cmd1")
    #
    w2.click("new command again")


if __name__ == "__main__":
    main()
