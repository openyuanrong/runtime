# Implementing Monte Carlo Method Based on openYuanrong

The Monte Carlo method, or computer random simulation method, is a computational method based on "random numbers" and is one of the common algorithms in High Performance Computing (HPC). A simple application is calculating the mathematical constant π (pi). The main idea is to inscribe a circle within a square of side length R, then randomly scatter points within the square. The probability of a point falling within the circle is the ratio of the circle's area to the square's area, i.e., `π/4`. By calculating this probability, we can estimate the value of π, with more points yielding higher precision.

This example demonstrates how to use openYuanrong to develop a simple HPC service that implements dynamic task parallelism, including:

- How to parallelize multiple tasks using stateless functions.
- How to obtain and save execution status of parallel tasks using stateful functions.

## Solution Overview

We use Python language, defining a stateless function to handle point-counting tasks and a stateful function to track task execution status. The code can run on a single host or be extended to a cluster to configure larger task volumes for improved computational precision.

## Prerequisites

Refer to [Deploy on Hosts](../../deploy/deploy_processes/index.md) to complete openYuanrong deployment.

## Implementation Process

### Define Stateful Function to Track Point-counting Task Status

We define a stateful function using the `@yr.instance` decorated class TaskSummary to track the status of point-counting tasks. Point-counting tasks report their status by calling its `report_status` method, and the main program retrieves all task statuses by calling its `get_task_status` method. Instances of this class run on remote processes, and their member variables like `task_started_num` maintain state during execution. Holding a handle to the class object allows calling its methods to change the state of member variables.

```python
class TaskStatus(Enum):
    PENDING = -1
    STARTED = 0
    COMPLETED = 1

@yr.instance
class TaskSummary:
    def __init__(self, total_task_num: int):
        self.total_task_num = total_task_num
        self.task_started_num = 0
        self.task_completed_num = 0

    def report_status(self, task_status: TaskStatus) -> None:
        if task_status == TaskStatus.STARTED:
            self.task_started_num += 1
        elif task_status == TaskStatus.COMPLETED:
            self.task_completed_num += 1

    def get_task_status(self) -> int:
        return self.task_started_num, self.task_completed_num
```

### Define Stateless Function to Handle Point-counting Tasks

We define a stateless function using the `@yr.invoke` decorated ordinary function `monte_carlo_pi` to handle point-counting tasks and count the number of points falling within the circle. Function instances run asynchronously on remote processes and report task status by calling the `report_status` method through the handle of the stateful function `TaskSummary`.

```python
@yr.invoke
def monte_carlo_pi(total_points: int, task_summary: yr.decorator.instance_proxy.InstanceProxy) -> int:
    # Report task has started
    task_summary.report_status.invoke(TaskStatus.STARTED)

    circle_points = 0
    for i in range(total_points):
        rand_x = random.uniform(-1, 1)
        rand_y = random.uniform(-1, 1)

        origin_dist = rand_x**2 + rand_y**2
        if origin_dist <= 1:
            circle_points += 1

    # Report task has completed
    task_summary.report_status.invoke(TaskStatus.COMPLETED)
    return circle_points
```

### Define Main Flow

The main flow initializes the openYuanrong runtime context through `yr.init()`. You can adjust the number of tasks according to your cluster size.

We specify the resources required for stateless function tasks through `yr.InvokeOptions` to better observe their parallel status. Use `monte_carlo_pi.options(opt).invoke(POINTS_PER_TASK, task_summary)` to start tasks, and query task execution status in a while loop until all tasks complete.

Finally, aggregate the point-counting data returned by each task to calculate π, and call `yr.finalize()` to clean up the context.

```python
import yr
import time
import random
from enum import Enum

if __name__ == '__main__':

    yr.init()

    # Adjust according to cluster size
    TASKS_NUM = 15
    POINTS_PER_TASK = 10000000
    TOTAL_POINTS = TASKS_NUM * POINTS_PER_TASK

    # Create stateful function instance responsible for task tracking
    task_summary = TaskSummary.invoke(POINTS_PER_TASK)

    # Execute all tasks in parallel, resource requirements can also be omitted here
    opt = yr.InvokeOptions(cpu=1000, memory=1000)
    results = [
        monte_carlo_pi.options(opt).invoke(POINTS_PER_TASK, task_summary)
        for i in range(TASKS_NUM)
    ]

    # Query task execution status
    while True:
        started_num, completed_num = yr.get(task_summary.get_task_status.invoke())
        print("Total tasks:", TASKS_NUM, "Started:", started_num, "Completed", completed_num)

        if completed_num == TASKS_NUM:
            break

        time.sleep(1)

    # Calculate final result
    circle_points = sum(yr.get(results))
    pi = (circle_points * 4) / TOTAL_POINTS
    print(f"π is: {pi}")

    yr.finalize()
```

