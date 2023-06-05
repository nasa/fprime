#include "test/ut/Tester.hpp"
#include "CmdTests.hpp"

#include "Fw/Cmd/CmdArgBuffer.hpp"

// ----------------------------------------------------------------------
// Command tests
// ----------------------------------------------------------------------

void Tester ::
  testCommand(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::NoParams& data
)
{
  ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));
  ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));

  component.regCommands();

  Fw::CmdArgBuffer buf;

  // Test success
  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_NO_ARGS,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);

  // Test too many arguments
  buf.serialize(0);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_NO_ARGS,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);
}

void Tester ::
  testCommand(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::PrimitiveParams& data
)
{
  ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));
  ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));

  component.regCommands();

  Fw::CmdArgBuffer buf;

  // Test incorrect deserialization of first argument
  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_PRIMITIVE,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test incorrect deserialization of second argument
  buf.serialize(data.args.val1);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_PRIMITIVE,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test incorrect deserialization of third argument
  buf.serialize(data.args.val2);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_PRIMITIVE,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test incorrect deserialization of fourth argument
  buf.serialize(data.args.val3);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_PRIMITIVE,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test incorrect deserialization of fifth argument
  buf.serialize(data.args.val4);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_PRIMITIVE,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test incorrect deserialization of sixth argument
  buf.serialize(data.args.val5);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_PRIMITIVE,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test success
  buf.serialize(data.args.val6);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_PRIMITIVE,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);
  ASSERT_EQ(component.primitiveCmd.args.val1, data.args.val1);
  ASSERT_EQ(component.primitiveCmd.args.val2, data.args.val2);
  ASSERT_EQ(component.primitiveCmd.args.val3, data.args.val3);
  ASSERT_EQ(component.primitiveCmd.args.val4, data.args.val4);
  ASSERT_EQ(component.primitiveCmd.args.val5, data.args.val5);
  ASSERT_EQ(component.primitiveCmd.args.val6, data.args.val6);

  // Test too many arguments
  buf.serialize(data.args.val5);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_PRIMITIVE,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);
}

void Tester ::
  testCommand(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::CmdStringParams& data
)
{
  ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));
  ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));

  component.regCommands();

  Fw::CmdArgBuffer buf;

  // Test incorrect serialization of first argument
  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRINGS,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test incorrect serialization of second argument
  buf.serialize(data.args.val1);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRINGS,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test success
  buf.serialize(data.args.val2);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRINGS,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);
  ASSERT_EQ(component.stringCmd.args.val1, data.args.val1);
  ASSERT_EQ(component.stringCmd.args.val2, data.args.val2);

  // Test too many arguments
  buf.serialize(data.args.val1);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRINGS,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);
}

void Tester ::
  testCommand(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::EnumParam& data
)
{
  ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));
  ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));

  component.regCommands();

  Fw::CmdArgBuffer buf;

  // Test incorrect serialization of first argument
  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_ENUM,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test success
  buf.serialize(data.args.val);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_ENUM,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);
  ASSERT_EQ(component.enumCmd.args.val, data.args.val);

  // Test too many arguments
  buf.serialize(data.args.val);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_ENUM,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);
}

void Tester ::
  testCommand(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::ArrayParam& data
)
{
  ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));
  ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));

  component.regCommands();

  Fw::CmdArgBuffer buf;

  // Test incorrect serialization of first argument
  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_ARRAY,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test success
  buf.serialize(data.args.val);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_ARRAY,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);
  ASSERT_EQ(component.arrayCmd.args.val, data.args.val);

  // Test too many arguments
  buf.serialize(data.args.val);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_ARRAY,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);
}

void Tester ::
  testCommand(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::StructParam& data
)
{
  ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));
  ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));

  component.regCommands();

  Fw::CmdArgBuffer buf;

  // Test incorrect serialization of first argument
  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRUCT,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test success
  buf.serialize(data.args.val);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRUCT,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);
  ASSERT_EQ(component.structCmd.args.val, data.args.val);

  // Test too many arguments
  buf.serialize(data.args.val);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRUCT,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);
}
