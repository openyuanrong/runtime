# Data Objects

```{eval-rst}
.. toctree::
   :glob:
   :maxdepth: 1
   :hidden:

   KV
```

Data objects are memory data that can be shared and accessed across nodes among multiple openYuanrong functions in a distributed manner, supporting high-performance put/get access and modification based on shared memory. openYuanrong caches remote objects in a distributed shared memory object store, with one created on each node in the cluster.

Object references are essentially pointers or unique IDs used to reference remote objects without needing to obtain their actual values, conceptually similar to Futures. openYuanrong manages data object lifecycles automatically based on reference counting, automatically deleting data objects when the reference count reaches 0.

## Usage Limitations

- Data objects cannot be modified. If you need to modify, please create a new data object.
- The size of a single data object has no limit except for Java language, but cannot exceed the shared memory size configured for the openYuanrong data system. For Java language, a single data object size cannot exceed `Integer.MAX_VALUE - 8`.
- Data objects do not guarantee data reliability. Data may be lost when failures occur.
- When using the `get` interface to batch retrieve data, the total data size retrieved at once cannot exceed the shared memory size of the openYuanrong data system.

## Creating Data Objects

Data objects can be created in the following two ways. After creation, an object reference is returned and the reference count is automatically incremented. When the object reference instance dies, the reference count is automatically decremented.

- Function call returns an object reference.
- Directly calling the `put` interface returns an object reference.

::::{tab-set}
:::{tab-item} Python

```python
import yr

yr.init()
object_ref = yr.put(123)
yr.finalize()
```

:::
:::{tab-item} Java

```java
import org.yuanrong.Config;
import org.yuanrong.api.YR;
import org.yuanrong.runtime.client.ObjectRef;
import org.yuanrong.exception.YRException;

public class Main {
    public static void main(String[] args) throws YRException {
        YR.init(new Config());
        ObjectRef objectRef = YR.put(123);
        YR.Finalize();
    }
}
```

:::

:::{tab-item} C++

```c++
#include <yr/yr.h>

int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);
    YR::ObjectRef<int> objectRef = YR::Put<int>(123);
    YR::Finalize();
    return 0;
}
```

:::
::::

## Getting Data Objects

Use the `get` interface to obtain the result of a remote object from an object reference. If the object does not exist in the openYuanrong data system on the current node, it is automatically downloaded from a remote node. The `get` interface supports parallel retrieval of multiple object results, supports setting a retrieval timeout. If the data object is not produced within the timeout period, an exception is thrown.

::::{tab-set}
:::{tab-item} Python

```python
import time
import yr

yr.init()

# Get the value of one object ref.
object_ref = yr.put(123)
assert yr.get(object_ref) == 123

# Get the values of multiple object refs in parallel.
assert yr.get([yr.put(i) for i in range(3)]) == [0, 1, 2]

# Get timeout example.
@yr.invoke
def long_running_function():
    time.sleep(10)

object_ref = long_running_function.invoke()
try:
    yr.get(object_ref, 3)
except TimeoutError as e:
    print(e)

yr.finalize()
```

:::
:::{tab-item} Java

```java
import org.yuanrong.Config;
import org.yuanrong.api.YR;
import org.yuanrong.runtime.client.ObjectRef;
import org.yuanrong.exception.YRException;
import org.yuanrong.storage.WaitResult;

import java.util.concurrent.TimeUnit;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.List;

public class Main {
    // Get timeout example.
    public static class MyApp {
        public static int longRunningFunction() throws InterruptedException {
            TimeUnit.SECONDS.sleep(10);
            return 1;
        }
    }

    public static void main(String[] args) throws YRException {
        YR.init(new Config());

        // Get the value of one object ref.
        ObjectRef objectRef = YR.put(123);
        System.out.println(YR.get(objectRef, 30));

        // Get the values of multiple object refs in parallel.
        List<ObjectRef> objectRefs = new ArrayList<>();
        for (int i = 0; i < 3; i++) {
            objectRefs.add(YR.put(i));
        }

        WaitResult waitResult = YR.wait(objectRefs, objectRefs.size(), -1);
        for (ObjectRef ref : waitResult.getReady()) {
            System.out.println(YR.get(ref, 30));
        }

        try {
            ObjectRef resultRef = YR.function(MyApp::longRunningFunction).invoke();
            YR.get(resultRef, 3);
        } catch (YRException e) {
            System.out.println(e);
        }

        YR.Finalize();
    }
}
```

:::
:::{tab-item} C++

