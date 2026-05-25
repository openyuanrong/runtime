# Fault Tolerance

openYuanrong supports restarting stateful function instances when they fail, controlling the maximum number of restart attempts through the `recover_retry_times` parameter. After exceeding the retry limit, an exception is thrown. By default, instances are not restarted.

openYuanrong implements at-least-once semantics for function instance calls through the instance fault restart mechanism, meaning that when user code executes a call to a function instance, openYuanrong guarantees that the call will be executed at least once.

## Usage Limitations

In scenarios with irrecoverable exceptions, openYuanrong may stop retrying prematurely and throw an exception.

## Configuring Instance Restart

::::{tab-set}

:::{tab-item} Python

```python
import yr

@yr.instance
class Counter:
    def __init__(self):
        self.count = 0

if __name__ == "__main__":
    yr.init()

    opt =  yr.InvokeOptions()
    # Set maximum restart attempts to 3
    opt.recover_retry_times = 3
    counter = Counter.options(opt).invoke()

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
    YR_STATE(count);
private:
    int count;
};

YR_INVOKE(Counter::FactoryCreate)

int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);

    YR::InvokeOptions opt;
    // Set maximum restart attempts to 3
    opt.recoverRetryTimes = 3;
    auto counter = YR::Instance(Counter::FactoryCreate).Options(opt).Invoke();

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

        // Set maximum restart attempts to 3
        InvokeOptions opt = new InvokeOptions.Builder().recoverRetryTimes(3).build();
        InstanceHandler counter = YR.instance(Counter::new).options(opt).invoke();

        counter.terminate();
        YR.Finalize();
    }
}
```

:::
::::
