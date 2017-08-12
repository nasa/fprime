# From http://python-3-patterns-idioms-test.readthedocs.io/en/latest/Observer.html

# Class support for "observer" pattern.
from synchronization import *

class Observer(object):
    def update(self, observable, arg):
        '''Called when the observed object is
        modified. You call an Observable object's
        notifyObservers method to notify all the
        object's observers of the change.'''
        pass

class Observable(Synchronization):
    def __init__(self):
        super(Observable, self).__init__()

        self.obs = []

    def addObserver(self, observer):
        if observer not in self.obs:
            self.obs.append(observer)

    def deleteObserver(self, observer):
        self.obs.remove(observer)

    def notifyObservers(self, arg = None):
        '''Notify all its observers. Each observer
        has its update() called with two
        arguments: this observable object and the
        generic 'arg'.'''

        self.mutex.acquire()
        try:
            # Make a local copy in case of synchronous
            # additions of observers:
            localArray = self.obs[:]
        finally:
            self.mutex.release()
        # Updating is not required to be synchronized:
        for observer in localArray:
            observer.update(self, arg)

    def deleteObservers(self): self.obs = []
    def countObservers(self): return len(self.obs)

synchronize(Observable,
  "addObserver deleteObserver deleteObservers " +
  "countObservers")
