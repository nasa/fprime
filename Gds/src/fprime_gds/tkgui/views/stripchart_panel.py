import Tkinter
import ttk
import tkFileDialog
import Pmw
import os
import time
import datetime
import tkFileDialog
from itertools import cycle

import matplotlib
matplotlib.use('TKAgg') # Which matplotlib backend to use?
from matplotlib.figure import Figure
import matplotlib.mlab
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib import pyplot as plt
from matplotlib.lines import Line2D
import matplotlib.dates as md
from matplotlib.ticker import FuncFormatter
import numpy as np

from fprime_gds.tkgui.controllers import channel_loader
from fprime_gds.tkgui.controllers import event_loader
from fprime_gds.tkgui.controllers import status_updater
from fprime_gds.tkgui.controllers import event_listener
from fprime_gds.tkgui.controllers import stripchart_listener
from fprime_gds.tkgui.controllers import status_bar_updater
from fprime_gds.tkgui.controllers import observer

from fprime_gds.tkgui.utils import ConfigManager
from fprime_gds.tkgui.utils import tkGui_misc
from fprime_gds.common.models.common.event import Severity


import telemetry_filter_panel



class StripPanelException(Exception):
    def __init__(self,val):
        self.except_msg = val
    def getMsg(self):
        return self.except_msg

class ScalingException(StripPanelException):
    def __init__(self, val):
        super(StripPanelException,self).__init__("Scaling Exception: %s !" % str(val))


