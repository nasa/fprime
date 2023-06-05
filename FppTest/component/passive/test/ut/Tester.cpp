// ======================================================================
// \title  PassiveTest.hpp
// \author tiffany
// \brief  cpp file for PassiveTest test harness implementation class
// ======================================================================

#include "test/ut/Tester.hpp"
#include "STest/Pick/Pick.hpp"


  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
      PassiveTestGTestBase("Tester", Tester::MAX_HISTORY_SIZE),
      component("PassiveTest"),
      primitiveBuf(primitiveData, sizeof(primitiveData)),
      stringBuf(stringData, sizeof(stringData)),
      enumBuf(enumData, sizeof(enumData)),
      arrayBuf(arrayData, sizeof(arrayData)),
      structBuf(structData, sizeof(structData)),
      serialBuf(serialData, sizeof(serialData))
  {
    this->initComponents();
    this->connectPorts();

    prmValid = static_cast<Fw::ParamValid::T>(
      STest::Pick::lowerUpper(1, Fw::ParamValid::NUM_CONSTANTS - 1)
    );
  }

  Tester ::
    ~Tester()
  {

  }

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        Tester::TEST_INSTANCE_ID
    );
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_arrayArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    this->pushFromPortEntry_arrayArgsOut(a, aRef);
  }

  FormalParamArray Tester ::
    from_arrayReturnOut_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    this->pushFromPortEntry_arrayReturnOut(a, aRef);
    return arrayReturnVal.val;
  }

  void Tester ::
    from_enumArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    this->pushFromPortEntry_enumArgsOut(en, enRef);
  }

  FormalParamEnum Tester ::
    from_enumReturnOut_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    this->pushFromPortEntry_enumReturnOut(en, enRef);
    return enumReturnVal.val;
  }

  void Tester ::
    from_noArgsOut_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->pushFromPortEntry_noArgsOut();
  }

  bool Tester ::
    from_noArgsReturnOut_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->pushFromPortEntry_noArgsReturnOut();
    return noParamReturnVal.val;
  }

  void Tester ::
    from_primitiveArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    this->pushFromPortEntry_primitiveArgsOut(u32, u32Ref, f32, f32Ref, b, bRef);
  }

  U32 Tester ::
    from_primitiveReturnOut_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    this->pushFromPortEntry_primitiveReturnOut(u32, u32Ref, f32, f32Ref, b, bRef);
    return primitiveReturnVal.val;
  }

  void Tester ::
    from_stringArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        const str80String &str80,
        str80RefString &str80Ref,
        const str100String &str100,
        str100RefString &str100Ref
    )
  {
    this->pushFromPortEntry_stringArgsOut(str80, str80Ref, str100, str100Ref);
  }

  void Tester ::
    from_structArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    this->pushFromPortEntry_structArgsOut(s, sRef);
  }

  FormalParamStruct Tester ::
    from_structReturnOut_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    this->pushFromPortEntry_structReturnOut(s, sRef);
    return structReturnVal.val;
  }

  // ----------------------------------------------------------------------
  // Handlers for serial from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_serialOut_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    Fw::SerializeStatus status;

    switch (portNum) {
      case SerialPortIndex::NO_ARGS:
        status = Fw::FW_SERIALIZE_OK;
        break;

      case SerialPortIndex::PRIMITIVE:
        status = Buffer.copyRaw(
          this->primitiveBuf,
          Buffer.getBuffCapacity()
        );
        break;

      case SerialPortIndex::STRING:
        status = Buffer.copyRaw(
          this->stringBuf,
          Buffer.getBuffCapacity()
        );
        break;

      case SerialPortIndex::ENUM:
        status = Buffer.copyRaw(
          this->enumBuf,
          Buffer.getBuffCapacity()
        );
        break;

      case SerialPortIndex::ARRAY:
        status = Buffer.copyRaw(
          this->arrayBuf, 
          Buffer.getBuffCapacity()
        );
        break;

      case SerialPortIndex::STRUCT:
        status = Buffer.copyRaw(
          this->structBuf,
          Buffer.getBuffCapacity()
        );
        break;

      case SerialPortIndex::SERIAL:
        status = Buffer.copyRaw(
          this->serialBuf,
          Buffer.getBuffCapacity()
        );
        break;
    }

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
  }

  // ----------------------------------------------------------------------
  // Handlers for serial from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_cmdRegIn_handler(
        const NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode
    )
  {
    this->pushFromPortEntry_cmdRegIn(opCode);
  }

  void Tester ::
    from_cmdResponseIn_handler(
        const NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        const Fw::CmdResponse &response
    )
  {
    this->cmdResp = response;
    this->pushFromPortEntry_cmdResponseIn(opCode, cmdSeq, response);
  }

  Fw::ParamValid Tester ::
    from_prmGetIn_handler(
        const NATIVE_INT_TYPE portNum,
        FwPrmIdType id,
        Fw::ParamBuffer &val
    )
  {
    val.resetSer();

    Fw::SerializeStatus status;
    U32 id_base = component.getIdBase();

    FW_ASSERT(id >= id_base);

    switch (id - id_base) {
      case PassiveTestComponentBase::PARAMID_PARAMBOOL: 
        status = val.serialize(boolPrm.args.val);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
        break;

      case PassiveTestComponentBase::PARAMID_PARAMU32:
        status = val.serialize(u32Prm.args.val);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
        break;

      case PassiveTestComponentBase::PARAMID_PARAMSTRING:
        status = val.serialize(stringPrm.args.val);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
        break;

      case PassiveTestComponentBase::PARAMID_PARAMENUM:
        status = val.serialize(enumPrm.args.val);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
        break;

      case PassiveTestComponentBase::PARAMID_PARAMARRAY:
        status = val.serialize(arrayPrm.args.val);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
        break;

      case PassiveTestComponentBase::PARAMID_PARAMSTRUCT:
        status = val.serialize(structPrm.args.val);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
        break;
    }

    this->pushFromPortEntry_prmGetIn(id, val);

    return prmValid;
  }

  void Tester ::
    from_prmSetIn_handler(
        const NATIVE_INT_TYPE portNum,
        FwPrmIdType id,
        Fw::ParamBuffer &val
    )
  {
    this->pushFromPortEntry_prmSetIn(id, val);
  }
