"""A Tkinter-based console for conversing with the Python interpreter,
featuring more tolerant pasting of code from other interactive sessions,
better handling of continuations than the standard Python interpreter,
highlighting of the most recently-executed code block, the ability to
edit and reexecute previously entered code, a history of recently-entered
lines, automatic multi-level completion with pop-up menus, and pop-up help.

Ka-Ping Yee <ping@lfw.org>, 18 April 1999.  This software is in the public
domain and is provided without express or implied warranty.  Permission to
use, modify, or distribute the software for any purpose is hereby granted."""

# TODO: autoindent to matching bracket after an unbalanced line (hard)
# TODO: outdent after line starting with "break", "raise", "return", etc.
# TODO: keep a stack of indent levels for backspace to jump back to
# TODO: blink or highlight matching brackets
# TODO: delete the prompt when joining lines; allow a way to break lines

from Tkinter import *
import sys, string, traceback, types, __builtin__

REVISION = "$Revision: 1.4 $"
VERSION = string.split(REVISION)[1]

class OutputPipe:
    """A substitute file object for redirecting output to a function."""

    def __init__(self, writer):
        self.writer = writer
        self.closed = 0

    def __repr__(self):
        return "<OutputPipe to %s>" % repr(self.writer)

    def read(self, length):
        return ""

    def write(self, data):
        if not self.closed: self.writer(data)

    def close(self):
        self.closed = 1


