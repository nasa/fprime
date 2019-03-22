import Tkinter
import ttk
import logging
import Pmw
import sys
import os
import csv
import tkFileDialog

from optparse import OptionParser

from fprime_gds.tkgui.utils import ConfigManager
from fprime_gds.tkgui.controllers import channel_loader
from fprime_gds.tkgui.tpe_models import SessionModel
from fprime_gds.tkgui.controllers.tpe_controllers import SessionController
from fprime_gds.tkgui.controllers.exceptions import GseControllerUndefinedDirectoryException

# Setup logging
Logger = logging.getLogger(__name__)
sh     = logging.StreamHandler()
#sh.setFormatter(logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s'))
Logger.addHandler(sh)
Logger.setLevel(logging.DEBUG)



def main(opts):

	root = Tkinter.Tk()
	root.minsize(500, 600)
	root.title("F Prime Telemetry Packet Editor")

	# Try to load channels.
	ch_path = os.path.join(opts.gen_path, "channels")
	ch_loader = channel_loader.ChannelLoader.getInstance()

	# Exit if they are not found.
	try:
		ch_loader.create(ch_path)
	except GseControllerUndefinedDirectoryException, e:
		alert_dialog(root)
		return

	session_model = SessionModel(ch_loader)
	session_controller = SessionController(opts, root, session_model)
	session_controller.create_session()

	root.mainloop()


def alert_dialog(parent):
	dialog = Pmw.MessageDialog(parent,
	    title = 'Error',
	    buttons=('Exit',),
	    iconpos = 'w',
	    icon_bitmap = 'error',
	    message_text = 'Cannot find channel dictionaries. Specify generated dictionary path with -d.')
	dialog.iconname('Generated Dictionary Error')
	dialog.activate()


if __name__ == '__main__':

	argv = sys.argv[1:]

	program_version_string = "1.0"
	program_longdesc       = ""
	program_license        = ""

	config_manager         = ConfigManager.ConfigManager.getInstance()
	generated_path         = config_manager.get("filepaths", "generated_path")


	parser = OptionParser(version=program_version_string, epilog=program_longdesc, description=program_license)
	parser.add_option("-d", "--dictionary", dest="gen_path", action="store", type="string", \
                          help="Set base path to generated command/telemetry definition files [default: %default]", \
                          default=generated_path)

	(opts, args) = parser.parse_args(argv)

	# Add to path so we can import channels
	sys.path.append(opts.gen_path)

	main(opts)
