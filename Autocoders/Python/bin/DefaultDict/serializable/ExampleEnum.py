from models.serialize.DefaultTypeExceptions import *
from models.serialize.type_base import *
from models.serialize.default_enum_type import *
from models.serialize.default_string_type import *
from models.serialize.default_serializable_type import *

def ExampleEnum():
  return DefaultEnumType(
    "ExampleEnum",
    {
      "HSM_NOT_USED":0,
      "HSM_NOMINAL":1,
      "HSM_IDLE":2,
      "HSM_SOFT_SAFE":3,
      "HSM_OFF":4,
      "HSM_MAX":5,
    }
  )
