from models.serialize.DefaultTypeExceptions import *
from models.serialize.type_base import *
from models.serialize.default_enum_type import *
from models.serialize.default_string_type import *
from models.serialize.default_serializable_type import *

def PacketRecvStatus():
  return DefaultEnumType(
    "PacketRecvStatus",
    {
    "PACKET_STATE_NO_PACKETS",
    "PACKET_STATE_OK",
    "PACKET_STATE_ERRORS",
    }
  )
