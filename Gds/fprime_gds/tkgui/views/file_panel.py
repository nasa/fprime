import Tkinter
import ttk
import tkFileDialog
import Pmw
import os
import time

from fprime_gds.tkgui.utils import ConfigManager

from fprime_gds.tkgui.controllers import file_uplink_client
from fprime_gds.tkgui.controllers import file_downlink_client
from fprime_gds.tkgui.controllers import event_listener
from fprime_gds.tkgui.controllers import status_updater
from fprime_gds.tkgui.controllers import command_loader
from fprime_gds.tkgui.controllers import commander

class StatusFrame(Tkinter.LabelFrame):
    """
    Hold file transfer status.
    """
    def __init__(self, **kwargs):
        Tkinter.LabelFrame.__init__(self, **kwargs)

        self.__fs      = "Filesize: {}"
        self.__fs_var  = Tkinter.StringVar()
        self.__fs_var.set(self.__fs.format("-"))
        fs_label  = Tkinter.Label(self, textvariable=self.__fs_var)
        fs_label.pack(side=Tkinter.BOTTOM)

        self.__bs             = "Data Bytes Sent: {}"
        self.__bs_var  = Tkinter.StringVar()
        self.__bs_var.set(self.__bs.format("-"))
        bs_label      = Tkinter.Label(self, textvariable=self.__bs_var)
        #bs_label.pack(side=Tkinter.BOTTOM)

        self.__ts      = "Total Bytes Sent: {}"
        self.__ts_var  = Tkinter.StringVar()
        self.__ts_var.set(self.__ts.format("-"))
        ts_label      = Tkinter.Label(self, textvariable=self.__ts_var)
        ts_label.pack(side=Tkinter.BOTTOM)

        self.__progress = ttk.Progressbar(self, orient="horizontal",
                                         mode="determinate")
        self.__progress["maximum"] = 100
        self.__progress.pack(side=Tkinter.BOTTOM, expand=1, fill=Tkinter.X)

    def set_filesize(self, size):
        self.__fs_var.set(self.__fs.format(size))
    def set_databytes_sent(self, sent):
        self.__bs_var.set(self.__bs.format(sent))
    def set_total_sent(self, sent):
        self.__ts_var.set(self.__ts.format(sent))
    def set_progress(self, p):
        self.__progress['value'] = p


