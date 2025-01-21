# Software Bill Of Materials Generation

A software bill of materials is a record of the software that constitutes a product. F Prime will automatically generate a Bill of Materials for a your project as part of the build system. Generation requires the `syft` tool to be installed.

## Running Software Bill of Materials

To generate the software bill of material you must first install [`syft`](https://github.com/anchore/syft). Follow the instruction in the README to install `syft` and ensure that it is on the PATH.

Once `syft` is installed the path, your software bill of materials will be installed in the `build-artifacts/` folder.

## Details and Idiosyncrasies

F Prime uses the `spdx-json` format for the bill of materials using the `syft` tool. It will capture software tools installed in the filesystem rooted at the project root. This will include python installations, `requirements.txt` packages, and various other tools detectable by `syft`.

To see the full catalog run `syft cataloger list`.

>[!WARNING]
> `cmake` and your C++ compiler are not likely installed within the project file system. To generate a bill of materials including these external tools, you will need to build a container to build your product and scan that container.

## Scanning for Vulnerabilities

To scan for vulnerabilities in the bill of materials, you must first install [`grype`](https://github.com/anchore/grype). Follow the instructions in the README to install `grype` and ensure it is on the PATH.

Once `grype` is installed, you can scan the bill of materials using the following command.

```
grype ./build-artifacts/*_sbom.json
```

![WARNING]
! `grype` is just one tool to look for vulnerabilities in your project. Vulnerabilities may be found by other means.
