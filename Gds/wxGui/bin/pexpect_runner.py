#!/usr/bin/python

import sys
import pexpect
import os
import threading
import signal
import time
import Queue
import wx
import PexpectRunnerConsolImpl

def sighandler(signum, frame):
#    print "Signaled: %d" %signum
    if signum == signal.SIGCHLD:
        sys.exit(1)
    else:
        text_queue.put("SIGQUIT")    

def process_poller():
    # command to run is arguments 2->end
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
                child.kill(signal.SIGINT)
                child.wait()
                break
            if len(child.before):
                text_queue.put(child.before)              
        except pexpect.EOF:
            text_queue.put("**EXIT**")
            break
        except pexpect.TIMEOUT:
            if exitThread:
                child.kill(signal.SIGINT)
                child.wait()
                break
            if len(child.before):
                text_queue.put(child.before)
                
def poll_text():
    
    # check queue
    try:
        while True:
            msg = text_queue.get(block = False)
            if msg == "SIGQUIT":
                consol.Close()
                return
            else:
                consol.TextCtrlConsol.AppendText(msg + "\n\n")
            text_queue.task_done()
    except Queue.Empty:
        pass
    # reschedule
    wx.CallLater(100, poll_text)

def main(argv=None):
    
    # argument list
    # 0 = binary (pexpect_runner.py)
    # 1 = log file
    # 2 = title for window
    # 3... = binary to run
    
    app = wx.App(False)

    global consol
    consol = PexpectRunnerConsolImpl.PexpectRunnerImpl(None)
    consol.SetTitle(sys.argv[2])
    consol.Show(True)
    
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

    wx.CallLater(1l, poll_text)
    app.MainLoop()
    
    exitThread = True
    
    # start poller thread
    poller_thread.join()
    
                
if __name__ == "__main__":
    sys.exit(main())

    
