# Building Tree-Shaped Job Graphs with Stateful Functions

This pattern decouples "business logic" from "task orchestration" to build efficient distributed systems.

## Core Architecture Logic

In this mode, the Driver is like a company's CEO, who only interfaces with a few Supervisors. Each Supervisor leads a team of Workers.

- Driver: Responsible for high-level decision-making and initializing Supervisors.
- Supervisor: Responsible for creating Workers, assigning tasks, monitoring status, and restarting Workers when they crash (Handle failures).
- Worker: Executes the actual heavy lifting.

:::{Note}

- If a Supervisor is destroyed, the Workers it manages will also be destroyed due to reference counting maintaining the lifecycle of stateful functions.
- Stateful functions can be nested in multiple layers, thereby constructing a tree structure.

:::

## Building a Two-Layer Structure Using Data Training as an Example

First layer: Launch multiple stateful functions. Each stateful function is responsible for a specific set of hyperparameters (e.g., learning rate \(0.01\), \(0.001\)).

Second layer: Each stateful function internally creates a set of Worker stateful functions. These Workers share the same set of hyperparameters but each processes different slices of data (Data Shards), and synchronizes gradients through all-reduce or parameter server.

### Resource Management Recommendations

In this pattern, resource calculation becomes complex. If you have 32 CPUs:

- If each Supervisor occupies 1 CPU, and it creates 3 Workers each occupying 1 CPU.
- Then one experiment occupies 4 CPUs in total.
- You can run at most \(32 / 4 = 8\) hyperparameter experiments simultaneously.

:::{Warning}

Be sure to set num_cpus=1 or smaller for Supervisors, otherwise if you have a large number of hyperparameters, all CPUs will be occupied by Supervisors, preventing Workers from starting and causing resource deadlock.

:::

### Usage Example

```python
import yr
import time

# Create stateful function class instances and set their required runtime resources (1 CPU core)
opt = yr.InvokeOptions(cpu=1000)


# --- 2. Bottom Layer: Workers that execute actual training ---
@yr.instance(invoke_options=opt)
class TrainingWorker:
    def train(self, shard_id, hyperparams):
        # Simulate training process
        lr = hyperparams["lr"]
        print(f"Worker {shard_id} is training data shard with lr={lr}...")
        time.sleep(2)
        return f"Loss: {0.1 / lr}"


# --- 1. Middle Layer: Supervisors responsible for orchestration ---
@yr.instance(invoke_options=opt)
class TrainingSupervisor:
    def __init__(self, hyperparams):
        self.hyperparams = hyperparams
        self.workers = []

    def start_training(self, num_workers):
        # Dynamically create your Worker team
        self.workers = [TrainingWorker.invoke() for i in range(num_workers)]

        # Schedule all Workers to train data shards in parallel
        results = yr.get([
            w.train.invoke(i, self.hyperparams)
            for i, w in enumerate(self.workers)
        ])
        return f"Hyperparameter {self.hyperparams} experiment completed, results: {results}"


# --- Main Controller ---
yr.init()

# Launch two experiments with different hyperparameters in parallel (two Supervisors)
configs = [{"lr": 0.01}, {"lr": 0.001}]
supervisors = [TrainingSupervisor.invoke(cfg) for cfg in configs]

# All experiments run in parallel
final_reports = yr.get([s.start_training.invoke(num_workers=2) for s in supervisors])

for report in final_reports:
    print(report)

yr.finalize()
```
