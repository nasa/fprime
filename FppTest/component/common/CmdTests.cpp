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

  // Test too many arguments
  buf.serialize(0);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_NO_ARGS,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test success
  buf.resetSer();

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_NO_ARGS,
      1,
      buf
  );

  ASSERT_from_cmdResponseIn_SIZE(2);
  ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);
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

  // Test too few arguments
  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_PRIMITIVE,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test too many arguments
  buf.serialize(data.args.val1);
  buf.serialize(data.args.val2);
  buf.serialize(data.args.val3);
  buf.serialize(data.args.val4);
  buf.serialize(data.args.val5);
  buf.serialize(data.args.val6);
  buf.serialize(data.args.val6);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_PRIMITIVE,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test success
  buf.resetSer();
  buf.serialize(data.args.val1);
  buf.serialize(data.args.val2);
  buf.serialize(data.args.val3);
  buf.serialize(data.args.val4);
  buf.serialize(data.args.val5);
  buf.serialize(data.args.val6);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_PRIMITIVE,
      1,
      buf
  );

  ASSERT_from_cmdResponseIn_SIZE(3);
  ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);
  ASSERT_EQ(component.primitiveCmd.args.val1, data.args.val1);
  ASSERT_EQ(component.primitiveCmd.args.val2, data.args.val2);
  ASSERT_EQ(component.primitiveCmd.args.val3, data.args.val3);
  ASSERT_EQ(component.primitiveCmd.args.val4, data.args.val4);
  ASSERT_EQ(component.primitiveCmd.args.val5, data.args.val5);
  ASSERT_EQ(component.primitiveCmd.args.val6, data.args.val6);
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

  // Test too few arguments
  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRINGS,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test too many arguments
  buf.serialize(data.args.val1);
  buf.serialize(data.args.val2);
  buf.serialize(data.args.val2);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRINGS,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test success
  buf.resetSer();
  buf.serialize(data.args.val1);
  buf.serialize(data.args.val2);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRINGS,
      1,
      buf
  );

  ASSERT_from_cmdResponseIn_SIZE(3);
  ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);
  ASSERT_EQ(component.stringCmd.args.val1, data.args.val1);
  ASSERT_EQ(component.stringCmd.args.val2, data.args.val2);
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

  // Test too few arguments
  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_ENUM,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test too many arguments
  buf.serialize(data.args.val);
  buf.serialize(data.args.val);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_ENUM,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test success
  buf.resetSer();
  buf.serialize(data.args.val);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_ENUM,
      1,
      buf
  );

  ASSERT_from_cmdResponseIn_SIZE(3);
  ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);
  ASSERT_EQ(component.enumCmd.args.val, data.args.val);
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

  // Test too few arguments
  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_ARRAY,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test too many arguments
  buf.serialize(data.args.val);
  buf.serialize(data.args.val);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_ARRAY,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test success
  buf.resetSer();
  buf.serialize(data.args.val);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_ARRAY,
      1,
      buf
  );

  ASSERT_from_cmdResponseIn_SIZE(3);
  ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);
  ASSERT_EQ(component.arrayCmd.args.val, data.args.val);
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

  // Test too few arguments
  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRUCT,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test too many arguments
  buf.serialize(data.args.val);
  buf.serialize(data.args.val);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRUCT,
      1,
      buf
  );

  ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);

  // Test success
  buf.resetSer();
  buf.serialize(data.args.val);

  this->invoke_to_cmdOut(
      portNum,
      component.OPCODE_CMD_STRUCT,
      1,
      buf
  );

  ASSERT_from_cmdResponseIn_SIZE(3);
  ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);
  ASSERT_EQ(component.structCmd.args.val, data.args.val);
}
