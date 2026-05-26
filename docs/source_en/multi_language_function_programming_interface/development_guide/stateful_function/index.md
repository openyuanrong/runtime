# Stateful Functions

```{eval-rst}
.. toctree::
   :glob:
   :maxdepth: 1
   :hidden:

   named-instance
   parallel-execution
   fault-tolerance
```

A stateful function is essentially a stateful worker process (or service). When you instantiate a stateful function, openYuanrong creates a new worker process and schedules method calls of the stateful function to that worker process, through which you can access and modify the state of the worker process.

Method calls to different stateful function instances execute in parallel, while method calls on the same stateful function instance execute sequentially and share state.

:::{Note}

openYuanrong does not actively recycle stateful function instances. You need to actively call the `terminate` method to destroy them when all tasks are completed, otherwise resource leaks will occur.

:::

::::{tab-set}
:::{tab-item} Python

```python
import yr

# Declare stateful function
@yr.instance
class Counter:
    def __init__(self):
        self.count = 0

    def add(self, n):
        self.count += n
        return self.count

    def get(self):
        return self.count

if __name__ == "__main__":
    # Initialize openYuanrong runtime environment
    yr.init()

    # Create stateful function instance
    counter = Counter.invoke()

    # Asynchronously (non-blocking) call stateful function methods to interact with it
    result_add = counter.add.invoke(3)
    result_get = counter.get.invoke()

    # Synchronously (blocking) get results, output 3 3
    # You can also use yr.wait() method, which only waits for call completion without directly getting results
    print(yr.get(result_add), yr.get(result_get))

    # Destroy function instance
    counter.terminate()
    # Release openYuanrong environment resources
    yr.finalize()
```

:::
:::{tab-item} C++

```cpp
#include <iostream>
#include "yr/yr.h"

class Counter {
public:
    Counter() : count(0) {}
    static Counter *FactoryCreate()
    {
        return new Counter();
    }

    int Add(int n)
    {
        count += n;
        return count;
    }

    int Get()
    {
        return count;
    }
    YR_STATE(count);
private:
    int count;
};
// Declare stateful function
YR_INVOKE(Counter::FactoryCreate, &Counter::Add, &Counter::Get)

int main(int argc, char *argv[])
{
    // Initialize openYuanrong runtime environment
    YR::Init(YR::Config{}, argc, argv);
    // Create stateful function instance
    auto counter = YR::Instance(Counter::FactoryCreate).Invoke();
    // Asynchronously (non-blocking) call stateful function methods to interact with it
    auto resultAdd = counter.Function(&Counter::Add).Invoke(3);
    auto resultGet = counter.Function(&Counter::Get).Invoke();

    // Synchronously (blocking) get results, output 3:3
    // You can also use YR::Wait() method, which only waits for call completion without directly getting results
    std::cout << *YR::Get(resultAdd) << ":" << *YR::Get(resultGet) << std::endl;

    // Destroy function instance
    counter.Terminate();
    // Release openYuanrong environment resources
    YR::Finalize();
    return 0;
}
```

:::
:::{tab-item} Java

```java
// Counter.java
package com.example;

// Define stateful function  
public class Counter {
    private int count;

    public Counter() {
       this.count = 0;
    }

    public int add(int n) {
        this.count += n;
        return this.count;
    }

    public int get() {
        return this.count;
    }
}
```

```java
// Main.java
package com.example;

import org.yuanrong.Config;
import org.yuanrong.InvokeOptions;
import org.yuanrong.api.YR;
import org.yuanrong.call.InstanceHandler;
import org.yuanrong.exception.YRException;
import org.yuanrong.runtime.client.ObjectRef;


public class Main {
    public static void main(String[] args) throws YRException {
        // Initialize openYuanrong runtime environment
        YR.init();
        // Create stateful function instance
        InstanceHandler counter = YR.instance(Counter::new).invoke();
        // Asynchronously (non-blocking) call stateful function methods to interact with it
        ObjectRef refAdd = counter.function(Counter::add).invoke(3);
        ObjectRef refGet = counter.function(Counter::get).invoke();

        // Synchronously (blocking) get results, output 3:3
        // You can also use YR.wait() method, which only waits for call completion without directly getting results
        System.out.println(YR.get(refAdd, 9) + ":" + YR.get(refGet, 9));
        // Destroy function instance
        counter.terminate();
        // Release openYuanrong environment resources
        YR.Finalize();
    }
}
```

:::
::::

## Specifying Resources Required by Stateful Functions

When instantiating a stateful function, you can dynamically configure its resources. When not configured, the default resources are `cpu` 500 millicores and `memory` 500 MiB. Other custom resources (such as NPU, GPU, etc.) can be configured through the `custom_resources` field as key-value pairs. For more information about resources, please refer to the [Resources](../scheduling/logical_resource.md) chapter.

The types and total amounts of custom resources need to be specified when deploying openYuanrong. Except for GPU and NPU, openYuanrong does not detect other custom resources. Refer to the following example to specify custom resources on nodes. Custom resource amounts are only used for logical deduction during scheduling and do not limit openYuanrong functions' usage of actual physical resources.

```bash
yr start --master -s 'function_agent.args.custom_resources="{\"ssd\":1}"'
```

The following is an example of configuring resources for stateful functions.

::::{tab-set}

:::{tab-item} Python

