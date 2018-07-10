import binascii

from models.serialize import u32_type
from utils import data_desc_type


# NOTE decoder function to call is called data_callback(data)
# TODO close the socket when you're done
class Distributor(object):
	"""A distributor contains a socket client that connects to a ThreadedTCPServer.
	It then sends and recvs data from a FPrime deployment.
	Decoders can register with a distributor to recv packets of data of a certain description.
	"""

	def __init__(self):
		"""Sets up the dictionary of connected encoders and socket client object. Encoder dictionary is of the form
		{data descriptor name: list of decoder objects registered for that data}
		"""

		self.__decoders = {key.name: [] for key in list(data_desc_type.DataDescType)}

	# NOTE we could use either the type of the object or an enum as the type argument. It should indicate what the decoder decodes.
	# TODO implement as an ENUM name as key
	def register(self, typeof, obj):
		"""Register a decoder with the distributor

		Arguments:
			typeof {string} -- The name of the data descriptor that the decoder will decode
			obj {decoder} -- The decoder object that will process the data
		"""
                #TODO check that typeof is a valid DataDescType Enum value
		self.__decoders[typeof].append(obj)

                # TODO remove
                print("Decoders=%s"%self.__decoders)

	def on_recv(self, data):
		"""Called by the internal socket client when data is recved from the socket client

		Arguments:
			data {binary} -- the data recved from the socket client
		"""


		# NOTE make these selectable with a configuration later

		offset = 0 # Represents our offset into the packet so far

		# 4 byte length header for sanity
		pkt_len = u32_type.U32Type()
		pkt_len.deserialize(data, offset)
		offset += pkt_len.getSize()
		#print("Length of the packet in bytes: %d")%pkt_len.val
		if len(data) - offset != pkt_len.val:
			#TODO the packet length is not right
			#NOTE sometimes we recv two packets at once and we need to find a way to detect and deal with that
			pass

		data_desc = u32_type.U32Type()
		data_desc.deserialize(data, offset)
		offset += data_desc.getSize()
		#print("Packet description: %s")%data_desc_type.DataDescType(data_desc.val).name
		data_pass_thru = data[offset:] # This will get passed up to decoders
		#print("Data to pass through: %s")%binascii.hexlify(data_pass_thru)
		data_desc_key = data_desc_type.DataDescType(data_desc.val).name



		for d in self.__decoders[data_desc_key]:
			d.data_callback(data_pass_thru)


if __name__ == "__main__":
    pass
