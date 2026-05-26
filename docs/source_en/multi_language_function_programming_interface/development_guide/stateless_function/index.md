# Stateless Functions

```{eval-rst}
.. toctree::
   :glob:
   :maxdepth: 1
   :hidden:

   concurrency
   fault-tolerance
```

Stateless functions are a special case of stateful functions. Their execution does not depend on state, only on input parameters. openYuanrong will automatically release resources occupied by stateless functions after they finish execution.

Simple example:

:::::{tab-set}
::::{tab-item} Python

```python
import yr
# Initialize openYuanrong runtime environment
yr.init()

# Declare add function can be executed remotely
@yr.invoke
def add(n):
    return n + 1

# Asynchronously (non-blocking) execute add function remotely, result is a reference to the return value
result = add.invoke(1)
# Synchronously (blocking) get actual return value through yr.get() method, will output 2
# You can also use yr.wait() method, which only waits for call completion without directly getting results
print(yr.get(result))

# Release openYuanrong environment resources
yr.finalize()
```

::::
::::{tab-item} C++

```cpp
#include <iostream>
#include <yr/yr.h>

int Add(int n)
{
    return n + 1;
}
// Declare Add function can be executed remotely
YR_INVOKE(Add)

int main(int argc, char *argv[])
{
    // Initialize openYuanrong runtime environment
    YR::Init(YR::Config{}, argc, argv);

    // Asynchronously (non-blocking) execute Add function remotely, ref is a reference to the return value
    auto ref = YR::Function(Add).Invoke(1);
    // Synchronously (blocking) get actual return value through YR::Get() method, will output 2
    // You can also use YR::Wait() method, which only waits for call completion without directly getting results
    std::cout << *YR::Get(ref) << std::endl;

    // Release openYuanrong environment resources
    YR::Finalize();
    return 0;
}
```

::::
::::{tab-item} Java

```java
import org.yuanrong.InvokeOptions;
import org.yuanrong.Config;
import org.yuanrong.api.YR;
import org.yuanrong.runtime.client.ObjectRef;

public class Main {
    public static class MyApp {
        // Define stateless function add, must be a static method
        public static int add(int n) {
            return n + 1;
        }
    }

    public static void main(String[] args) throws Exception {
        // Initialize openYuanrong runtime environment
        Config conf = new Config();
        YR.init(conf);

        // Asynchronously (non-blocking) execute add function remotely, ref is a reference to the return value
        ObjectRef ref = YR.function(MyApp::add).invoke(1);
        // Synchronously (blocking) get actual return value through YR::get() method, will output 2
        // You can also use YR.wait() method, which only waits for call completion without directly getting results
        System.out.println(YR.get(ref, 9));

        // Release openYuanrong environment
        YR.Finalize();
    }
}
```

::::
:::::

## Specifying Resources Required by Stateless Functions

You can ensure stateless functions run on nodes that meet specific resource conditions by specifying resources. Use the `InvokeOptions` interface to dynamically specify required CPU (unit: millicores), memory (unit: MB), or custom resources. For more information about resources, please refer to the [Resources](../scheduling/logical_resource.md) chapter.

The types and total amounts of custom resources need to be specified when deploying openYuanrong. Except for GPU and NPU, openYuanrong does not detect other custom resources. Refer to the following example to specify custom resources on nodes. Custom resource amounts are only used for logical deduction during scheduling and do not limit openYuanrong functions' usage of actual physical resources.

```bash
yr start --master -s 'function_agent.args.custom_resources="{\"ssd\":1}"'
```

The following is an example of configuring resources for stateless functions.

::::{tab-set}
:::{tab-item} Python

```python
import yr

@yr.invoke
def add(n):
    return n + 1

yr.init()

# Run add function on 1 CPU core, 1G memory, 1 custom ssd resource
opt = yr.InvokeOptions(cpu=1000, memory=1024)
opt.custom_resources={"ssd":1}
result = add.options(opt).invoke(1)
print(yr.get(result))

yr.finalize()
```

:::
:::{tab-item} C++

```c++
#include <iostream>
#include <yr/yr.h>

int Add(int n)
{
    return n + 1;
}

YR_INVOKE(Add)

int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);

    // Run add function on 1 CPU core, 1G memory, 1 custom ssd resource
    YR::InvokeOptions opt;
    opt.cpu = 1000;
    opt.memory = 1024;
    opt.customResources["ssd"] = 1;

    auto ref = YR::Function(Add).Options(opt).Invoke(1);
    std::cout << *YR::Get(ref) << std::endl;

    YR::Finalize();
    return 0;
}
```

:::
:::{tab-item} Java

```java
import org.yuanrong.InvokeOptions;
import org.yuanrong.Config;
import org.yuanrong.api.YR;
import org.yuanrong.runtime.client.ObjectRef;

public class Main {
    public static class MyApp {
        public static int add(int n) {
            return n + 1;
        }
    }

    public static void main(String[] args) throws Exception {
        Config conf = new Config();
        YR.init(conf);

        // Run add function on 1 CPU core, 1G memory, 1 custom ssd resource
        InvokeOptions opt = new InvokeOptions.Builder().addCustomResource("ssd", 1.0f).cpu(1000).memory(1024).build();
        ObjectRef ref = YR.function(MyApp::add).options(opt).invoke(1);
        System.out.println(YR.get(ref, 9));

        YR.Finalize();
    }
}
```

:::
::::

## Passing Data Object References as Parameters to Stateless Functions

Parameters of stateless functions can pass values or references to data objects. This data object can be the return value of the `yr.put()` interface, or the return of an openYuanrong function. When the stateless function executes, the data object passed as a parameter will be automatically converted to a value.

::::{tab-set}
:::{tab-item} Python

