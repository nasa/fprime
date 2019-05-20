# cmd
cmd("INST ABORT")
cmd("INST ARYCMD with ARRAY []")
cmd("INST CLEAR")
cmd("INST COLLECT with DURATION 1.0, TEMP 0.0, TYPE 'NORMAL'")
cmd("INST FLTCMD with FLOAT32 0.0, FLOAT64 0.0")
cmd("INST SETPARAMS with VALUE1 0, VALUE2 0, VALUE3 0, VALUE4 0, VALUE5 0")
cmd("INST SLRPNLDEPLOY")
cmd("INST SLRPNLRESET")
cmd("INST ASCIICMD with STRING 'ARM LASER'")

cmd("INST", "ABORT")
cmd("INST", "ARYCMD", "ARRAY" => [])
cmd("INST", "CLEAR")
cmd("INST", "COLLECT", "DURATION" => 1.0, "TEMP" => 0.0, "TYPE" => 'NORMAL')
cmd("INST", "FLTCMD", "FLOAT32" => 0.0, "FLOAT64" => 0.0)
cmd("INST", "SETPARAMS", "VALUE1" => 0, "VALUE2" => 0, "VALUE3" => 0, "VALUE4" => 0, "VALUE5" => 0)
cmd("INST", "SLRPNLDEPLOY")
cmd("INST", "SLRPNLRESET")
cmd("INST", "ASCIICMD", "STRING" => 'ARM LASER')

# cmd should fail
begin
  cmd("INST COLLECT with DURATION 11, TYPE 'NORMAL'")
rescue RuntimeError => e
  raise "Fail" if e.message != "Command parameter 'INST COLLECT DURATION' = 11 not in valid range of 0.0 to 10.0"
rescue
  raise "Fail"
end

cmd()
cmd("BOB")
cmd("BOB", "ABORT")
cmd("INST", "BOB")
cmd("INST", "ABORT", "BOB" => "BOB")
cmd("INST", "ABORT", "TED", "BOB" => "BOB")
cmd("BOB")
cmd("BOB ABORT")
cmd("INST BOB")

# cmd_no_range_check
cmd_no_range_check("INST ABORT")
cmd_no_range_check("INST ARYCMD with ARRAY []")
cmd_no_range_check("INST CLEAR")
cmd_no_range_check("INST COLLECT with DURATION 1.0, TEMP 0.0, TYPE 'NORMAL'")
cmd_no_range_check("INST FLTCMD with FLOAT32 0.0, FLOAT64 0.0")
cmd_no_range_check("INST SETPARAMS with VALUE1 0, VALUE2 0, VALUE3 0, VALUE4 0, VALUE5 0")
cmd_no_range_check("INST SLRPNLDEPLOY")
cmd_no_range_check("INST SLRPNLRESET")
cmd_no_range_check("INST ASCIICMD with STRING 'ARM LASER'")
cmd_no_range_check("INST COLLECT with DURATION 11, TYPE 'NORMAL'")

cmd_no_range_check("INST", "ABORT")
cmd_no_range_check("INST", "ARYCMD", "ARRAY" => [])
cmd_no_range_check("INST", "CLEAR")
cmd_no_range_check("INST", "COLLECT", "DURATION" => 1.0, "TEMP" => 0.0, "TYPE" => 'NORMAL')
cmd_no_range_check("INST", "FLTCMD", "FLOAT32" => 0.0, "FLOAT64" => 0.0)
cmd_no_range_check("INST", "SETPARAMS", "VALUE1" => 0, "VALUE2" => 0, "VALUE3" => 0, "VALUE4" => 0, "VALUE5" => 0)
cmd_no_range_check("INST", "SLRPNLDEPLOY")
cmd_no_range_check("INST", "SLRPNLRESET")
cmd_no_range_check("INST", "ASCIICMD", "STRING" => 'ARM LASER')
cmd_no_range_check("INST", "COLLECT", "DURATION" => 11, "TYPE" => 'NORMAL')

# cmd_no_range_check should fail
cmd_no_range_check()
cmd_no_range_check("BOB")
cmd_no_range_check("BOB", "ABORT")
cmd_no_range_check("INST", "BOB")
cmd_no_range_check("INST", "ABORT", "BOB" => "BOB")
cmd_no_range_check("INST", "ABORT", "TED", "BOB" => "BOB")
cmd_no_range_check("BOB")
cmd_no_range_check("BOB ABORT")
cmd_no_range_check("INST BOB")

