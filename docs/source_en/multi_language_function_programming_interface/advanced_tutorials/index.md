# Advanced Tutorials

```{eval-rst}
.. toctree::
   :glob:
   :maxdepth: 1
   :hidden:

   yr_shutdown
   generator
   buffer_get
   avoid_excessive_concurrency
   use_nested_call
   use_wait
   use_InvokeOptions_limit_concurrent_num
   hierarchical_scheduling
   ai_agent_session
   worldwide_shared_signal_station
```

This section introduces how to use openYuanrong's advanced features and common design patterns.

- [Custom Graceful Shutdown](./yr_shutdown.md)
- [openYuanrong Generators](./generator.md)
- [Zero Serialization/Deserialization for APIs](./buffer_get.md)
- [Avoid Excessive Concurrency](./avoid_excessive_concurrency.md)
- [Nested Calls](./use_nested_call.md)
- [Using yr.wait to Limit Concurrent/Pending Tasks](./use_wait.md)
- [Using Resource Limits to Control Task Concurrency](./use_InvokeOptions_limit_concurrent_num.md)
- [Building Tree-Shaped Job Graphs with Stateful Functions](./hierarchical_scheduling.md)
- [AI Agent Sessions and Affinity Scheduling](./ai_agent_session.md)
- [Using Stateful Functions as Global Signal Stations](./worldwide_shared_signal_station.md)