class Console(Frame):
    def __init__(self, parent=None, dict={}, **options):
        """Construct from a parent widget, an optional dictionary to use
        as the namespace for execution, and any configuration options."""
        Frame.__init__(self, parent)

        # Continuation state.

        self.continuation = 0
        self.error = 0
        self.intraceback = 0
        self.pasted = 0

        # The command history.

        self.history = []
        self.historyindex = None
        self.current = ""

        # Completion state.

        self.compmenus = []
        self.compindex = None
        self.compfinish = ""

        # Redirection.
            
        self.stdout = OutputPipe(lambda data, w=self.write: w(data, "stdout"))
        self.stderr = OutputPipe(lambda data, w=self.write: w(data, "stderr"))

        # Interpreter state.

        if not hasattr(sys, "ps1"): sys.ps1 = ">>> "
        if not hasattr(sys, "ps2"): sys.ps2 = "... "
        self.prefixes = [sys.ps1, sys.ps2, ">> ", "> "]
        self.startup = "Python %s\n%s\n" % (sys.version, sys.copyright) + \
            "Python Console v%s by Ka-Ping Yee <ping@lfw.org>\n" % VERSION
        self.dict = dict

        # The text box.

        self.text = Text(self, insertontime=200, insertofftime=150)
        self.text.insert("end", self.startup)
        self.text.insert("end", sys.ps1)
        self.text.bind("<Return>", self.cb_return)
        self.text.bind("<Button-1>", self.cb_select)
        self.text.bind("<ButtonRelease-1>", self.cb_position)
        self.text.bind("<ButtonRelease-2>", self.cb_paste)
        self.text.bind("<Home>", self.cb_home)
        self.text.bind("<Control-Home>", self.cb_ctrlhome)
        self.text.bind("<Up>", self.cb_back)
        self.text.bind("<Down>", self.cb_forward)
        self.text.bind("<Configure>", self.cb_cleanup)
        self.text.bind("<Expose>", self.cb_cleanup)
        self.text.bind("<Key>", self.cb_cleanup)
        self.text.bind("<Tab>", self.cb_complete)
        self.text.bind("<Left>", self.cb_position)
        self.text.bind("<space>", self.cb_space)
        self.text.bind("<BackSpace>", self.cb_backspace)
        self.text.bind("<KeyRelease-BackSpace>", self.cb_nothing)
        self.text.bind("<F1>", self.cb_help)
        self.text.bind("<Control-slash>", self.cb_help)
        self.text.bind("<Alt-h>", self.cb_help)

        # The scroll bar.

        self.scroll = Scrollbar(self, command=self.text.yview)
        self.text.config(yscrollcommand=self.scroll.set)
        self.scroll.pack(side=RIGHT, fill=Y)
        self.text.pack(fill=BOTH, expand=1)
        self.text.focus()

        # Configurable options.

        self.options = {"stdoutcolour": "#7020c0",
                        "stderrcolour": "#c03020",
                        "morecolour": "#a0d0f0",
                        "badcolour": "#e0b0b0",
                        "runcolour": "#90d090"}
        apply(self.config, (), self.options)
        apply(self.config, (), options)

    def __getitem__(self, key):
        return self.options[key]

    def __setitem__(self, key, value):
        if not self.options.has_key(key):
            raise KeyError, 'no such configuration option "%s"' % key
        self.options[key] = value
        if key == "stdoutcolour":
            self.text.tag_configure("stdout", foreground=value)
        if key == "stderrcolour":
            self.text.tag_configure("stderr", foreground=value)

    def config(self, *args, **dict):
        """Get or set configuration options in a Tkinter-like style."""
        if args == () and dict == {}:
            return self.options
        if len(args) == 1:
            return self.options[args[0]]
        for key, value in dict.items():
            self[key] = value

    # Text box routines.

    def trim(self, command):
        """Trim any matching prefix from the given command line, returning
        the amount trimmed and the trimmed result."""
        for prefix in self.prefixes:
            if command[:len(prefix)] == prefix:
                return len(prefix), command[len(prefix):]
        return 0, command

    def getline(self, line=None, trim=0):
        """Return the command on the current line."""
        if line is None:
            line, pos = self.cursor()
        command = self.text.get("%d.0" % line, "%d.end" % line)
        if trim:
            trimmed, command = self.trim(command)
        return command

    def cursor(self):
        """Get the current line and position of the cursor."""
        cursor = self.text.index("insert")
        [line, pos] = map(string.atoi, string.split(cursor, "."))
        return line, pos
    
    def write(self, data, tag=None):
        """Show output from stdout or stderr in the console."""
        if self.intraceback and data[-2:] == "\n ": data = data[:-1]
        start = self.text.index("insert")
        self.text.insert("insert", data)
        end = self.text.index("insert")
        if tag: self.text.tag_add(tag, start, end)

    # History mechanism.

    def cb_back(self, event):
        """Step back in the history."""
        if self.history:
            if self.historyindex == None:
                self.current = self.getline(trim=1)
                self.historyindex = len(self.history) - 1
            elif self.historyindex > 0:
                self.historyindex = self.historyindex - 1
            self.recall()
            
        return "break"

    def cb_forward(self, event):
        """Step forward in the history."""
        if self.history and self.historyindex is not None:
            self.historyindex = self.historyindex + 1
            if self.historyindex < len(self.history):
                self.recall()
            else:
                self.historyindex = None
                self.recall(self.current)

        return "break"

    def recall(self, command=None):
        """Show a command from the history on the current line."""
        if command is None:
            command = self.history[self.historyindex]
        line, pos = self.cursor()
        current = self.getline(line)
        trimmed, trimmedline = self.trim(current)
        cutpos = "%d.%d" % (line, trimmed)
        self.text.delete(cutpos, "%d.end" % line)
        self.text.insert(cutpos, command)
        self.text.mark_set("insert", "%d.end" % line)

    # Completion mechanism.

    def precontext(self):
        # Scan back for the identifier currently being typed.
        line, pos = self.cursor()
        command = self.getline()
        preceding = command[:pos]
        startchars = string.letters + "_"
        identchars = string.letters + string.digits + "_"
        while pos > 0 and preceding[pos-1] in identchars:
            pos = pos - 1
        preceding, ident = preceding[:pos], preceding[pos:]
        start = "%d.%d" % (line, pos)

        preceding = string.strip(preceding)
        context = ""
        if not ident or ident[0] in startchars:
            # Look for context before the start of the identifier.
            while preceding[-1:] == ".":
                preceding = string.strip(preceding[:-1])
                if preceding[-1] in identchars:
                    pos = len(preceding)-1
                    while pos > 0 and preceding[pos-1] in identchars:
                        pos = pos - 1
                    if preceding[pos] in startchars:
                        context = preceding[pos:] + "." + context
                        preceding = string.strip(preceding[:pos])
                    else: break
                else: break

        line, pos = self.cursor()
        endpos = pos
        while endpos < len(command) and command[endpos] in identchars:
            endpos = endpos + 1
        end = "%d.%d" % (line, endpos)

        return command, context, ident, start, end

    def cb_complete(self, event):
        """Attempt to complete the identifier currently being typed."""
        if self.compmenus:
            if self.cursor() == self.compindex:
                # Second attempt to complete: add finishing char and continue.
                self.text.insert("insert", self.compfinish)
                self.compindex = None
                self.unpostmenus()
            return "break"

        command, context, ident, start, end = self.precontext()

        # Get the list of possible choices.
        if context:
            try:
                object = eval(context[:-1], self.dict)
                keys = members(object)
            except:
                object = None
                keys = []
        else:
            class Lookup:
                def __init__(self, dicts):
                    self.dicts = dicts

                def __getattr__(self, key):
                    for dict in self.dicts:
                        if dict.has_key(key): return dict[key]
                    return None
            object = Lookup([self.dict, __builtin__.__dict__])
            keys = self.dict.keys() + dir(__builtin__)

        keys = matchingkeys(keys, ident)
        if not ident:
            public = []
            for key in keys:
                if key[:1] != "_": public.append(key)
            keys = public
        skip = len(ident)

        # Produce the completion.
        if len(keys) == 1:
            # Complete with the single possible choice.
            if self.cursor() == self.compindex:
                # Second attempt to complete: add finisher and continue.
                self.text.insert("insert", self.compfinish)
                self.compindex = None
            else:
                self.text.delete("insert", end)
                self.text.insert("insert", keys[0][skip:])
                try: self.compfinish = finisher(getattr(object, keys[0]))
                except: self.compfinish = " "
                if self.compfinish == " ":
                    # Object has no members; stop here.
                    self.text.insert("insert", " ")
                else:
                    self.compindex = self.cursor()
        elif len(keys) > 1:
            # Present a menu.
            prefix = commonprefix(keys)
            keys.sort()
            if len(prefix) > skip:
                self.text.delete("insert", end)
                self.text.insert("insert", keys[0][skip:len(prefix)])
                skip = len(prefix)

            if len(keys[0]) == skip:
                # Common prefix is a valid choice; next try can finish.
                self.compindex = self.cursor()
                try: self.compfinish = finisher(getattr(object, keys[0]))
                except: self.compfinish = " "

            self.postmenus(keys, skip, end, object)

        return "break"

    def postmenus(self, keys, skip, cut, object):
        """Post a series of menus listing all the given keys, given the
        length of the existing part so we can position the menus under the
        cursor, and the index at which to insert the completion."""
        width = self.winfo_screenwidth()
        height = self.winfo_screenheight()
        bbox = self.text.bbox("insert - %d c" % skip)
        x = self.text.winfo_rootx() + bbox[0] - 4
        y = self.text.winfo_rooty() + bbox[1] + bbox[3]

        self.compmenus = []
        menufont = self.text.cget("font")
        menu = Menu(font=menufont, bd=1, tearoff=0)
        self.compmenus.append(menu)
        while keys:
            try: finishchar = finisher(getattr(object, keys[0]))
            except: finishchar = " "
            def complete(s=self, k=keys[0][skip:], c=cut, f=finishchar):
                if f == " ": k = k + f
                s.text.delete("insert", c)
                s.text.insert("insert", k)
                s.unpostmenus()
                if f != " ":
                    s.compfinish = f
                    s.compindex = s.cursor()
            menu.add_command(label=keys[0], command=complete)
            menu.update()
            if y + menu.winfo_reqheight() >= height:
                menu.delete("end")
                x = x + menu.winfo_reqwidth()
                y = 0
                menu = Menu(font=menufont, bd=1, tearoff=0)
                self.compmenus.append(menu)
            else:
                keys = keys[1:]
            if x + menu.winfo_reqwidth() > width:
                menu.destroy()
                self.compmenus = self.compmenus[:-1]
                self.compmenus[-1].delete("end")
                self.compmenus[-1].add_command(label="...")
                break
        
        x = self.text.winfo_rootx() + bbox[0] - 4
        y = self.text.winfo_rooty() + bbox[1] + bbox[3]
        for menu in self.compmenus:
            maxtop = height - menu.winfo_reqheight()
            if y > maxtop: y = maxtop
            menu.post(x, y)
            x = x + menu.winfo_reqwidth()
        self.text.focus()
        self.text.grab_set()
                        
    def unpostmenus(self):
        """Unpost the completion menus."""
        for menu in self.compmenus:
            menu.destroy()
        self.compmenus = []
        self.text.grab_release()

    def cb_cleanup(self, event=None):
        if self.compmenus:
            self.unpostmenus()
        if self.pasted:
            self.text.tag_remove("sel", "1.0", "end")
            self.pasted = 0

    def cb_select(self, event):
        """Handle a menu selection event.  We have to check and invoke the
        completion menus manually because we are grabbing events to give the
        text box keyboard focus."""
        if self.compmenus:
            for menu in self.compmenus:
                x, y = menu.winfo_rootx(), menu.winfo_rooty()
                w, h = menu.winfo_width(), menu.winfo_height()
                if x < event.x_root < x + w and \
                   y < event.y_root < y + h:
                    item = menu.index("@%d" % (event.y_root - y))
                    menu.invoke(item)
                    break
            else:
                self.unpostmenus()
            return "break"

    # Help mechanism.

    def cb_help(self, event):
        command, context, ident, start, end = self.precontext()
        word = self.text.get(start, end)

        object = parent = doc = None
        skip = 0

        try:
            parent = eval(context[:-1], self.dict)
        except: pass

        # Go merrily searching for the help string.
        if not object:
            try:
                object = getattr(parent, word)
                skip = len(word) - len(ident)
            except: pass

        if not object:
            try:
                object = getattr(parent, ident)
            except: pass

        if not object:
            try:
                object = self.dict[word]
                skip = len(word) - len(ident)
            except: pass

        if not object:
            try:
                object = self.dict[ident]
            except: pass

        if not object:
            try:
                object = __builtin__.__dict__[word]
                skip = len(word) - len(ident)
            except: pass

        if not object:
            try:
                object = __builtins__.__dict__[ident]
            except: pass

        if not object:
            if not ident:
                object = parent

        try:
            doc = object.__doc__
        except: pass

        try:
            if hasattr(object, "__bases__"):
                doc = object.__init__.__doc__ or doc
        except: pass

        if doc:
            doc = string.rstrip(string.expandtabs(doc))
            leftmargin = 99
            for line in string.split(doc, "\n")[1:]:
                spaces = len(line) - len(string.lstrip(line))
                if line and spaces < leftmargin: leftmargin = spaces

            bbox = self.text.bbox("insert + %d c" % skip)
            width = self.winfo_screenwidth()
            height = self.winfo_screenheight()
            menufont = self.text.cget("font")

            help = Menu(font=menufont, bd=1, tearoff=0)
            try:
                classname = object.__class__.__name__
                help.add_command(label="<object of class %s>" % classname)
                help.add_command(label="")
            except: pass
            for line in string.split(doc, "\n"):
                if string.strip(line[:leftmargin]) == "":
                    line = line[leftmargin:]
                help.add_command(label=line)
            self.compmenus.append(help)

            x = self.text.winfo_rootx() + bbox[0] - 4
            y = self.text.winfo_rooty() + bbox[1] + bbox[3]
            maxtop = height - help.winfo_reqheight()
            if y > maxtop: y = maxtop
            help.post(x, y)
            self.text.focus()
            self.text.grab_set()

        return "break"

    # Entering commands.

    def cb_position(self, event):
        """Avoid moving into the prompt area."""
        self.cb_cleanup()
        line, pos = self.cursor()
        trimmed, command = self.trim(self.getline())
        if pos <= trimmed:
            self.text.mark_set("insert", "%d.%d" % (line, trimmed))
            return "break"

    def cb_backspace(self, event):
        self.cb_cleanup()
        if self.text.tag_ranges("sel"): return

        # Avoid backspacing over the prompt.
        line, pos = self.cursor()
        trimmed, command = self.trim(self.getline())
        if pos <= trimmed: return "break"

        # Extremely basic outdenting.  Needs more work here.
        if not string.strip(command[:pos-trimmed]):
            step = (pos - trimmed) % 4
            cut = pos - (step or 4)
            if cut < trimmed: cut = trimmed
            self.text.delete("%d.%d" % (line, cut), "%d.%d" % (line, pos))
            return "break"

    def cb_space(self, event):
        self.cb_cleanup()
        line, pos = self.cursor()
        trimmed, command = self.trim(self.getline())

        # Extremely basic indenting.  Needs more work here.
        if not string.strip(command[:pos-trimmed]):
            start = trimmed + len(command) - len(string.lstrip(command))
            self.text.delete("insert", "%d.%d" % (line, start))
            step = 4 - (pos - trimmed) % 4
            self.text.insert("insert", " " * step)
            return "break"

    def cb_home(self, event):
        """Go to the first non-whitespace character in the line."""
        self.cb_cleanup()
        line, pos = self.cursor()
        trimmed, command = self.trim(self.getline())
        indent = len(command) - len(string.lstrip(command))
        self.text.mark_set("insert", "%d.%d" % (line, trimmed + indent))
        return "break"

    def cb_ctrlhome(self, event):
        """Go to the beginning of the line just after the prompt."""
        self.cb_cleanup()
        line, pos = self.cursor()
        trimmed, command = self.trim(self.getline())
        self.text.mark_set("insert", "%d.%d" % (line, trimmed))
        return "break"

    def cb_nothing(self, event):
        return "break"

    def cb_return(self, event, doindent=1):
        """Handle a <Return> keystroke by running from the current line
        and generating a new prompt."""
        self.cb_cleanup()
        self.text.tag_delete("compiled")
        self.historyindex = None
        command = self.getline(trim=1)
        if string.strip(command):
            self.history.append(command)

        line, pos = self.cursor()
        self.text.mark_set("insert", "%d.end" % line)
        self.text.insert("insert", "\n")
        self.runline(line)

        line, pos = self.cursor()
        self.text.mark_set("insert", "%d.end" % line)
        prompt = self.continuation and sys.ps2 or sys.ps1
        if pos > 0:
            self.text.insert("insert", "\n" + prompt)
        else:
            self.text.insert("insert", prompt)

        if doindent and not self.error:
            self.autoindent(command)
        self.error = 0
        self.text.see("insert")
        return "break"

    def autoindent(self, command):
        # Extremely basic autoindenting.  Needs more work here.
        indent = len(command) - len(string.lstrip(command))
        if string.lstrip(command):
            self.text.insert("insert", command[:indent])
            if string.rstrip(command)[-1] == ":":
                self.text.insert("insert", "    ")

    def cb_paste(self, event):
        """Handle a paste event (middle-click) in the text box.  Pasted
        text has any leading Python prompts stripped (at last!!)."""
        self.text.tag_delete("compiled")
        self.error = 0
        self.pasted = 1

        try: lines = string.split(self.selection_get(), "\n")
        except: return

        for i in range(len(lines)):
            trimmed, line = self.trim(lines[i])
            line = string.rstrip(line)
            if not line: continue

            self.text.insert("end", line)
            self.text.mark_set("insert", "end")
            if i == len(lines) - 2 and lines[i+1] == "":
                # Indent the last line if it's blank.
                self.cb_return(None, doindent=1)
            elif i < len(lines) - 1:
                self.cb_return(None, doindent=0)

            if self.error: break

        return "break"

    # Executing commands.

    def runline(self, line):
        """Run some source code given the number of the last line in the
        text box.  Scan backwards to get the entire piece of code to run
        if the line is a continuation of previous lines.  Tag the compiled
        code so that it can be highlighted according to whether it is
        complete, incomplete, or illegal."""
        lastline = line
        lines = [self.getline(line)]
        while lines[0][:len(sys.ps2)] == sys.ps2:
            trimmed, lines[0] = self.trim(lines[0])
            self.text.tag_add(
                "compiled", "%d.%d" % (line, trimmed), "%d.0" % (line+1))
            line = line - 1
            if line < 0: break
            lines[:0] = [self.getline(line)]
        if lines[0][:len(sys.ps1)] == sys.ps1:
            trimmed, lines[0] = self.trim(lines[0])
            self.text.tag_add(
                "compiled", "%d.%d" % (line, trimmed), "%d.0" % (line+1))
        else:
            self.text.tag_add("compiled", "%d.0" % line, "%d.0" % (line+1))

        source = string.join(lines, "\n")
        if not source:
            self.continuation = 0
            return
            
        status, code = self.compile(source)

        if status == "more":
            self.text.tag_configure("compiled", background=self["morecolour"])
            self.continuation = 1

        elif status == "bad":
            self.text.tag_configure("compiled", background=self["badcolour"])
            self.error = 1
            self.continuation = 0
            self.intraceback = 1
            oldout, olderr = sys.stdout, sys.stderr
            sys.stdout, sys.stderr = self.stdout, self.stderr
            traceback.print_exception(SyntaxError, code, None)
            self.stdout, self.stderr = sys.stdout, sys.stderr
            sys.stdout, sys.stderr = oldout, olderr
            self.intraceback = 0

        elif status == "okay":
            if self.getline(lastline) == sys.ps2:
                self.text.tag_remove("compiled", "%d.0" % lastline, "end")
            self.text.tag_configure("compiled", background=self["runcolour"])
            self.continuation = 0
            self.run(code)

    def compile(self, source):
        """Try to compile a piece of source code, returning a status code
        and the compiled result.  If the status code is "okay" the code is
        complete and compiled successfully; if it is "more" then the code
        can be compiled, but an interactive session should wait for more
        input; if it is "bad" then there is a syntax error in the code and
        the second returned value is the error message."""
        err = err1 = err2 = None
        code = code1 = code2 = None

        try:
            code = compile(source, "<console>", "single")
        except SyntaxError, err:
            pass
        else:
            return "okay", code

        try:
            code1 = compile(source + "\n", "<console>", "single")
        except SyntaxError, err1:
            pass
        else:
            return "more", code1

        try:
            code2 = compile(source + "\n\n", "<console>", "single")
        except SyntaxError, err2:
            pass

        try:
            code3 = compile(source + "\n", "<console>", "exec")
        except SyntaxError, err3:
            pass
        else:
            return "okay", code3

        try:
            code4 = compile(source + "\n\n", "<console>", "exec")
        except SyntaxError, err4:
            pass

        if err3[1][2] != err4[1][2]:
            return "more", None

        if err1[1][2] != err2[1][2]:
            return "more", None

        return "bad", err1

    def run(self, code):
        """Run a code object within the sandbox for this console.  The
        sandbox redirects stdout and stderr to the console, and executes
        within the namespace associated with the console."""
        oldout, olderr = sys.stdout, sys.stderr
        sys.stdout, sys.stderr = self.stdout, self.stderr

        try:
            exec code in self.dict
        except:
            self.error = 1
            sys.last_type = sys.exc_type
            sys.last_value = sys.exc_value
            sys.last_traceback = sys.exc_traceback.tb_next
            self.intraceback = 1
            traceback.print_exception(
                sys.last_type, sys.last_value, sys.last_traceback)
            self.intraceback = 0
            
        self.stdout, self.stderr = sys.stdout, sys.stderr
        sys.stdout, sys.stderr = oldout, olderr