# cmd_no_hazardous_check
cmd_no_hazardous_check("INST ABORT")
cmd_no_hazardous_check("INST ARYCMD with ARRAY []")
cmd_no_hazardous_check("INST CLEAR")
cmd_no_hazardous_check("INST COLLECT with DURATION 1.0, TEMP 0.0, TYPE 'NORMAL'")
cmd_no_hazardous_check("INST FLTCMD with FLOAT32 0.0, FLOAT64 0.0")
cmd_no_hazardous_check("INST SETPARAMS with VALUE1 0, VALUE2 0, VALUE3 0, VALUE4 0, VALUE5 0")
cmd_no_hazardous_check("INST SLRPNLDEPLOY")
cmd_no_hazardous_check("INST SLRPNLRESET")
cmd_no_hazardous_check("INST ASCIICMD with STRING 'ARM LASER'")

cmd_no_hazardous_check("INST", "ABORT")
cmd_no_hazardous_check("INST", "ARYCMD", "ARRAY" => [])
cmd_no_hazardous_check("INST", "CLEAR")
cmd_no_hazardous_check("INST", "COLLECT", "DURATION" => 1.0, "TEMP" => 0.0, "TYPE" => 'NORMAL')
cmd_no_hazardous_check("INST", "FLTCMD", "FLOAT32" => 0.0, "FLOAT64" => 0.0)
cmd_no_hazardous_check("INST", "SETPARAMS", "VALUE1" => 0, "VALUE2" => 0, "VALUE3" => 0, "VALUE4" => 0, "VALUE5" => 0)
cmd_no_hazardous_check("INST", "SLRPNLDEPLOY")
cmd_no_hazardous_check("INST", "SLRPNLRESET")
cmd_no_hazardous_check("INST", "ASCIICMD", "STRING" => 'ARM LASER')

# cmd_no_hazardous_check should fail
cmd_no_hazardous_check("INST COLLECT with DURATION 11, TYPE 'NORMAL'")
cmd_no_hazardous_check()
cmd_no_hazardous_check("BOB")
cmd_no_hazardous_check("BOB", "ABORT")
cmd_no_hazardous_check("INST", "BOB")
cmd_no_hazardous_check("INST", "ABORT", "BOB" => "BOB")
cmd_no_hazardous_check("INST", "ABORT", "TED", "BOB" => "BOB")
cmd_no_hazardous_check("BOB")
cmd_no_hazardous_check("BOB ABORT")
cmd_no_hazardous_check("INST BOB")

# cmd_no_checks
cmd_no_checks("INST ABORT")
cmd_no_checks("INST ARYCMD with ARRAY []")
cmd_no_checks("INST CLEAR")
cmd_no_checks("INST COLLECT with DURATION 1.0, TEMP 0.0, TYPE 'NORMAL'")
cmd_no_checks("INST FLTCMD with FLOAT32 0.0, FLOAT64 0.0")
cmd_no_checks("INST SETPARAMS with VALUE1 0, VALUE2 0, VALUE3 0, VALUE4 0, VALUE5 0")
cmd_no_checks("INST SLRPNLDEPLOY")
cmd_no_checks("INST SLRPNLRESET")
cmd_no_checks("INST ASCIICMD with STRING 'ARM LASER'")
cmd_no_checks("INST COLLECT with DURATION 11, TYPE 'NORMAL'")

cmd_no_checks("INST", "ABORT")
cmd_no_checks("INST", "ARYCMD", "ARRAY" => [])
cmd_no_checks("INST", "CLEAR")
cmd_no_checks("INST", "COLLECT", "DURATION" => 1.0, "TEMP" => 0.0, "TYPE" => 'NORMAL')
cmd_no_checks("INST", "FLTCMD", "FLOAT32" => 0.0, "FLOAT64" => 0.0)
cmd_no_checks("INST", "SETPARAMS", "VALUE1" => 0, "VALUE2" => 0, "VALUE3" => 0, "VALUE4" => 0, "VALUE5" => 0)
cmd_no_checks("INST", "SLRPNLDEPLOY")
cmd_no_checks("INST", "SLRPNLRESET")
cmd_no_checks("INST", "ASCIICMD", "STRING" => 'ARM LASER')
cmd_no_checks("INST", "COLLECT", "DURATION" => 11, "TYPE" => 'NORMAL')

# cmd_no_checks should fail
cmd_no_checks()
cmd_no_checks("BOB")
cmd_no_checks("BOB", "ABORT")
cmd_no_checks("INST", "BOB")
cmd_no_checks("INST", "ABORT", "BOB" => "BOB")
cmd_no_checks("INST", "ABORT", "TED", "BOB" => "BOB")
cmd_no_checks("BOB")
cmd_no_checks("BOB ABORT")
cmd_no_checks("INST BOB")