class StripChartPanel(observer.Observer):
    '''
    Panel object. Display stripcharts
    '''
    def __init__(self, parent, top, opts):
        '''
        Constructor
        '''
        # The parent notebook widget
        self.__parent = parent
        # The main top widget with the menus
        self.__top = top
        #
        self.__config = ConfigManager.ConfigManager.getInstance()
        # Root window
        self.__root = top.root()
        self.__after_id = None

        self.__opts = opts
        #
        self.__sc_listener = stripchart_listener.StripChartListener.getInstance()

        #
        # Instance the channel loader here and get all channel names
        #
        self.__channel_loader = channel_loader.ChannelLoader.getInstance()
        self.__channel_names_list = self.__channel_loader.getNameDict().values()
        self.__channel_names_dict = self.__channel_loader.getNameDict()
        #
        # Dictionary to hold stripcharts
        self.__axes_dict = dict()
        self.__axes_list = []
        self.__channels_active_list = []
        self.__refresh_calls = dict()
        self.__num_plots    = 0
        #
        self.__ev_listener  = event_listener.EventListener.getInstance()
        self.__status       = status_updater.StatusUpdater.getInstance()

        # Container Frame
        f = Tkinter.Frame(parent)
        f.pack(side=Tkinter.TOP, fill=Tkinter.BOTH, expand=1)
        f.columnconfigure(0, weight=1)
        f.rowconfigure(0, weight=1)

        # Controls specific to this panel
        self.__submenu_bar = Tkinter.LabelFrame(f, text="Sub menu")
        self.__submenu_bar.grid(row=1, column=0, sticky='nsew')
        strip_button = Tkinter.Button(self.__submenu_bar, text="Add Stripchart", command=self.__add_strip)
        strip_button.pack(side=Tkinter.LEFT, anchor=Tkinter.NW)
        hist_button = Tkinter.Button(self.__submenu_bar, text="Add Histogram", command=self.__add_hist)
        hist_button.pack(side=Tkinter.LEFT)
        print_button = Tkinter.Button(self.__submenu_bar, text="Save All", command=self.__print_all)
        print_button.pack(side=Tkinter.LEFT)
        settings_button = Tkinter.Button(self.__submenu_bar, text="Settings", command=self.__panel_settings)
        settings_button.pack(side=Tkinter.LEFT)
        # Graph Container to hold n number of graphs
        # Use ScrolledFrame.interior() for actual frame
        self.__graph_container  = Pmw.ScrolledFrame(parent=f, vertflex="expand", horizflex="expand", borderframe=1)
        self.__graph_container._clipper.config(height=0, width=0)
        self.__graph_container.grid(row=0, column=0, sticky='nsew')
        self.__graph_container_frame = self.__graph_container.interior()
        self.__grid_configure()

        # Row and column counters
        self.__row = 0
        self.__col = 0

        ## Panel Settings
        self.__settings_dialog = None
        self.__update_rate_var = Tkinter.StringVar()
        self.__plot_height_var = Tkinter.StringVar()

    def __grid_configure(self):
        for col in range(int(self.__config.get('strippanel', 'max_columns'))):
            self.__graph_container_frame.columnconfigure(col, weight=1)

    def __panel_settings(self):
        """
        Show dialog for panel settings.
        """
        self.__settings_dialog = Pmw.Dialog(self.__root,
            buttons = ('OK', 'Cancel'),
            defaultbutton = 'OK',
            title = 'Pannel Settings',
            command = self.__execute_panel_settings)
        self.__settings_dialog.withdraw()

        p = self.__settings_dialog.interior() # Parent

        default_height = self.__config.get('strippanel', 'plot_height')
        self.__plot_height_var.set(default_height)
        plot_height_entry = Pmw.EntryField(p, labelpos='w', label_text='Plot Heights(Inches)')
        plot_height_entry.component('entry').config(textvariable=self.__plot_height_var)
        plot_height_entry.pack(side=Tkinter.TOP)

        default_rate = self.__config.get('performance', 'stripchart_update_rate')
        self.__update_rate_var.set(default_rate)
        update_rate_entry = Pmw.EntryField(p, labelpos='w', label_text='Update Rate(ms)')
        update_rate_entry.component('entry').config(textvariable=self.__update_rate_var)
        update_rate_entry.pack(side=Tkinter.TOP)

        self.__settings_dialog.activate()

    def __execute_panel_settings(self, button):

        # Set Plot height
        h = self.__plot_height_var.get()
        self.__config.set('strippanel', 'plot_height', h)

        # Set update rate
        u = self.__update_rate_var.get()
        self.__config.set('performance', 'stripchart_update_rate', u)

        self.__settings_dialog.deactivate()
        self.__settings_dialog = None


    def __print_all(self):
        """
        Print all graphs to folder.
        """
        # Get all figures
        fig_list = []
        for row in self.__axes_dict.values():
            for a_frame in row.values():
                fig_list.append(a_frame.get_fig())


        d = tkFileDialog.askdirectory(initialdir = "/", \
                                      mustexist  = False,
                                      title = "Select a folder to save into.")
        # d is None if user cancelled save
        if not d:
            return

        i = 1
        for fig in fig_list:
            fname = os.path.join(d, "fig{}".format(i))
            fig.savefig(fname, dpi=None, facecolor='w', edgecolor='w',
        orientation='portrait', papertype=None, format=None,
        transparent=False, bbox_inches=None, pad_inches=0.1,
        frameon=None)
            i = i+1

    def __add_hist(self):
        self.__add_axes(HistFrame)

    def __add_strip(self):
        self.__add_axes(StripFrame)

    def __add_axes(self, FrameObject):
        """
        Add an empty graph to the graph container frame.
        @param FrameObject: An AxesFrame object
        """
        # Starting a row so create the row


        m = self.__graph_container_frame
        label_text = "Chart {}".format(self.__num_plots)

        # Create a_frame
        a_frame = FrameObject(master=m, parent=self.__parent, panel=self, id=self.__num_plots, label_text=label_text)
        a_frame.default_axis_format()

        # Add to dictionary
        self.__axes_dict[self.__num_plots] = a_frame
        a_frame.grid(row=self.__row, column=self.__col, sticky="nsew")

        self.__increment_grid()

        # Must be called because vert/horizflex == expand
        self.__graph_container.reposition()
        a_frame.update_idletasks()

    def refresh(self):
        """
        Timed refresh.
        AxesFrames register their refresh function after they create a channel.
        """
        #start = time.time()
        for refresh_call in self.__refresh_calls.values():
            refresh_call()

        update_period = self.__config.get('performance', 'stripchart_update_period')
        self.__after_id = self.__root.after(update_period, self.refresh)

    def refresh_cancel(self):
        self.__root.after_cancel(self.__after_id)

    def register_refresh(self, frame_id, func):
        """
        Allow AxesFrame to register their refresh method after
        they start getting data.
        """
        self.__refresh_calls[frame_id] = func

    def notify_exit(self, id):
        """
        Remove a_frame from dictionary.
        Reset grid or regrid as nessecary.
        """
        self.__axes_dict.pop(id, None)
        self.__num_plots -= 1
        if self.__num_plots == 0:
            self.__reset_grid()
            return

        self.__regrid()


    def __regrid(self):
        """
        Regrid axes frames.
        """

        old_list = self.__axes_dict.values()
        self.__reset_grid()

        for a_frame in old_list:
            self.__axes_dict[self.__num_plots] = a_frame
            a_frame.grid(row=self.__row, column=self.__col, sticky="nsew")
            self.__increment_grid()

    def __increment_grid(self):
        """
        Update plot number, rows, and columns
        """
        self.__num_plots += 1
        self.__col += 1
        if self.__col == int(self.__config.get('strippanel','max_columns')):
            self.__col  = 0
            self.__row += 1

    def __reset_grid(self):

        self.__row = 0
        self.__col = 0
        self.__num_plots = 0
        self.__axes_dict = dict()

    def unregister_refresh(self, id):
        self.__refresh_calls.pop(id, None)

    def update(self, observable, arg):
        """
        Listener update.
        Called from StripchartListener

        Only update a_frame if item is not None AND is an active item within the a_frame.
        """
        #now = time.time()
        tlm = observable.get_tlm()
        event_name = observable.get_event_name()
        severity = observable.get_severity()

        for a_frame in self.__axes_dict.values():

            if tlm and tlm['ch_name'] in a_frame.get_active_ch_list():
                a_frame.update_telem(tlm)

            if event_name and event_name in a_frame.get_active_event_list():
                a_frame.update_event(event_name)

            if severity and severity.name in a_frame.get_active_severity_list():
                    a_frame.update_severity(severity)

        #print "STRIPCHART {}".format(time.time() - now)
    def __del__(self):
        """
        Delete all StripFrames.
        """
        for a_frame in self.__axes_dict.values():
            a_frame.__del__()
            a_frame.destroy()

        self.__sc_listener.deleteObserver(self)
        del self.__axes_dict



