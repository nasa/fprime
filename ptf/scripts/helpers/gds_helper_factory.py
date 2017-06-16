import os
import scripts.helpers.gds_helpers.mpcs_gds_helper
import scripts.helpers.gds_helpers.isf_gse_helper

class GdsHelperFactory:
    def getGdsHelper(self, type = os.environ["GDS"]):
        # there is probably a more anonymous way to do this...
        return None
        