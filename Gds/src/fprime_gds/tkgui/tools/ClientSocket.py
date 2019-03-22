#!/usr/bin/env python
##########################################
#
# Quick client sockets example to emulate
# what FSW target will expect.
#
# reder@jpl.nasa.gov
#
##########################################
#
from Tkinter import *

import Tkinter
import socket
import struct
import time

from fprime_gds.gse.controllers import client_sock
# Import the types this way so they do not need prefixing for execution.
from fprime_gds.gse.models.serialize.type_exceptions import *
from fprime_gds.gse.models.serialize.type_base import *

from fprime_gds.gse.models.serialize.bool_type import *
from fprime_gds.gse.models.serialize.enum_type import *
from fprime_gds.gse.models.serialize.f32_type import *
from fprime_gds.gse.models.serialize.f64_type import *

from fprime_gds.gse.models.serialize.u8_type import *
from fprime_gds.gse.models.serialize.u16_type import *
from fprime_gds.gse.models.serialize.u32_type import *
from fprime_gds.gse.models.serialize.u64_type import *

from fprime_gds.gse.models.serialize.i8_type import *
from fprime_gds.gse.models.serialize.i16_type import *
from fprime_gds.gse.models.serialize.i32_type import *
from fprime_gds.gse.models.serialize.i64_type import *

from fprime_gds.gse.models.serialize.string_type import *
from fprime_gds.gse.models.serialize.serializable_type import *