```c++
#include <iostream>
#include <yr/yr.h>

// Get timeout example.
int LongRunningFunction()
{
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 1;
}
YR_INVOKE(LongRunningFunction);


int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);

    // Get the value of one object ref.
    YR::ObjectRef<int> objectRef = YR::Put<int>(123);
    std::cout << *YR::Get(objectRef) << std::endl;

    // Get the values of multiple object refs in parallel.
    std::vector<YR::ObjectRef<int>> object_refs;
    for (int i = 0; i < 3; i++) {
        object_refs.emplace_back(YR::Put(i));
    }

    auto waitResults = YR::Wait(object_refs, object_refs.size());
    for (auto ref : waitResults.first) {
        std::cout << *YR::Get(ref) << std::endl;
    }

    YR::ObjectRef<int> object_ref = YR::Function(LongRunningFunction).Invoke();
    try {
        int result = *YR::Get(object_ref, 3);
    } catch (YR::Exception &e) {
        std::cout << e.what() << std::endl;
    }

    YR::Finalize();
    return 0;
}
```

:::
::::

## Passing Data Objects

Object references can be freely passed between applications, and objects are automatically managed for lifecycle through distributed reference counting.

- **Passing object references as top-level parameters to functions:** When passing object references as top-level parameters, openYuanrong will automatically call the `get` interface to obtain the data object's value and pass it to the called function, which means the task will only execute when the object data is fully available.

    ```python
    import yr

    yr.init()

    @yr.invoke
    def get_nums():
        return [1, 2, 3]

    @yr.invoke
    def dis_sum(args): # When dis_sum is called, the passed value is [1, 2, 3]
        return sum(args)

    ref = dis_sum.invoke(get_nums.invoke()) # The parameter passed to dis_sum is an object reference, at runtime openYuanrong will automatically obtain the object's value and pass it
    num = yr.get(ref, 30)
    assert num == 6
    ```  

- **Passing object references as nested parameters:** When data objects are passed nested within other data objects, the task needs to call the `get` interface to obtain the specific values. When data objects are passed nested, the outer object reference automatically increments the reference count of the inner object reference, thereby ensuring that when the outer object reference exists, the inner object reference will definitely not be released.

    ```python
    import yr

    yr.init()

    @yr.invoke
    def get_num(x):
        return x

    @yr.invoke
    def dis_sum(args): # When dis_sum is called, the passed value is [objref1, objref2, objref3]
        return sum(yr.get(args)) # When calling get, the values of objref1/objref2/objref3 will be transmitted to the current machine.

    objref1 = get_num.invoke(1) # Returns object ref here, can return after issuing the call request, no need to wait for get_num to complete execution
    objref2 = get_num.invoke(2)
    objref3 = get_num.invoke(3)
    objref = yr.put([objref1, objref2, objref3]) # Nested passing of object ref
    ref = dis_sum.invoke(objref)
    num = yr.get(ref, 30)
    assert num == 6
    ```

## Data Object Spilling to Disk

Data object data is stored in the shared memory of the openYuanrong data system. When memory is insufficient, it supports automatically spilling data to disk and deleting data from memory. When data needs to be read, it is automatically loaded from disk to shared memory. Note that when disk space is also insufficient, data object writes will fail.

To use the data object spilling feature, relevant parameters need to be specified during deployment, which is disabled by default.

```yaml
# Specify the path for object spilling to disk. If configured as empty, it means the object spilling feature is disabled.
spillDirectory: ""
```

Data object spilling has the following parameters that can be used to set disk space limits, spilling concurrent threads, file size, and other parameters for performance tuning.<br>

```yaml
# Specify the maximum disk space occupied when spilling to disk, in bytes. When configured as 0, openYuanrong will use 95% of the remaining disk space when the openYuanrong cluster was started as the maximum usable space.
# When the spillDirectory configuration directory occupies a dedicated disk, it is recommended to configure this value as 0, letting the system automatically calculate the available value. Otherwise, it is recommended to configure this value according to actual disk space planning.
spillSizeLimit: "0"
# Specify the maximum parallelism when writing data to disk. When disk performance is very high, you can try increasing this value to improve performance.
spillThreadNum: 8
# Specify the size of a single file spilled to disk. Unit is MB, value range: 200-10240.
# When objects are small, multiple objects are merged and written to one file. When an object is very large and exceeds this value, one object per file.
spillFileMaxSizeMb: 200
# Used to specify the maximum number of open file handles for object spilling. When this value is decreased, it may reduce performance.
spillFileOpenLimit: 512
# Whether to allow filesystem readahead functionality. When set to false, filesystem readahead is disabled, which can reduce read amplification but may affect performance.
spillEnableReadahead: true
```

## More Information

openYuanrong also provides near-computation KV caching capabilities, implementing copy-free KV data read and write based on shared memory. View the [KV Cache Development Guide](./KV.md).