### Run Program

:::
:::{dropdown} Complete Code
:chevron: down-up
:icon: chevron-down

```python
import yr
import time
import random
from enum import Enum

class TaskStatus(Enum):
    PENDING = -1
    STARTED = 0
    COMPLETED = 1

@yr.invoke
def monte_carlo_pi(total_points: int, task_summary: yr.decorator.instance_proxy.InstanceProxy) -> int:
    # Report task has started
    task_summary.report_status.invoke(TaskStatus.STARTED)

    circle_points = 0
    for i in range(total_points):
        rand_x = random.uniform(-1, 1)
        rand_y = random.uniform(-1, 1)

        origin_dist = rand_x**2 + rand_y**2
        if origin_dist <= 1:
            circle_points += 1

    # Report task has completed
    task_summary.report_status.invoke(TaskStatus.COMPLETED)
    return circle_points


@yr.instance
class TaskSummary:
    def __init__(self, total_task_num: int):
        self.total_task_num = total_task_num
        self.task_started_num = 0
        self.task_completed_num = 0

    def report_status(self, task_status: TaskStatus) -> None:
        if task_status == TaskStatus.STARTED:
            self.task_started_num += 1
        elif task_status == TaskStatus.COMPLETED:
            self.task_completed_num += 1

    def get_task_status(self) -> tuple[int, int]:
        return self.task_started_num, self.task_completed_num

if __name__ == '__main__':

    yr.init()

    # Adjust according to cluster size
    TASKS_NUM = 15
    POINTS_PER_TASK = 10000000
    TOTAL_POINTS = TASKS_NUM * POINTS_PER_TASK

    # Create stateful function instance responsible for task tracking
    task_summary = TaskSummary.invoke(TASKS_NUM)

    # Execute all tasks in parallel, resource requirements can also be omitted here
    opt = yr.InvokeOptions(cpu=1000, memory=1000)
    results = [
        monte_carlo_pi.options(opt).invoke(POINTS_PER_TASK, task_summary)
        for i in range(TASKS_NUM)
    ]

    # Query task execution status
    while True:
        started_num, completed_num = yr.get(task_summary.get_task_status.invoke())
        print("Total tasks:", TASKS_NUM, "Started:", started_num, "Completed", completed_num)

        if completed_num == TASKS_NUM:
            break

        time.sleep(1)

    # Calculate final result
    circle_points = sum(yr.get(results))
    pi = (circle_points * 4) / TOTAL_POINTS
    print(f"π is: {pi}")

    yr.finalize()
```

:::

Running on an openYuanrong environment with a single node CPU resource of 9000 (unit: 1/1000 core), the output is as follows:

```bash
Total tasks: 15 Started: 8 Completed 0
Total tasks: 15 Started: 9 Completed 0
Total tasks: 15 Started: 9 Completed 0
Total tasks: 15 Started: 9 Completed 0
Total tasks: 15 Started: 9 Completed 0
Total tasks: 15 Started: 9 Completed 0
Total tasks: 15 Started: 9 Completed 0
Total tasks: 15 Started: 9 Completed 0
Total tasks: 15 Started: 9 Completed 0
Total tasks: 15 Started: 9 Completed 0
Total tasks: 15 Started: 9 Completed 0
Total tasks: 15 Started: 9 Completed 0
Total tasks: 15 Started: 10 Completed 1
Total tasks: 15 Started: 13 Completed 4
Total tasks: 15 Started: 14 Completed 5
Total tasks: 15 Started: 15 Completed 7
Total tasks: 15 Started: 15 Completed 9
Total tasks: 15 Started: 15 Completed 9
Total tasks: 15 Started: 15 Completed 9
Total tasks: 15 Started: 15 Completed 9
Total tasks: 15 Started: 15 Completed 9
Total tasks: 15 Started: 15 Completed 9
Total tasks: 15 Started: 15 Completed 9
Total tasks: 15 Started: 15 Completed 9
Total tasks: 15 Started: 15 Completed 11
Total tasks: 15 Started: 15 Completed 13
Total tasks: 15 Started: 15 Completed 13
Total tasks: 15 Started: 15 Completed 14
Total tasks: 15 Started: 15 Completed 14
Total tasks: 15 Started: 15 Completed 15
π is: 3.14155216
```
