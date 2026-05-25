# Resources

openYuanrong abstracts physical machines, allowing you to express computational needs in the form of resources. The system will schedule and auto-scale based on resource requests. This section will introduce how to specify node and openYuanrong function resources.

Resources in openYuanrong are represented as key-value pairs, where the key represents the resource name and the value is a floating-point number. openYuanrong natively supports CPU and memory resource types, known as predefined resources. In addition, it also supports custom resources such as GPU, NPU, etc.

Resources in openYuanrong are logical resources, scheduled based on the resource amounts specified by openYuanrong functions (for example, preventing tasks with cpu=1000 from being allocated to nodes declaring logical CPU as 0), but will not limit the function's actual usage of physical resources. You need to ensure that the resources used by functions do not exceed the values they specify. openYuanrong does not provide CPU isolation mechanisms; a function specifying 1 CPU core resource will not exclusively occupy a physical CPU.

## Specifying Node Resources

Specify the total logical resources of nodes through the following [parameters](../../../deploy/deploy_processes/parameters.md) when deploying openYuanrong:

- `values.cpu_num`: Total available CPU on the node, in millicores (1/1000 core). When deploying the master node, the system defaults to occupying 1 millicore. You can set it as `-s 'values.cpu_num=1'` when deploying the master node, then that node will not be used for running distributed tasks.
- `values.memory_num`: Total available memory on the node, in MB. Part of it is used for storing "data objects", and part is used for function stack memory.
- `values.shared_memory_num`: The amount of memory used for storing "data objects" from the total available memory on the node, in MB.
- `function_agent.args.gpu_collection_enable`: Automatically detect GPU resources on the node.
- `function_agent.args.npu_collection_mode`: Automatically detect NPU resources on the node.
- `function_agent.args.custom_resources`: Other custom resources besides GPU and NPU, such as SSD.

Configuration example is as follows, indicating configuration of 4000 millicores CPU (of which 3999 millicores can be used for running distributed tasks), 8192MB total memory, 4096MB shared memory, enabling NPU auto-detection and including 5 custom ssd resources on the master node.

```bash
yr start --master \
-s 'values.cpu_num="4000"' -s 'values.memory_num="8192"' -s 'values.shared_memory_num="4096"' \
-s 'function_agent.args.npu_collection_mode="all"' \
-s 'function_agent.args.custom_resources="{\"ssd\":5}"'
```

## Specifying Function Instance Resources

When calling stateless functions or creating stateful function instances, you can use the `InvokeOptions` interface to dynamically specify required resources.

::::{tab-set}
:::{tab-item} Python

```python
import yr

@yr.invoke
def add(n):
    return n + 1

yr.init()

# Configure running stateless function requires 1 CPU core, 1G memory, 1 ssd custom resource, 1 NPU card of any model, 1 GPU card of any model
opt = yr.InvokeOptions(cpu=1000, memory=1024)
opt.custom_resources={"ssd":1,"NPU/.+/count":1,"GPU/.+/count":1}
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

    // Configure running stateless function requires 1 CPU core, 1G memory, 1 ssd custom resource, 1 NPU card of any model, 1 GPU card of any model
    YR::InvokeOptions opt;
    opt.cpu = 1000;
    opt.memory = 1024;
    opt.customResources["ssd"] = 1;
    opt.customResources["NPU/.+/count"] = 1;
    opt.customResources["GPU/.+/count"] = 1;

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

import java.util.HashMap;

public class Main {
    public static class MyApp {
        public static int add(int n) {
            return n + 1;
        }
    }

    public static void main(String[] args) throws Exception {
        Config conf = new Config();
        YR.init(conf);

        // Configure running stateless function requires 1 CPU core, 1G memory, 1 ssd custom resource, 1 NPU card of any model, 1 GPU card of any model
        HashMap<String, Float> customResources = new HashMap<>();
        customResources.put("ssd", 1.0f);
        customResources.put("NPU/.+/count", 1.0f);
        customResources.put("GPU/.+/count", 1.0f);

        InvokeOptions opt = new InvokeOptions.Builder().customResources(customResources).cpu(1000).memory(1024).build();
        ObjectRef ref = YR.function(MyApp::add).options(opt).invoke(1);
        System.out.println(YR.get(ref, 3));

        YR.Finalize();
    }
}
```

:::
::::

(development-scheduling-config-npu)=

### NPU Resource Usage Rules

When specifying function instances, NPU resources are declared through a triplet in the format `NPU/{chip name}/{resource type}`. The configured resource amount must be greater than 0.

`chip name` is used to specify the NPU chip device model (for example: `Ascend310`, `Ascend910`), and can be matched using regular expressions.

- Match multiple specific models:

    ```python
    # Match all devices with model Ascend310 or Ascend910
    npu_resource = {"NPU/Ascend(310|910)/count": 3}
    ```

- Match device models with a specific prefix:

    ```python
    # Match all device models starting with `Ascend` (such as Ascend310, Ascend910, etc.)
    npu_resource = {"NPU/Ascend.*/count": 3}
    ```

- Using `^` in the device model part will cause the regular expression to match from the beginning of the device model, but because there is already `NPU/` before the device model, `^` is meaningless here, causing incorrect matching.

    ```python
    # Invalid configuration
    npu_resource = {"NPU/^Ascend910.*/YY": 3}
    ```

- Missing closing parenthesis, causing regular expression parsing failure and inability to match correctly.

    ```python
    # Invalid configuration
    npu_resource = {"NPU/(Ascend910/YY": 3}
    ```

:::{hint}

- Ensure regular expression syntax is correct, it is recommended to test and verify first.
- Use caution with overly broad matching, such as: `NPU/.+/count`, which will match all devices and may result in scheduling to unexpected devices. Ensure that NPU resources for all device models in the cluster can accommodate your code.

:::

`resource type` can be configured as `count`, `HBM`, `latency`, `stream`, with usage instructions and constraints as follows.

- `count` is used to specify the number of cards. This parameter is mutually exclusive with other parameters, meaning specifying `count` will prevent setting other parameters.
- `HBM`, `latency`, `stream` are used to support fine-grained resource allocation, all three need to be configured simultaneously. For example: `npu_resource = {"NPU/Ascend910/HBM":30000, "NPU/Ascend910/stream":2, "NPU/Ascend910/latency":50}`.
- `HBM` specifies the amount of video memory occupied by the model.
- `latency`: The latency of a single inference execution by the model (unit: ms).
- `stream`: The number of internal streams in the model, default is 1. If greater than 1, it will not be co-scheduled with other processes sharing a card.

openYuanrong achieves NPU isolation by automatically setting the `ASCEND_RT_VISIBLE_DEVICES` environment variable. When using NPU resources, users need to obtain the NPU ID allocated for the instance through the environment variable `ASCEND_RT_VISIBLE_DEVICES` in the function, refer to the [example](../../examples/use_NPU_resource.md).

:::{attention}

- When a user requests to use M NPUs, they can use at most M cards. Exceeding M cards will cause Ascend to throw an exception.
- When a user requests not to use NPUs, the function can still obtain and use all NPU cards on the current node. Users must ensure they do not use NPU cards to avoid interference with other NPU instances.

:::

### GPU Resource Usage Rules

When specifying function instance resources, GPU resources are declared through a triplet in the format `GPU/{chip name}/count`, where `chip name` is used to specify the GPU chip device model. The configured resource amount must be greater than ``0``.
