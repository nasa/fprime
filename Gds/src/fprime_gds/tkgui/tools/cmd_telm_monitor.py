#!/usr/bin/env python

import matplotlib
matplotlib.use('TkAgg')

from numpy import arange, sin, pi
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure

from Tkinter import *
import sys
import threading
import Queue
import os
import math
import Pmw
import struct
import math
import time
from fprime_gds.gse.controllers import client_sock
#import ModeMgrSm


class Plotter:
    """
    Class to accept telemetry from multi-partiiions and sends commands.
    It also launches thread to generate fake simulation data.
    """
    def __init__(self, master, width=500, height=350):
        """
        Constructor
        """
        self.root = master
        # Title
        self.status = Label(master, text="Multi-Partition Telemetry & Commander", justify=LEFT)
        self.status.pack(side=TOP)
        # Entry fields to support commands
        self.control3 = Frame(master)
        self.control3.pack(side=BOTTOM, fill=X, padx=2)
        # Buttons to launch demos
        self.control2 = Frame(master)
        self.control2.pack(side=BOTTOM, fill=X, padx=2)
        # Buttons to launch demos
        self.control = Frame(master)
        self.control.pack(side=BOTTOM, fill=X, padx=2)

        # Build temp plot here for testing

        self.fig = Figure(figsize=(6,3), dpi=100)
        self.axis = self.fig.add_subplot(111)

        t = arange(0.0,3.0,0.01)
        s = sin(2*pi*t)
        t=[]
        s=[]
        # Initialize plots
        self.axis.plot(t,s)

        # Label title, axis and turn on grid
        self.label_plot()

        # a tk.DrawingArea
        canvas = FigureCanvasTkAgg(self.fig, master=master)
        canvas.show()
        canvas.get_tk_widget().pack(side=TOP, fill=BOTH, expand=1)
        #
        toolbar = NavigationToolbar2TkAgg( canvas, master )
        toolbar.update()
        canvas._tkcanvas.pack(side=TOP, fill=BOTH, expand=1)
        #
        self.display = Pmw.ScrolledText(self.root, hscrollmode='dynamic',
                      vscrollmode='dynamic', hull_relief='sunken',
                      hull_background='gray40', hull_borderwidth=10, 
                      text_background='white', text_width=16,
                      text_foreground='black', text_height=10,
                      text_padx=10, text_pady=10, text_relief='groove',
                      text_font=('arial', 12, 'bold'))
        self.display.pack(side=TOP, expand=YES, fill=BOTH)
        self.display.tag_config('ans', foreground='red')
        self.display.tag_config('blue', foreground='blue')
        self.display.tag_config('red', foreground='red')
        self.display.tag_config('green', foreground='green')
        self.display.tag_config('black', foreground='black')
        # Quit
        self.quit_bt = Button(self.control, text='Quit', activebackground="pink", command=self.quit)
        self.quit_bt.pack(side=LEFT, fill=X, expand=True)
        # Connect
        self.connect_bt = Button(self.control, text='Connect', activebackground="pink", command=self.connect)
        self.connect_bt.pack(side=LEFT, fill=X, expand=True)
        # Clear
        self.clear_bt = Button(self.control, text='Clear', activebackground="pink", command=self.clear)
        self.clear_bt.pack(side=LEFT, fill=X, expand=True)
        # Play
        self.play_bt = Button(self.control, text='List', activebackground="pink", command=self.list)
        self.play_bt.pack(side=LEFT, fill=X, expand=True)
        # Pause
        self.pause_bt = Button(self.control, text='Play', activebackground="pink", command=self.play)
        self.pause_bt.pack(side=LEFT, fill=X, expand=True)
        # Stop
        self.stop_bt = Button(self.control, text='Stop', activebackground="pink", command=self.stop)
        self.stop_bt.pack(side=LEFT, fill=X, expand=True)
        # opcode 1
        self.op1_bt = Button(self.control2, text='Mode Change', activebackground="pink", command=self.op1)
        self.op1_bt.pack(side=LEFT, fill=X, expand=True)
        # opcode 2
        self.op2_bt = Button(self.control2, text='Set Param', activebackground="pink", command=self.op2)
        self.op2_bt.pack(side=LEFT, fill=X, expand=True)
        # opcode 3
        self.op3_bt = Button(self.control2, text='Get Param', activebackground="pink", command=self.op3)
        self.op3_bt.pack(side=LEFT, fill=X, expand=True)
        # opcode 4
        self.op4_bt = Button(self.control2, text='Test Cent. (x,y)', activebackground="pink", command=self.op4)
        self.op4_bt.pack(side=LEFT, fill=X, expand=True)
        # opcode 5
        self.op5_bt = Button(self.control2, text='Test ARS Data', activebackground="pink", command=self.op5)
        self.op5_bt.pack(side=LEFT, fill=X, expand=True)
        #
        # Entry fields to support command.
        #
        self.mode_signal = 1
        self.signals = ["OnCmd", "OffCmd", "ARS_On", "ARS_Off", "Centroid_On", "Centroid_Off", 
                        "Show_SM_GUI", "Hide_SM_GUI"]
        self.mode_entry = Pmw.ComboBox(self.control3,
                                       scrolledlist_items=self.signals,
                                       selectioncommand = self.changeMode)
        self.mode_entry.pack(side=LEFT, fill=X, expand=False)
        self.mode_entry.selectitem(0, setentry = 1)
        self.mode_entry.component("entryfield").configure(validate=self.validateMode)
        #
        self.set_param  = Pmw.EntryField(self.control3, value=1.0,
                                         validate='real')
        self.set_param.pack(side=LEFT, fill=X, expand=False)
        #
        self.get_param  = Pmw.EntryField(self.control3, value="Params",
                                         validate='alphanumeric')
        self.get_param.pack(side=LEFT, fill=X, expand=False)
        #
        self.test_cent  = Pmw.EntryField(self.control3, value='0 0',
                                         validate=self.cent_coord_validate)
        self.test_cent.pack(side=LEFT, fill=X, expand=False)
        self.cent_coord_validate_ok = True
        #
        self.test_ars   = Pmw.EntryField(self.control3, value='0.0 0.0 0.0',
                                         validate=self.ars_validate)
        self.test_ars.pack(side=LEFT, fill=X, expand=False)
        self.ars_validate_ok = True 
        #
        # Socket when connection established
        #
        self.sock = None
        # Store thread here
        self.thread = threading.Thread()
        self.thread2 = threading.Thread()
        # Instance a lock and make all socket read/write atomic
        self.lock = threading.Lock()
        # Create Queue for telemetry here
        self.queue = Queue.Queue()
        # Create Queue for simulation data here
        self.queue2 = Queue.Queue()
        # Define plot format for each instance here
        # Note that instance id is index to list
        # Note: Adding a "-" results in lines in replot only.
        self.plot_fmt = []
        self.plot_fmt.append("ro")
        self.plot_fmt.append("g^")
        self.plot_fmt.append("c<")
        self.plot_fmt.append("m>")
        #
        self.i = 0
        #
        width = 600
        height = 600
        x = 500
        y = 500
