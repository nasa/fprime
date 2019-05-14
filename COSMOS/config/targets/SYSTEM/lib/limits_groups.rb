# encoding: ascii-8bit

# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

require 'cosmos/tools/cmd_tlm_server/limits_groups_background_task'

module Cosmos
  # Inheriting from the LimitsGroupsBackgroundTask provides a framework for
  # automatically enabling and disabling limits groups based on telemetry.
  class LimitsGroups < LimitsGroupsBackgroundTask
    def initialize(initial_delay = 0, task_delay = 0.5)
      # Initial delay upon starting the server before staring the group checks
      # followed by the background task delay between check iterations
      super(initial_delay, task_delay)
      # Creating a Proc allows for arbitrary code to be executed when a group
      # is enabled or disabled.

    end
  end
end
