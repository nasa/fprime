set_line_delay(0)
set_replay_mode(true)
filenames = get_output_logs_filenames()
replay_select_file(filenames[-1])
status = 'Analyzing'
while status =~ /Analyzing/
  status, playback_delay, filename, file_start, file_current, file_end, file_index, file_max_index = replay_status
  wait(1)
end
100.times do
  replay_step_forward()
end
100.times do
  replay_step_back()
end
replay_move_end()
replay_move_index(file_max_index / 2)
replay_move_start()
replay_set_playback_delay(0.1)
replay_play()
wait(2)
replay_set_playback_delay(0.125)
wait(5)
replay_set_playback_delay(nil)
wait(2)
replay_set_playback_delay(0.0)
wait(4)
replay_stop()
replay_reverse_play()
wait(5)
cmd_tlm_clear_counters()
cmd_tlm_reload()
