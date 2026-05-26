# Named Instances

When creating stateful function instances, users can specify the name of the function instance. This has significant application value in scenarios where function instances need to be shared but instance handles cannot be passed.

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

    # Create named function instance, name is counter-1, namespace is demo
    opt = yr.InvokeOptions()
    opt.name = "counter-1"
    opt.ns = "demo"
    counter = Counter.options(opt).invoke()
    result_add = counter.add.invoke(3)
    print(yr.get(result_add))

    # You can use the named function instance in other applications, this will not create a new function instance, will use the one created above
    counter_exist = Counter.options(opt).invoke()
    result_get = counter_exist.get.invoke()
    # Get the state written by calling the add method earlier, output 3
    print(yr.get(result_get))

    # You can also use yr.get_instance to get an already created function instance
    counter_query = yr.get_instance("counter-1")
    result_recheck = counter_exist.get.invoke()
    # Get the state written by calling the add method earlier, output 3
    print(yr.get(result_recheck))

    counter.terminate()
    yr.finalize()
```

:::
:::{tab-item} C++

```c++
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
    // Create named function instance, name is counter-1, namespace is demo
    auto counter = YR::Instance(Counter::FactoryCreate, "counter-1", "demo").Invoke();
    auto resultAdd = counter.Function(&Counter::Add).Invoke(3);
    std::cout << *YR::Get(resultAdd) << std::endl;

    // You can use the named function instance in other applications, this will not create a new function instance, use the one created above
    auto counter_exist = YR::Instance(Counter::FactoryCreate, "counter-1", "demo").Invoke();
    auto resultGet = counter_exist.Function(&Counter::Get).Invoke();
    // Get the state written by calling the Add method earlier, output 3
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
        YR.init();
        // Create named function instance, name is counter-1, namespace is demo
        InstanceHandler counter = YR.instance(Counter::new, "counter-1", "demo").invoke();
        ObjectRef refAdd = counter.function(Counter::add).invoke(3);
        System.out.println(YR.get(refAdd, 9));

        // You can use the named function instance in other applications, this will not create a new function instance, use the one created above
        InstanceHandler counter_exist = YR.instance(Counter::new, "counter-1", "demo").invoke();
        ObjectRef refGet = counter_exist.function(Counter::get).invoke();
        // Get the state written by calling the add method earlier, output 3
        System.out.println(YR.get(refGet, 9));

        counter.terminate();
        YR.Finalize();
    }
}
```

:::
::::