```python
import yr

@yr.instance
class Counter:
    def __init__(self):
        self.count = 0

    def add(self, n):
        self.count += n
        return self.count

    def get(self):
        return self.count

if __name__ == "__main__":
    yr.init()

    # Run Counter function on 1 CPU core, 1G memory, 1 custom ssd resource
    opt = yr.InvokeOptions()
    opt.cpu = 1000
    opt.memory = 1024
    opt.custom_resources = {"ssd": 1}
    counter = Counter.options(opt).invoke()
    result_add = counter.add.invoke(3)
    print(yr.get(result_add))

    counter.terminate()
    yr.finalize()
```

:::

:::{tab-item} C++

```cpp
#include <iostream>
#include "yr/yr.h"

class Counter {
public:
    Counter() : count(0) {}
    static Counter *FactoryCreate()
    {
        return new Counter();
    }

    int Add(int n)
    {
        count += n;
        return count;
    }

    int Get()
    {
        return count;
    }
    YR_STATE(count);
private:
    int count;
};

YR_INVOKE(Counter::FactoryCreate, &Counter::Add, &Counter::Get)

int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);

    // Run Counter function on 1 CPU core, 1G memory, 1 custom ssd resource
    YR::InvokeOptions opt;
    opt.cpu = 1000;
    opt.memory = 1024;
    opt.customResources["ssd"] = 1;

    auto counter = YR::Instance(Counter::FactoryCreate).Options(opt).Invoke();
    auto resultAdd = counter.Function(&Counter::Add).Invoke(3);
    std::cout << *YR::Get(resultAdd) << std::endl;

    counter.Terminate();
    YR::Finalize();
    return 0;
}
```

:::

:::{tab-item} Java

```java
// Counter.java
package com.example;

// Define stateful function  
public class Counter {
    private int count;

    public Counter() {
       this.count = 0;
    }

    public int add(int n) {
        this.count += n;
        return this.count;
    }

    public int get() {
        return this.count;
    }
}
```

```java
// Main.java
package com.example;

import org.yuanrong.Config;
import org.yuanrong.InvokeOptions;
import org.yuanrong.api.YR;
import org.yuanrong.call.InstanceHandler;
import org.yuanrong.call.InstanceCreator;
import org.yuanrong.exception.YRException;
import org.yuanrong.runtime.client.ObjectRef;


public class Main {
    public static void main(String[] args) throws YRException {
        YR.init();

        // Run Counter function on 1 CPU core, 1G memory, 1 custom ssd resource
        InvokeOptions opt = new InvokeOptions.Builder().addCustomResource("ssd", 1.0f).cpu(1000).memory(1024).build();
        InstanceHandler counter = YR.instance(Counter::new).options(opt).invoke();
        ObjectRef refAdd = counter.function(Counter::add).invoke(3);
        System.out.println(YR.get(refAdd, 9));

        counter.terminate();
        YR.Finalize();
    }
}
```

:::
::::

## Passing Stateful Function Handles

Stateful function handles can be passed as parameters to other stateless or stateful functions, and the handle can be used to call stateful function methods within those functions.

::::{tab-set}
:::{tab-item} Python

```python
import yr

@yr.invoke
def get(counter):
    return yr.get(counter.get.invoke())

@yr.instance
class Counter:
    def __init__(self):
        self.count = 0

    def add(self, n):
        self.count += n
        return self.count

    def get(self):
        return self.count

if __name__ == "__main__":
    yr.init()

    counter = Counter.invoke()
    result_add = counter.add.invoke(3)
    print(yr.get(result_add))

    result_get = get.invoke(counter)
    # Output 3
    print(yr.get(result_get))

    counter.terminate()
    yr.finalize()
```

:::
:::{tab-item} C++

```cpp
#include <iostream>
#include "yr/yr.h"

class Counter {
public:
    Counter() : count(0) {}
    static Counter *FactoryCreate()
    {
        return new Counter();
    }

    int Add(int n)
    {
        count += n;
        return count;
    }

    int Get()
    {
        return count;
    }
    YR_STATE(count);
private:
    int count;
};
YR_INVOKE(Counter::FactoryCreate, &Counter::Add, &Counter::Get)

int Get(YR::NamedInstance<Counter> counter)
{
    return *YR::Get(counter.Function(&Counter::Get).Invoke());
}
YR_INVOKE(Get)

int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);
    auto counter = YR::Instance(Counter::FactoryCreate).Invoke();
    auto resultAdd = counter.Function(&Counter::Add).Invoke(3);
    std::cout << *YR::Get(resultAdd) << std::endl;

    auto resultGet = YR::Function(Get).Invoke(counter);
    std::cout << *YR::Get(resultGet) << std::endl;

    // Destroy function instance
    counter.Terminate();
    // Release openYuanrong environment resources
    YR::Finalize();
    return 0;
}
```

:::
:::{tab-item} Java

This feature is not currently supported.

:::
::::

## Communication Between Stateful Functions

Communication between stateful function instances can be completed through method calls within functions, and data can be shared through [Data Objects](../data_object/index.md) or data streams to achieve coordination.

## Scheduling

openYuanrong will select appropriate nodes to run stateful functions based on the specified resources and configured scheduling policies. For details, please refer to the [Scheduling](../scheduling/index.md) chapter.

## More Usage Methods

- [Named Instances](./named-instance.md)
- [Concurrency](./parallel-execution.md)
- [Fault Tolerance](./fault-tolerance.md)
- [Custom Graceful Shutdown](../../advanced_tutorials/yr_shutdown.md)