class StripMode(object):
    """
    Auto adjust view to always show a time window.
    """
    def __init__(self):
         self.__config             = ConfigManager.ConfigManager()
         self.__default_time_width = float(self.__config.get('strippanel', 'default_timewindow'))
         self.__window_width       = self.__default_time_width

    def get_name(self):
        return "Strip Mode"

    def gen_x_scale(self, instance):
        """
        Generate dialog for x scale.
        """
        f1 = Tkinter.Frame(instance._StripFrame__scale_dialog.interior())
        f1.pack(side=Tkinter.BOTTOM)

        # Max Label
        mxl = Tkinter.Label(f1,
            text = 'Time Window in Seconds', pady=4)
        mxl.pack(side=Tkinter.LEFT, anchor=Tkinter.NW, padx = 4, pady = 4)
        # Max Entry
        mxe = Tkinter.Entry(f1, textvariable=instance._StripFrame__max_var)
        mxe.pack(side=Tkinter.RIGHT, anchor=Tkinter.NE, pady=4)

    def gen_y_scale(self, instance):
        """
        Generate dialog for y scale.
        """
        self.gen_dialog(instance, axis='Y')


    def gen_dialog(self, instance, axis):
        """
        Generate general scale dialog.
        @param axis: 'X' or 'Y'
        """
        f1 = Tkinter.Frame(instance._StripFrame__scale_dialog.interior())
        f1.pack(side=Tkinter.BOTTOM)

        # Max Label
        mxl = Tkinter.Label(f1,
            text = '{} Max'.format(axis), pady=4)
        mxl.pack(side=Tkinter.LEFT, anchor=Tkinter.NW, padx = 4, pady = 4)
        # Max Entry
        mxe = Tkinter.Entry(f1, textvariable=instance._StripFrame__max_var)
        mxe.pack(side=Tkinter.RIGHT, anchor=Tkinter.NE, pady=4)

        f2 = Tkinter.Frame(instance._StripFrame__scale_dialog.interior())
        f2.pack(side=Tkinter.TOP)
        #Min Label
        mnl = Tkinter.Label(f2,
            text = '{} Min'.format(axis), pady = 4)
        mnl.pack(side=Tkinter.LEFT, anchor=Tkinter.SW, padx = 4, pady = 4)
        #Min Entry
        mne = Tkinter.Entry(f2, textvariable=instance._StripFrame__min_var)
        mne.pack(side=Tkinter.RIGHT, anchor=Tkinter.SE)

    def execute_default_scale(self, instance):
        """
        Setup default scale.
        """
        self.execute_x_scale(instance, self.__default_time_width, 0)
        instance.set_xscale_toggle(1)

    def execute_x_scale(self, instance, max, min):
        """
        @param instance: StripFrame instance
        @param max: Time window width in seconds
        @param min: Not used here

        """
        instance.show_scroll()
        self.__window_width = float(max)


    def handle_x_scale(self, instance):
        """
        Update the Axes to maintain window width.
        """
        window_width_us = 0
        window_width_s = self.__window_width


        # Regard decimal entry as microseconds
        if window_width_s < 0:
            window_width_us = int(window_width_s * 1000000)
            window_width_s = 0

        channel_lines = instance.get_channel_lines_dict()
        line, t_data, y_data = channel_lines.itervalues().next()

        try:
            window_stop = md.num2date(t_data.getHead())
        except ValueError, e:
            window_stop = datetime.datetime.now()

        window_start = window_stop - datetime.timedelta(seconds=window_width_s, microseconds=window_width_us)
        #print "WINDOW WIDTH: %d"%window_width_s

        #print window_stop
        #print window_start

        ## Scale window
        instance.set_x_limits(window_start, window_stop)
        instance.set_x_ticks(window_start, window_stop)

    def execute_y_scale(self, instance, max, min):
        """
        Setup y scale.
        """
        instance.set_yscale_toggle(1)
        instance.autoscale_y(False) # Custom scale disables autoscale
        instance.set_y_limits(float(min), float(max))

    def handle_y_scale(self, instance):
        """
        Do nothing by default
        """
        pass

    def remove_scale(self, instance, axis):
        if axis == 'X':
            self.remove_x_scale(instance)
        elif axis == 'Y':
            self.remove_y_scale(instance)

    def remove_x_scale(self, instance):
        self.__window_width  = self.__default_time_width
        #instance.set_xscale_toggle(0)

    def remove_y_scale(self, instance):
        """
        Restore y autoscale
        """
        instance.set_yscale_toggle(0)
        instance.autoscale_y(True)


    def get_window_width(self):
        return self.__window_width



