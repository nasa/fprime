# Reusable Integration Tests

F´ ships portable integration tests that exercise framework components (`Svc/*`) and standard subtopologies (`Svc/Subtopologies/*`) against any deployment. They are written once in F´ and reused unchanged in your project — you only supply a small JSON file mapping framework component names to your deployment's instance names.

This guide shows how to run them in your own deployment.

## How it works

Reusable tests live next to the component or subtopology under `test/int/` (e.g. [Svc/CmdDispatcher/test/int/test_cmd_dispatcher.py](../../../Svc/CmdDispatcher/test/int/test_cmd_dispatcher.py), [Svc/Subtopologies/CdhCore/test/int/test_cdh_core.py](../../../Svc/Subtopologies/CdhCore/test/int/test_cdh_core.py)). Instead of hardcoding instance mnemonics like `Ref.cmdDisp`, they call `fprime_test_api.get_mnemonic(...)` with the qualified component name:

```python
fprime_test_api.send_and_assert_command(
    f"{fprime_test_api.get_mnemonic('Svc.CommandDispatcher')}.CMD_NO_OP",
)
```

At runtime, `get_mnemonic("Svc.CommandDispatcher")` reads a JSON file (`int_config.json` by convention) supplied via the `--deployment-config` pytest flag and returns the deployment-specific instance mnemonic — e.g. `CdhCore.cmdDisp` for the Ref deployment. If a key is missing, it falls back to the qualified name itself, so a partial config still works.

Source: `IntegrationTestAPI.get_mnemonic` in [fprime-gds api.py](https://github.com/nasa/fprime-gds/blob/devel/src/fprime_gds/common/testing_fw/api.py).

## Writing your `int_config.json`

Create one JSON file in your deployment (typical location: `<MyDeployment>/test/int/int_config.json`) mapping each `Svc.*` (and any other reusable) component name to the instance mnemonic in your topology:

```json
{
    "Svc.CommandDispatcher": "CdhCore.cmdDisp",
    "Svc.CmdSequencer": "Ref.cmdSeq",
    "Svc.FileDownlink": "FileHandling.fileDownlink",
    "Svc.FileManager": "FileHandling.fileManager",
    ... add more as needed per the tests you want to run ...
}
```

The full reference example is [TestDeploymentsProject/Ref/test/int/int_config.json](../../../TestDeploymentsProject/Ref/test/int/int_config.json) — copy it and replace the right-hand side with your instance names. Only include the components whose tests you intend to run.

## Running the tests

Build and start your deployment, then start the GDS pointed at your dictionary, and run pytest against the `test/int` directory of any component or subtopology you want to exercise:

```bash
# 1. Start GDS + flight software (separate terminal, from your deployment dir)
fprime-gds --dictionary build-artifacts/*/MyDeployment/dict/MyDeploymentTopologyDictionary.json

# 2. Run the reusable tests
pytest ./lib/fprime/Svc/CmdDispatcher/test/int \
       ./lib/fprime/Svc/Subtopologies/CdhCore/test/int \
       --dictionary build-artifacts/*/MyDeployment/dict/MyDeploymentTopologyDictionary.json \
       --deployment-config ./MyDeployment/test/int/int_config.json
```

`--deployment-config` is provided by the `fprime-gds` pytest plugin (see [pytest_integration.py](https://github.com/nasa/fprime-gds/blob/devel/src/fprime_gds/common/testing_fw/pytest_integration.py)). All other flags (`--dictionary`, `--logs`, etc.) are the standard GDS pipeline options.

## CI example

The Ref deployment's CI job is the canonical reference — see [`.github/workflows/ref.yml`](../../../.github/workflows/ref.yml):

```yaml
- name: "Integration Tests"
  uses: nasa/fprime-actions/run-integration-tests@devel
  with:
    test-working-directory: "TestDeploymentsProject/Ref/test/int"
    binary: "build-artifacts/*/Ref/bin/Ref"
    gds-args: "--dictionary build-artifacts/*/Ref/dict/RefTopologyDictionary.json"
    pytest-args: "--deployment-config ${{ github.workspace }}/TestDeploymentsProject/Ref/test/int/int_config.json"
```

To also run the framework's reusable tests, pass extra paths via `pytest-args` to include the relevant `Svc/*/test/int/test_*.py` test files.

## See also

- [GDS Integration Test API Guide](gds-test-api-guide.md) — full `IntegrationTestAPI` reference
- [Ref `int_config.json`](../../../TestDeploymentsProject/Ref/test/int/int_config.json) — complete example
