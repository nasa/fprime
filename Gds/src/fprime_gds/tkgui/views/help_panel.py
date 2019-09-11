import Tkinter
from Tkinter import END
import tkFont
import re
import Pmw
import os

from fprime_gds.tkgui.utils import ConfigManager
from fprime_gds.tkgui.utils import tkGui_misc

class HelpPanel(object):
    '''
    Read amd display help files from the docs folder.
    '''
    def __init__(self, parent, top):
        '''
        Constructor
        '''
       
        # The parent notebook widget
        self.__parent = parent
        # The main top widget with the menus
        self.__top = top
        # Config
        self.__config = ConfigManager.ConfigManager.getInstance()

        self.__md_file_name_list = self.__get_file_list()

        self.__link_text_view = Tkinter.Text(parent, width=16, height=3, relief=Tkinter.GROOVE, borderwidth=1)
        self.__initalize_links()
        self.__link_text_view.pack(side=Tkinter.TOP, expand=0, fill=Tkinter.X)

        self.__help_text_view = None
        self.__md_file   = None

        docs_folder = self.__config.get("helppanel", "doc_folder")
        inital_file = self.__md_file_name_list[0]
        inital_file = os.path.join(docs_folder, inital_file)

        self.__visitors = [HeaderVisitor(), TextVisitor()]
        self.__initalize_panel(inital_file)
       



        # Here are some examples on bullets. 

        #self.__text_view.insert(END, u"This is a normal paragraph. Let's make it a bit long to see that it wraps as expected.\n", 'text_body')
        #self.__text_view.insert(END, u'\t\u25C6', 'bullets')
        #self.__text_view.insert(END, u"\tThis is the first item in the list.\n",'bulleted_list')
        #self.__text_view.insert(END, u'\u25C6', 'bullets')
        #self.__text_view.insert(END, u"\tThis is the second item in the list. Let's make this one quite long too to see how it wraps.\n", 'bulleted_list')


    def __get_file_list(self):
        """
        Get list of file names
        """
        doc_folder = self.__config.get('helppanel','doc_folder')
        md_file_list = []
        for file in os.listdir(doc_folder):
            if file.endswith(".md"):
                md_file_list.append(file)

        return md_file_list

    def __link_click(self, link_name):
        doc_folder = self.__config.get('helppanel','doc_folder')
        file_path  = os.path.join(doc_folder, link_name)
        self.__initalize_panel(file_path)

    def __initalize_links(self):
        """
        - Insert help_panel header
        - Attach tag and callback to links.
        - Insert them into link_text_view
        """


        # Header
        font_size = int(self.__config.get("helppanel", "panel_header_font_size"))
        font = tkFont.Font(family="Helvetica", size=font_size, weight="bold")
        self.__link_text_view.tag_config("panel_header", font=font, justify=Tkinter.CENTER)
        self.__link_text_view.insert(Tkinter.END, "Help Panel\n", "panel_header")


        # Attach tag and callback
        hyperlink = tkGui_misc.HyperlinkManager(self.__link_text_view, justify=Tkinter.CENTER)
        for file_name in self.__md_file_name_list:
            self.__link_text_view.insert(Tkinter.END, file_name, hyperlink.add(self.__link_click, file_name) )
            self.__link_text_view.insert(Tkinter.END, "    ")

        # Make text view read only
        self.__link_text_view.config(state=Tkinter.DISABLED)


    def __initalize_panel(self, md_filepath):
        """
        Opens a markdown file and allows listeners to operate.
        """
        # Destroy if needed then recreate
        if self.__help_text_view:
            self.__help_text_view.pack_forget()
        self.__help_text_view = Pmw.ScrolledText(self.__parent)

        # Open file and count lines
        self.__md_file  = open(md_filepath)
        num_lines = sum(1 for line in self.__md_file)  
        self.__md_file.seek(0) # Remember to return to start!

        # Insert lines
        for i in range(num_lines):
            self.__help_text_view.insert(Tkinter.END, '\n')

        # Let visitors operate
        for v in self.__visitors:
            self.accept_tag_visitor(v)

        for v in self.__visitors:
            self.accept_md_visitor(v)
        
        # Pack it up
        self.__help_text_view.pack(side=Tkinter.TOP, expand=1, fill=Tkinter.BOTH)
        self.__help_text_view.component('text').config(state=Tkinter.DISABLED)


    def accept_tag_visitor(self, v):
        """
        Allow visitors to configure tags.
        """
        v.visit_tag_config(self)

    def accept_md_visitor(self, v):
        """
        Allow visitors to parse md_file.
        """
        v.visit_md_file(self)

    def insert(self, *args):
        """
        Specific insertion method.
        """
        self.__help_text_view.insert(*args)

    def insert_matches(self, matches, tag_name):
        """
        General visitor insertion method. Implement private 
        visitor method for more specific operations.  
        """
        for row_number, m in matches.iteritems():
            self.__help_text_view.insert('{}.0'.format(row_number), m.group(), tag_name)

    def match_file(self, re_pattern, tag_name):
        """
        Search for lines given regex expression.
        Map matches to their line number. Then return that dictionary.
        """
        matches = {}

        for row_number,line in enumerate(self.__md_file):
            # Widget rows start at 1 
            row_number += 1
            m = re_pattern.match(line)
            if m:
                #print m.group(), row_number #Uncomment for regex debugging
                matches[row_number] = m


        self.__md_file.seek(0)
        return matches

    def create_tag(self, *args, **kwargs):
        """
        Configure a tag given options.
        """
        self.__help_text_view.tag_configure(args, kwargs)

    def __del__(self):
        pass


