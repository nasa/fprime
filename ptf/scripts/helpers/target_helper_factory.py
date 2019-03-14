import os
import scripts.helpers.target_helpers.linux_ut_helper
import scripts.helpers.target_helpers.linux_helper
import scripts.helpers.target_helpers.isf_linux_helper

class TargetHelperFactory:
    def getTargetHelper(self, type = os.environ["TARGET_type"],deployment = "FSW",instance_id = None):
        # there is probably a more anonymous way to do this...
        if type == "linux_ut":
            return scripts.helpers.target_helpers.linux_ut_helper.LinuxUtHelper(deployment,instance_id)
        if type == "linux":
            return scripts.helpers.target_helpers.linux_helper.LinuxHelper(deployment,instance_id)
        if type == "isf_linux":
            return scripts.helpers.target_helpers.isf_linux_helper.IsfLinuxHelper(deployment,instance_id)
        else:
            print("Unknown target " + type)

        