# send_raw should fail (on demo cmd/tlm server)
send_raw()
send_raw("INT1")
send_raw("INT1", "\x00\x00")
send_raw("INT1", "\x00\x00", "\x00\x00")

# get_cmd_list
expected_list = [["ABORT", "Aborts a collect on the INST instrument"], ["ARYCMD", "Command with array parameter"], ["ASCIICMD", "Enumerated ASCII command"], ["CLEAR", "Clears counters on the INST instrument"], ["COLLECT", "Starts a collect on the INST target"], ["FLTCMD", "Command with float parameters"], ["SETPARAMS", "Sets numbered parameters"], ["SLRPNLDEPLOY", "Deploy solar array panels"], ["SLRPNLRESET", "Reset solar array panels"]]
list = get_cmd_list("INST")
puts list.inspect
if list != expected_list
  raise "Fail"
end

# get_cmd_list should fail
get_cmd_list()
get_cmd_list("BOB")
get_cmd_list("BOB", "TED")

# get_cmd_param_list
expected_list = [["CCSDSVER", 0, nil, "CCSDS primary header version number", nil, nil, false, "UINT"],
  ["CCSDSTYPE", 1, nil, "CCSDS primary header packet type", nil, nil, false, "UINT"],
  ["CCSDSSHF", 0, nil, "CCSDS primary header secondary header flag", nil, nil, false, "UINT"],
  ["CCSDSAPID", 999, nil, "CCSDS primary header application id", nil, nil, false, "UINT"],
  ["CCSDSSEQFLAGS", 3, nil, "CCSDS primary header sequence flags", nil, nil, false, "UINT"],
  ["CCSDSSEQCNT", 0, nil, "CCSDS primary header sequence count", nil, nil, false, "UINT"],
  ["CCSDSLENGTH", 12, nil, "CCSDS primary header packet length", nil, nil, false, "UINT"],
  ["PKTID", 1, nil, "Packet id", nil, nil, false, "UINT"],
  ["TYPE", 0, {"NORMAL"=>0, "SPECIAL"=>1}, "Collect type", nil, nil, true, "UINT"],
  ["DURATION", 1.0, nil, "Collect duration", nil, nil, false, "FLOAT"],
  ["OPCODE", "0xAB", nil, "Collect opcode", nil, nil, false, "UINT"],
  ["TEMP", 0.0, nil, "Collect temperature", "Celcius", "C", false, "FLOAT"]]
list = get_cmd_param_list("INST", "COLLECT")
puts list.inspect(100)
if list != expected_list
  raise "Fail"
end

# get_cmd_param_list should fail
get_cmd_param_list()
get_cmd_param_list("INST")
get_cmd_param_list("INST", "BOB")
get_cmd_param_list("INST", "COLLECT", "DURATION")

# get_cmd_hazardous
hazardous, hazardous_description = get_cmd_hazardous("INST", "COLLECT", "TYPE" => "SPECIAL")
puts "#{hazardous}:#{hazardous_description}"
hazardous, hazardous_description = get_cmd_hazardous("INST", "COLLECT", "TYPE" => "NORMAL")
puts "#{hazardous}:#{hazardous_description}"
hazardous, hazardous_description = get_cmd_hazardous("INST", "ABORT")
puts "#{hazardous}:#{hazardous_description}"
hazardous, hazardous_description = get_cmd_hazardous("INST", "CLEAR")
puts "#{hazardous}:#{hazardous_description}"

# get_cmd_hazardous should fail
get_cmd_hazardous()
get_cmd_hazardous("INST")
get_cmd_hazardous("INST", "COLLECT", "BOB" => 5)
get_cmd_hazardous("INST", "COLLECT", 5)

# tlm
tlm("INST HEALTH_STATUS ARY")
tlm("INST HEALTH_STATUS ASCIICMD")
tlm("INST HEALTH_STATUS CCSDSAPID")
tlm("INST HEALTH_STATUS TEMP1")

tlm("INST", "HEALTH_STATUS", "ARY")
tlm("INST", "HEALTH_STATUS", "ASCIICMD")
tlm("INST", "HEALTH_STATUS", "CCSDSAPID")
tlm("INST", "HEALTH_STATUS", "TEMP1")

# tlm should fail
tlm()
tlm("BOB")
tlm("INST")
tlm("INST BOB")
tlm("INST HEALTH_STATUS")
tlm("INST HEALTH_STATUS BOB")
tlm("INST HEALTH_STATUS ARY BOB")
tlm("INST", "BOB")
tlm("INST", "HEALTH_STATUS")
tlm("INST", "HEALTH_STATUS", "BOB")
tlm("INST", "HEALTH_STATUS", "ARY", "BOB")

