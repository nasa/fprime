from models.serialize.DefaultTypeExceptions import *
from models.serialize.type_base import *
from models.serialize.default_enum_type import *
from models.serialize.default_string_type import *
from models.serialize.default_serializable_type import *

def Enum1():
  return DefaultEnumType(
    "Enum1",
    {
    "Item1",
    "Item2",
    "Item3",
    }
  )