#
# Main loop
#
def main():
    port = 50007

    s = client_sock.ClientSocket("127.0.0.1", port)
    print "Test socket client\n"
    while 1:
        print "Enter command: \"Register\",\"name of registered client\",\"Listen\",\"List\""
        print "Enter command: \"FSW\" for testing"
        c = sys.stdin.readline()
        c = c.strip('\n')
        if c.title() == 'Register':
            print "Enter client name: "
            n=sys.stdin.readline()
            n = n.strip('\n')
            cmd = c.title() + " " + n + "\n"
        elif c.title() == "List":
            cmd = c.title() + "\n"
        elif c.upper() == 'FSW':
            desc = u32_type.U32Type( 0x5B5B5B5B )
            desc_len = desc.getSize()
            server_desc = 'A5A5 GUI '
            cmd = ''
            print "Enter event choice: 0-Event1, 1-Event2, 2-StringEvent, 3-EnumEvent, 4-BoolEvent:"
            n = sys.stdin.readline()
            n = int(n.strip('\n'))
            if n == 0:
                # Event 1 Generation
                i  = U32Type(100)
                cmd += i.serialize()
                #
                i32 = int(raw_input("Enter I32 arg: "))
                i32 = I32Type(i32)
                cmd += i32.serialize()
                #
                f32 = float(raw_input("Enter F32 arg: "))
                f32 = F32Type(f32)
                cmd += f32.serialize()
                #
                q   = raw_input("Enter Quaternion F32 (Q1, Q2, Q3, Q4): ")
                q   = map(lambda y: float(y), q.split())
                q1  = F32Type(q[0])
                cmd += q1.serialize()
                q2  = F32Type(q[1])
                cmd += q2.serialize()
                q3  = F32Type(q[2])
                cmd += q3.serialize()
                q4  = F32Type(q[3])
                cmd += q4.serialize()
                # Make event packet
                data_len = u32_type.U32Type( desc_len + len(cmd) )
                cmd = server_desc + data_len.serialize() + desc.serialize() + cmd

            elif n == 1:
                # Event 2 Generation
                i  = U32Type(101)
                cmd += i.serialize()
                #
                i32 = int(raw_input("Enter I32 arg: "))
                i32 = I32Type(i32)
                cmd += i32.serialize()
                #
                u32 = abs(int(raw_input("Enter U32 arg: ")))
                u32 = U32Type(u32)
                cmd += u32.serialize()
                #
                u8 = abs(int(raw_input("Enter u8 arg: ")))
                u8 = U8Type(u8)
                cmd += u8.serialize()
                # Make event packet
                data_len = u32_type.U32Type( desc_len + len(cmd) )
                cmd = server_desc + data_len.serialize() + desc.serialize() + cmd

            elif n == 2:
                # Event String Generation
                i = U32Type(102)
                cmd += i.serialize()
                #
                i32 = int(raw_input("Enter I32 arg: "))
                i32 = I32Type(i32)
                cmd += i32.serialize()
                #
                string = raw_input("Enter String arg: ")
                string = StringType(string)
                cmd += string.serialize()
                #
                u8 = int(raw_input("Enter U8 arg: "))
                u8 = U8Type(u8)
                cmd += u8.serialize()
                # Make event packet
                data_len = u32_type.U32Type( desc_len + len(cmd) )
                cmd = server_desc + data_len.serialize() + desc.serialize() + cmd

            elif n == 3:
                # Event Enum Generation
                i = U32Type(103)
                cmd += i.serialize()
                #
                i32 = int(raw_input("Enter I32 arg: "))
                i32 = I32Type(i32)
                cmd += i32.serialize()
                #
                en = raw_input("Enter Enum String Value (\"MEMB1\", \"MEMB2\", \"MEMB3\"): ")
                en = EnumType("SomeTypeEnum",{"MEMB1":0 , "MEMB2":7, "MEMB3":15}, en)
                cmd += en.serialize()
                #
                u8 = int(raw_input("Enter U8 arg: "))
                u8 = U8Type(u8)
                cmd += u8.serialize()
                # Make event packet
                data_len = u32_type.U32Type( desc_len + len(cmd) )
                cmd = server_desc + data_len.serialize() + desc.serialize() + cmd

            elif n == 4:
                # Event Enum Generation
                i = U32Type(104)
                cmd += i.serialize()
                #
                i32 = int(raw_input("Enter I32 arg: "))
                i32 = I32Type(i32)
                cmd += i32.serialize()
                #
                b = raw_input("Enter Bool String Value (\"True\", \"False\"): ")
                if b == "True":
                    b = BoolType(True)
                else:
                    b = BoolType(False)
                cmd += b.serialize()
                #
                u8 = int(raw_input("Enter U8 arg: "))
                u8 = U8Type(u8)
                cmd += u8.serialize()
                # Make event packet
                data_len = u32_type.U32Type( desc_len + len(cmd) )
                cmd = server_desc + data_len.serialize() + desc.serialize() + cmd

            else:
                print "Unrecognized event code!"

        elif c.title() == 'Listen':
            while 1:
                print "Listening:"
                buff = s.recv(4)
                desc = U32Type()
                desc.deserialize(buff, len(buff))
                buff = s.recv(4)
                size = U32Type()
                size.deserialize(buff, len(buff))
                print "Descripter: 0x%x, Size: %d" % (desc.val, size.val)
                buff = s.recv(size.val)
                opcode = U32Type()
                opcode.deserialize(buff, 4)
                print "0x%x" % opcode.val
                if opcode.val == 0x600:
                    print "START_PKTS"
                elif opcode.val == 0x10:
                    print "NO_OP"
                elif opcode.val == 0x102:
                    print "TEST_CMD_3"
                    # arg1
                    u32 = I32Type()
                    ptr = 4
                    u32.deserialize(buff[ptr:], u32.getSize())
                    ptr += u32.getSize()
                    # arg2
                    str1 = StringType()
                    # Note must know size apriori...
                    str1.deserialize(buff[ptr:], 22)
                    ptr += str1.getSize()
                    # arg3
                    u8  = U8Type()
                    u8.deserialize(buff[ptr:], u8.getSize())
                    print "Args: U32: %d, String: %s, U8: %d" % (u32.val, str1.val, u8.val)
                elif opcode.val == 0x100:
                    print "TEST_CMD_1"
                    # arg1
                    i32 = I32Type()
                    ptr = 4
                    i32.deserialize(buff[ptr:], i32.getSize())
                    ptr += i32.getSize()
                    print i32.val
                    # arg2
                    f32 = F32Type()
                    f32.deserialize(buff[ptr:], f32.getSize())
                    ptr += f32.getSize()
                    print f32.val
                    # arg3
                    u8    = U8Type()
                    u8.deserialize(buff[ptr:], u8.getSize())
                    print "Args: I32: %d, F32: %f, U8: %d" % (i32.val, f32.val, u8.val)
                elif opcode.val == 0x101:
                    print "TEST_CMD_2"
                    # arg1
                    i32 = I32Type()
                    ptr = 4
                    i32.deserialize(buff[ptr:], i32.getSize())
                    ptr += i32.getSize()
                    # arg2
                    b   = BoolType()
                    b.deserialize(buff[ptr:], b.getSize())
                    ptr += b.getSize()
                    # arg3
                    en = EnumType("SomeTypeEnum",{"MEMB1":0 , "MEMB2":6, "MEMB3":9})
                    en.deserialize(buff[ptr:], en.getSize())
                    print "Args: I32: %d, BOOL: %s, ENUM: %s" % (i32.val, b.val, en.val)
                elif opcode.val == 0x11:
                    print "NO_OP_STRING"
                    # arg1
                    u32 = U32Type()
                    ptr = 4
                    u32.deserialize(buff[ptr:], u32.getSize())
                    ptr += u32.getSize()
                    # arg2
                    str2 = StringType()
                    str2.deserialize(buff[ptr:], u32.val+4)
                    print "Args: U32: %d, STRING: %s" % (u32.val, str2.val)
                elif opcode.val == 0x601:
                    print "INJECT_PKT_ERROR"
                else:
                    print "Unrecognized op code!"
        else:
            print "Unreognized command!"
            continue
        print "Command is: " + cmd
        type_base.showBytes(cmd)
        s.send(cmd)


if __name__ == "__main__":
    main()
