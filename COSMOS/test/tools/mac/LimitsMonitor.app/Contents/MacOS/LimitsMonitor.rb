#!/usr/bin/env ruby
# encoding: ascii-8bit

# Copyright 2015 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

require_relative 'tool_launch'
tool_launch do
  require 'cosmos/tools/limits_monitor/limits_monitor'
  Cosmos::LimitsMonitor.run
end
