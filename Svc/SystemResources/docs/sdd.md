\page SvcSystemResourcesComponent Svc::SystemResources Component
# SystemResources Component

The system resources component downlinks information about the running F´ system. This information includes:

1. Free Memory
2. CPU load
3. Disk space

These items are downlinked as telemetry channels in response to a rate group port invocation.

**Note:** system resources requires `U64` types to be available on the target architecture.