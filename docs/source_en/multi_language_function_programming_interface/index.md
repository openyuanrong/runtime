# Multi-Language Function Programming Interface

```{eval-rst}
.. toctree::
   :glob:
   :maxdepth: 1
   :hidden:

   key_concept.md
   development_guide/index.md
   advanced_tutorials/index.md
   examples/index.md
   api/index
```

The multi-language function programming interface provides basic primitives for extending distributed applications: [Stateful Functions](key-concept-statefull-function), [Stateless Functions](key-concept-stateless-function), [Data Objects](key-concept-data-object), and [Data Streams](key-concept-data-stream), supporting functions to run as tasks or Serverless services. We will introduce these core concepts through simple examples.

## Getting Started

Install openYuanrong using pip, which includes the openYuanrong SDK and command-line tool yr.

```bash
pip install https://openyuanrong.obs.cn-southwest-2.myhuaweicloud.com/release/0.7.0/linux/x86_64/openyuanrong-0.7.0-cp39-cp39-manylinux_2_34_x86_64.whl
```

First, import and initialize openYuanrong:

```python
import yr

# Init only once
yr.init()
```

:::{admonition} SDK Automatically Initializes Environment
:class: note

When calling the `yr.init()` interface in a [Driver](#glossary-driver) (without configuring the openYuanrong cluster address), when running on a non-openYuanrong node, the SDK will attempt to start a temporary openYuanrong environment, which is **automatically destroyed** when the process exits.

:::

## Stateful Functions

Stateful functions allow you to create stateful processes that maintain their internal state when methods are called. When instantiating a stateful function:

1. openYuanrong will launch a dedicated process in the cluster where the stateful function's methods run, and can access and modify its state.

2. Stateful functions will execute method calls in sequence.

A simple example is as follows:

```python
# Define stateful function
@yr.instance
class Object:
    def __init__(self):
        self.value = 0

    def save(self, value):
        self.value = value

    def get(self):
        return self.value

# Create a stateful function instance
obj = Object.invoke()

# Asynchronously invoke stateful function, two calls executed in sequence
obj.save.invoke(9)
result_ref = obj.get.invoke()
print(yr.get(result_ref))

# Destroy stateful function instance
obj.terminate()
```

## Stateless Functions

Stateless functions are the simplest way to parallelize Python, C++, and Java functions across the openYuanrong cluster. Create a stateless function through the following steps:

1. Decorate your function with `@yr.invoke`, indicating it should run remotely.

2. Call the function using `.invoke()`, which returns a reference to a data object.

3. Use `yr.get()` to retrieve the value of the data object.

A simple example is as follows:

```python
# Define stateless function
@yr.invoke
def say_hello(name):
    return 'hello, ' + name

# Asynchronously invoke stateless functions in parallel
results_ref = [say_hello.invoke('yuanrong') for i in range(3)]

# Retrieve the value of the returned object
print(yr.get(results_ref))
```

## Data Objects

openYuanrong's distributed shared object store can efficiently manage data across the entire cluster. In openYuanrong, there are mainly three ways to handle data objects:

1. Implicit creation: Return values of stateless and stateful functions are automatically stored in openYuanrong's distributed object store, returning only an object reference.

2. Explicit creation: Call `yr.put()` to directly store a data object.

3. Passing references: You can pass data object references as parameters to other stateless and stateful functions to avoid unnecessary data copying and achieve lazy function execution.

A simple example is as follows:

```python
# Define stateless function
@yr.invoke
def add(n):
    return n + 1

# Function invoke will return a reference to a data object
result_ref = add.invoke(1)
# The reference to the data object is passed as a parameter
next_result_ref = add.invoke(result_ref)
print(yr.get(next_result_ref))

# Put data to object store
data_ref = yr.put({"key": "value"})

# Get data from object store
print(yr.get(data_ref))  # output {"key": "value"}
```

## Data Streams

Data streams are memory data that can be passed across nodes among multiple openYuanrong functions, accessed via pub/sub. Data streams are implicitly created with a unique stream name when creating a producer or consumer.

A simple example is as follows:

```python
# Define stream name
stream_name = "this-stream"

# Create producer, implicitly creating the stream.
producer_config = yr.ProducerConfig(delay_flush_time=5, page_size=1024 * 1024, max_stream_size=1024 * 1024 * 1024, auto_clean_up=True)
producer = yr.create_stream_producer(stream_name, producer_config)
# Produce a piece of data
element = yr.Element(value=b"hello", ele_id=0)
producer.send(element)

# Create consumer and associate it with the stream
consumer_config = yr.SubscriptionConfig("local-consumer")
consumer = yr.create_stream_consumer(stream_name, consumer_config)
# Consume a piece of data
elements = consumer.receive(1000, 1)
```

## Function Services

You can deploy openYuanrong functions as Serverless services, accessible via HTTP requests. Function services define function signatures as request entry points. Implementing this function allows deployment as a Serverless service.

A simple example is as follows:

```python
# handler is the function execution entry point, triggered on every request.
# event is data passed via HTTP request (Header, Body, etc.).
# context is the runtime context provided by openYuanrong, containing function, execution environment, and other information.
def handler(event, context):
    print("received request,event content:", event)

    response = ""
    try:
        response = "hello " + event.get("name")
    except Exception as e:
        print(e)
        response = "please enter your name,for example:{'name':'yuanrong'}"

    return response
```

## Next Steps

You can combine openYuanrong's simple primitives to express almost any distributed computing pattern. To learn more about openYuanrong's [Key Concepts](./key_concept.md), browse the following user guides:

- [Stateful Functions](./development_guide/stateful_function/index.md)
- [Stateless Functions](./development_guide/stateless_function/index.md)
- [Data Objects](./development_guide/data_object/index.md)
- [Data Streams](./development_guide/data_stream/index.md)
- [Function Services](./development_guide/function_service/index.md)
