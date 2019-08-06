from models.serialize.DefaultTypeExceptions import *
from models.serialize.type_base import *
from models.serialize.default_enum_type import *
from models.serialize.default_string_type import *
from models.serialize.default_serializable_type import *

def Enum1():
  return DefaultEnumType(
    "Enum1",
    {
      "Item1":-1952875139,
      "Item2":2,
      "Item3":2000999333,
      "Item4":21,
      "Item5":-8324876,
    }
  )
