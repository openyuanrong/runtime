# Configure Function Instance Affinity

This section introduces how to use resource and instance affinity through Python examples to implement custom scheduling strategies.

## Configure Resource Affinity for Function Instances

Resource affinity is suitable for solving the problem of scheduling function instances to specific resources. You need to pre-label resources, and when deploying on hosts, labels are applied to nodes. We will demonstrate two usages of strong affinity resources in the examples.

### Configure Resource Affinity in Host Deployment Environment

We deploy two nodes.

First, deploy the master node with two labels `{"master":"dev"}` and `{"agent":"dev"}`.

```bash
yr start --master \
-s 'mode.master.dashboard=true' -s 'mode.master.collector=true' \
-s 'function_agent.args.enable_separated_redirect_runtime_std=true' \
-s 'function_agent.env.INIT_LABELS="{\"master\":\"dev\",\"agent\":\"dev\"}"'
```

Then deploy the worker node with one label `{"agent":"uat"}`.

```bash
# Replace {http_scheme}, {function_master_ip} and {function_master_port} with the corresponding information output when the master node was successfully deployed
yr start \
-s 'mode.agent.collector=true' -s 'function_agent.args.enable_separated_redirect_runtime_std=true' \
-s 'function_agent.env.INIT_LABELS="{\"agent\":\"uat\"}"' \
--master_address {http_scheme}://{function_master_ip}:{function_master_port}
```

The `show` method of the stateful function Detector in the code will help print the node information where the function instance is located. We set resource affinity labels for two Detector instances respectively. The master node has a label with key `master`, so the first Detector instance will not be deployed on the master node. The second Detector instance requires not only the label key to match, but also the label value to match. Therefore, the second Detector instance will only be deployed on the worker node.

```python
# resource-affinity.py
import os
import yr

@yr.instance
class Detector:
    def __init__(self, number):
        self.number = number

    def show(self):
        print("Detector " + str(self.number) + ",NODE_ID:" + os.getenv('NODE_ID') + ",INIT_LABELS:" + os.getenv('INIT_LABELS'))

if __name__ == '__main__':
    yr.init()

    # First Detector instance, affinity set to launch on nodes without master label
    affinity_not_master = yr.Affinity(yr.AffinityKind.RESOURCE, yr.AffinityType.REQUIRED, [yr.LabelOperator(yr.OperatorType.LABEL_NOT_EXISTS, "master")])
    opt_zero = yr.InvokeOptions()
    opt_zero.schedule_affinities = [affinity_not_master]

    try:
        detector = Detector.options(opt_zero).invoke(0)
        yr.get(detector.show.invoke())
        detector.terminate()
    except RuntimeError as e:
        print(e)


    # Second Detector instance, affinity set to launch on nodes with label key "agent" and value "uat"
    affinity_agent_with_uat = yr.Affinity(yr.AffinityKind.RESOURCE, yr.AffinityType.REQUIRED, [yr.LabelOperator(yr.OperatorType.LABEL_IN, "agent", ["uat"])])
    opt_one = yr.InvokeOptions()
    opt_one.schedule_affinities = [affinity_agent_with_uat]

    try:
        detector = Detector.options(opt_one).invoke(1)
        yr.get(detector.show.invoke())
        detector.terminate()
    except RuntimeError as e:
        print(e)
```

Execute the command `python resource-affinity.py` to run the program. Check the function [log](../../observability/logs.md) file `{node_id}-user_func_std.log` on the worker node, you can see the following output, indicating that both instances are deployed on the node with label `{"agent":"uat"}`.

```bash
2025-07-18 17:12:33|56412d11-0000-4000-8000-005cef06b506|runtime-56412d11-0000-4000-8000-005cef06b506-c6d59c3a409e|INFO|Detector 0,NODE_ID:dggphis35945-2731346,INIT_LABELS:{"agent":"uat"}
2025-07-18 17:12:33|04d8cf02-727f-4714-8000-000000000071|runtime-04d8cf02-727f-4714-8000-000000000071-000000d8f917|INFO|Detector 1,NODE_ID:dggphis35945-2731346,INIT_LABELS:{"agent":"uat"}
```

## Configure Affinity Between Function Instances

