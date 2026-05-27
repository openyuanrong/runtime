# Monitoring and Debugging

```{eval-rst}
.. toctree::
   :glob:
   :maxdepth: 1
   :hidden:

   dashboard
   logs
   metrics/index
   traces
   distributed_stack
```

Observability refers to the ability to observe and infer the internal state of openYuanrong applications and openYuanrong clusters through various external outputs (such as logs, metrics, events, etc.). openYuanrong provides observability capabilities to help users easily monitor and debug openYuanrong applications and clusters.

Monitoring and debugging openYuanrong applications includes the following steps:

1. Monitor clusters and applications through Dashboard, logs, and metrics to identify problems or errors that occur.
2. Debug using tools and data such as remote debugger (coming soon).
3. Identify problem points, fix them, test, and verify results.

This section will elaborate on related tools and data provided by openYuanrong.

- [Dashboard](dashboard.md)
- [Logs](logs.md)
- [Metrics](metrics/index.md)

By recording and correlating the call processes of various services in a distributed system, tracing provide end-to-end request tracing capabilities.

- [Tracing](traces.md)
- [Distributed Fusion Call Stack](distributed_stack.md)
