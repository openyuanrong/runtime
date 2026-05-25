# Concurrency

Stateful functions support configuring concurrency, allowing a single function instance to handle multiple concurrent requests. Stateful function instances without configured concurrency default to single-threaded. For function calls from the same submitter, they will be processed in order, but order cannot be guaranteed between different submitters. You can use the `InvokeOptions` interface to configure the parallelism of stateful functions. When parallelism is greater than 1, the processing order of function calls is not guaranteed.

## Single Instance Single Thread

By default, stateful functions are single instance single thread. Function calls from the same submitter execute sequentially, but order is not guaranteed between different submitters.

:::::{tab-set}
::::{tab-item} Python

```python
import time
import yr

@yr.instance
class Counter:
    def __init__(self):
        self.count = 0

    def add(self, n):
        self.count += n
        return self.count

# Simulate a caller
@yr.invoke
def caller(instance, n):
    return yr.get(instance.add.invoke(n))

@yr.invoke
def delayed_output(n):
    time.sleep(1)
    return n

yr.init()
instance = Counter.invoke()

# Call in remote instance
result0 = caller.invoke(instance, delayed_output.invoke(1))

# Call in main program
result1 = instance.add.invoke(2)
result2 = instance.add.invoke(3)

# Output [6, 2, 5]
# result1 and result2 come from the same caller, execute sequentially, results are 2(0+2) and 5(2+3)
# result0 comes from another submitter, although submitted first, due to delay, executes last, result is 6(5+1)
print(yr.get([result0, result1, result2]))

instance.terminate()
yr.finalize()
```

::::
::::{tab-item} C++

```cpp
#include <iostream>
#include <unistd.h>
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
    YR_STATE(count);
private:
    int count;
};
YR_INVOKE(Counter::FactoryCreate, &Counter::Add)


int Caller(YR::NamedInstance<Counter> counter, int n)
{
    return *YR::Get(counter.Function(&Counter::Add).Invoke(n));
}
YR_INVOKE(Caller)


int DelayedOutput(int n)
{
    sleep(1);
    return n;
}
YR_INVOKE(DelayedOutput)

int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);
    auto instance = YR::Instance(Counter::FactoryCreate).Invoke();

    // Call in remote instance
    auto n = YR::Function(DelayedOutput).Invoke(1);
    auto result0 = YR::Function(Caller).Invoke(instance, n);

    // Call in main program
    auto result1 = instance.Function(&Counter::Add).Invoke(2);
    auto result2 = instance.Function(&Counter::Add).Invoke(3);

    // Output 6:2:5
    // result1 and result2 come from calls in the main program, execute sequentially, results are 2(0+2) and 5(2+3)
    // result0 comes from another submitter, although called first, due to delay, executes last, result is 6(5+1)
    std::cout << *YR::Get(result0) << ":" << *YR::Get(result1) << ":" << *YR::Get(result2) << std::endl;

    instance.Terminate();
    YR::Finalize();
    return 0;
}
```

::::
::::{tab-item} Java

```java
// Counter.java
package com.example;

public class Counter {
    private int count;

    public Counter() {
       this.count = 0;
    }

    public int add(int n) {
        this.count += n;
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
    public static class Caller {
        public static int delayedOutput(int n) {
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            return n;
        }

        public static int caller(int n) {
            // Reuse named function instance from main program
            try {
                InstanceHandler counter_exist = YR.instance(Counter::new, "counter-1", "demo").invoke();
                ObjectRef refGet = counter_exist.function(Counter::add).invoke(n);
                return (int)YR.get(refGet, 9);
            } catch (YRException e) {
                e.printStackTrace();
            }
            return n;
        }
    }

    public static void main(String[] args) throws YRException {
        YR.init();
        // Create named function instance, name is counter-1, namespace is demo
        InstanceHandler counter = YR.instance(Counter::new, "counter-1", "demo").invoke();
        // Call in remote instance
        ObjectRef n = YR.function(Caller::delayedOutput).invoke(1);
        ObjectRef result0 = YR.function(Caller::caller).invoke(n);
        // Call in main program
        ObjectRef result1 = counter.function(Counter::add).invoke(2);
        ObjectRef result2 = counter.function(Counter::add).invoke(3);

        System.out.println(YR.get(result0, 9) + ":" + YR.get(result1, 9) + ":" + YR.get(result2, 9));

        counter.terminate();
        YR.Finalize();
    }
}
```

::::
:::::

## Configuring Single Instance Multi-threaded Concurrency

Configuring single instance multi-threaded support for multiple concurrency, at this time, regardless of whether it is the same submitter or different submitters, execution order is not guaranteed.

:::::{tab-set}
::::{tab-item} Python

```python
import yr

@yr.instance
class Counter:
    def __init__(self):
        self.count = 0

    def add(self, n):
        self.count += n
        return self.count

if __name__ == "__main__":
    yr.init()

    # Configure instance concurrency to 2
    opt = yr.InvokeOptions()
    opt.concurrency = 2
    counter = Counter.options(opt).invoke()

    result0 = counter.add.invoke(1)
    result1 = counter.add.invoke(2)

    # Output may be 1 3, or may be 3 2
    print(yr.get(result0), yr.get(result1))

    counter.terminate()
    yr.finalize()
```

::::
::::{tab-item} C++

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
    YR_STATE(count);
private:
    int count;
};

YR_INVOKE(Counter::FactoryCreate, &Counter::Add)

int main(int argc, char *argv[])
{

    YR::Init(YR::Config{}, argc, argv);
    // Configure instance concurrency to 2
    YR::InvokeOptions opt;
    opt.customExtensions.insert({"Concurrency", "2"});
    auto counter = YR::Instance(Counter::FactoryCreate).Options(opt).Invoke();

    auto result0 = counter.Function(&Counter::Add).Invoke(1);
    auto result1 = counter.Function(&Counter::Add).Invoke(2);
    // Output may be 1:3, or may be 3:2
    std::cout << *YR::Get(result0) << ":" << *YR::Get(result1) << std::endl;

    counter.Terminate();
    YR::Finalize();
    return 0;
}
```

::::
::::{tab-item} Java

```java
// Counter.java
package com.example;

public class Counter {
    private int count;

    public Counter() {
       this.count = 0;
    }

    public int add(int n) {
        this.count += n;
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

import java.util.HashMap;


public class Main {
    public static void main(String[] args) throws YRException {
        YR.init();
        // Configure instance concurrency to 2
        HashMap<String, String> customExtensions = new HashMap<>();
        customExtensions.put("Concurrency", "2");
        InvokeOptions opt = new InvokeOptions();
        opt.setCustomExtensions(customExtensions);
        InstanceHandler counter = YR.instance(Counter::new).options(opt).invoke();

        ObjectRef result0 = counter.function(Counter::add).invoke(1);
        ObjectRef result1 = counter.function(Counter::add).invoke(2);
        // Output may be 1:3, or may be 3:2
        System.out.println(YR.get(result0, 9) + ":" + YR.get(result1, 9));

        counter.terminate();
        YR.Finalize();
    }
}
```

::::
:::::
