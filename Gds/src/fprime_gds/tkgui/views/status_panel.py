
import Tkinter
import Pmw

from fprime_gds.tkgui.controllers import exceptions
from fprime_gds.tkgui.controllers import observer

class StatusPanel(observer.Observer):
    '''
    A class that instances and communicates with a sequencer
    panel that is used to create sequences of commands for
    execution by various types of sequencers.
    '''
    def __init__(self, parent, top):
        '''
        Constructor
        '''
        # The parent notebook widget
        self.__parent = parent
        # The main top widget with the menus
        self.__top = top

        self.__display = Pmw.ScrolledText(parent, hscrollmode='dynamic',
                                            vscrollmode='dynamic', hull_relief='sunken',
                                            hull_height=10,
                                            hull_background='lightgray', hull_borderwidth=10,
                                            text_background='linen', text_width=1,
                                            text_foreground='black', text_height=5,
                                            text_padx=10, text_pady=10, text_relief='groove',
                                            text_font=('arial', 16, 'bold'),
                                            usehullsize=1)
        self.__display.pack(side=Tkinter.TOP, expand=Tkinter.YES, fill=Tkinter.BOTH)
        #self.display.grid(row=3,sticky=Tkinter.S)
        self.__display.tag_config('blue', foreground='blue')
        self.__display.tag_config('red', foreground='red')
        self.__display.tag_config('green', foreground='green')
        self.__display.tag_config('black', foreground='black')


    # Interface for updating status text via observer singleton
    def update(self, observer):
        """
        Update the status text widget for each
        instanced main panel.  This is only called
        from the status_updater singleton observer
        class.
        """
        if observer.update_mode == "Update":
            self.statusUpdate(observer.status_string, observer.text_color)
        elif observer.update_mode == "Clear":
            self.statusClear()
        elif observer.update_mode == "Text":
            self.statusSetText(observer.status_string)
        else:
            raise exceptions.GseControllerStatusUpdateException(observer.update_mode)

    def statusUpdate(self, s, color='black'):
        """
        Update status on the text widget.
        """
        self.__display.insert(Tkinter.END, s+'\n', color)
        self.__display.see(Tkinter.END)
        self.__parent.update()


    def statusClear(self):
        """
        Clear the text widget.
        """
        self.__display.clear()


    def statusSetText(self, str):
        """
        Reset the text in the status text widget.
        This clears any text and puts new text in.
        """
        self.__display.setvalue(str)



    # Interface for updating status text via observer singleton
    def update(self, observable, arg):
        """
        Update the status text widget for each
        instanced main panel.  This is only called
        from the status_updater singleton observable
        class.
        """
        if observable.update_mode == "Update":
            self.statusUpdate(observable.status_string, observable.text_color)
        elif observable.update_mode == "Clear":
            self.statusClear()
        elif observable.update_mode == "Text":
            self.statusSetText(observable.status_string)
        else:
            raise exceptions.GseControllerStatusUpdateException(observable.update_mode)
