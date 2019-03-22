import Tkinter
import pickle
import collections
from fprime_gds.tkgui.utils import ConfigManager
from fprime_gds.tkgui.controllers import status_updater

class Memento(object):

	def __init__(self, state):
		self.__state = state

	def getState(self):
		return self.__state

	def setState(self, state):
		self.__state = state


def Tree():
	"""
	Simple tree implementation
	"""
	return collections.defaultdict(Tree)

class MementoCaretaker(object):
	"""
	MementoCaretaker is a singleton that maintains a model of the application structure. 

	Saving
	''''''
	MementoOriginators carry an instance of the MementoCaretaker. This allows Originators to 
	add their mementos to the Caretaker app_tree. 

	The Caretaker app_tree is pickled and saved to file.
	
	Loading
	''''''' 
	Caretaker loads an app_tree from a pickled file. 

	MementoCaretaker is given access to the MainPanelFactory when loading a configuration. This
	allows the caretaker to iterate through all window instances and apply window and panel mementos. 
	
	Control
	'''''''
	main_panel controls the MementoCaretaker through it's File menu
	
	"""

	__instance = None

	def __init__(self):
		self.__root           = None
		self.__top			  = None
		self.__app_tree       = Tree()
		self.__config         = ConfigManager.ConfigManager.getInstance()
		self.__status_updater = status_updater.StatusUpdater.getInstance()
		self.__save_file      = self.__config.get("persist", "save_file")

	def addWindowMemento(self, m, wid):
		"""
		Save window memento to app_tree.

		@param m: Memento object
		@param wid: Window id
		"""
		self.__app_tree[wid]['memento'] = m 

	def addPanelMemento(self, m, wid, name):
		"""
		Save panel memento to app_tree.

		@param m: Memento object
		@param wid: Window id
		@param name: Panel name
		"""
		self.__app_tree[wid]['panels'][name]['memento'] = m

	def restoreWindows(self, mpf):
		"""
		Walk through app_tree and set mementos for
		all windows and panels. 

		@param mpf : main panel factory
		"""

		# Set window mementos
		for wid in self.__app_tree.keys():

			m = self.__app_tree[wid]['memento']

			# Try to get main panel. If key does not exist
			# we need to create a new window.
			try:
				main_panel = mpf.get_panel_dict()[wid]
			except KeyError, e:
				top = Tkinter.Toplevel()
				mpf.create(top, False)
				main_panel = mpf.get_panel_dict()[wid]

			main_panel.set_memento(m)

			# Set panel mementos
			#for panel_name in self.__app_tree[wid]['panels'].keys():

				# Search for matching panel
				# Skip panels that have not been decorated.
			#	for panel in main_panel.get_panels():
			#		try:
			#			if panel_name == panel.get_name():
			#				m = self.__app_tree[wid]['panels'][panel_name]['memento']
			#				panel.set_memento(m)
			#		except AttributeError, e:
			#			pass

		self.__root.update()

	def set_root(self, root):
		self.__root = root

	def set_top(self, top):
		self.__top = top

	def save_to_file(self):
		"""
		Save Memento object structure to file.
		"""
		try: 
			with open(self.__save_file, 'wb') as file:
				pickle.dump(self.__app_tree, file, -1)
		except IOError, e:
			raise e

		self.__status_updater.update("Saved window configuration.")

	def load_from_file(self):
		""" 
		Load Memento object structure from file 
		"""
		try:
			with open(self.__save_file, 'rb') as file:
				self.__app_tree = pickle.load(file)
		except IOError, e:
			msg = "Unable to find saved state."
			print msg
			self.__status_updater.update(msg)
			
	def getInstance():
	    """
	    Return instance of singleton.
	    """
	    if(MementoCaretaker.__instance is None):
	        MementoCaretaker.__instance = MementoCaretaker()
	    return MementoCaretaker.__instance

	#define static method
	getInstance = staticmethod(getInstance)

	
def WindowMementoOriginator(OrigClass):

	class WindowMementoOriginator(OrigClass):
		"""
		Decorator class for windows. Windows are represented by their main_panel.
		"""
	
		def __init__(self, *args, **kwargs):
			super(WindowMementoOriginator, self).__init__(*args, **kwargs)
			self.__memento_caretaker = MementoCaretaker.getInstance()

		def create_memento(self):
			state             = dict()
			state['w_width']  = self._parent.winfo_width()
			state['w_height'] = self._parent.winfo_height()
			state['root_x']   = self._parent.winfo_rootx()
			state['root_y']   = self._parent.winfo_rooty()
			state['panel']    = self.notebook().getcurselection()
			wid               = self.id()

			m = Memento(state)
			self.__memento_caretaker.addWindowMemento(m, wid)

		def set_memento(self, m):
			state = m.getState()

			self._parent.geometry('{w_width}x{w_height}+{root_x}+{root_y}'.format(**state))
			self.notebook().selectpage(state['panel'])
			self._parent.update()


	return WindowMementoOriginator

def PanelMementoOriginator(OrigClass):
	
	class PanelMementoOriginator(OrigClass):
		"""
		Decorator class for panels. Panels create their own
		implementation of create and set. 

		Class implemenations must return (memento, wid, panel_name)
		so memento can be added to caretaker. 
		"""
		def __init__(self, *args, **kwargs):
			super(PanelMementoOriginator, self).__init__(*args, **kwargs)
			self.__memento_caretaker = MementoCaretaker.getInstance()
			
			self.__widget_tree = Tree()
			self.__node_idx    = 0

		def create_memento(self):
			m, wid, panel_name = super(PanelMementoOriginator, self).create_memento()
			self.__memento_caretaker.addPanelMemento(m, wid, panel_name)

		def set_memento(self, m):
			super(PanelMementoOriginator, self).set_memento(m)
			
	return PanelMementoOriginator

