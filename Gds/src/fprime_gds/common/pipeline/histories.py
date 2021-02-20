"""
histories.py:

Module used to handle the wrangling of histories for the standard pipeline. This allows the standard pipeline, and other
to compose in this code.

@author mstarch
"""
import fprime_gds.common.history.ram


class Histories:
    """
    Class to handle the individual histories. This handles the following histories:

    1. Channel history
    2. Event history
    3. Command history (short-circuited feedback from encoder)
    """

    def __init__(self):
        """ Constructor of histories composer """
        self._command_hist = None
        self._event_hist = None
        self._channel_hist = None

    def setup_histories(self, coders):
        """
        Setup a set of history objects in order to store the events of the decoders. This registers itself with the
        supplied coders object.

        :param coders: coders object to register histories with
        """
        # Create histories, RAM histories for now
        self._command_hist = fprime_gds.common.history.ram.RamHistory()
        self._event_hist = fprime_gds.common.history.ram.RamHistory()
        self._channel_hist = fprime_gds.common.history.ram.RamHistory()
        # Register histories where channels and packets are routed together
        coders.register_event_consumer(self._event_hist)
        coders.register_channel_consumer(self._channel_hist)
        coders.register_packet_consumer(self._channel_hist)
        coders.register_command_consumer(self._command_hist)

    @property
    def events(self):
        """
        Events history property
        """
        return self._event_hist

    @property
    def channels(self):
        """
        Channels history property
        """
        return self._channel_hist

    @property
    def commands(self):
        """
        Commands history property
        """
        return self._command_hist
