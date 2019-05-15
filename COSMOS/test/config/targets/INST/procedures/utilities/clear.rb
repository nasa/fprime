def clear
  #Command the collect
  cmd("INST CLEAR")

  #Wait for telemetry to update
  wait_check("INST HEALTH_STATUS COLLECTS == 0", 10)
end