#        self.mode_mgr_sm_win = ModeMgrSm.ModeMgrSm("ModeMgrSm", big_name="Mode Manager State Machine", size=16)
#        self.mode_mgr_sm_win.win.geometry("%dx%d+%d+%d" % (width, height, x, y))
#        self.mode_mgr_sm_win.win.withdraw()
                
    def cent_coord_validate(self, text):
        """
        Check that entry is (x y) where x and y are integer.
        """
        self.cent_coord_validate_ok = False
        if len(text.split(" ")) != 2:
            return Pmw.PARTIAL
        x,y = text.split(" ")
        if not x.isdigit():
            return Pmw.PARTIAL
        if not y.isdigit():
            return Pmw.PARTIAL
        self.cent_coord_validate_ok = True
        return Pmw.OK
    
    def ars_validate(self, text):
        """
        Check that entry is (x y z) where x, y, z are float.
        """
        self.ars_validate_ok = False
        if len(text.split(" ")) != 3:
            return Pmw.PARTIAL
        x,y,z = text.split(" ")
        xp = x.partition(".")
        if not (xp[0].isdigit() and xp[1]=='.' and xp[2].isdigit()):
            return Pmw.PARTIAL
        yp = y.partition(".")
        if not (yp[0].isdigit() and yp[1]=='.' and yp[2].isdigit()):
            return Pmw.PARTIAL
        zp = z.partition(".")
        if not (zp[0].isdigit() and zp[1]=='.' and zp[2].isdigit()):
            return Pmw.PARTIAL
        self.ars_validate_ok = True
        return Pmw.OK
    
    def set_demo_lim(self):
        """
        Set axis limits for long trajectory demo.
        """
        self.axis.set_xlim(0.0, 50.0)
        #self.axis.set_ylim(-1.25, 1.25)
        self.axis.set_ylim(-50.0, 50.0)

    def set_auto_lim(self):
        """
        Set autoscale of axis
        """
        self.axis.relim()
        self.axis.autoscale_view(True,True,True)


    def label_plot(self):
        """
        Refresh plot labels and settings
        """
        self.axis.grid()

        self.axis.set_title("Notional Tip/Tilt Position Update")
        self.axis.set_ylabel("Tip/Tilt (radians)")
        self.axis.set_xlabel("N (counts)")

        # Set default xy axis
        self.set_demo_lim()
    
    # Button row 1 functionality here...
    def list(self):
        self.root.update_idletasks()
        if self.sock != None:
            self.lock.acquire()
            self.sock.send("List\n")
            self.lock.release()
        else:
            self.status.config(text="SOCKET NOT CONNECTED TO SERVER...", fg="red")
        
    def play(self):
        self.root.update_idletasks()
        if self.sock != None:
            self.queue2.put("PLAY")
            self.display.insert(END, "Sent PLAY request to sim thread...\n", "green")
        else:
            self.status.config(text="SOCKET NOT CONNECTED TO SERVER...", fg="red")            

    def stop(self):
        self.root.update_idletasks()
        if self.sock != None:
            self.queue2.put("STOP")
            self.display.insert(END, "Sent STOP request to sim thread...\n", "green")
        else:
            self.status.config(text="SOCKET NOT CONNECTED TO SERVER...", fg="red")

    # Button row 2 functionality here...
    def app_cmd(self, msg):
        """
        Package command for server to send to APP
        """
        if self.sock != None:
            cmd = "A5A5 " + "APP " + msg
            self.lock.acquire()
            self.sock.send(cmd)
            self.lock.release()
        else:
            self.status.config(text="SOCKET NOT CONNECTED TO SERVER...", fg="red")
    
    def app_cmd_fmt(self, op,s1,i1,i2,f1,f2):
        """
        Packet format for basic command
        """
        n = 0
        l = 80
        fmt = "Message sent: (%d,%s,%d,%d,%d,%f,%f,%d)\n"
        msg = struct.pack(">I80s3i",op,s1,l,i1,i2) + struct.pack("2f",f1,f2) + struct.pack(">i",n)
        print fmt % (op,s1,l,i1,i2,f1,f2,n)
        self.display.insert(END, fmt % (op,s1,l,i1,i2,f1,f2,n), "black")
        self.display.see(END)
        return msg
    
    def op1(self):
        """
        Execute op 1 command
        """
        val = int(self.mode_signal)
        msg = self.app_cmd_fmt(1, "", val, 0, 0.0, 0.0)
        self.app_cmd(msg)

    def changeMode(self, text):
        """
        Set the mode manager mode, or hide/show mode mgr display
        """
        item = self.mode_entry.getvalue()[0]
        if item == "Show_SM_GUI":
            self.mode_signal = 1
            self.mode_mgr_sm_win.win.deiconify()
            self.mode_mgr_sm_win.win.lift(self.root)
        elif item == "Hide_SM_GUI":
            self.mode_signal = 1
            self.mode_mgr_sm_win.win.withdraw()
        elif item.isdigit():
            self.mode_signal = int(item)
        else:
            self.mode_signal = self.signals.index(item) + 1
