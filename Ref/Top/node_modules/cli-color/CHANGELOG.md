# Changelog

All notable changes to this project will be documented in this file. See [standard-version](https://github.com/conventional-changelog/standard-version) for commit guidelines.

### [2.0.2](https://github.com/medikoo/cli-color/compare/v2.0.1...v2.0.2) (2022-03-31)

### Bug Fixes

- Ensure to use widely supported ANSI codes for line move ([6838339](https://github.com/medikoo/cli-color/commit/6838339ae80d36e77c291645c6de891928197933))

### [2.0.1](https://github.com/medikoo/cli-color/compare/v2.0.0...v2.0.1) (2021-10-15)

### Maintenance Improvements

- Drop `ansi-regex` dependency due to security issues ([#38](https://github.com/medikoo/cli-color/issues/38)) ([9072cda](https://github.com/medikoo/cli-color/commit/9072cda305181dcc64d657d4de95a813db6dbdf3)) ([Jorge Cabot](https://github.com/jcabot21))

## [2.0.0](https://github.com/medikoo/cli-color/compare/v1.4.0...v2.0.0) (2019-10-09)

### Features

- Support NO_COLOR standard ([8f2a4eb](https://github.com/medikoo/cli-color/commit/8f2a4eb))

### BREAKING CHANGES

- ANSI color codes won't be generated for output, when NO_COLOR env var is set

<a name="1.4.0"></a>

# [1.4.0](https://github.com/medikoo/cli-color/compare/v1.3.0...v1.4.0) (2018-10-23)

### Features

- introduce move top, bottom, lineBegin and lineEnd instructions ([ad53db1](https://github.com/medikoo/cli-color/commit/ad53db1))

<a name="1.3.0"></a>

# [1.3.0](https://github.com/medikoo/cli-color/compare/v1.2.0...v1.3.0) (2018-08-20)

### Bug Fixes

- downgrade ansi-regex to not break support for old Node.js versions ([c4f765f](https://github.com/medikoo/cli-color/commit/c4f765f))

### Features

- **colums:** support multine cells ([585fc59](https://github.com/medikoo/cli-color/commit/585fc59))

## Old changelog

See `CHANGES`