class AxesFrame(Tkinter.LabelFrame):
    """
    Base class for graph frames.
    """

    def __init__(self, master, parent, panel, id, label_text):
        """
        @param master: Master widget within stripchart panel (graph container)
        @param parent: Panel widget
        @param label_text: String description
        """
        Tkinter.LabelFrame.__init__(self, master=master, text=label_text)

        self._master = master
        self._parent = parent
        self._panel  = panel
        self._id     = id
        self._config = ConfigManager.ConfigManager.getInstance()

        # Used to cycle through 6 distinct colors
        self._cycol = cycle('bgrcmk').next

        # Create figure and axes
        self._fig, self._axes = plt.subplots()

        self._set_default_plot_margins()
        #plt.tight_layout(pad=.6, w_pad=0.5, h_pad=1.0)

        # Set height of figure in inches
        self._fig.set_figheight(int(self._config.get("strippanel", "plot_height")))

        self._axes.set_autoscaley_on(True)
        self._axes.set_autoscalex_on(True)

        # Channel config
        self._channel_loader = channel_loader.ChannelLoader.getInstance()
        self._available_channels_list = self._channel_loader.getNameDict().values()

        self._active_channels_list = []
        self._active_event_list = []
        self._active_severity_list = []


        # Common buttons
        top_button_frame = Tkinter.Frame(self)
        top_button_frame.pack(side=Tkinter.TOP, anchor=Tkinter.NE)
        self.__exit_button  = Tkinter.Button(top_button_frame, text="Exit", command=self.__exit)
        self.__exit_button.pack(side=Tkinter.RIGHT)
        self.__clear_button = Tkinter.Button(top_button_frame, text="Clear", command=self.clear_axes_frame)
        self.__clear_button.pack(side=Tkinter.RIGHT)
        self.__remove_legend_button = Tkinter.Button(top_button_frame, text="Toggle Legend", command=self.__toggle_legend)
        self.__remove_legend_button.pack(side=Tkinter.RIGHT)
        self.__settings_button = Tkinter.Button(top_button_frame, text='Label Settings', command=self.__prompt_settings)
        self.__settings_button.pack(side=Tkinter.RIGHT)


        # Create canvas
        self._canvas = FigureCanvasTkAgg(self._fig, master=self)
        # Attach canvas to self
        self._canvas.get_tk_widget().pack(side=Tkinter.TOP, expand=1, fill=Tkinter.X, padx=5, pady=5)
        self._canvas.show()

        # Label Settings
        self.__settings_dialog = None
        self.__x_label = Tkinter.StringVar()
        self.__y_label = Tkinter.StringVar()
        self.__plot_title = Tkinter.StringVar()
        self.__show_legend = True

    def __prompt_settings(self):
        """
        Display window where user can choose plot
        xaxis label, yaxis label, and plot title.
        """
        self.__settings_dialog = Pmw.Dialog(self,
            buttons = ('OK', 'Cancel'),
            defaultbutton = 'OK',
            title = 'Plot Settings',
            command = lambda button: self.__execute_settings(button))
        self.__settings_dialog.withdraw()

        f1 = Tkinter.Frame(self.__settings_dialog.interior())
        f1.pack(side=Tkinter.TOP)

        # Entry Frame 1
        x_label_entry = Pmw.EntryField(f1, labelpos='w', label_text="X Axis Label")
        x_label_entry.component('entry').config(textvariable=self.__x_label)
        x_label_entry.pack(side=Tkinter.TOP)

        y_label_entry = Pmw.EntryField(f1, labelpos='w', label_text='Y Axis Label')
        y_label_entry.component('entry').config(textvariable=self.__y_label)
        y_label_entry.pack(side=Tkinter.TOP)

        title_label_entry = Pmw.EntryField(f1, labelpos='w', label_text='Plot Title')
        title_label_entry.component('entry').config(textvariable=self.__plot_title)
        title_label_entry.pack(side=Tkinter.TOP)


        self.__settings_dialog.activate()

    def __execute_settings(self, button):
        """
        Executed by settings dialog.
        """
        if button == 'Cancel':
            return

        x_label = self.__x_label.get()
        y_label = self.__y_label.get()
        title   = self.__plot_title.get()

        if x_label is not '':
            self._axes.set_xlabel(x_label)
        if y_label is not '':
            self._axes.set_ylabel(y_label)
        if title is not '':
            self._axes.set_title(title)

        # Clean up
        self.__settings_dialog.deactivate()
        self.__scale_dialog = None
        self.__in_scale_dialog = False

        self.draw_canvas()

    def _set_default_plot_margins(self):
        """
        Set plot margins from .ini settings.
        """
        b = float(self._config.get("strippanel", "plot_bottom_margin"))
        l = float(self._config.get("strippanel", "plot_left_margin"))
        self._fig.subplots_adjust(bottom=b, left=l)

    def __toggle_legend(self):
        """
        Turn legend on/off.
        """
        try:
            self.__show_legend = not self.__show_legend
            self._axes.get_legend().set_visible(self.__show_legend)
        except Exception, e:
            pass


    def get_active_ch_list(self):
        return self._active_channels_list

    def get_active_event_list(self):
        return self._active_event_list

    def get_active_severity_list(self):
        return self._active_severity_list

    def set_x_ticks(self, window_start, window_stop):
        """
        Sets x axis ticks given datetime limits.
        """
        if window_start > window_stop:
            raise Exception("window_start must be < window_stop")

        tick_sequence = []
        tick_sequence.append(window_start)
        interval = (window_stop - window_start) // int(self._config.get('strippanel','default_tick_amount'))
        while window_start < window_stop:
            window_start += interval
            tick_sequence.append(window_start)

        self._axes.xaxis.set_ticks(tick_sequence)

    def autoscale_y(self, enable):
        """
        @param enable: Boolean to toggle autoscale
        """
        self._axes.set_autoscaley_on(enable)

    def autoscale_x(self, enable):
        """
        @param enable: Boolean to toggle autoscale
        """
        self._axes.set_autoscalex_on(enable)

    def set_y_limits(self, y_min, y_max):
        self._axes.set_ylim(y_min, y_max)

    def set_x_limits(self, x_min, x_max):
        self._axes.set_xlim([x_min, x_max])

    def get_fig(self):
        return self._fig

    def draw_canvas(self):

        self._fig.canvas.draw()
        self._fig.canvas.flush_events()
        self._parent.update_idletasks()


    def __exit(self):
        self._panel.notify_exit(self._id)
        self.destroy()


    def clear_axes_frame(self):
        """
        Leave it to child class to implement.
        """
        pass
    def update_telem(self, tlm):
        """
        By default do nothing
        """
        pass
    def update_severity(self, severity):
        """
        By default do nothing
        """
        pass
    def update_event(self, event_name):
        """
        By default do nothing
        """
        pass
    def default_axis_format(self):
        """
        By default do nothing
        """
        pass

    def __del__(self):
        """
        Destructor
        """
        del self._axes
        del self._fig



