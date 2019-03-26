# Window Persistence Doc

Window and panel persistence is achieved through the memento design pattern: Each window state is saved as a 'memento'. An object tree of mementos is created and saved to a pickled file.

During restore the tree is traversed and window configurations applied.

Memento creation and setting ability is added to windows via a decorator. Windows are treated uniformly thus needing a decorator only on main_panel.

Panels need more specific memento creation. This can be implemented at the panel level. 

Panel persistence is currently commented out.