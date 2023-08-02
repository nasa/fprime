**Note:** auto-generated from comments in: ./autocoder/deployment_validation.cmake

## autocoder/deployment_validation.cmake

An autocoder that performs validation steps on the deployment to ensure that it is well-formed. This will produce
warning output when validation fails.

Note: this autocoder is for validation purposes and DOES NOT produce any autocoded files.


## `deployment_validation_is_supported`:

Required function, processes .fpp files


## `deployment_validation_is_supported`:

Required function, look for .fpp files defining a topology block and ensure the following equivalence:
    ${PROJECT_NAME} == topology name
No files are produced by this autocoder. It runs to validate items as part of the autocoder system.