class FilePanel(object):
    '''
    A class that instances and communicates with a sequencer
    panel that is used to create sequences of commands for
    execution by various types of sequencers.
    '''
    def __init__(self, parent, top, opts):
        '''
        Constructor
        '''
        # The parent notebook widget
        self.__parent = parent
        # The main top widget with the menus
        self.__top = top
        self.__config = ConfigManager.ConfigManager.getInstance()

        # Root window
        self.__root = top.root()
        self.__upload_folder   = None
        self.__upload_file     = None
        self.__download_folder = None
        self.__dest_path_var = Tkinter.StringVar()
        self.__downlink_src_var = Tkinter.StringVar()
        self.__downlink_dest_var = Tkinter.StringVar()


        self.__opts = opts
        #
        self.__cmds         = command_loader.CommandLoader.getInstance()
        self.__commander    = commander.Commander.getInstance()
        self.__ev_listener  = event_listener.EventListener.getInstance()
        self.__status       = status_updater.StatusUpdater.getInstance()


        self.__down_files = {}

        # Uplink/Downlink settings
        if 'BUILD_ROOT' in os.environ:
            build_root = os.environ['BUILD_ROOT']
            log_folder = os.path.join(self.__opts.log_file_path, self.__opts.log_file_prefix)

            # Configure the uplink shell
            self.__uplink_shell = file_uplink_client.UplinkShell.getInstance()
            self.__uplink_shell.config(build_root=build_root, log_folder=log_folder)

            # Configure downlink shell
            self.__downlink_shell = file_downlink_client.DownlinkShell.getInstance()
            self.__downlink_shell.config(build_root=build_root, log_folder=log_folder)
            self.__downlink_shell.spawn(host=self.__opts.addr, port=self.__opts.port)
        else:
            self.__status.update("WARNING: BUILD_ROOT env. variable must be set to uplink files!", color='red')

        ## Panel components
        #
        f = Tkinter.Frame(parent)
        f.pack(side=Tkinter.LEFT, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1)
        f.config(bg="lightblue")

        # Uplink section
        uplink_frame=Tkinter.LabelFrame(f, text="Uplink Files")
        uplink_frame.pack(side=Tkinter.LEFT, anchor=Tkinter.N, fill=Tkinter.X, expand=1, padx=10, pady=10)

        self.__upload_list = Pmw.ScrolledListBox(uplink_frame, listbox_selectmode=Tkinter.SINGLE,
                                  hull_height=120,
                                  selectioncommand=self.__upload_list_select,
                                  usehullsize=1)
        self.__upload_list.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1, padx=10, pady=10)
        folder_select_button = Tkinter.Button(uplink_frame, text="Select Folder", command=self.__folder_select)
        folder_select_button.pack(side=Tkinter.TOP ,anchor=Tkinter.NE)

        # Dest Path entry
        entry_widget = Pmw.EntryField(uplink_frame, labelpos='w', label_text ='Destination Path')
        entry_widget.component('entry').config(textvariable=self.__dest_path_var)
        entry_widget.pack(side=Tkinter.TOP, anchor=Tkinter.W)

        upload_button = Tkinter.Button(uplink_frame,text="Upload", command=self.__upload)
        upload_button.pack(side=Tkinter.TOP, anchor=Tkinter.NE)

        # Upload Status Frame
        self.__uplink_status_frame = StatusFrame(master=uplink_frame, text="Uplink Status", padx=5, pady=5)
        self.__uplink_status_frame.pack(side=Tkinter.BOTTOM, anchor=Tkinter.NW, expand=1, fill=Tkinter.X)

        # Downlink section
        self.__downlink_frame = Tkinter.LabelFrame(f, text="Downlink Files")
        self.__downlink_frame.pack(side=Tkinter.LEFT, anchor=Tkinter.N, fill=Tkinter.X, expand=1, padx=10, pady=10)
        self.__download_list = Pmw.ScrolledListBox(self.__downlink_frame, listbox_selectmode=Tkinter.SINGLE,
                                                   hull_height=120,
                                                   selectioncommand=self.__downlink_list_select,
                                                   usehullsize=1)
        self.__download_list.pack(side=Tkinter.LEFT)
        self.__downlink_status_frame = Tkinter.Frame(self.__downlink_frame)

        # Downlink Source
        entry_widget = Pmw.EntryField(self.__downlink_frame,labelpos='w', label_text = 'Source File', modifiedcommand=self.__downlink_src_modified)
        entry_widget.component("entry").config(textvariable=self.__downlink_src_var)
        entry_widget.pack(side=Tkinter.TOP, anchor=Tkinter.NW)

        # Downlink Destination
        entry_widget = Pmw.EntryField(self.__downlink_frame, labelpos='w', label_text='Destination Path')
        entry_widget.component("entry").config(textvariable=self.__downlink_dest_var)
        entry_widget.pack(side=Tkinter.TOP, anchor=Tkinter.NW)

        request_file_button = Tkinter.Button(self.__downlink_frame, text="Request File", command=self.__request_file)
        request_file_button.pack(side=Tkinter.TOP, anchor=Tkinter.NE)


    def __request_file(self):
        """
        Send downlink command to target.
        """
        src  = self.__downlink_src_var.get()
        dest = self.__downlink_dest_var.get()
        cmd_obj = self.__cmds.getCommandObj("FileDownlink_SendFile")
        cmd_obj.setArgs([src,dest])

        self.__commander.cmd_send(None, cmd_obj)

    def __downlink_src_modified(self):
        """
        Update downlink destination entry
        whenever downlink source is entered.
        """
        src = self.__downlink_src_var.get()
        default_downlink_path = self.__config.get("file_management", "default_downlink_path")
        path = os.path.join(default_downlink_path, src)
        self.__downlink_dest_var.set(path)

    def __downlink_list_select(self):
        """
        Update downlink src entry when a file is
        selected from the selection box.
        """
        file = self.__download_list.getcurselection()[0]
        self.__downlink_file_var.set(file)

    def __upload_list_select(self):
        self.__upload_file  = self.__upload_list.getcurselection()[0]
        default_uplink_path = self.__config.get("file_management", "default_uplink_path")
        path =  os.path.join(default_uplink_path, self.__upload_file)
        self.__dest_path_var.set(path)


    def __set_upload_list(self, dir):
        for f in os.listdir(dir):
            self.__upload_list.insert(Tkinter.END, f)


    def __upload(self):
        """
        Initiate file uplink process.
        """
        src_path   = os.path.join(self.__upload_folder, self.__upload_file)
        dest_path  = self.__dest_path_var.get()

        # Create uplink shell instance.
        self.__uplink_shell.spawn(host=self.__opts.addr, port=self.__opts.port, src_path=src_path, dest_path=dest_path)


    def update(self, observer, arg):
        """
        Updated by file listener.
        """
        uplink_data = observer.get_uplink_data()
        downlink_data = observer.get_downlink_data()

        #print downlink_data
        self.__subprocess_data(type="uplink", **uplink_data)
        self.__subprocess_data(type="downlink", **downlink_data)

        self.__root.update_idletasks()


    def __subprocess_data(self, type, filesize=None, filename=None, progress=None, bytessent=None, checksum=None, totalsent=None, returncode=None):
        """
        Update panel with subproccess data.
        """

        if type == 'uplink':

            if filesize:
                self.__uplink_status_frame.set_filesize(filesize)
            if progress:
                self.__uplink_status_frame.set_progress(progress)
            if totalsent:
                self.__uplink_status_frame.set_total_sent(totalsent)


        elif type == 'downlink':

            # If filename is defined we have a new downlink.
            # Create a status frame
            if filename:
                self.__downlink_status_frame.destroy()
                self.__downlink_status_frame = StatusFrame(master=self.__downlink_frame, text=filename)
                self.__downlink_status_frame.pack(side=Tkinter.TOP, expand=1, fill=Tkinter.X)

            if filesize:
                self.__downlink_status_frame.set_filesize(filesize)

            if progress:
                self.__downlink_status_frame.set_progress(progress)

            if totalsent:
                self.__downlink_status_frame.set_total_sent(totalsent)


        else:
            pass


    def __folder_select(self):
        # Get base default path

        d = tkFileDialog.askdirectory(initialdir = "/", \
                                      mustexist  = True,
                                      title = "Select a folder to display")
        # d is None if user cancelled save
        if not d:
            return

        self.__set_upload_list(d)
        self.__upload_folder = d


    def set_default_upload_folder(self, upload=None, download=None):
        """
        Set upload and download paths.
        File paths default to:
        ~/fprime/files_upload
        ~/fprime/files_download
        @param upload: Upload selection folder
        @param download: Downlink destination
        """

        ## TODO: Clean up
        if upload is None:
            upload = os.environ['HOME'] + os.sep + 'fprime' + os.sep + 'files_upload'
            if not os.path.exists(upload):
                os.makedirs(upload)
        if download is None:
            download = os.environ['HOME'] + os.sep + 'fprime' + os.sep + 'files_download'
            if not os.path.exists(download):
                os.makedirs(download)

        self.__upload_folder   = upload
        self.__download_folder = download
        self.__set_upload_list(upload)
