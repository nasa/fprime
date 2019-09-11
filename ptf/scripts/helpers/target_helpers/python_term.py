#! /dsw/Python-2.6.1-2/bin/python

# This script will create two PTYs. One through a forkpty call
# that creates a copy of this script on a new PTY for control/shell
# PTY control. The second is in raw mode and just transfers bytes
# to the first one, allowing the TTY layer of the first to perform
# all the canonical translations

import os, sys, select, re, time, termios, fcntl
import subprocess
import optparse
import time

parser = optparse.OptionParser()
parser.disable_interspersed_args()
parser.add_option("-s", "--symbolic-link", dest="symlink",
                  help="The name of a symbolic link to create, pointing to the user's TTY", default="", type="string", action="store")
parser.add_option("-l", "--logfile", dest="logfile",
                  help="The path to a log file that this program will store all traffic between the PTY and contained process.", default="", type="string", action="store")
(options, args) = parser.parse_args()

# An enumeration for which part of the
# termios structure we want to access
IFLAG = 0
OFLAG = 1
LFLAG = 3

# An enumeration for whether we're referencing
# the master tty for the child proc or the
# master tty for the pseudo serial port
PROC = 0
EXT = 1

masters = dict()
slaves = dict()
symlink = ""
log_fd = None

pid, masters[PROC] = os.forkpty()

if pid == 0: # Child

    # The current process is replaced by the desired process with this call.
    ret = os.execvp(args[0], args)
    os._exit(ret)

else:        # Parent

    old = dict()
    new = dict()

    try:
        masters[EXT], slaves[EXT] = os.openpty()
        print("Access this terminal at ", os.ttyname(slaves[EXT]))

        if options.symlink is not "":
            if os.path.exists(options.symlink):
                print("Symbolic link '%s' already exists!" % options.symlink)
                symlink = ""
            else:
                os.symlink(os.ttyname(slaves[EXT]), options.symlink)
                symlink = options.symlink

        for key in masters:
            old[key] = termios.tcgetattr(masters[key])
            new[key] = termios.tcgetattr(masters[key])

        # Configure Process's PTY connection
        new[PROC][IFLAG] &= ~(termios.IGNCR | termios.INLCR)
        new[PROC][IFLAG] |= termios.ICRNL
        new[PROC][OFLAG] |= termios.ONLCR
        new[PROC][LFLAG] |= termios.ICANON | termios.ECHO | termios.ECHOE | termios.ECHOK | termios.ECHOKE | termios.ECHONL
        new[PROC][LFLAG] &= ~(termios.ECHOPRT)

        #new[PROC][IFLAG] &= ~(termios.ICRNL)
        #new[PROC][OFLAG] &= ~(termios.ONLCR)
        #new[PROC][LFLAG] &= ~(termios.ECHO | termios.ECHOE | termios.ECHOK | termios.ECHOKE | termios.ECHONL)

        # Configure External PTY as RAW
        new[EXT][IFLAG] &= ~(termios.IGNCR | termios.INLCR | termios.ICRNL)
        new[EXT][OFLAG] &= ~(termios.ONLCR)
        new[EXT][LFLAG] &= ~(termios.ECHOPRT | termios.ICANON | termios.ECHO | termios.ECHOE | termios.ECHOK | termios.ECHOKE | termios.ECHONL)
        
        # Set attributes for all master PTYs
        for key in masters:
            termios.tcsetattr(masters[key], termios.TCSADRAIN, new[key])

        # Define data routing
        transfers = {masters[EXT]: [masters[PROC]], masters[PROC]: [masters[EXT]]}

        # Open a logfile if we should
        if options.logfile != "":
            log_fd = open(options.logfile, "w")

        need_timetag = True
        keep_running = True
        while (keep_running):
            readlist, writelist, exceptlist = select.select(list(transfers.keys()), [], list(transfers.keys()))

            for src_fd in readlist:
                str = ""
                try :
                    str = os.read(src_fd, 1024)
                except Exception as e:
                    print("Connected process has terminated.")
                    keep_running = False

                if len(str) > 0:
                    # Log the data to the file, if it's open
                    if (log_fd) and (src_fd == masters[PROC]):
                        logstr = str
                        timetag = time.strftime("[%Y%m%dT%H%M%S] ", time.gmtime())

                        # If we need to timetag from last time, do it now
                        if need_timetag:
                            log_fd.write(timetag)

                        # If our last character is a timetag, remove it and set 
                        # the need timetag flag
                        if logstr[-1] == "\n":
                            logstr = logstr[:-1]
                            need_timetag = True
                        else:
                            need_timetag = False

                        # Write the data to the log file
                        log_fd.write(logstr.replace("\n", "\n" + timetag))

                        # If we need a timetag, then put the newline we removed
                        # earlier (to avoid it triggering the replace function)
                        if need_timetag:
                            log_fd.write("\n")

                    #print "READ (%d) '%s'" % (src_fd, str)
                    #print "READ (%d) '" % src_fd,
                    #for char in str:
                    #    print "%02x " % ord(char),
                    #print "'"

                    #if src_fd == tty_master:
                    #    str = str.replace("\r", "\r\n")
                    for dest_fd in transfers[src_fd]:
                        size = os.write(dest_fd, str)

            for src_fd in exceptlist:
                print("I GOT A EXCEPTIONAL EVENT ON FD %d" % src_fd)
                
    except KeyboardInterrupt as e:
        print("Buuuuuhhh bye!")
    except Exception as e:
        print("Recorded an unexpected exception!")
        print(e)

    if log_fd is not None:
        log_fd.close()

    for key in masters:
        if masters[key] > 0:
            termios.tcsetattr(masters[key], termios.TCSANOW, old[key])
            os.close(masters[key])

    for key in slaves:
        if slaves[key] > 0:
            os.close(slaves[key])

    if symlink is not "":
        os.remove(symlink)
