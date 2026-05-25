# Concurrency

By default, a stateless function instance only handles one concurrent request. openYuanrong supports configuring instance concurrency, allowing a single stateless function instance to handle multiple concurrent requests. The single instance multi-concurrency feature is suitable for scenarios where functions spend considerable time waiting for downstream service responses. Waiting for responses generally does not consume resources.

## Usage Limitations

- Instance concurrency configuration range is `[1, 1000]`.
- When configuring stateless function instances for multiple concurrency, you need to ensure that the function code logic is multi-thread safe.

## Configuring Single Instance Multi-concurrency

Configure single instance multi-concurrency through the `InvokeOptions` interface. The following example sets the function instance concurrency to `4`.

::::{tab-set}

:::{tab-item} Python

```python
import yr

yr.init()

@yr.invoke
def add(n):
    return n + 1

# Configure function instance resource usage as 1 CPU core, 1G memory, concurrency of 4
opt = yr.InvokeOptions(cpu=1000, memory=1024)
opt.concurrency = 4

# Concurrently call add function 8 times
results = [add.options(opt).invoke(i) for i in range(8)]
wait_results = yr.wait(results, len(results))
print(yr.get(wait_results[0]))

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

    // Configure function instance resource usage as 1 CPU core, 1G memory, concurrency of 4
    YR::InvokeOptions opt;
    opt.cpu = 1000;
    opt.memory = 1024;
    opt.customExtensions.insert({"Concurrency","4"});

    // Concurrently call add function 8 times
    std::vector<YR::ObjectRef<int>> results;
    for (int i = 0; i < 8; i++) {
        auto ref = YR::Function(Add).Options(opt).Invoke(i);
        results.emplace_back(ref);
    }

    auto waitResults = YR::Wait(results, results.size());
    for (auto ref : waitResults.first) {
        std::cout << *YR::Get(ref) << std::endl;
    }

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

        // Configure function instance resource usage as 1 CPU core, 1G memory, concurrency of 4
        HashMap<String, String> customExtensions = new HashMap<>();
        customExtensions.put("Concurrency", "4");
        InvokeOptions opt = new InvokeOptions();
        opt.setCustomExtensions(customExtensions);
        opt.setCpu(1000);
        opt.setMemory(1024);

        // Concurrently call add function 8 times
        List<ObjectRef> objectRefs = new ArrayList<>();
        for (int i = 0; i < 8; i++) {
            ObjectRef ref = YR.function(MyApp::add).options(opt).invoke(1);
            objectRefs.add(ref);
        }

        WaitResult waitResult = YR.wait(objectRefs, objectRefs.size(), -1);
        for (ObjectRef ref : waitResult.getReady()) {
            System.out.println(YR.get(ref, 9));
        }

        YR.Finalize();
    }
}
```

::::

In the above example, the function instance concurrency is `4`, meaning one function instance can process `4` tasks in parallel. The code triggers `8` function executions. When resources in the cluster are sufficient, two function instances will be started, with each function instance processing `4` requests in parallel.

Function instances are not immediately destroyed after processing tasks. They will survive for a period of time. During the survival period, if there are new tasks, requests will still be scheduled to that instance rather than creating new instances. You should call the `finalize` interface to immediately release occupied resources after all tasks have completed.
