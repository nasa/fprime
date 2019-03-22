import os
import Tkinter
import signal
import threading
import Queue

from fprime_gds.tkgui.controllers import observer
from fprime_gds.tkgui.controllers import file_uplink_client
from fprime_gds.tkgui.controllers import file_downlink_client
from fprime_gds.tkgui.controllers import status_updater


class FileListener(observer.Observable):
    """
    Listens for Uplink and Downlink
    data. Updates file panel.
    """
    __instance = None


    def __init__(self):
        """
        Constructor.
        """
        super(FileListener, self).__init__()

        self.__after_id = None
        self.__root     = None

        self.__status_updater = status_updater.StatusUpdater.getInstance()

        # Stop Variable
        self.__thread_stop     = threading.Event()


        # Uplink Client
        self.__uplink_client   = file_uplink_client.UplinkShell.getInstance()
        self.__uplink_listen   = None
        self.__uplink_queue    = Queue.Queue()
        self.__uplink_finished = threading.Event()
        self.__uplink_data     = {}
        self.__UPLK            = None

        # Downlink Client
        self.__downlink_client = file_downlink_client.DownlinkShell.getInstance()
        self.__downlink_listen = None
        self.__downlink_queue  = Queue.Queue()
        self.__downlink_finished = threading.Event()
        self.__downlink_data   = {}
        self.__DNLK            = None

    def set_after_id(self, id):
        self.__after_id = id
    def get_after_id(self):
        return self.__after_id
    def set_root(self, root):
        self.__root = root
    def set_uplink(self, UPLK):
        self.__UPLK = UPLK
    def set_downlink(self, DNLK):
        self.__DNLK = DNLK

    def get_uplink_data(self):
        return self.__uplink_data

    def get_downlink_data(self):
        return self.__downlink_data

    def update_task(self):
        time_period = 2


        # Process uplink
        #
        if self.__UPLK:

            # Uplink has finished. Keep getting data untill queue is empty
            if self.__uplink_finished.is_set():

                try:
                    self.__uplink_data = self.__uplink_queue.get_nowait()
                except Queue.Empty, e:
                    # We have all data. Clean up now
                    self.__uplink_finish()
                    clean_up_process(self.__UPLK)

            # Uplink is still running
            else:

                try:
                    self.__uplink_data = self.__uplink_queue.get_nowait()

                except Queue.Empty, e:
                    # It is okay if we don't get data
                    self.__uplink_data = {}



        # Start new uplink if there is one in uplink_client queue
        else:
            self.__UPLK = self.__uplink_client.startNext()

            # Subprocess started. Start listen thread
            if self.__UPLK:
                self.__uplink_finished.clear()
                self.__uplink_listen = threading.Thread(target=monitor_uplink, args=(self.__UPLK, self.__uplink_queue, self.__uplink_finished, self.__thread_stop))
                self.__uplink_listen.start()


        # Process Downlink
        #
        if self.__DNLK:

            try:
                self.__downlink_data = self.__downlink_queue.get_nowait()
            except Queue.Empty, e:
                self.__downlink_data = {}

                if self.__downlink_finished.is_set():
                    self.__downlink_finished.clear()

                    self.__downlink_finish()



        # Start downlink if there is one in downlink_client queue
        else:
            self.__DNLK = self.__downlink_client.start()

            # Subprocess started. Start listen thread
            if self.__DNLK:
                self.__downlink_listen = threading.Thread(target=monitor_downlink, args=(self.__DNLK, self.__downlink_queue, self.__downlink_finished, self.__thread_stop))
                self.__downlink_listen.start()


        self.notifyObservers()
        self.__after_id = self.__root.after(time_period, self.update_task)


    def __uplink_finish(self):
        """
        If success set progress bar to end.
        Else set to 0.

        Also display status update.
        """
        rc = self.__uplink_data['returncode']

        if rc >= 0:
            self.__uplink_data['progress'] = 100
            self.__status_updater.update("Uplink Completed".format())
        else:
            self.__uplink_data['progress'] = 0
            self.__status_updater.update("Upink Error: {}".format(rc))


        self.__UPLK = None

    def __downlink_finish(self):
        """
        Set progress bar to end.
        Display status update.
        """
        self.__status_updater.update("Downlink Completed")
        self.__downlink_data['progress'] = 100



    def exit(self):
        """
        Stop threads and make sure all subprocesses are killed.
        """

        self.__thread_stop.set()

        try:
            if self.__UPLK:
                os.killpg(os.getpgid(self.__UPLK.pid), signal.SIGTERM)
            if self.__DNLK:
                os.killpg(os.getpgid(self.__DNLK.pid), signal.SIGTERM)
        except Exception, e:
            # Process is already killed
            pass


    def update_task_cancel(self):
        """
        Cancel the update task here...
        """
        self.__root.after_cancel(self.__after_id)


    def getInstance():
        """
        Return instance of singleton.
        """
        if(FileListener.__instance is None):
            FileListener.__instance = FileListener()
        return FileListener.__instance

    #define static method
    getInstance = staticmethod(getInstance)



def clean_up_process(subprocess):
    """
    Kill subprocess.
    """
    try:
        os.killpg(os.getpgid(subprocess.pid), signal.SIGTERM)
    except Exception, e:
        # Process is already killed
        pass



def monitor_uplink(subprocess, queue, uplink_finished_flag, exit):
    """
    Monitor uplink subprocess thread.

    Runs until completion for every file uplink.
    """

    while(True):

        if exit.is_set():
            break

        data = {}

        # Subprocess is running
        if subprocess.poll() is None:
            line = subprocess.stdout.readline()
            data = process_line(line)
            queue.put_nowait(data)

        # Subprocess has completed. Put rest of output on queue then break.
        else:
            stdout, stderr = subprocess.communicate()
            stdout = stdout.split('\n')
            for line in stdout:
                data = process_line(line)

            c = subprocess.returncode

            data['returncode'] = c

            queue.put_nowait(data)
            uplink_finished_flag.set()
            break

def monitor_downlink(subprocess, queue, downlink_finished, exit):
    """
    Monitor downlink subprocess thread.

    Continuously runs.
    """

    files_downlinking = 0

    while(True):

        if exit.is_set():
            break

        line = subprocess.stdout.readline()

        # Downlink subprocess is returning empty lines
        # For some reason...Using check to skip
        if line == '':
            continue

        data = process_line(line)

        if data['totalsent']:
            downlink_finished.set()

        #print data


        queue.put_nowait(data)


def process_line(l):
    """
    Process tokens from a subprocess.
    """


    data = {'filesize': None,
        'filename':None,
        'progress': None,
        'bytessent': None,
        'checksum': None,
        'totalsent': None}

    if "<" in l:
        l = l.strip('\n')
        l = l.strip('<>')
        tokens = l.split('|')
        #print tokens

        # Start
        if tokens[0] == 's':
            data['filesize'] = tokens[1]
            data['filename'] = tokens[2]

        # Data
        elif tokens[0] == 'd':
            data['progress'] = tokens[1] # Progress
            data['bytessent'] = tokens[2] # Num Bytes sent

        # End
        elif tokens[0] == 'e':
            x = tokens[1]
            if 'x' in x:
                data['checksum'] = True
            else:
                data['totalsent'] = x
    return data
