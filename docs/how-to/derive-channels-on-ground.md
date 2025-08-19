# Create Ground-Derived Channels in F Prime GDS

Ground-derived channels allow you to compute new values from incoming telemetry on the ground, using logic that runs entirely within the F Prime Ground Data System (GDS). These values can be republished into the system as if they came from flight, enabling flexible monitoring, UI presentation, or conversions on the ground.

This guide walks through the process of creating a basic plugin that listens to incoming telemetry and publishes it back to F Prime.


> [!NOTE]
> **Prerequisite**  
> Before following this guide, you should first complete the [Develop GDS Plugins](../how-to/develop-gds-plugins.md) guide.  
> It explains the plugin system, registration process, and runtime behavior essential to this example.

---

## When to Use This

Use a ground-derived channel when:

- You want to transform or scale telemetry (e.g., converting a raw sensor ADC measurement to engineering units)
- You want to compute a value based on multiple telemetry channels (e.g., battery differential)
- You want to normalize or rename telemetry values for downstream consumers

---

## Plugin Architecture

Ground-derived channels are implemented using a `DataHandlerPlugin`, one of the built-in plugin types in the F Prime GDS.

Our plugin will:

- **Receives decoded telemetry data** via the `data_callback` API
- **Applies your transformation logic**
- **Re-publishes the result** using the standard pipeline

The plugin runs in the `CustomDataHandler` process, isolated from the core GDS.

---

## Basic Setup

To get started, create a Python file (e.g., `ground_derived_channels.py`) and define a plugin class that listens for telemetry.

> **TODO**: Add basic `DataHandlerPlugin` class that subscribes to telemetry and logs incoming values.

---

## Registering the Plugin

To make this plugin available to the GDS:

1. Save the file in a discoverable Python module or package.
2. Ensure the plugin is installed or visible in your `PYTHONPATH`.
3. Run the GDS normally — the plugin system will auto-load the plugin via the `@gds_plugin` decorator.

You should see output like the following in the GDS terminal when telemetry arrives:

```
```

---

## Next Steps

In the next section of this guide, we will:

- Transform this value (e.g., scale it to degrees Celsius)
- Publish a new derived channel (e.g., `TEMP_C`)
- Discuss naming and validation best practices

---

## See Also

- [DataHandlerPlugin Reference](../reference/data-handler-plugin.md)
- [Plugin System Overview](../how-to/develop-gds-plugins.md)
- [App Plugin Example (OpenMCT)](https://github.com/fprime-community/fprime-openmct)