class HistFrame(AxesFrame):
    """
    Displays a histogram of the frequency of channels, severity, and events.
    """
    def __init__(self, master, parent, panel, id,  label_text):
        """
        @param master: Master widget within stripchart panel (graph container)
        @param parent: Panel widget
        @param label_text: String description
        """
        AxesFrame.__init__(self, master, parent, panel, id, label_text)

        # Add severity to available selections
        self._severity_list = [item.name for item in Severity]
        self._active_severity_list = []

        # Add events to available selections
        self.__event_list    = [obj.getName() for obj in event_loader.EventLoader.getInstance().getEventsDict().values()]
        self._active_event_list = []
        self.__all_active = dict() # Severity, channels, events
        self.__color_list = []


        button_frame  = Tkinter.Frame(self)
        button_frame.pack(side=Tkinter.BOTTOM, anchor=Tkinter.W, expand=1, fill=Tkinter.X)
        self.__add_channel_button = Tkinter.Button(button_frame, text="Add channel", command=self.__select_channel)
        self.__add_channel_button.pack(side=Tkinter.LEFT)
        self.__add_severity_button = Tkinter.Button(button_frame, text="Add Severity", command=self.__select_severity)
        self.__add_severity_button.pack(side=Tkinter.LEFT)
        self.__add_event_button    = Tkinter.Button(button_frame, text="Add event", command=self.__select_event)
        self.__add_event_button.pack(side=Tkinter.LEFT)

        self.__total_count = 0
        self.__inital_run  = True

    def default_axis_format(self):
        """
        Default histogram format
        """
        self._axes.set_autoscaley_on(False)
        self._axes.set_ylim([0.0, 1.0])

        self._axes.set_xlabel("Bins")
        self._axes.set_ylabel("")
        self._axes.set_title("")

        self._set_default_plot_margins()
        self._axes.xaxis.set_ticks([])


    def clear_axes_frame(self):
        """
        Reset all counters and clear axes.
        """

        self.__total_count = 0

        self._severity_list.extend(self._active_severity_list)
        self._active_severity_list = []

        self.__event_list.extend(self._active_event_list)
        self._active_event_list = []

        self._available_channels_list.extend(self._active_channels_list)
        self._active_channels_list = []

        self.__all_active = dict()

        self._axes.cla()
        self.default_axis_format()
        self.refresh_plot()



    def __select_severity(self):
        """
        Open severity selection dialog
        """
        active_list, avail_list = self.__select(self._severity_list, self._active_severity_list)
        self._active_severity_list = active_list
        self._severity_list        = avail_list

    def __select_channel(self):
        """
        Open channel selection dialog.
        """
        active_list, avail_list = self.__select(self._available_channels_list, self._active_channels_list)
        self._available_channels_list = avail_list
        self._active_channels_list    = active_list

    def __select_event(self):
        """
        Open event selection dialog.
        """
        active_list, avail_list = self.__select(self.__event_list, self._active_event_list)
        self.__event_list = avail_list
        self._active_event_list = active_list

    def __select(self, avail_list, active_list):
        """
        Open selection dialog given available and active lists.
        """
        ch = telemetry_filter_panel.telemetryFilterDialog(self._parent, avail_list, active_list)
        if ch[0] == None: # if cancelled, keep current state
            return active_list, avail_list

        active_list    = ch[1]
        avail_list     = ch[3]

        for name in active_list:
            self.add_to_display(name)

        return active_list, avail_list


    def add_to_display(self, name):
        """
        Add event, severity, or channel to all_active list.
        all_active list is a list of bins displayed in the histogram.
        """
        if name in self.__all_active:
            return

        self.__all_active[name] = 0
        self.__color_list.append(self._cycol())

    def update_telem(self, tlm):
        """
        Increment total number of channel packets recieved.
        """
        ch_name = tlm['ch_name']
        self.update_increment(ch_name)

    def update_severity(self, severity):
        """
        Increment total number of events by severity.
        """
        self.update_increment(severity.name)

    def update_event(self, event_name):
        """
        Increment total number of specific events
        """
        self.update_increment(event_name)

    def update_increment(self, name):
        """
        Increment a dictionary entry.
        Return is entry not found.
        """
        try:
            self.__all_active[name] = self.__all_active[name] + 1
        except Exception, e:
            return

        self.__total_count = self.__total_count + 1

        if self.__inital_run:
            self.__inital_run = False
            self._panel.register_refresh(self._id, self.refresh_plot)


    def refresh_plot(self):
        """
        Registered to StripChartPanel.

        - Get names of active event, channel, severity items
        - Get their values
        - Normalize to the total count

        - Create rectangles
        - Set Labels
        - Redraw canvas
        """
        names    = self.__all_active.keys()
        heights  = np.array(self.__all_active.values()) # Get as np array so we can normalize
        heights   = heights.astype(float) / self.__total_count
        num_bars = len(heights)

        self._axes.cla()
        rects = self._axes.bar(left=range(num_bars), height=heights, tick_label=names, color=self.__color_list, align='center')
        self._axes.set_autoscaley_on(False)
        self._axes.set_ylim([0.0,1.0])

        # Set labels
        for idx, rec in enumerate(rects):
            height = rec.get_height()
            self._axes.text(rec.get_x() + rec.get_width()/2., 1.05*height, "{}".format(self.__all_active.values()[idx]))

        self.draw_canvas()



