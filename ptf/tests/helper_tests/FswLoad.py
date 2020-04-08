import scripts.helpers.target_helper_factory
import time

target_helper = scripts.helpers.target_helper_factory.TargetHelperFactory().getTargetHelper()

target_helper.start_target()
target_helper.load_fsw()

target_helper.exit()
target_helper.wait()
