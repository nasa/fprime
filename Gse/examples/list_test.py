import Tkinter as tk
import Pmw

root = tk.Tk()
Pmw.initialise()

f1 = tk.LabelFrame(root, text="Immediate Command Entries", padx=5, pady=5)
f1.pack(side="top", anchor="nw", fill="x", expand=True,padx=10,pady=10)

tk.Button(f1, text="Test").pack(side='top',fill='x',expand=True)

#scrollbar = tk.Scrollbar(root, orient="vertical")
#lb = tk.Listbox(root, width=50, height=20, yscrollcommand=scrollbar.set)
#scrollbar.config(command=lb.yview)

#scrollbar.pack(side="right", fill="y")

f = tk.LabelFrame(root, text="Command History", padx=5, pady=5)
f.pack(side="right", anchor='n',fill="both", expand=1, padx=10,pady=10)

lb = Pmw.ScrolledListBox(f)

lb.pack(side="left",fill="both", expand=True)
for i in range(0,100):
    lb.insert("end", "item #%s" % i)

root.mainloop()

