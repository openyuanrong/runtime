# Glossary

This section introduces commonly used terminology in openYuanrong.

{.glossary}

{#glossary-yuanrong-function}
openYuanrong Function
: The basic unit of distributed scheduling and operation in openYuanrong. Compared to traditional Serverless function concepts, openYuanrong functions are more general-purpose, supporting dynamic creation during runtime, long-running execution, asynchronous inter-calls, statefulness, etc. They can express running instances of arbitrary distributed applications, serving a role similar to processes in a standalone OS.

{#glossary-stateful-function}
Stateful Function
: openYuanrong functions support statefulness, where state refers to private in-process variables that can be accessed and modified during program execution. Typical states include static variables within a process, member variables in object-oriented programming, etc. Based on the stateful function concept, openYuanrong provides multi-language programming interfaces, allowing classes originally developed using Python, Java, and C++ to be automatically converted into openYuanrong stateful functions for execution.

{#glossary-stateless-function}
Stateless Function
: Stateless functions are a special case of stateful functions where execution does not depend on state, only on input parameters. Based on the stateless function concept, openYuanrong provides multi-language programming interfaces, allowing functions originally developed using Python, Java, and C++ to be automatically converted into openYuanrong stateless functions for execution.

{#glossary-object}
Data Object
: Data objects are in-memory data that can be shared and accessed across nodes between multiple openYuanrong functions, supporting high-performance put/get access and modification based on shared memory; additionally, they can serve as function call parameters and return values, automatically distributed and shared, and support asynchronous Future semantics: for example, calling a function can return a Data Object Future reference, at which point the object reference can also be used as a new function call parameter; openYuanrong automatically resolves references during runtime and manages data object lifecycles through automatic distributed reference counting.

{#glossary-stream}
Data Stream
: Data streams are ordered unbounded in-memory datasets that can be passed and shared across nodes between multiple openYuanrong functions, supporting high-performance pub/sub access based on shared memory, and supporting one-to-one, one-to-many, many-to-one, and other publish/subscribe patterns. Data streams facilitate decoupling of multiple different functions, enabling asynchronous streaming data transfer and computation between functions.

{#glossary-master-node}
Master Node
: A node in the openYuanrong cluster that contains both control plane (cluster management, scheduling, etc.) and data plane (running distributed tasks) components. Control plane components include `function master`, and data plane components include `function proxy`, `function agent`, `runtime manager`, and `data worker`.

{#glossary-agent-node}
Agent Node (Worker Node)
: A node in the openYuanrong cluster that contains only data plane (running distributed tasks) components, including `function proxy`, `function agent`, `runtime manager`, and `data worker`. Multiple agent nodes can be deployed on a single host.

{#glossary-functionsystem}
Function System
: One of the openYuanrong systems, providing large-scale distributed dynamic scheduling, supporting rapid elastic scaling and cross-node migration of function instances, achieving efficient cluster resource utilization.

{#glossary-datasystem}
Data System
: One of the openYuanrong systems, providing heterogeneous distributed multi-level caching, supporting Object and Stream semantics, achieving high-performance data sharing and transfer between function instances.

{#glossary-driver}
Driver
: The name of an application's startup process. For example, Python startup scripts, C++ binary executable files, or Java executable jar packages.

{#glossary-instance}
Function Instance (Worker)
: The process running an openYuanrong function.

{#glossary-runtime}
Runtime
: The running environment for openYuanrong functions.

{#glossary-process-deployment}
Host Deployment (Process Deployment)
: Starting all openYuanrong components as processes on a host, providing basic health monitoring and process re-launch mechanisms. Process deployment is typically used for lightweight local validation and other scenarios with lower requirements for availability, reliability, and isolation.
