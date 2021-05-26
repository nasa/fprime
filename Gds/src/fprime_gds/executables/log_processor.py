""" Basic processor for binary log files produced within fprime"""

from fprime_gds.common.utils.config_manager import ConfigManager
from fprime_gds.common.pipeline.standard import StandardPipeline


class ChannelConsumer(object):
    """A duck-typed receiver of data_callback calls"""

    def data_callback(self, item):
        """ Receive a manufactured item, send to data """
        # TODO: code to handle individual channel items here
        # csvfile.write(item)


class EventConsumer(object):
    """A duck-typed receiver of data_callback calls"""

    def data_callback(self, item):
        """ Receive a manufactured item, send to data """
        # TODO: code to handle individual events here
        pass



def main():
    """ Main function """
    #TODO: add argument processing
    #TODO: arguments.dictionary: file system path to dictionary
    #TODO: arguments.file: path to com logger file


    standard = StandardPipeline()
    config = fprime.gds.utils.config_manager.ConfigManager()
    dictionary = arguments.dictionary

    config.set("framing", "use_key", "True")
    config.set('types', 'msg_len', 'U16')

    standard.setup(config, dictionary, None)

    standard.coders.register_event_consumer(EventConsumer())
    standard.coders.register_channel_consumer(ChannelConsumer())

    with open(arguments.file, "rb") as file_handle:
        standard.distributor.on_recv(file_handle.read())