```python
import yr

yr.init()

@yr.invoke
def add(n):
    return n + 1

result = add.invoke(1)
# Use the data object returned by the previous function call as a parameter to call the stateless function add again
result_next = add.invoke(result)
print(yr.get(result_next))

yr.finalize()
```

:::
:::{tab-item} C++

```c++
#include <iostream>
#include <yr/yr.h>

int Add(int n)
{
    return n + 1;
}

YR_INVOKE(Add)

int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);

    auto ref = YR::Function(Add).Invoke(1);
    // Use the data object returned by the previous function call as a parameter to call the stateless function Add again
    auto ref_next = YR::Function(Add).Invoke(ref);
    std::cout << *YR::Get(ref_next) << std::endl;

    YR::Finalize();
    return 0;
}
```

:::
:::{tab-item} Java

```java
import org.yuanrong.InvokeOptions;
import org.yuanrong.Config;
import org.yuanrong.api.YR;
import org.yuanrong.runtime.client.ObjectRef;

public class Main {
    public static class MyApp {
        public static int add(int n) {
            return n + 1;
        }
    }

    public static void main(String[] args) throws Exception {
        Config conf = new Config();
        YR.init(conf);

        ObjectRef ref = YR.function(MyApp::add).invoke(1);
        // Use the data object returned by the previous function call as a parameter to call the stateless function add again
        ObjectRef ref_next = YR.function(MyApp::add).invoke(ref);
        System.out.println(YR.get(ref_next, 9));

        YR.Finalize();
    }
}
```

:::
::::

In the above example, the second function call task will only truly start executing after the first function call completes, because it depends on the return result of the first function call.

Both function calls are asynchronous and do not block the main program. Before calling `yr.get(ref2)` / `YR::Get(ref2)` / `YR.get(ref2)` to synchronously wait for return results, you can perform other tasks.

## Waiting for Stateless Function Execution to Complete

Stateless functions that have started executing can block and wait for return results through the `yr.get()` interface. When executing multiple stateless functions, you may want to know which ones have completed. In this case, you can achieve this by calling the `yr.wait()` interface.

Simple example:

:::::{tab-set}
::::{tab-item} Python

```python
import yr

yr.init()

@yr.invoke
def add(n):
    return n + 1

results_ref = [add.invoke(i) for i in range(1)]

# Keep waiting until 1 execution completes or 9 seconds timeout
finished_ref, unfinished_ref = yr.wait(results_ref, 1, 9)
for i in range(len(finished_ref)):
    print(yr.get(finished_ref[0]))

yr.finalize()
```

::::
::::{tab-item} C++

```cpp
#include <iostream>
#include <yr/yr.h>

int Add(int n)
{
    return n + 1;
}

YR_INVOKE(Add)

int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);

    std::vector<YR::ObjectRef<int>> results;
    for (int i = 0; i < 2; i++) {
        auto ref = YR::Function(Add).Invoke(i);
        results.emplace_back(ref);
    }

    // Keep waiting until 1 execution completes or 9 seconds timeout
    auto waitResults = YR::Wait(results, 1, 9);
    for (auto ref : waitResults.first) {
        std::cout << *YR::Get(ref) << std::endl;
    }

    YR::Finalize();
    return 0;
}
```

::::
::::{tab-item} Java

```java
import org.yuanrong.Config;
import org.yuanrong.api.YR;
import org.yuanrong.runtime.client.ObjectRef;
import org.yuanrong.storage.WaitResult;

import java.util.HashMap;
import java.util.ArrayList;
import java.util.List;

public class Main {
    public static class MyApp {
        public static int add(int n) {
            return n + 1;
        }
    }

    public static void main(String[] args) throws Exception {
        Config conf = new Config();
        YR.init(conf);

        List<ObjectRef> objectRefs = new ArrayList<>();
        for (int i = 0; i < 2; i++) {
            ObjectRef ref = YR.function(MyApp::add).invoke(i);
            objectRefs.add(ref);
        }

        // Keep waiting until 1 execution completes or 9 seconds timeout
        WaitResult waitResult = YR.wait(objectRefs, 1, 9);
        for (ObjectRef ref : waitResult.getReady()) {
            System.out.println(YR.get(ref, 9));
        }

        YR.Finalize();
    }
}
```

::::
:::::

## Canceling Stateless Function Execution

You can use the `cancel()` interface to cancel the execution of stateless functions.

:::::{tab-set}
::::{tab-item} Python

```python
import time
import yr

yr.init()

@yr.invoke
def delay():
    time.sleep(10)
    return 0

result_ref = delay.invoke()
# Cancel stateless function execution
yr.cancel(result_ref)

# Successful cancellation will throw RuntimeError exception when getting the object reference value
try:
    yr.get(result_ref)
except RuntimeError as e:
    print(e)

yr.finalize()
```

::::
::::{tab-item} C++

```cpp
#include <iostream>
#include <yr/yr.h>

int Delay()
{
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}

YR_INVOKE(Delay)

int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);

    auto ref = YR::Function(Delay).Invoke();
    // Cancel stateless function execution
    YR::Cancel(ref);

    // Successful cancellation will throw Exception exception when getting the object reference value
    try {
        int result = *YR::Get(ref);
    } catch (YR::Exception &e) {
        std::cout << e.what() << std::endl;
    }

    YR::Finalize();
    return 0;
}
```

::::
::::{tab-item} Java

Not currently supported.

::::
:::::

## Scheduling

openYuanrong will select appropriate nodes to run stateless functions based on the specified resources and configured scheduling policies. For details, please refer to the [Scheduling](../scheduling/index.md) chapter.

## More Usage Methods

- [Concurrency](./concurrency.md)
- [Fault Tolerance](./fault-tolerance.md)
