'''
Module used to wrap various versions of the GDS API (previously GSE API).
These wrappers provide a common interface to the GDS API, buffering changes
and preventing upstream failures.

Created on Aug 28, 2018

@author: mstarch
'''
import copy
import fprime.gse.utils.gse_api

class GdsApiWrapper(object):
    '''
    Base class for the API wrappers. Used to implement standard functions,
    as part of the wrapper API. Also provides searching algorithms for
    the API wrapper.
    '''
    @classmethod
    def get_configured_wrapper(cls, config):
        '''
        Finds a wrapper based on configuration
        @param cls: class variable for this method
        @param config: config to forward to instantiation
        '''
        config = copy.copy(config)
        name = config["wrapper_class"]
        del config["wrapper_class"] #Remove "wrapper_class" config, forward everything else
        return cls.get_wrapper(name, **config)

    @classmethod
    def get_wrapper(cls, wrapper_class, *args, **kwargs):
        '''
        Searches for an appropriate API wrapper, and instantiates it. It then passes
        back the wrapper class for use in above TestCases.
        @param cls: class object
        @param wrapper_class: wrapper_class object to instantiate
        @param kwargs: key-word arguments to pass for API instantiation
        '''
        #Instantiate class and check it for correctness
        try:
            missing = []
            wrapper = wrapper_class(*args, **kwargs)
            for required in ["wait_evr", "wait_tlm", "get_evr_id", "get_tlm_id", "send"]:
                if not hasattr(wrapper, required) and callable(getattr(wrapper, required)):
                    missing.append(required)
            #With no missing items, use this wrapper
            if not missing:
                return wrapper
            raise Exception("Required '{1}' not defined in '{0}'".format(wrapper_class.__name__,
                                                                         missing))
        except Exception as ex:
            raise GdsApiCreationError(wrapper_class.__name__, ex)

class GseWrapper(GdsApiWrapper):
    '''
    Wraps the old style GseApi.
    Note: this is basically a call forwarder for now.
    '''
    def __init__(self, *args, **kwargs):
        '''
        Constructor used to build the GseApi
        '''
        self.api = fprime.gse.utils.gse_api.GseApi(*args, **kwargs)
        #self.api = fprime.gse.utils.test_api.TestApi(*args, **kwargs)

    def wait_evr(self, *args, **kwargs):
        '''
        Call API's command to wait for EVR.
        '''
        return self.api.wait_evr(*args, **kwargs)

    def wait_tlm(self, *args, **kwargs):
        '''
        Call API's command to wait for TLM.
        '''
        return self.api.wait_tlm(*args, **kwargs)

    def get_evr_id(self, *args, **kwargs):
        '''
        Call API's command to get EVR ID.
        '''
        return self.api.get_evr_id(*args, **kwargs)

    def get_tlm_id(self, *args, **kwargs):
        '''
        Call API's command to get TLM ID.
        '''
        return self.api.get_tlm_id(*args, **kwargs)

    def send(self, *args, **kwargs):
        '''
        Call to send a command
        '''
        return self.api.send(*args, **kwargs)
    def flush(self, *args, **kwargs):
        '''
        Call to send a command
        '''
        return self.api.flush(*args, **kwargs)
    def receive(self, *args, **kwargs):
        '''
        Call to send a command
        '''
        return self.api.receive(*args, **kwargs)
    def _pop_queue(self, *args, **kwargs):
        '''
        Call to send a command
        '''
        return self.api._pop_queue(*args, **kwargs)
    def get_events(self):
        '''
        Get api events wrapper
        '''
        return self.api._events

    def get_channels(self):
        '''
        Get channels from api
        '''
        return self.api._channels
class GdsWrapper(GdsApiWrapper):
    '''
    Placeholder for fututre GdsWrapper
    '''
    pass

class CosmosWrapper(GdsApiWrapper):
    '''
    Placeholder for fututre Cosmos Wrapper
    '''
    pass

class GdsApiNonExistent(Exception):
    '''
    Exception for non-existent API
    '''
    def __init__(self, name):
        message = "Wrapper '{0}' does not exist".format(name)
        super(GdsApiNonExistent, self).__init__(message)

class GdsApiCreationError(Exception):
    '''
    Exception during creation
    '''
    def __init__(self, name, cause):
        message = "Wrapper '{0}' experienced \"{1}\" during creation".format(name, cause)
        super(GdsApiCreationError, self).__init__(message)