#        print self.mode_signal
    
    def validateMode(self, text):
        """
        Validate mode is in the list or a number of 1 to 6 here.
        """
        if text in self.signals:
            return Pmw.OK
        if text.isdigit():
            if 0 < int(text) < 7:
                return Pmw.OK
        return Pmw.PARTIAL
    
    def op2(self):
        val = float(self.set_param.getvalue())
        msg = self.app_cmd_fmt(2, "", 0, 0, val, 0.0)
        self.app_cmd(msg)

    def op3(self):
        val = self.get_param.getvalue()
        msg = self.app_cmd_fmt(3, val, 0, 0, 0.0, 0.0)
        self.app_cmd(msg)
        
    def op4(self):
        """
        Send x, y and increment each time...
        """
        if self.cent_coord_validate_ok:
            val = self.test_cent.getvalue().split(" ")
            n = 1
            x = int(val[0])
            y = int(val[1])
            msg = struct.pack(">3i",x,y,n)
            #
            # Make msg 108 bytes like command
            buf = 96*"A5"
            msg = struct.pack(">96s",buf) + msg
            self.app_cmd(msg)
            self.display.insert(END, "opcode=%d, Coord=(%d,%d)\n" % (n,x,y),"black")
            self.display.see(END)

    
    def op5(self):
        """
        Send x, y, z and increment each fractionally...
        """
        if self.ars_validate_ok:
            c = self.test_ars.getvalue().split(" ")
            n = 2
            f1 = float(c[0])
            f2 = float(c[1])
            f3 = float(c[2])
            msg = struct.pack("3f",f1,f2,f3) + struct.pack(">i",n)
            #
            # Make msg 108 bytes like command
            buf = 92*"A5"
            msg = struct.pack(">92s",buf) + msg
            self.app_cmd(msg)
            self.display.insert(END, "opcode=%d, Angles=(%f,%f,%f)\n" % (n,f1,f2,f3),"black")
            self.display.see(END)


    def quit(self):
        """
        Quit the plotter
        """
        self.clear()
        root.quit()
        root.destroy()


    def receive_telemetry(self, sock):
        """
        Receive telemetry by first reading 4 byte size,
        then reading size bytes of serial telemetry
        and returning it as a message.
        """
        msg = sock.recv(4)
        size = int(struct.unpack("i",msg)[0])
        data = sock.recv(size)
        return data


    def enqueue_output(self, sock, queue):
        """
        Queue up socket telemetry for TK processing
        """
        while 1:
            try:
                #x = sock.receive()
                x = self.receive_telemetry(sock)
                queue.put(x)
            except RuntimeError:
                queue.put("terminate")
                print "Socket connection terminated"
                break
        return


    def enqueue_sim_output(self, sock, queue):
        """
        Queue up socket sim ARS data for C++ app commanding
        """
        angle = 0.0
        while 1:
            try:
                msg = queue.get()
                if msg == "PLAY":
                    while 1:
                        # Update rate is 2 hz.
                        time.sleep(0.5)
                        # Compute x, y, z sin phased by 45 degree each
                        P = 45.0 * (math.pi/180.0)
                        # Phase
                        G = 1.0
                        # Angle sin values
                        x = G * math.sin(angle)
                        y = G * math.sin(angle + P)
                        z = G * math.sin(angle + 2*P)
                        # Angle increment value
                        angle += 10.0 * (math.pi/180.0)
                        #print time.time(), x, y, z
                        n = 2
                        msg = struct.pack("3f",x,y,z) + struct.pack(">i",n)
                        #
                        # Make msg 108 bytes like command
                        buf = 92*"A5"
                        msg = struct.pack(">92s",buf) + msg
                        #
                        self.app_cmd(msg)
                                         
                        if not queue.empty():
                            msg = queue.get()
                            print msg
                            if msg == "STOP":
                                angle = 0.0
                                break
                elif msg == "STOP":
                    pass
                elif msg == "RESET":
                    pass
                else:
                    pass
            except RuntimeError:
                queue.put("terminate")
                print "Socket connection terminated"
                break


    def connect(self):
        """
        Connect to TRN filter state telemetry socket
        and start listener thread.
        """
        if self.thread.isAlive() == True:
            print "LISTENER THREAD IS ALIVE!"
            return
        # connect to server TRN
        try:
            port = 50007
            server = "127.0.0.1"
            str = "Connected to server (host addr %s, port %d)" % (server, port)
            self.sock=client_sock.ClientSocket(server,port)
            #
            # Register the GUI with the server
            self.lock.acquire()
            self.sock.send("Register GUI\n")
            self.lock.release()
        except IOError:
            str = "EXCEPTION: Could not connect to socket at host addr %s, port %s" % (server, port)
            # update status
            self.status.config(text=str, fg="red")
            self.connect_bt.config(text="Disconnected", fg="black")
            self.sock = None
            return

        if self.thread.daemon == False:
            # create background listener thread
            self.thread = threading.Thread(target=self.enqueue_output, args=(self.sock, self.queue))
            # thread dies with the program
            self.thread.daemon = True
            # state listener thread here
            self.thread.start()
            
        if self.thread2.daemon == False:
            # create background sim data generator thread
            self.thread2 = threading.Thread(target=self.enqueue_sim_output, args=(self.sock, self.queue2))
            # thread dies with the program
            self.thread2.daemon = True
            # start simulation thread here
            self.thread2.start()
        # update status
        self.status.config(text=str, fg="red")
        self.connect_bt.config(text="Connected", fg="red")


    def clear(self):
        """
        Clear all plots
        """
        #if self.thread.isAlive() == True:
        #    self.status.config(text="LISTENER THREAD IS ALIVE!",fg="red")
        #    return
        self.i = 0
        self.axis.cla()
        self.label_plot()
        self.fig.canvas.draw()
        self.status.config(text="Cleared plot areas.",fg="black")
        self.fig.canvas.draw()
        self.display.clear()


    def updatePlots(self, n, x, y):
        """
        Update plot with new point
        """
        fmt = self.plot_fmt[0]
        fmt2 = self.plot_fmt[1]
        if n % 50.0 == 0:
            self.axis.set_xlim(n, n+50.0)
        self.axis.plot(n,x,fmt)
        self.axis.plot(n,y,fmt2)
        #
        # Update figure here
        self.fig.canvas.draw()
        
    
    def decode_telm(self, bmsg):
        """
        Decode APP telemetry here and return a tuple.
        """
        if bmsg[:4] == "List":
            bmsg = bmsg.split(" ")
            print "---Server connected to %s" % bmsg[1]
            self.display.insert(END, "---Server connected to %s\n" % bmsg[1], "blue")
            return []
        l = len(bmsg)
        msg = struct.unpack(">" + "%d" % l + "s",bmsg)
        #print msg
        #tid = struct.unpack(">i",msg[0][-5:-1])[0]
        tid = struct.unpack(">i",msg[0][-4:])[0]
        if tid == 1:
            args = struct.unpack("3f",msg[0][:-4])
            print "--- (raw tlm ARSDATA) %f, %f, %f\n" % args
            self.display.insert(END, "--- (raw tlm ARSDATA) %f, %f, %f\n" % args, "ans")
            l = [tid, args[0],args[1],args[2]]
        elif tid == 2:
            args = struct.unpack("2f",msg[0][:-4])
            print "--- (raw tlm FSMDATA) %f, %f\n" % args
            self.display.insert(END, "--- (raw tlm FSMDATA) %f, %f\n" % args, "ans")
            l = [tid, args[0],args[1]]
        elif tid == 3:
            args = struct.unpack("f",msg[0][:-4])
            print "--- (raw tlm PARAMDATA) %f\n" % args[0]
            self.display.insert(END, "--- (raw tlm PARAMDATA) %f\n" % args, "ans")
            l = [tid, args[0]]
        elif tid == 4:
            m = msg[0][0:80]
            args = struct.unpack("80s",m)
            print "--- (State Data) %s\n" % args
            self.display.insert(END, "--- (State Data) %s\n\n" % args, "ans")
            self.display.insert(END, "\n")
            l = [tid, args]
        else:
            print "Unrecognized telemetry id = %d\n" % tid
            self.display.insert(END, "ERROR: Unrecognized telemetry id = %d\n" % tid, "ans")
            l = []
        self.display.see(END)
        #print "Id is %d, msg is %s, args is %s" % (tid,msg,args)
        return l


    def update_task(self):
        """
        Update telemetry and plots from socket here...
        """
        time_period = 20
        time_period_no_output = 200
        # read line without blocking
        try:
            msg = self.queue.get_nowait()
            #print msg
            msg = self.decode_telm(msg)
            if len(msg) > 0:
                if msg[0] == 2:
                    n = self.i
                    x = msg[1]
                    y = msg[2]
                    self.updatePlots(n, x, y)
                    self.i += 1
                if msg[0] == 4:
                    state = msg[1][0].split(":")
                    e = state[1].split(" ")[-1].strip("\x00")
                    s1 = state[0] 
                    s2 = state[1].strip(" "+e).replace(" ","")
                    s2 = s2.strip("\x00")
                    s2 = s2.strip("ENTRY")
                    s2 = s2.strip("EXIT")
                    
                    state = s2 + ":" + s1
                    #
                    if e == "EXIT":
                        self.mode_mgr_sm_win.ExitState(state)

                    if e == "ENTRY":
                        self.mode_mgr_sm_win.EnterState(state)
                        
                    #                    
        except Queue.Empty:
            #print('no output yet')
            self.root.after(time_period_no_output, self.update_task)
            return
            # ... do something with line
        #
        self.mode_mgr_sm_win.win.update_idletasks()
        self.root.update_idletasks()
        self.root.after(time_period, self.update_task)
        return


if __name__ == '__main__':
    root = Tk()
    root.option_add('*Font', 'Verdana 10')
    root.title('Multi-Partition Demo Commander and Telmetry Monitor')
    p = Plotter(root)
    root.after(200, p.update_task)
    root.mainloop()
