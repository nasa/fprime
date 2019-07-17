# Import the types this way so they do not need prefixing for execution.
from fprime.common.models.serialize.type_exceptions import *
from fprime.common.models.serialize.type_base import *

from fprime.common.models.serialize.bool_type import *
from fprime.common.models.serialize.enum_type import *
from fprime.common.models.serialize.f32_type import *
from fprime.common.models.serialize.f64_type import *

from fprime.common.models.serialize.u8_type import *
from fprime.common.models.serialize.u16_type import *
from fprime.common.models.serialize.u32_type import *
from fprime.common.models.serialize.u64_type import *

from fprime.common.models.serialize.i8_type import *
from fprime.common.models.serialize.i16_type import *
from fprime.common.models.serialize.i32_type import *
from fprime.common.models.serialize.i64_type import *

from fprime.common.models.serialize.string_type import *
from fprime.common.models.serialize.serializable_type import *

from fprime_gds.common.models.common import channel_telemetry
# Each file represents the information for a single event
# These module variables are used to instance the channel object within the Gse


COMPONENT = "DictGen::TestComponent"

NAME = "Inst1_Test_Tlm0"
ID  = 0x1
CHANNEL_DESCRIPTION = "Test Tlm"
TYPE = U32Type()
FORMAT_STRING = None

LOW_RED = None
LOW_ORANGE = None
LOW_YELLOW = None
HIGH_YELLOW = None
HIGH_ORANGE = None
HIGH_RED = None