class HeaderVisitor(object):

    def __init__(self):
        self.__config = ConfigManager.ConfigManager.getInstance()
        self.__tag_name = 'header{}'
        self.__re_pattern = re.compile("^[#].*")

    def visit_md_file(self, help_panel):
        """
        Get matches from help_panel. Execute header specific
        insertion.
        """

        matches = help_panel.match_file(self.__re_pattern, self.__tag_name)
        self.__insert_matches(help_panel, matches)

    def __insert_matches(self, help_panel, matches):
        """
        Specific insertion operation.
        """

        for row_number, match in matches.iteritems():

            line = match.group()
            header_size = line.count('#') 

            text = line.split('#')[-1]
            text = text[1:]
 
            help_panel.insert('{}.0'.format(row_number), text, self.__tag_name.format(header_size))


    def visit_tag_config(self, help_panel):
        """
        Create various header sizes and their
        associated tags.
        """

        max_header_num = 5
        for header_num in range(max_header_num):
            # Start index at 1
            header_num += 1

            header_max_size = int(self.__config.get('helppanel', 'max_header_size'))
            header_min_size = int(self.__config.get('helppanel', 'min_header_size'))
            header_size_diff = header_max_size - header_min_size 
            header_num_diff = max_header_num - 1

            # Scale size between max and min possible header sizes 
            font_size = - ( (header_size_diff // header_num_diff) * (header_num - 1) ) + header_max_size 

            font = tkFont.Font(family="Helvetica", size=font_size, weight="bold")
            help_panel.create_tag(self.__tag_name.format(header_num), font=font)

class TextVisitor(object):
    """
    Visitor for plain text.
    """
    def __init__(self):
        self.__config = ConfigManager.ConfigManager.getInstance()
        self.__tag_name = "standard_text"
        self.__re_pattern = re.compile("^([^#\s].*)") # Insert token if handler is written for it. 

    def visit_md_file(self, help_panel):
        """
        Get matches then use help_panel's general insertion method.
        """
        matches = help_panel.match_file(self.__re_pattern, self.__tag_name)
        help_panel.insert_matches(matches, self.__tag_name)

    def visit_tag_config(self, help_panel):
        """
        Configure tag
        """
        font_size = int(self.__config.get('helppanel', 'default_font_size'))
        font = tkFont.Font(family="Helvetica", size=font_size)
        help_panel.create_tag(self.__tag_name, font=font)




