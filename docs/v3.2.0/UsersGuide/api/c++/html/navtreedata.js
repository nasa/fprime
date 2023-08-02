/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "F´ Flight Software - C/C++ Documentation", "index.html", [
    [ "Contributing Guidelines", "md__c_o_n_t_r_i_b_u_t_i_n_g.html", [
      [ "Ways of Contributing", "md__c_o_n_t_r_i_b_u_t_i_n_g.html#autotoc_md1", null ],
      [ "Where to Start", "md__c_o_n_t_r_i_b_u_t_i_n_g.html#autotoc_md2", null ],
      [ "Project Structure", "md__c_o_n_t_r_i_b_u_t_i_n_g.html#autotoc_md3", [
        [ "F´ Repository Structure", "md__c_o_n_t_r_i_b_u_t_i_n_g.html#autotoc_md4", null ]
      ] ],
      [ "Development Process", "md__c_o_n_t_r_i_b_u_t_i_n_g.html#autotoc_md5", [
        [ "Submission Review", "md__c_o_n_t_r_i_b_u_t_i_n_g.html#autotoc_md6", null ],
        [ "Automated Checking", "md__c_o_n_t_r_i_b_u_t_i_n_g.html#autotoc_md7", null ]
      ] ],
      [ "Final Approval and Submission", "md__c_o_n_t_r_i_b_u_t_i_n_g.html#autotoc_md8", null ],
      [ "Helpful Tips", "md__c_o_n_t_r_i_b_u_t_i_n_g.html#autotoc_md9", [
        [ "Keep Submissions Small", "md__c_o_n_t_r_i_b_u_t_i_n_g.html#autotoc_md10", null ]
      ] ],
      [ "Run Tests", "md__c_o_n_t_r_i_b_u_t_i_n_g.html#autotoc_md11", null ]
    ] ],
    [ "Communication Adapter Interface", "md_docs_2_design_2communication-adapter-interface.html", [
      [ "Ports", "md_docs_2_design_2communication-adapter-interface.html#autotoc_md13", [
        [ "comDataIn Description", "md_docs_2_design_2communication-adapter-interface.html#autotoc_md14", null ],
        [ "comDataOut Description", "md_docs_2_design_2communication-adapter-interface.html#autotoc_md15", null ],
        [ "comStatus Description", "md_docs_2_design_2communication-adapter-interface.html#autotoc_md16", null ]
      ] ],
      [ "Communication Queue Protocol", "md_docs_2_design_2communication-adapter-interface.html#autotoc_md17", null ],
      [ "Framer Status Protocol", "md_docs_2_design_2communication-adapter-interface.html#autotoc_md18", null ],
      [ "Communication Adapter Protocol", "md_docs_2_design_2communication-adapter-interface.html#autotoc_md19", null ]
    ] ],
    [ "Design and Philosophy of F´", "md_docs_2_design_2general.html", null ],
    [ "F´ Memory Management", "md_docs_2_design_2memory.html", null ],
    [ "F´ Numerical Types", "md_docs_2_design_2numerical-types.html", [
      [ "Fixed Width Types", "md_docs_2_design_2numerical-types.html#autotoc_md23", null ],
      [ "F´ Logical Integer Type Design", "md_docs_2_design_2numerical-types.html#autotoc_md24", [
        [ "Platform Configured Types", "md_docs_2_design_2numerical-types.html#autotoc_md25", null ],
        [ "Configurable Integer Types", "md_docs_2_design_2numerical-types.html#autotoc_md26", null ]
      ] ]
    ] ],
    [ "Fw Components, Ports, and Classes", "fw.html", "_fw" ],
    [ "Os Components Ports and Classes", "os.html", "_os" ],
    [ "Svc Components, Ports, and Classes", "svc.html", "_svc" ],
    [ "Drv Components and Classes", "drv.html", "_drv" ],
    [ "Utils Classes", "utils.html", "_utils" ],
    [ "Security Policy", "md__s_e_c_u_r_i_t_y.html", null ],
    [ "Svc::BufferRepeater Component", "svc_buffer_repeater_component.html", [
      [ "Svc::BufferRepeater: Buffer Repeater (Passive Component)", "svc_buffer_repeater_component.html#autotoc_md260", [
        [ "1. Introduction", "svc_buffer_repeater_component.html#autotoc_md261", null ],
        [ "2. Requirements", "svc_buffer_repeater_component.html#autotoc_md262", null ],
        [ "3. Design", "svc_buffer_repeater_component.html#autotoc_md263", null ],
        [ "3.1 Ports", "svc_buffer_repeater_component.html#autotoc_md264", [
          [ "3.2 Events", "svc_buffer_repeater_component.html#autotoc_md266", null ]
        ] ],
        [ "4. Configuration", "svc_buffer_repeater_component.html#autotoc_md267", null ]
      ] ]
    ] ],
    [ "Svc::ComQueue Component", "svc_com_queue_component.html", [
      [ "Svc::ComQueue (Active Component)", "svc_com_queue_component.html#autotoc_md332", [
        [ "1. Introduction", "svc_com_queue_component.html#autotoc_md333", null ],
        [ "2. Assumptions", "svc_com_queue_component.html#autotoc_md334", null ],
        [ "3. Requirements", "svc_com_queue_component.html#autotoc_md335", null ],
        [ "4. Design", "svc_com_queue_component.html#autotoc_md336", [
          [ "4.1. Ports", "svc_com_queue_component.html#autotoc_md337", null ],
          [ "4.2. State", "svc_com_queue_component.html#autotoc_md338", null ],
          [ "4.2.1 State Machine", "svc_com_queue_component.html#autotoc_md339", null ],
          [ "4.3 Model Configuration", "svc_com_queue_component.html#autotoc_md340", null ],
          [ "4.4 Runtime Setup", "svc_com_queue_component.html#autotoc_md341", null ],
          [ "4.5 Port Handlers", "svc_com_queue_component.html#autotoc_md342", [
            [ "4.5.1 buffQueueIn", "svc_com_queue_component.html#autotoc_md343", null ],
            [ "4.5.2 comQueueIn", "svc_com_queue_component.html#autotoc_md344", null ],
            [ "4.5.3 comStatusIn", "svc_com_queue_component.html#autotoc_md345", null ],
            [ "4.5.4 run", "svc_com_queue_component.html#autotoc_md346", null ]
          ] ],
          [ "4.6 Telemetry", "svc_com_queue_component.html#autotoc_md347", null ],
          [ "4.7 Events", "svc_com_queue_component.html#autotoc_md348", null ],
          [ "4.8 Helper Functions", "svc_com_queue_component.html#autotoc_md349", [
            [ "4.8.1 sendComBuffer", "svc_com_queue_component.html#autotoc_md350", null ],
            [ "4.8.2 sendBuffer", "svc_com_queue_component.html#autotoc_md351", null ],
            [ "4.8.3 processQueue", "svc_com_queue_component.html#autotoc_md352", null ]
          ] ]
        ] ]
      ] ]
    ] ],
    [ "Svc::ComStub Component", "svc_com_stub_component.html", [
      [ "Svc::ComStub (Passive Component)", "svc_com_stub_component.html#autotoc_md353", [
        [ "1. Introduction", "svc_com_stub_component.html#autotoc_md354", null ],
        [ "2. Assumptions", "svc_com_stub_component.html#autotoc_md355", null ],
        [ "3. Requirements", "svc_com_stub_component.html#autotoc_md356", null ],
        [ "4. Design", "svc_com_stub_component.html#autotoc_md357", [
          [ "4.1. Ports", "svc_com_stub_component.html#autotoc_md358", null ],
          [ "4.2. State, Configuration, and Runtime Setup", "svc_com_stub_component.html#autotoc_md359", null ],
          [ "4.3. Port Handlers", "svc_com_stub_component.html#autotoc_md360", [
            [ "4.3.1 comDataIn", "svc_com_stub_component.html#autotoc_md361", null ],
            [ "4.3.1 drvConnected", "svc_com_stub_component.html#autotoc_md362", null ],
            [ "4.3.1 drvDataIn", "svc_com_stub_component.html#autotoc_md363", null ]
          ] ]
        ] ]
      ] ]
    ] ],
    [ "System Resources Component", "md__svc_2_system_resources_2docs_2sdd.html", null ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", "namespacemembers_dup" ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ],
        [ "Enumerator", "namespacemembers_eval.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", "functions_vars" ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Enumerator", "functions_eval.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"a_m_p_c_s_sequence_8cpp.html",
"command_dispatcher_impl_8hpp.html",
"file_system_8hpp.html#a8bcd5b75e145d2e7f77d68eec9b5ca9fa378e0405d1b2d5d768e7fb42f5b6a60d",
"health_component_vx_works_checks_8cpp.html",
"os_2_linux_2_system_resources_8cpp.html#a3c6b273aaa572ea6dde51277efc41153",
"string_test_8hpp.html",
"svc_fatal_handler_component.html#autotoc_md413",
"svc_tlm_chan_component.html#autotoc_md639",
"array_2main_8cpp.html#a7c9196bd3b27c9f26039e1b9161e9df7",
"class_drv_1_1_tcp_client_socket.html#a39e6851fe99a5dc5ea530ccd59bd6f56",
"class_fw_1_1_cmd_string_arg.html#ad4db24f055270b36d250ab7b31c16909",
"class_fw_1_1_log_packet.html#adf4a96e9301f2bd592a079f5e451bc8a",
"class_fw_1_1_queued_component_base.html#a949fc72adb06a376aef83acc97943681a5c17c4ed3bbbb8eada20779e00d654e6",
"class_fw_1_1_time.html#a84869d06d1fdfb1050ff43da9c95abecaf75a4bea358b1d514ae00d6d2f6cac70",
"class_os_1_1_file.html#a7ee286422e714489bb92d2228075f0b7ab195b6300645908a16f113ed9eae7ee3",
"class_os_1_1_queue_registry.html",
"class_svc_1_1_active_text_logger_component_impl.html#a997805030740a620811d60634077de2e",
"class_svc_1_1_command_dispatcher_impl.html#adc9237cddbce1d99d2191f05cc9ff7d3",
"class_svc_1_1_system_resources.html#afb2212e568710ba11e83c81bed4bcdc2",
"dir_38fb30749794eba79ffbbabe203380e2.html",
"lib__crc_8c.html#a9fdc5bf2c361ec613f6b40c510822fa3",
"namespace_os_1_1_validate_file.html#aafefcd3043d3757fb000d372f5cf2705",
"struct_fpp_test_1_1_port_1_1_struct_return.html#a4eb13acaa721a5e26e18258dbf8799a8",
"union_fw_1_1_file_packet.html#aa93c04640430be90bcf19d4b58f6501ba11269c0eccaffad1be6dbb6a1777c46f"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';