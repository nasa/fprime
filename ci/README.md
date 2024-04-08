# F´Continuous Integration

> Note: The information below is historical. Modern F´ uses GitHub Action for all of its CI.

F´continuous integration was developed to work with Jenkins. The continuous integration project is
designed to checkout a new pull request that has been submitted to F´, merge it with the base 
branch, and then build the merged construct. Then the unit tests are built and run against this
merge. This proves that the build is ready for review and merge.

## Jenkins Description

The Jenkins setup exists in the `config.xml` file in this directory. It should be noted that the
secrets (passwords, keys, etc.) have been stripped and should be replaced.

The Jenkins CI is configured with the expressed purpose of checking the build and unit tests for
a given pull request. It does not build on a set interval, nor does it build a specific branch
(master, devel) but rather whatever branch the pull request was based on.

The Jenkins CI is setup to be built inside a docker container. This docker container is used to
encapsulate the exact build environment without needing to setup a specific Jenkins machine with
this build environment. It needs to be run on a Docker capable Jenkins machine.

Once Jenkins CI builds the Docker container from the Dockerfile, it runs the Python Test Framework, which includes the following test suites:

 1. Reference Build and Unit Tests
 2. Autocoder Unit Tests

## Using Jenkins

In order to use the existing Jenkins configuration, one can post it to the server of their choice
using the following basic command. This command may need to be altered to supply credentials or
setup proxies, but this is outside this guide.

This `config.xml` contains `FILL-ME` tokens where various project specific configuration items
belong. These mostly represent passwords, or other secrets that should not be exposed. A user can
edit the config.xml file directly before POSTing the XML to the server. Alternatively, the user can
POST the XML and make the edits in the Jenkins GUI.


**POST Jenkins Config as New Job**
```bash
fprime>curl -X POST http://<jenkins host>/createItem?name=<job name>' --header "Content-Type: application/xml" -d mk/ci/config.xml
```
Here the user must supply the `<jenkins host>` and `<job name>` to create a job on the given host with the given job name.

## Advanced Jenkins 

### Daily and Manual Builds

Some users like to have a repeating build of Jenkins on a Daily or Weekly schedule to ensure that
the common branch (master, devel) are consistent and correct. This can be done by setting a
scheduled build trigger and, if using the above `config.xml`, the `${ghprbActualCommit}` and
`${ghprbTargetBranch}` environment variables should be set to the desired branch to build. This can be done
in Jenkins's environment setup. This allows manual and scheduled builds without creating a separate
job by defaulting the variables used by the Pull Request builder. The user could also setup a
separate job to handle this.

## Future Jenkins Work

The following CI items are yet to be implemented:

1. Run `Ref` App and ensure that it is stable
2. Setup system level tests against `Ref`

