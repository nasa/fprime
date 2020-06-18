"""
The implementation code for the command-send GDS CLI commands
"""

from typing import List

from fprime_gds.common.gds_cli.base_commands import BaseCommand
import fprime_gds.common.gds_cli.test_api_utils as test_api_utils


class CommandSendCommand(BaseCommand):
    """
    The implementation for sending a command via the GDS to the spacecraft
    """

    # TODO: Cut down on the number of arguments here?
    @classmethod
    def handle_arguments(
        cls,
        dictionary: str,
        ip_address: str,
        port: int,
        command_name: str,
        arguments: List[str],
        *args,
        **kwargs
    ):
        """
        Handle the command-send arguments to connect to the Test API correctly,
        then send the command via the Test API.

        For more details on these arguments, see the command-send definition at:
            Gds/src/fprime_gds/executables/fprime_cli.py
        """
        # TODO: Make this api setup part of a decorator somehow, since it
        # recurs in several places?
        # ======================================================================
        pipeline, api = test_api_utils.initialize_test_api(
            dictionary, server_ip=ip_address, server_port=port
        )
        # ======================================================================

        api.send_command(command_name, arguments)

        # ======================================================================
        pipeline.disconnect()
        api.teardown()
        # ======================================================================