class StripFrame(AxesFrame):
    """
    Hold axes, figure, canvas objects of the plot.
    """
    def __init__(self, master, parent, panel, id, label_text):
        """
        @param master: Master widget within stripchart panel (graph container)
        @param parent: Panel widget
        @param label_text: String description
        """
        AxesFrame.__init__(self, master, parent, panel, id, label_text)

        self.__stat_bar_updater = status_bar_updater.StatusBarUpdater.getInstance()

        #
        self.__channel_lines = {}
        self.__spectrum_lines = []
        self.__limit_lines = []

        # X Axis formating
        self.__grid_on    = False
        self.__toggle_grid()

        # Set scale mode
        self.__SCALE_MODE = StripMode()
        self.__in_scale_dialog = False
        #
        self.__scale_dialog = None
        self.__max_var      = Tkinter.StringVar() # Used for scale dialog
        self.__min_var      = Tkinter.StringVar() #
        #
        self.__user_xscale = Tkinter.IntVar() # X scale toggle
        self.__user_yscale = Tkinter.IntVar() # Y scale toggle
        self.__user_xscale.set(0) # Custom toggle is off
        self.__user_yscale.set(0)
        #
        self.__xscale_button_toggle = Tkinter.IntVar()
        self.__yscale_button_toggle = Tkinter.IntVar()
        self.__xscale_button_toggle.set(0)
        self.__yscale_button_toggle.set(0)
        #
        #
        button_frame  = Tkinter.Frame(self)
        button_frame.pack(side=Tkinter.BOTTOM, anchor=Tkinter.W, expand=1, fill=Tkinter.X)
        self.__stop_button   = Tkinter.Button(button_frame, text="Stop", command=self.__toggle_stop)
        self.__stop_button.pack(side=Tkinter.LEFT)
        self.__xscale_button = Tkinter.Button(button_frame, text="Scale X", command=self.__prompt_xscale)
        self.__xscale_button.pack(side=Tkinter.LEFT)
        self.__yscale_button = Tkinter.Button(button_frame, text="Scale Y", command=self.__prompt_yscale)
        self.__yscale_button.pack(side=Tkinter.LEFT)
        self.__select_button = Tkinter.Button(button_frame, text="Add Channel", command=self.__select_channel)
        self.__select_button.pack(side=Tkinter.LEFT)
        self.__grid_button   = Tkinter.Button(button_frame, text="Toggle Grid", command=self.__toggle_grid)
        #self.__grid_button.pack(side=Tkinter.LEFT)
        self.__spectrum_button = Tkinter.Button(button_frame, text="Toggle Spectrum", command=self.__toggle_spectrum)
        self.__spectrum_button.pack(side=Tkinter.LEFT)

        # Scroll Bar
        self.__scroll_bar  = Tkinter.Scrollbar(self, command=self.__xview, orient=Tkinter.HORIZONTAL)
        self.__scroll_frame = Tkinter.Frame(button_frame)
        self.__scroll_var   = Tkinter.IntVar()
        self.__scroll_var.set(0)
        scroll_cb    = Tkinter.Checkbutton(self.__scroll_frame, variable=self.__scroll_var, command=self.__toggle_scroll)
        scroll_cb.pack(side=Tkinter.RIGHT)
        scroll_text  = Tkinter.Label(self.__scroll_frame, text="Enable Scroll")
        scroll_text.pack(side=Tkinter.RIGHT)

        # Spectrum
        self.__do_spectrum     = False  # Graph spectrum or not
        self.__last_spectrum_update = 0 # Time of last spectrum update. Used to limit how fast spectrum updates.
        self.__spectrum_label       = "(Spectrum)"


        self.__stop = False # Stop/Start refresh
        self.__inital_run = True


    def default_axis_format(self):
        """
        Restore xaxis format to default.
        """

        def custom_date_format(x, pos=None):
            """
            Strip off superfluous microsecond padding
            """
            x = md.num2date(x)
            fmt = '%H:%M:%S.%f'
            label = x.strftime(fmt)
            arr = label.split('.')

            hms        = arr[0]
            micro_secs = arr[1]
            micro_secs = micro_secs[:2]
            label = hms + '.' + micro_secs
            return label


        cfmt = matplotlib.ticker.FuncFormatter(custom_date_format)

        self._axes.xaxis.cla()
        self._axes.xaxis_date()

        self._axes.xaxis.set_major_formatter(cfmt)
        plt.xticks(rotation='35')

        # Axis Labels
        self._axes.set_title('')
        self._axes.set_ylabel('Amplitude')
        self._axes.set_xlabel('Time (H:M:S.MS)')


    def clear_axes_frame(self):
        """
        Clear all selected channels and reset the plot.
        """
        self._available_channels_list.extend(self._active_channels_list)
        self._active_channels_list = []

        self.__channel_lines = {}

        self.__user_xscale.set(0) # Custom toggle is off
        self.__user_yscale.set(0)
        self.__xscale_button_toggle.set(0)
        self.__yscale_button_toggle.set(0)
        self.__scroll_var.set(0)
        self.hide_scroll()

        self.__stop = False
        self.__inital_run = True

        self._set_default_plot_margins()
        self._axes.cla()
        self.default_axis_format()
        self._axes.xaxis.set_ticks([])
        self.draw_canvas()


    def __xview(self, *args):
        """
        Registered to scroll bar. Called whenever scroll bar moves.
        """
        position = args[1]
        self.__scroll_bar.set(position, position)
        self.__handle_scroll(position)

    def __toggle_grid(self):
        """
        Turn Grid on/off
        """
        self.__grid_on = not self.__grid_on
        self._axes.grid(b=self.__grid_on, which='both')

    def __toggle_spectrum(self):
        """
        Turn spectrum plot on/off
        """
        self.__do_spectrum = not self.__do_spectrum


        if self.__do_spectrum:
            # Remove existing lines
            for line, t_data, y_data in self.__channel_lines.values():
                line.remove()

            # Turn off scaling
            self.__user_yscale.set(0)
            self.__user_xscale.set(0)
            self.autoscale_y(True)
            self.autoscale_x(True)

            # Create new line
            l = Line2D([],[])
            self.__spectrum_lines.append(l)
            self._axes.add_line(l)

            # Reformat xaxis
            self.__spectrum_xaxis_format()

            # Update legend
            legend = self._axes.get_legend()
            for l in legend.get_texts():
                l.set_text(l.get_text() + self.__spectrum_label)
        else:
            # Problems with going back.
            # Disable revert for now.
            self.__do_spectrum = True
            self.__stat_bar_updater.update_message("Unable to revert. Open new time plot.")
            return

            if self.__xscale_button_toggle.get() == 1:
                self.__prompt_xscale()

            # Remove Existing line
            for l in self._axes.get_lines():
                l.remove()

            # Restore previous lines
            for l in self.__channel_lines.values():
                self._axes.add_line(l)

            # Restore xaxis format
            self.default_axis_format()

            # Restore legend
            legend = self._axes.get_legend()
            for l in legend.get_texts():
                label_length = len(self.__spectrum_label)
                l.set_text(l.get_text()[:-label_length])

            # Restore default scaling
            self.__SCALE_MODE.remove_x_scale(self)

        self.refresh_plot()

    def __handle_scroll(self, position):
        """
        Connected to scroll bar.
        Move time window based off scroll bar position.
        @param position: Relative position of scrollbar 0.0 to 1.0
        """
        line_data, t_data, y_data = self.__channel_lines.itervalues().next()

        abs_max_timestamp = md.num2date(t_data.getHead())
        #print abs_max_timestamp

        abs_min_timestamp = md.num2date(t_data.getTail())
        #print abs_min_timestamp

        window_width = self.__SCALE_MODE.get_window_width()
        #print window_width

        delta = abs_max_timestamp - abs_min_timestamp
        #print delta

        max_start_offset_timestamp  = delta - datetime.timedelta(seconds=window_width)
        #print max_start_offset_timestamp

        seconds  = max_start_offset_timestamp.seconds
        us       = max_start_offset_timestamp.microseconds

        seconds  = seconds * float(position)
        us       = us * float(position)

        time_stamp_delta_offset = datetime.timedelta(seconds=seconds, microseconds=us)
        #print time_stamp_delta_offset

        window_start = abs_min_timestamp + time_stamp_delta_offset
        window_stop  = window_start + datetime.timedelta(seconds=window_width)

        #print window_start
        #print window_stop

        self.set_x_limits(window_start, window_stop)
        self.set_x_ticks(window_start, window_stop)
        self.draw_canvas()

    def __spectrum_xaxis_format(self):
        """
        Reconfigure xaxis format to handle spectral plot
        """
        # Reconfigure xaxis format
        scalar_formatter = matplotlib.ticker.ScalarFormatter()
        self._axes.xaxis.cla() # Clear all ticks
        self._axes.xaxis.set_major_formatter(scalar_formatter)


    def __toggle_scroll(self):
        """
        Show or remove scroll bar
        """
        if self.__scroll_var.get() == 1:
            # Set scroll bar to middle
            self.__scroll_bar.pack(side=Tkinter.TOP, fill=Tkinter.X, expand=1)
            self.__toggle_stop()
        elif self.__scroll_var.get() == 0:
            self.__scroll_bar.pack_forget()
            self.__toggle_stop()

        self.__scroll_bar.set(1.0,1.0)


    def __toggle_stop(self):
        """
        Enable/Disable visual refresh.
        Data is still collected if __stop == True
        """
        if self.__stop == False:
            self.__stop = True
            self.__stop_button.config(text="Start")
        else:
            self.__stop = False
            self.__stop_button.config(text="Stop")


    def __change_mode(self):
        """
        Changes to next scale mode.
        Scale modes are initalized when self.__cymod is created.
        """
        self.__SCALE_MODE = self.__cymod()
        self.__mode_text.config(text=self.__mt.format(self.__SCALE_MODE.get_name()))


    def add_channel(self, ch_name):
        """
        Add channel to axes.
        Set a unique line color.
        Also set max and min channel limit lines based off of gse.ini.
        @param ch_name: String channel name
        """

        # Create line
        color = self._cycol()
        line  = Line2D([],[], color=color, marker='.')

        # Add line to axes and save the reference in channel_lines
        # Also create fixed sized circular buffers for x and y data to stream into
        self._axes.add_line(line)
        buffer_capacity = int(self._config.get('strippanel', 'buffer_capacity'))
        self.__channel_lines[ch_name] = line, tkGui_misc.CircularBuffer(buffer_capacity), tkGui_misc.CircularBuffer(buffer_capacity)

        # Add limit lines.
        if self._config.has_option('channel_max_limit', ch_name):
            if ch_name not in self.__limit_lines:
                max = self._config.get('channel_max_limit', ch_name)
                min = self._config.get('channel_min_limit', ch_name)
                plt.axhline(color='r', y=max, label="{} MAX".format(max))
                plt.axhline(color='r', y=min, label="{} MIN".format(min))
                self.__limit_lines.append(ch_name)

        line_list = self._axes.get_lines()
        ch_name_list = self.__channel_lines.keys()
        self._axes.legend(line_list, ch_name_list, loc='upper left', shadow=True)
        self.draw_canvas()

    def update_telem(self, tlm):
        """
        Update data specific to a channel.
        Stream data into circular buffers.

        @param ch_name: Channel Name
        @param datetime_object: python datetime object created from telem timestamp
        @param ch_value: channel data
        """

        # Unpack
        ch_name = tlm['ch_name']
        ch_value = tlm['ch_value']
        datetime_object = tlm['datetime_object']

        # Get circular buffers of channel
        line_data, t_data, y_data = self.__channel_lines[ch_name]

        t_data.add(md.date2num(datetime_object))
        y_data.add(ch_value)


        if self.__do_spectrum:
            # Limit update time
            if time.time() - self.__last_spectrum_update < 2:
                pass
            else:
                y = y_data.asArray()
                self.__last_spectrum_update = time.time()
                spec, freq = matplotlib.mlab.magnitude_spectrum(y, Fs=25, window=None, pad_to=None, sides=None)

                # Get the line set in __toggle_spectrum.
                for spec_line in self.__spectrum_lines:
                    spec_line.set_xdata(freq)
                    spec_line.set_ydata(spec)


        # Do startup
        if self.__inital_run:

            self._panel.register_refresh(self._id, self.refresh_plot)
            self.__SCALE_MODE.execute_default_scale(self)
            self.__inital_run = False



    def refresh_plot(self):
        """
        Refresh method registered to StripchartPanel.
        """

        # Do not update the plot if stop button is pressed.
        if self.__stop:
            return

        # Do not set data and scaling if ploting spectrum
        if not self.__do_spectrum:
            for line_data, t_data, y_data in self.__channel_lines.values():
                line_data.set_xdata(t_data.asArray())
                line_data.set_ydata(y_data.asArray())

        try:
            self.__handle_scaling()
        except ScalingException, e:
            print e

        self.draw_canvas()


    def __select_channel(self):
        """
        Open channel selection dialog.
        """
        ch = telemetry_filter_panel.telemetryFilterDialog(self._parent, self._available_channels_list, self._active_channels_list)
        if ch[0] == None: # if cancelled, keep current state
            return

        self._active_channels_list    = ch[1]
        self._available_channels_list = ch[3]


        for ch_name in self._active_channels_list:
            self.add_channel(ch_name)


    def __prompt_xscale(self):
        """
        Start X scale sequence
        """
        self.__prompt_scale("X", self.__xscale_button, self.__xscale_button_toggle)

    def __prompt_yscale(self):
        """
        Start Y scale sequence
        """
        self.__prompt_scale("Y", self.__yscale_button, self.__yscale_button_toggle)


    def __prompt_scale(self, axis, button, toggle):
        """
        Create dialog to get min and max limits of axis to scale.
        @param axis: String of axis name
        @param button: Button that was pressed
        @param var: Tkinter variable that is toggled
        """

        ## Button toggle sequence
        #
        # Button is on. Turn button off and disable custom scaling
        if toggle.get() == 1:
            button.config(text="Scale {}".format(axis))
            toggle.set(0)
            self.__SCALE_MODE.remove_scale(self, axis)
            return
        else: # Button is off. So turn on
            button.config(text="Disable {} Scale".format(axis))
            toggle.set(1)


        self.__scale_dialog = Pmw.Dialog(self,
            buttons = ('OK', 'Cancel'),
            defaultbutton = 'OK',
            title = '{} Axis Scale'.format(axis),
            command = lambda button, axis=axis, widget=button: self.__execute_scale(button, axis, widget))
        self.__scale_dialog.withdraw()


        if axis == 'Y':
            self.__SCALE_MODE.gen_y_scale(self)
        elif axis == 'X':
            self.__SCALE_MODE.gen_x_scale(self)

        self.__in_scale_dialog = True
        self.__scale_dialog.activate()



    def __execute_scale(self, button, axis, widget):
        """
        Setup axes for scaling based off selected mode.

        @param button: String descriptor of dialog button that was clicked
        @param axis: String descriptor of which axis to scale
        @param widget: Widget that initalized scale sequence
        """
        if button == "Cancel" or button == None:
            widget.invoke() # Toggle button back to it's original position if dialog is canceled
        else:

            max = self.__max_var.get()
            min = self.__min_var.get()

            self._axes.relim()
            if axis == 'X':
                self.__SCALE_MODE.execute_x_scale(self, max, min)
            elif axis =='Y':
                self.__SCALE_MODE.execute_y_scale(self, max, min)


        # Clean up
        self.__max_var.set('')
        self.__min_var.set('')
        self.__scale_dialog.deactivate()
        self.__scale_dialog = None
        self.__in_scale_dialog = False

    def __handle_scaling(self):
        """
        Scale the axes based off selected mode.
        """

        # User is busy...don't continue
        if self.__in_scale_dialog:
            return

        scaley = self.__user_yscale.get()
        scalex = self.__user_xscale.get()

        self._axes.relim()
        if scalex and scaley:
            self.__SCALE_MODE.handle_x_scale(self)
            self.__SCALE_MODE.handle_y_scale(self)
        elif scalex:
            self._axes.autoscale(enable=True, axis='y')
            self.__SCALE_MODE.handle_x_scale(self)
        elif scaley:
            self._axes.autoscale(enable=True, axis='x')
            self.__SCALE_MODE.handle_y_scale(self)
        else:
            self.__auto_relim()

    def __auto_relim(self):
        """
        Relimit the axes and enable autoscale for both axes.
        """
        #Configure the axes
        self._axes.relim()
        self._axes.autoscale(enable=True, axis='both')
        self._axes.autoscale_view()

    def set_xscale_toggle(self, val):
        self.__user_xscale.set(val)

    def set_yscale_toggle(self, val):
        self.__user_yscale.set(val)

    def get_channel_lines_dict(self):
        return self.__channel_lines

    def show_scroll(self):
        self.__scroll_frame.pack(side=Tkinter.RIGHT)

    def hide_scroll(self):
        self.__scroll_frame.pack_forget()