# Helpers for the completion mechanism.

def scanclass(klass, result):
    for key in klass.__dict__.keys(): result[key] = 1
    for base in klass.__bases__: scanclass(base, result)

def members(object):
    result = {}
    try:
        for key in object.__members__: result[key] = 1
        result["__members__"] = 1
    except: pass
    try:
        for key in object.__methods__: result[key] = 1
        result["__methods__"] = 1
    except: pass
    try:
        for key in object.__dict__.keys(): result[key] = 1
        result["__dict__"] = 1
    except: pass
    if type(object) is types.ClassType:
        scanclass(object, result)
        result["__name__"] = 1
        result["__bases__"] = 1
    if type(object) is types.InstanceType:
        scanclass(object.__class__, result)
        result["__class__"] = 1
    return result.keys()

def matchingkeys(keys, prefix):
    prefixmatch = lambda key, l=len(prefix), p=prefix: key[:l] == p
    return filter(prefixmatch, keys)

def commonprefix(keys):
    if not keys: return ''
    max = len(keys[0])
    prefixes = map(lambda i, key=keys[0]: key[:i], range(max+1))
    for key in keys:
        while key[:max] != prefixes[max]:
            max = max - 1
            if max == 0: return ''
    return prefixes[max]

callabletypes = [types.FunctionType, types.MethodType, types.ClassType,
                 types.BuiltinFunctionType, types.BuiltinMethodType]
sequencetypes = [types.TupleType, types.ListType]
mappingtypes = [types.DictType]

try:
    import ExtensionClass
    callabletypes.append(ExtensionClass.ExtensionClassType)
except: pass
try:
    import curve
    c = curve.Curve()
    callabletypes.append(type(c.read))
except: pass

def finisher(object):
    if type(object) in callabletypes:
        return "("
    elif type(object) in sequencetypes:
        return "["
    elif type(object) in mappingtypes:
        return "{"
    elif members(object):
        return "."
    return " "


# Main program.

if __name__ == "__main__":
    c = Console(dict={})
    c.dict["console"] = c
    c.pack(fill=BOTH, expand=1)
    c.master.title("Python Console v%s" % VERSION)
    mainloop()
