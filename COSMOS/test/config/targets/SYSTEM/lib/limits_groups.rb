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
      @temp2_enable_code = Proc.new do
        enable_limits_group('INST2_GROUND') # Enable the INST2_GROUND group
      end
      @temp2_disable_code = Proc.new do
        disable_limits_group('INST2_GROUND') # Disable the INST2_GROUND group
      end
    end

    # This method name is important. It must begin with 'check_' and end with
    # the name of a Limits Group defined by the LIMITS_GROUP keyword. In the
    # demo this is defined in config/targets/SYSTEM/cmd_tlm/limits_groups.txt.
    # Note that there is a LIMITS_GROUP INST2_TEMP2 so we have a match.
    def check_inst2_temp2
      # Inside this method you must call process_group. The first parameter is
      # the number of seconds to delay before enabling the group when the telemetry
      # check is true. When the telemetry check is false the group is instantly
      # disabled. The next two parameters are Proc objects which are called with
      # the group is enabled and disabled respectively. We defined our Proc objects
      # in the constructor to enable and disable the GROUND group.
      process_group(0, @temp2_enable_code, @temp2_disable_code) do
        val = tlm("INST2 HEALTH_STATUS TEMP2")
        # The expression returns TRUE (to enable the group) when the value is
        # not NAN  and not Infinite. If the value is NAN or Infinite the group
        # is disabled. Note that this can't prevent false positives because the
        # value has to change to something invalid before we can turn off the group
        # at which point it is too late. Typically you enable or disable a group
        # based on some external telemetry point such as a power supply output
        # to enable a group of items powered by the supply.
        !val.nan? && !val.infinite?
      end
    end
  end
end
