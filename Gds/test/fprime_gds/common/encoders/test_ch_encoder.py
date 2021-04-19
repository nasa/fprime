"""
Tests the channel encoder

Created on Jul 10, 2020
@author: Joseph Paetz, hpaulson
"""


from fprime_gds.common.encoders.ch_encoder import ChEncoder
from fprime_gds.common.templates.ch_template import ChTemplate
from fprime_gds.common.utils.config_manager import ConfigManager
from fprime.common.models.serialize.time_type import TimeType
from fprime.common.models.serialize.numerical_types import U16Type, U32Type
from fprime_gds.common.data_types.ch_data import ChData


def test_ch_encoder():
    """
    Tests the encoding of the channel encoder
    """
    config = ConfigManager()
    config.set("types", "msg_len", "U16")

    enc = ChEncoder()
    enc_config = ChEncoder(config)

    temp = ChTemplate(101, "test_ch", "test_comp", U32Type())

    time_obj = TimeType(2, 0, 1533758629, 123456)

    ch_obj = ChData(U32Type(42), time_obj, temp)

    desc_bin = b"\x00\x00\x00\x01"
    id_bin = b"\x00\x00\x00\x65"
    time_bin = b"\x00\x02\x00\x5b\x6b\x4c\xa5\x00\x01\xe2\x40"
    val_bin = b"\x00\x00\x00\x2a"
    long_len_bin = b"\x00\x00\x00\x17"
    short_len_bin = b"\x00\x17"

    reg_expected = long_len_bin + desc_bin + id_bin + time_bin + val_bin
    config_expected = short_len_bin + desc_bin + id_bin + time_bin + val_bin

    reg_output = enc.encode_api(ch_obj)

    assert (
        reg_output == reg_expected
    ), "FAIL: expected regular output to be %s, but found %s" % (
        list(reg_expected),
        list(reg_output),
    )

    config_output = enc_config.encode_api(ch_obj)

    assert (
        config_output == config_expected
    ), "FAIL: expected configured output to be %s, but found %s" % (
        list(config_expected),
        list(config_output),
    )

    temp = ChTemplate(102, "test_ch2", "test_comp2", U16Type())

    time_obj = TimeType(2, 0, 1533758628, 123457)

    ch_obj = ChData(U16Type(40), time_obj, temp)

    desc_bin = b"\x00\x00\x00\x01"
    id_bin = b"\x00\x00\x00\x66"
    time_bin = b"\x00\x02\x00\x5b\x6b\x4c\xa4\x00\x01\xe2\x41"
    val_bin = b"\x00\x28"
    long_len_bin = b"\x00\x00\x00\x15"
    short_len_bin = b"\x00\x15"

    reg_expected = long_len_bin + desc_bin + id_bin + time_bin + val_bin
    config_expected = short_len_bin + desc_bin + id_bin + time_bin + val_bin

    reg_output = enc.encode_api(ch_obj)

    assert (
        reg_output == reg_expected
    ), "FAIL: expected regular output to be %s, but found %s" % (
        list(reg_expected),
        list(reg_output),
    )

    config_output = enc_config.encode_api(ch_obj)

    assert (
        config_output == config_expected
    ), "FAIL: expected configured output to be %s, but found %s" % (
        list(config_expected),
        list(config_output),
    )