Instance affinity is suitable for solving the problem of deploying different function instance combinations on the same node or distributed deployment for disaster recovery. We will demonstrate how to achieve 1:1 deployment of two function types on nodes.

### Configure Instance Affinity in Host Deployment Environment

We deploy two nodes.

First deploy the master node:

```bash
yr start --master \
-s 'mode.master.dashboard=true' -s 'mode.master.collector=true' \
-s 'function_agent.args.enable_separated_redirect_runtime_std=true'
```

Then deploy the worker node:

```bash
# Replace {http_scheme}, {function_master_ip} and {function_master_port} with the corresponding information output when the master node was successfully deployed
yr start \
-s 'mode.agent.collector=true' -s 'function_agent.args.enable_separated_redirect_runtime_std=true' \
--master_address {http_scheme}://{function_master_ip}:{function_master_port}
```

The `show` method of the stateful functions will help print the node information where the function instances are located. We configure the Detector instance label key as `detector`, so instances with the same label are not deployed on the same node. The Partner instance is configured with label key `partner`, so instances with the same label are not deployed on the same node, but must be deployed on the same node as instances with label key `detector`.

```python
# instance-affinity.py
import os
import yr

@yr.instance
class Detector:
    def __init__(self, number):
        self.number = number

    def show(self):
        print("Detector " + str(self.number) + ",NODE_ID:" + os.getenv('NODE_ID'))

@yr.instance
class Partner:
    def __init__(self, number):
        self.number = number

    def show(self):
        print("Partner " + str(self.number) + ",NODE_ID:" + os.getenv('NODE_ID'))


if __name__ == '__main__':
    yr.init()

    # Affinity set so detector instances are not deployed together
    affinity_not_detector = yr.Affinity(yr.AffinityKind.INSTANCE, yr.AffinityType.REQUIRED, [yr.LabelOperator(yr.OperatorType.LABEL_NOT_EXISTS, "detector")])
    opt_d = yr.InvokeOptions()
    opt_d.labels = ["detector"]
    opt_d.schedule_affinities = [affinity_not_detector]

    # Affinity set so partner instances are not deployed together, but must be deployed with detector instances
    operators = [yr.LabelOperator(yr.OperatorType.LABEL_NOT_EXISTS, "partner"), yr.LabelOperator(yr.OperatorType.LABEL_EXISTS, "detector")]
    affinity_with_detector = yr.Affinity(yr.AffinityKind.INSTANCE, yr.AffinityType.REQUIRED, operators)
    opt_p = yr.InvokeOptions()
    opt_p.labels = ["partner"]
    opt_p.schedule_affinities = [affinity_with_detector]


    try:
        instances_num = 2
        detectors = [Detector.options(opt_d).invoke(i) for i in range(instances_num)]
        for d in detectors:
            yr.get(d.show.invoke())

        partners = [Partner.options(opt_p).invoke(i) for i in range(instances_num)]
        for p in partners:
            yr.get(p.show.invoke())

        for i in range(instances_num):
            detectors[i].terminate()
            partners[i].terminate()
    except RuntimeError as e:
        print(e)
```

Execute the command `python instance-affinity.py` to run the program. Check the function log file `{node_id}-user_func_std.log` on the master and worker nodes, you can see the following output, with one Detector instance and one Partner instance deployed on each node.

Master node:

```bash
2025-07-18 17:20:43|74f15c86-1100-4000-8000-00006cee4186|runtime-74f15c86-1100-4000-8000-00006cee4186-000000001165|INFO|Detector 0,NODE_ID:dggphis35893-1360565
2025-07-18 17:20:44|bb1d0000-0000-4000-80be-8d1a712b5b0e|runtime-bb1d0000-0000-4000-80be-8d1a712b5b0e-00000000008e|INFO|Partner 0,NODE_ID:dggphis35893-1360565
```

Worker node:

```bash
2025-07-18 17:20:44|47120000-0000-4000-803c-5dff5d65a252|runtime-47120000-0000-4000-803c-5dff5d65a252-0083aa759f33|INFO|Detector 1,NODE_ID:dggphis35945-3030452
2025-07-18 17:20:44|ef042b74-7975-4e00-8000-00000000c91a|runtime-ef042b74-7975-4e00-8000-00000000c91a-00000000cd30|INFO|Partner 1,NODE_ID:dggphis35945-3030452
```