# tlm_raw
tlm_raw("INST HEALTH_STATUS ARY")
tlm_raw("INST HEALTH_STATUS ASCIICMD")
tlm_raw("INST HEALTH_STATUS CCSDSAPID")
tlm_raw("INST HEALTH_STATUS TEMP1")

tlm_raw("INST", "HEALTH_STATUS", "ARY")
tlm_raw("INST", "HEALTH_STATUS", "ASCIICMD")
tlm_raw("INST", "HEALTH_STATUS", "CCSDSAPID")
tlm_raw("INST", "HEALTH_STATUS", "TEMP1")

# tlm_raw should fail
tlm_raw()
tlm_raw("BOB")
tlm_raw("INST")
tlm_raw("INST BOB")
tlm_raw("INST HEALTH_STATUS")
tlm_raw("INST HEALTH_STATUS BOB")
tlm_raw("INST HEALTH_STATUS ARY BOB")
tlm_raw("INST", "BOB")
tlm_raw("INST", "HEALTH_STATUS")
tlm_raw("INST", "HEALTH_STATUS", "BOB")
tlm_raw("INST", "HEALTH_STATUS", "ARY", "BOB")

# tlm_formatted
tlm_formatted("INST HEALTH_STATUS ARY")
tlm_formatted("INST HEALTH_STATUS ASCIICMD")
tlm_formatted("INST HEALTH_STATUS CCSDSAPID")
tlm_formatted("INST HEALTH_STATUS TEMP1")

tlm_formatted("INST", "HEALTH_STATUS", "ARY")
tlm_formatted("INST", "HEALTH_STATUS", "ASCIICMD")
tlm_formatted("INST", "HEALTH_STATUS", "CCSDSAPID")
tlm_formatted("INST", "HEALTH_STATUS", "TEMP1")

# tlm_formatted should fail
tlm_formatted()
tlm_formatted("BOB")
tlm_formatted("INST")
tlm_formatted("INST BOB")
tlm_formatted("INST HEALTH_STATUS")
tlm_formatted("INST HEALTH_STATUS BOB")
tlm_formatted("INST HEALTH_STATUS ARY BOB")
tlm_formatted("INST", "BOB")
tlm_formatted("INST", "HEALTH_STATUS")
tlm_formatted("INST", "HEALTH_STATUS", "BOB")
tlm_formatted("INST", "HEALTH_STATUS", "ARY", "BOB")

# tlm_with_units
tlm_with_units("INST HEALTH_STATUS ARY")
tlm_with_units("INST HEALTH_STATUS ASCIICMD")
tlm_with_units("INST HEALTH_STATUS CCSDSAPID")
tlm_with_units("INST HEALTH_STATUS TEMP1")

tlm_with_units("INST", "HEALTH_STATUS", "ARY")
tlm_with_units("INST", "HEALTH_STATUS", "ASCIICMD")
tlm_with_units("INST", "HEALTH_STATUS", "CCSDSAPID")
tlm_with_units("INST", "HEALTH_STATUS", "TEMP1")

# tlm_with_units should fail
tlm_with_units()
tlm_with_units("BOB")
tlm_with_units("INST")
tlm_with_units("INST BOB")
tlm_with_units("INST HEALTH_STATUS")
tlm_with_units("INST HEALTH_STATUS BOB")
tlm_with_units("INST HEALTH_STATUS ARY BOB")
tlm_with_units("INST", "BOB")
tlm_with_units("INST", "HEALTH_STATUS")
tlm_with_units("INST", "HEALTH_STATUS", "BOB")
tlm_with_units("INST", "HEALTH_STATUS", "ARY", "BOB")

# override_tlm
override_tlm("INST HEALTH_STATUS ARY = [0,0,0,0,0,0,0,0,0,0]")
override_tlm("INST HEALTH_STATUS ASCIICMD = 'HI'")
override_tlm("INST HEALTH_STATUS CCSDSAPID = 1000")
override_tlm("INST HEALTH_STATUS TEMP1 = 15")

# override_tlm_raw
override_tlm_raw("INST HEALTH_STATUS ARY = [0,0,0,0,0,0,0,0,0,0]")
override_tlm_raw("INST HEALTH_STATUS ASCIICMD = 'HI'")
override_tlm_raw("INST HEALTH_STATUS CCSDSAPID = 1000")
override_tlm_raw("INST HEALTH_STATUS TEMP1 = 10000")

# normalize_tlm
normalize_tlm("INST HEALTH_STATUS ARY")
normalize_tlm("INST HEALTH_STATUS ASCIICMD")
normalize_tlm("INST HEALTH_STATUS CCSDSAPID")
normalize_tlm("INST HEALTH_STATUS TEMP1")
