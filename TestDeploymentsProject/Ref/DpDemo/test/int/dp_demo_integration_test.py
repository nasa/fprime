import json
from pathlib import Path

from fprime_gds.common.dp.decoder import DataProductDecoder


def test_dp_send(fprime_test_api):
    """Test that DPs are generated and received on the ground"""

    # Run Dp command to send a data product
    fprime_test_api.send_and_assert_command("Ref.dpDemo.Dp", ["IMMEDIATE", 1])
    # Wait for DpStarted event
    result = fprime_test_api.await_event("Ref.dpDemo.DpStarted", start=0, timeout=5)
    assert result
    # Wait for DpComplete event
    result = fprime_test_api.await_event("Ref.dpDemo.DpComplete", start=0, timeout=10)
    assert result
    # Check for FileWritten event and capture the name of the file that was created
    file_result = fprime_test_api.await_event(
        "DataProducts.dpWriter.FileWritten", start=0, timeout=10
    )
    dp_file_path = file_result.get_display_text().split().pop()
    # Verify that the file exists
    # Assumes that we are running the test from the Ref directory
    assert Path(dp_file_path).is_file()


def test_dp_decode(fprime_test_api):
    """Test that we can decode DPs on the ground via DataProductDecoder (`fprime-dp decode`)"""

    # Run Dp command to send a data product
    fprime_test_api.send_and_assert_command("Ref.dpDemo.Dp", ["IMMEDIATE", 1])
    # Check for FileWritten event and capture the name of the file that was created
    file_result = fprime_test_api.await_event(
        "DataProducts.dpWriter.FileWritten", start=0, timeout=10
    )
    dp_file_path = file_result.get_display_text().split().pop()
    # Verify that the file exists
    # Assumes that we are running the test from the Ref directory
    assert Path(dp_file_path).is_file(), "Dp file not downlinked correctly"
    # Decode DP file
    decoded_file_name = Path(dp_file_path).name.replace(".fdp", ".json")
    DataProductDecoder(
        fprime_test_api.dictionaries, dp_file_path, decoded_file_name
    ).process()
    assert Path(decoded_file_name).is_file(), "Decoded file not created"

    # Open both reference JSON and output JSON and compare
    with open(Path(__file__).parent / "dp_ref_output.json", "r") as ref_file, open(
        decoded_file_name, "r"
    ) as output_file:
        ref_json = json.load(ref_file)
        output_json = json.load(output_file)
        # Exclude Time and Checksum header fields since the timestamp will change every time
        ref_json["Header"].pop("Time")
        output_json["Header"].pop("Time")
        ref_json["Header"].pop("Checksum")
        output_json["Header"].pop("Checksum")
        # Every other fields in Header and Data should be exactly the same
        assert ref_json == output_json
