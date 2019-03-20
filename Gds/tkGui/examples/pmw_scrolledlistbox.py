from Tkinter import *
import Pmw
root = Tk()
#root.option_readfile('optionDB')
root.title('ScrolledListbox')
Pmw.initialise()

box = None

def selectionCommand():
    sels = box.getcurselection()
    if len(sels) == 0:
        print 'No selection'
    else:
        print 'Selection:', sels[0]

box = Pmw.ScrolledListBox(root, listbox_selectmode=SINGLE,
              items=('John Cleese', 'Eric Idle', 'Graham Chapman',
                     'Terry Jones', 'Michael Palin', 'Terry Gilliam'),
              labelpos=NW, label_text='Cast Members',
              listbox_height=5, vscrollmode='static',
              selectioncommand=selectionCommand,
	      dblclickcommand=selectionCommand,
	      usehullsize=1, hull_width=200, hull_height=200,)

box.pack(fill=BOTH, expand=1, padx=5, pady=5)

root.mainloop()

