"""
Tests the event encoder

Created on Jul 10, 2020
@author: Joseph Paetz, hpaulson
"""


from fprime_gds.common.encoders.event_encoder import EventEncoder
from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.utils.config_manager import ConfigManager
from fprime_gds.common.templates.event_template import EventTemplate
from fprime.common.models.serialize.time_type import TimeType
from fprime.common.models.serialize.numerical_types import U8Type, U16Type, U32Type
from fprime_gds.common.utils.event_severity import EventSeverity


def test_event_encoder():
    """
    Tests the encoding of the event encoder
    """
    config = ConfigManager()
    config.set("types", "msg_len", "U16")

    enc = EventEncoder()
    enc_config = EventEncoder(config)

    temp = EventTemplate(
        101,
        "test_ch",
        "test_comp",
        [("a1", "a1", U32Type()), ("a2", "a2", U32Type())],
        EventSeverity["DIAGNOSTIC"],
        "%d %d",
    )

    time_obj = TimeType(2, 0, 1533758629, 123456)

    event_obj = EventData((U32Type(42), U32Type(10)), time_obj, temp)

    desc_bin = b"\x00\x00\x00\x02"
    id_bin = b"\x00\x00\x00\x65"
    time_bin = b"\x00\x02\x00\x5b\x6b\x4c\xa5\x00\x01\xe2\x40"
    arg_bin = b"\x00\x00\x00\x2a\x00\x00\x00\x0a"
    long_len_bin = b"\x00\x00\x00\x1b"
    short_len_bin = b"\x00\x1b"

    reg_expected = long_len_bin + desc_bin + id_bin + time_bin + arg_bin
    config_expected = short_len_bin + desc_bin + id_bin + time_bin + arg_bin

    reg_output = enc.encode_api(event_obj)

    assert (
        reg_output == reg_expected
    ), "FAIL: expected regular output to be %s, but found %s" % (
        list(reg_expected),
        list(reg_output),
    )

    config_output = enc_config.encode_api(event_obj)

    assert (
        config_output == config_expected
    ), "FAIL: expected configured output to be %s, but found %s" % (
        list(config_expected),
        list(config_output),
    )

    temp = EventTemplate(
        102,
        "test_ch2",
        "test_comp2",
        [("a1", "a1", U8Type()), ("a2", "a2", U16Type())],
        EventSeverity["DIAGNOSTIC"],
        "%d %d",
    )

    time_obj = TimeType(2, 0, 1533758628, 123457)

    event_obj = EventData((U8Type(128), U16Type(40)), time_obj, temp)

    desc_bin = b"\x00\x00\x00\x02"
    id_bin = b"\x00\x00\x00\x66"
    time_bin = b"\x00\x02\x00\x5b\x6b\x4c\xa4\x00\x01\xe2\x41"
    arg_bin = b"\x80\x00\x28"
    long_len_bin = b"\x00\x00\x00\x16"
    short_len_bin = b"\x00\x16"

    reg_expected = long_len_bin + desc_bin + id_bin + time_bin + arg_bin
    config_expected = short_len_bin + desc_bin + id_bin + time_bin + arg_bin

    reg_output = enc.encode_api(event_obj)

    assert (
        reg_output == reg_expected
    ), "FAIL: expected regular output to be %s, but found %s" % (
        list(reg_expected),
        list(reg_output),
    )

    config_output = enc_config.encode_api(event_obj)

    assert (
        config_output == config_expected
    ), "FAIL: expected configured output to be %s, but found %s" % (
        list(config_expected),
        list(config_output),
    )
