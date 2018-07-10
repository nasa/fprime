#!/usr/bin/python

import sys
import pexpect
import os
from Tkinter import *
import Pmw
import threading
import signal
import time
import Queue

def sighandler(signum, frame):
#    print "Signaled: %d" %signum
    if signum == signal.SIGCHLD:
        sys.exit(1)
    else:
        text_queue.put("SIGQUIT")    

def process_poller():
    # command to run is arguments 2->end
    global text
    global exitThread
    command = " ".join(sys.argv[3:])
    print "Running command %s"%command
    child = pexpect.spawn(command)
    logfd = open(sys.argv[1],'w')
    child.logfile = logfd
    logfd.write("%s: Running command \"%s\"\n"%(sys.argv[0],command))
    time.sleep(2)
    while True:
        try:
            child.expect('\r\n',timeout=1)
            if exitThread:
                try:
                   child.kill(signal.SIGINT)
                   child.wait()
                except:
                   print("KILL")
                break
            if len(child.before):
                text_queue.put(child.before)
#                text.appendtext(child.before + "\n")
        except pexpect.EOF:
            text_queue.put("**EXIT**")
#            text.appendtext("**EXIT**")
            break
        except pexpect.TIMEOUT:
            if exitThread:
                try:
                   child.kill(signal.SIGINT)
                   child.wait()
                except:
                   print("KILL")
                break
            if len(child.before):
                text_queue.put(child.before)
#                text.appendtext(child.before + "\n")
                
def poll_text():
    
    # check queue
    try:
        while True:
            msg = text_queue.get(block = False)
            if msg == "SIGQUIT":
                root.quit()
                return
            else:
                text.appendtext(msg + "\n")
            text_queue.task_done()
    except Queue.Empty:
        pass
    # reschedule
    root.after(100,poll_text)

def main(argv=None):
    
    # argument list
    # 0 = binary (pexpect_runner.py)
    # 1 = log file
    # 2 = title for window
    # 3... = binary to run
    
    global root
    root = Pmw.initialise()
    root.title(sys.argv[2])
    global text
    text = Pmw.ScrolledText(root,hscrollmode="dynamic",vscrollmode="dynamic")
    text.pack(expand=1,fill='both')
    
    global exitThread 
    exitThread = False
    
    global text_queue
    text_queue = Queue.Queue()

    poller_thread = threading.Thread(target=process_poller)
    poller_thread.start()

    # register signal handler
    signal.signal(signal.SIGINT,sighandler)
    signal.signal(signal.SIGTERM,sighandler)
    signal.signal(signal.SIGCHLD,sighandler)

    root.after(0,poll_text)
    root.mainloop()
    
    exitThread = True
    
    # start poller thread
    poller_thread.join()
    
                
if __name__ == "__main__":
    sys.exit(main())