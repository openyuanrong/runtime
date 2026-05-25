# Custom Graceful Shutdown

openYuanrong supports graceful shutdown of stateful function instances, meaning that before a stateful function instance exits, a user-defined function is executed to clean up the function's state. This is triggered in the following scenarios:

- openYuanrong receives a `shutdown` request for the function instance, such as actively calling the `terminate` interface in the main program.
- openYuanrong captures a SIGTERM signal, such as when the cluster executes a scale-in policy, sending a SIGTERM signal to the function instance process.

## Use Cases

You can perform necessary operations such as cleanup, data persistence, and connection closure in the graceful shutdown interface to ensure orderly service shutdown.

## Usage Example

The example uses Python. For other development languages, refer to the [C++ API](../api/distributed_programming/Cpp/YR_SHUTDOWN.md) and [Java API](../api/distributed_programming/Java/yrShutdown.md).

The interface prototype is as follows. This interface has no return value and does not throw exceptions. The parameter `gracePeriodSecond` represents the timeout for graceful shutdown (in seconds). If the timeout is reached and the interface has not returned, openYuanrong will forcefully exit the function instance. The default timeout is 30s, which you can customize through the `InvokeOptions` interface.

```python
def __yr_shutdown__(self, gracePeriodSecond: int):
```

:::{note}

Your custom graceful shutdown function signature must match the interface prototype exactly, otherwise it will not be triggered.

:::

Example code:

```python
import yr

@yr.instance
class StatefulFunc:
    def __init__(self, key):
        self.key = key

    def get_key(self):
        return self.key

    # Function signature must match the interface prototype exactly
    def __yr_shutdown__(self, gracePeriodSecond):
        # Implement your shutdown logic here
        # You will see the following standard output in the function log
        print("graceful exit, timeout is %ds" % gracePeriodSecond)

yr.init()

# Configure custom timeout to 10s
opt = yr.InvokeOptions()
opt.custom_extensions["GRACEFUL_SHUTDOWN_TIME"] = "10"
instance = StatefulFunc.options(opt).invoke("phase")
# The following call will use the default timeout of 30s
# instance = StatefulFunc.invoke("phase")
result = instance.get_key.invoke()
print(yr.get(result))

# This will trigger graceful shutdown
instance.terminate()
yr.finalize()
```
