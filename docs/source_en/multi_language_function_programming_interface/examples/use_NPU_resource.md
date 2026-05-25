# Using NPU Resources in Stateful Functions

In AI applications such as machine learning tasks, heterogeneous computing resources have become essential. openYuanrong supports managing and scheduling different types of computing resources. This section uses NPU as an example to introduce how to specify and use them in stateful functions.

## Prerequisites

One or more hosts containing NPU cards. During deployment, use the `--npu_collection_mode` parameter to enable automatic NPU information collection.

Taking a single node as an example, refer to the deployment command below:

```bash
yr start --master --npu_collection_mode=all
```

## Usage Example

```python
import yr
import torch
import torch_npu
import os

@yr.instance
class NPUInstance:
    def __init__(self):
        self.device = None

    def getEnv(self):
        ascend_device_id = os.getenv('ASCEND_RT_VISIBLE_DEVICES')
        return (f"ASCEND_RT_VISIBLE_DEVICES: {ascend_device_id}")

    def run(self):
        if torch.npu.is_available():
            device0 = torch.device("npu:0")
            print(f"user can see device: {device0}")

            tensor0 = torch.randn(3, 3).to(device0)

            # if request 1 NPU, the following code will raise error
            # device1 = torch.device("npu:1")
            # tensor1 = torch.randn(3, 3).to(device1)
            return f"Tensor device: {tensor0.device}, result: \n {tensor0}"
        else:
            return "NPU is not available"


def main():
    yr.init()

    npu_resource = {"NPU/.+/count": 1} # request 1 NPU of any type
    opt = yr.InvokeOptions()
    opt.custom_resources = npu_resource
    npu_instance = NPUInstance.options(opt).invoke()

    res = npu_instance.run.invoke()
    print(yr.get(res))

    yr.finalize()

if __name__ == "__main__":
    main()
```

Run the program, normal output as follows:

```bash
Tensor device: npu:0, result:
 tensor([[-0.9499, -0.2434,  0.8055],
        [-0.6371,  0.7378, -0.5710],
        [ 0.8289, -0.4673,  1.3490]], device='npu:0')
```

Uncomment the commented code logic and run the program again. Because 1 NPU was specified but 2 NPUs are being used, the following exception will be thrown:

```bash
  File "/workspace/caseTest/npuTest/demoNpu.py", line 24, in run
    tensor1 = torch.randn(3, 3).to(device1)
RuntimeError: exchangeDevice:build/CMakeFiles/torch_npu.dir/compiler_depend.ts:42 NPU function error: c10_npu::SetDevice(d.index()), error code is 107001
[ERROR] 2025-04-10-11:26:58 (PID:1828121, Device:0, RankID:-1) ERR00100 PTA call acl api failed
[Error]: Invalid device ID.
```

This is because openYuanrong, based on Ascend native capabilities, provides NPU isolation using the environment variable `ASCEND_RT_VISIBLE_DEVICES`. If you wish to control NPU resource usage yourself, you can write the environment variable `export YR_NOSET_ASCEND_RT_VISIBLE_DEVICES=1` on the node before deploying openYuanrong.

View more [NPU Resource Usage Rules](development-scheduling-config-npu).
