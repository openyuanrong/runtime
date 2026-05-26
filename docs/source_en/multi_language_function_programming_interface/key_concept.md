# Key Concepts

(key-concept-statefull-function)=

## Stateful Functions

[openYuanrong Functions](glossary-yuanrong-function) support statefulness, where state refers to in-process private variables that can be accessed and modified during program execution. Typical states include static variables within a process, member variables in object-oriented programming, etc. Based on the concept of stateful functions, openYuanrong provides multi-language programming interfaces that allow single-machine classes developed in Python, Java, and C++ to be automatically converted into openYuanrong stateful functions for execution.

View the [Stateful Function Development Guide](./development_guide/stateful_function/index.md).

(key-concept-stateless-function)=

## Stateless Functions

Stateless functions are a special case of stateful functions. Their execution does not depend on state, only on input parameters. Based on the concept of stateless functions, openYuanrong provides multi-language programming interfaces that allow single-machine functions developed in Python, Java, and C++ to be automatically converted into openYuanrong stateless functions for execution.

View the [Stateless Function Development Guide](./development_guide/stateless_function/index.md).

(key-concept-data-object)=

## Data Objects

Data objects are memory data that can be shared and accessed across nodes among multiple openYuanrong functions in a distributed manner, supporting high-performance put/get access and modification based on shared memory. Additionally, they can serve as function call parameters and return values, automatically distributed and shared, supporting asynchronous Future semantics: for example, calling a function can return a data object Future reference, which can then be used as a new function call parameter; openYuanrong will automatically parse references during execution and manage data object lifecycles through automatic distributed reference counting.

View the [Data Object Development Guide](./development_guide/data_object/index.md).

(key-concept-data-stream)=

## Data Streams

Data streams are ordered, unbounded memory data sets that can be passed and shared across nodes among multiple openYuanrong functions in a distributed manner, supporting high-performance pub/sub access based on shared memory, and supporting one-to-one, one-to-many, many-to-one, and other publish-subscribe models. Through data streams, you can conveniently decouple multiple different functions, achieving asynchronous streaming data passing and computation among multiple functions.

View the [Data Stream Development Guide](./development_guide/data_stream/index.md).
