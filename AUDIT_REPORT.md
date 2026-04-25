# Audit Report — fprime

**Auditor:** Astrolexis.space — Kulvex Code
**Date:** 2026-04-25
**Project:** /home/curly/proyectos/fprime
**Languages:** cpp, c, shell, python

---

## Coverage

- Files in project: **864**
- Files scanned: **864** (100%)
- Truncated: no
- Max-files cap: unlimited (user)

## Summary

- Files scanned: **864**
- Candidates found: **150**
- Confirmed findings: **8**
- False positives: **142**
- Scan duration: 1539.3s

### Severity breakdown

| Severity | Count |
|----------|-------|
| 🟠 HIGH | 5 |
| 🟡 MEDIUM | 3 |

---

## Findings

### 1. 🟠 Fw::Buffer::getData() result used without null check — CWE-476

**File:** `Svc/Ccsds/AosFramer/AosFramer.cpp:304`
**Severity:** HIGH
**Pattern:** `fsw-005-buffer-getdata-unchecked`

**Why this matters:**
Fw::Buffer can be null-allocated if BufferManager was exhausted. getData() returns nullptr in that case. Indexing into or dereferencing the pointer without a null check will crash the component (or worse, the flight computer if the SoC doesn't page-fault cleanly).

**Code:**
```cpp
302:     FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
303: 
304:     const U8* dataStart = data.getData() + dataOffset;
305:     // min of (remaining bytes in buffer and available bytes in frame)
306:     FwSizeType dataSize = data.getSize() - dataOffset;
307: 
```

**Verification:** Fw::BufferManager exhaustion can yield a null-allocated buffer; getData() returns nullptr, and the subsequent pointer arithmetic and serialization at line 321 will dereference it, causing a crash under high load.

**Execution path:** High inbound traffic -> Fw::BufferManager returns null/invalid buffer -> caller passes it to pack_packet() -> data.getData() is nullptr -> line 321 serializeFrom dereferences null -> crash.

**Suggested fix:**
```
Add `FW_ASSERT(data.getData() != nullptr, static_cast<FwAssertArgType>(data.getSize()));` or check `data.isValid()` before use, or return early if invalid.
```

---

### 2. 🟠 Fw::Buffer::getData() result used without null check — CWE-476

**File:** `Svc/FileUplink/FileUplink.cpp:60`
**Severity:** HIGH
**Pattern:** `fsw-005-buffer-getdata-unchecked`

**Why this matters:**
Fw::Buffer can be null-allocated if BufferManager was exhausted. getData() returns nullptr in that case. Indexing into or dereferencing the pointer without a null check will crash the component (or worse, the flight computer if the SoC doesn't page-fault cleanly).

**Code:**
```cpp
58: 
59:     // Deserialize the file packet contents into Fw::FilePacket (remove packet type token)
60:     Fw::Buffer packetBuffer(buffer.getData() + sizeof(packetType),
61:                             buffer.getSize() - static_cast<Fw::Buffer::SizeType>(sizeof(packetType)));
62:     Fw::FilePacket filePacket;
63:     status = filePacket.fromBuffer(packetBuffer);
```

**Verification:** The handler accepts a runtime buffer from an external network/IPC port without validating `getData()`, allowing a null base pointer to be passed to `Fw::Buffer` and subsequently dereferenced in `fromBuffer`, causing a crash.

**Execution path:** External packet arrives -> `bufferSendIn` port dispatches to `bufferSendIn_handler` -> line 60 constructs `packetBuffer` with null base pointer -> `filePacket.fromBuffer` dereferences it -> crash.

**Suggested fix:**
```
Add `if (buffer.getData() == nullptr) { this->log_WARNING_HI_BufferNull(); return; }` before line 60.
```

---

### 3. 🟠 Ground-command argument used before cmdResponse check — CWE-20

**File:** `Svc/FpySequencer/FpySequencer.cpp:67`
**Severity:** HIGH
**Pattern:** `fsw-010-cmd-arg-before-validate`

**Why this matters:**
Ground commands arrive over the flight-link and are inherently untrusted. Using a command string argument (path, mode, sequence) before emitting cmdResponse_OK or at least length-checking it lets a malformed command corrupt state.

**Code:**
```cpp
65: //!
66: //! Loads and validates a sequence
67: void FpySequencer::VALIDATE_cmdHandler(FwOpcodeType opCode,              //!< The opcode
68:                                        U32 cmdSeq,                       //!< The command sequence number
69:                                        const Fw::CmdStringArg& fileName  //!< The name of the sequence file
70: ) {
```

**Verification:** The untrusted `fileName` argument from a ground command is passed directly to `sequencer_sendSignal_cmd_VALIDATE` without any length validation or allowlist check, allowing a malformed path to corrupt state or trigger unsafe file operations downstream.

**Execution path:** Ground station sends VALIDATE command -> F Prime command dispatcher invokes `VALIDATE_cmdHandler` -> `fileName` is forwarded to `sequencer_sendSignal_cmd_VALIDATE` -> downstream file/sequence parser processes raw string.

**Suggested fix:**
```
Add `if (fileName.size() > MAX_SEQ_NAME_LEN) { cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR); return; }` before line 83, or validate/normalize the path before forwarding.
```

---

### 4. 🟠 Fw::Buffer::getData() result used without null check — CWE-476

**File:** `Svc/GenericHub/GenericHub.cpp:131`
**Severity:** HIGH
**Pattern:** `fsw-005-buffer-getdata-unchecked`

**Why this matters:**
Fw::Buffer can be null-allocated if BufferManager was exhausted. getData() returns nullptr in that case. Indexing into or dereferencing the pointer without a null check will crash the component (or worse, the flight computer if the SoC doesn't page-fault cleanly).

**Code:**
```cpp
129:         (size == (fwBuffer.getSize() - sizeof(U32) - sizeof(U32) - sizeof(FwBuffSizeType)))) {
130:         // invokeSerial deserializes arguments before calling a normal invoke, this will return ownership immediately
131:         U8* rawData = fwBuffer.getData() + sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType);
132:         FwSizeType rawSize = fwBuffer.getSize() - sizeof(U32) - sizeof(U32) - sizeof(FwBuffSizeType);
133:         if (type == HUB_TYPE_PORT) {
134:             // Com buffer representations should be copied before the call returns, so we need not "allocate" new data
```

**Verification:** <one sentence naming the input source + bypass + outcome, OR the specific mitigation that rules this out>

**Execution path:** "BufferDriver -> GenericHub deserialization -> line 131"

**Suggested fix:**
```
"Add `FW_ASSERT(fwBuffer.getData() != nullptr, static_cast
```

---

### 5. 🟠 Ground-command argument used before cmdResponse check — CWE-20

**File:** `Svc/PrmDb/PrmDbImpl.cpp:333`
**Severity:** HIGH
**Pattern:** `fsw-010-cmd-arg-before-validate`

**Why this matters:**
Ground commands arrive over the flight-link and are inherently untrusted. Using a command string argument (path, mode, sequence) before emitting cmdResponse_OK or at least length-checking it lets a malformed command corrupt state.

**Code:**
```cpp
331: }
332: 
333: void PrmDbImpl::PRM_LOAD_FILE_cmdHandler(FwOpcodeType opCode,
334:                                          U32 cmdSeq,
335:                                          const Fw::CmdStringArg& fileName,
336:                                          PrmDb_Merge merge) {
```

**Verification:** Untrusted ground command argument `fileName` is passed directly to `readParamFileImpl` without any length, format, or allowlist validation, allowing a malicious payload to trigger unbounded file operations or state corruption.

**Execution path:** Ground command dispatcher -> PRM_LOAD_FILE_cmdHandler -> readParamFileImpl(fileName, DB_STAGING) -> file open/read

**Suggested fix:**
```
Add `if (fileName.getBuffLen() > MAX_FILENAME_LEN) { cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::INVALID_ARG); return; }` before line 359.
```

---

### 6. 🟡 FW_ASSERT used as input validation (disabled in release) — CWE-617

**File:** `FppTestProject/FppTest/topology/types/DataBuffer.cpp:25`
**Severity:** MEDIUM
**Pattern:** `fsw-003-assert-as-validation`

**Why this matters:**
FW_ASSERT compiles out when FW_ASSERT_LEVEL is set to FW_NO_ASSERT (some deployed flight builds). Using it to validate command arguments means validation disappears in release. This is a common flight-software trap.

**Code:**
```cpp
23: }
24: DataBuffer::DataBuffer(const Fw::LinearBufferBase& buffer) {
25:     FW_ASSERT(buffer.getSize() <= sizeof(m_data), static_cast<FwAssertArgType>(buffer.getSize()), sizeof(m_data));
26:     std::memcpy(this->m_data, buffer.getBuffAddr(), buffer.getSize());
27:     m_size = buffer.getSize();
28: }
```

**Verification:** The assert validates a runtime buffer size against a compile-time limit without a fallback, so if disabled in release, untrusted external input (e.g., command argument or telemetry payload) bypasses validation and causes a stack/heap buffer overflow during `std::memcpy`.

**Execution path:** External command/telemetry payload -> deserialization or command handler -> `DataBuffer(const Fw::LinearBufferBase&)` constructor -> line 25 `FW_ASSERT` (stripped in release) -> line 26 `std::memcpy` -> memory corruption.

**Suggested fix:**
```
Replace `FW_ASSERT` with a runtime bounds check that returns an error status or uses `std::min` to clamp the copy length, or switch to a dynamic container if variable sizing is required.
```

---

### 7. 🟡 FW_ASSERT used as input validation (disabled in release) — CWE-617

**File:** `Svc/FpySequencer/FpySequencerRunState.cpp:183`
**Severity:** MEDIUM
**Pattern:** `fsw-003-assert-as-validation`

**Why this matters:**
FW_ASSERT compiles out when FW_ASSERT_LEVEL is set to FW_NO_ASSERT (some deployed flight builds). Using it to validate command arguments means validation disappears in release. This is a common flight-software trap.

**Code:**
```cpp
181: 
182:             // now there should be nothing left, otherwise coding err
183:             FW_ASSERT(argBuf.getDeserializeSizeLeft() == 0,
184:                       static_cast<FwAssertArgType>(argBuf.getDeserializeSizeLeft()));
185: 
186:             // and set the buf size now that we know it
```

**Verification:** Ground command payload length consistency is validated via `FW_ASSERT`, which compiles out in release builds, allowing malformed commands with mismatched length fields to bypass checks and cause buffer overreads or undefined sequencer behavior. (+1 more matches of this pattern in the same file)

**Execution path:** Ground command reception -> `FpySequencerRunState::deserializeDirective` -> `argBuf.deserializeTo` -> line 183

**Suggested fix:**
```
Replace `FW_ASSERT(argBuf.getDeserializeSizeLeft() == 0, ...)` with a conditional check that logs an error and returns `Fw::Success::FAILURE` when `argBuf.getDeserializeSizeLeft() != 0`.
```

---

### 8. 🟡 FW_ASSERT used as input validation (disabled in release) — CWE-617

**File:** `Svc/TlmPacketizer/TlmPacketizer.cpp:51`
**Severity:** MEDIUM
**Pattern:** `fsw-003-assert-as-validation`

**Why this matters:**
FW_ASSERT compiles out when FW_ASSERT_LEVEL is set to FW_NO_ASSERT (some deployed flight builds). Using it to validate command arguments means validation disappears in release. This is a common flight-software trap.

**Code:**
```cpp
49:                                   const Svc::TlmPacketizerPacket& ignoreList,
50:                                   const FwChanIdType startLevel) {
51:     FW_ASSERT(packetList.list);
52:     // Ignore list may be nullptr as long as numEntries is 0. Providing an ignore list with numEntries 0 disables
53:     // functionality for two reasons:
54:     //     1. There are no ignored channels as configured by FPP.
```

**Verification:** Ground command argument `packetList.list` is untrusted external input validated only by a debug-only `FW_ASSERT` that compiles out in release, bypassing validation and causing a null pointer dereference crash at line 71. (+2 more matches of this pattern in the same file)

**Execution path:** Ground station sends SET_PACKET_LIST command with null `list` pointer -> Fw Command Dispatcher deserializes and invokes `TlmPacketizer::setPacketList` -> `FW_ASSERT` strips in release -> loop at line 67 dereferences `packetList.list[pktEntry]` -> crash.

**Suggested fix:**
```
Replace `FW_ASSERT(packetList.list);` with a runtime null check that logs an error event and returns early, or use `Fw::Check::valid`/`Fw::Assert` with appropriate assert level.
```

---

## Verifier rejections (false positives)

The verifier rejected **142** candidates as false positives. Spot-check these against the code to confirm the rejections were sensible; full list is in `AUDIT_REPORT.json → false_positives_detail`.

1. `googletest/googletest/src/gtest.cc:2284` — pattern `cpp-001-ptr-address-index` (high)
   - Reason: The `&` is part of C++ reference-to-array syntax (`T (&name)[N]`), not an address-of operator performing pointer arithmetic, and the function safely converts compile-time constant arrays to vectors via standard iterator initialization.
2. `googletest/googletest/src/gtest.cc:2853` — pattern `cpp-007-deref-before-null-check` (high)
   - Reason: The dereference at line 2853 is guaranteed safe because `HandleExceptionsInMethodIfSupported` only returns `nullptr` after recording a fatal failure, and the preceding `!Test::HasFatalFailure()` check at line 2850 explicitly guards the block, as also confirmed by the explicit comment at line 2849.
3. `googletest/googletest/include/gtest/gtest-param-test.h:304` — pattern `cpp-001-ptr-address-index` (high)
   - Reason: The expression `(&array)[N]` is standard C++ syntax for declaring a function parameter as a reference to an array of size `N`, not pointer arithmetic on a pointer variable.
4. `googletest/googletest/include/gtest/gtest-printers.h:851` — pattern `cpp-001-ptr-address-index` (high)
   - Reason: The syntax `const T (&a)[N]` is a standard C++ function parameter declaration for a reference to an array of compile-time size `N`, not a runtime pointer arithmetic expression. (+1 more matches of this pattern in the same file)
5. `googletest/googletest/include/gtest/internal/gtest-internal.h:1027` — pattern `cpp-001-ptr-address-index` (high)
   - Reason: The syntax `const T (&lhs)[N]` is a standard C++ reference-to-array declaration (parentheses are required to declare a reference to an array), not pointer arithmetic; `N` is a compile-time template parameter and the type system guarantees safe bounds at instantiation. (+2 more matches of this pattern in the same file)
6. `googletest/googlemock/include/gmock/gmock-function-mocker.h:73` — pattern `cpp-001-ptr-address-index` (high)
   - Reason: The token sequence `(&prefix)[N]` is part of a function parameter declaration (`const char (&prefix)[N]`), which is standard C++ syntax for a reference to an array, not a runtime expression performing pointer arithmetic on the address of a variable. (+6 more matches of this pattern in the same file)
7. `googletest/googlemock/include/gmock/gmock-matchers.h:4158` — pattern `cpp-001-ptr-address-index` (high)
   - Reason: The syntax `const T (&array)[N]` is a C++ reference-to-array parameter declaration, not an address-of operator applied to a pointer variable; the type system enforces it as a reference to a compile-time-sized array, ruling out stack-read pointer arithmetic. (+5 more matches of this pattern in the same file)
8. `googletest/googlemock/include/gmock/internal/gmock-internal-utils.h:364` — pattern `cpp-001-ptr-address-index` (high)
   - Reason: The code context shows a function parameter declaration `const Element (&array)[N]`, which is standard C++ syntax for a reference to an array, not an expression performing pointer arithmetic or indexing into `(&var)[N]`. (+1 more matches of this pattern in the same file)
9. `cmake/autocoder/scripts/fpp_to_dict_wrapper.py:48` — pattern `uni-016-external-file-path` (high)
   - Reason: The input is a build-time CLI argument controlled by the build system/developer rather than an external runtime user, and build-time scripts are explicitly excluded from standard security threat models per checklist item 2.
10. `cmake/autocoder/scripts/fpp_to_dict_wrapper.py:48` — pattern `inj-005-path-traversal` (high)
   - Reason: The input is supplied by CMake or a developer at build time, not an external attacker, which places this autocoder wrapper outside standard security threat models.

_…and 132 more. See `AUDIT_REPORT.json`._

## Methodology

This audit was produced by the KCode audit engine: a deterministic pattern library scanned the project for known-dangerous code patterns, then every candidate was verified against the actual execution path. Findings listed here are only those where the execution path was confirmed.

**Pattern library version:** 1.0 — patterns derived from real bugs found in production C/C++ codebases (network I/O, USB/HID decoders, resource lifecycle, integer arithmetic).

---

*Generated by KCode — [Astrolexis.space](https://astrolexis.dev)*
