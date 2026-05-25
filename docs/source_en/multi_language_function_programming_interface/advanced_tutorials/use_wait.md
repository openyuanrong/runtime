# Using yr.wait to Limit Concurrent/Pending Tasks

If the rate of sending jobs exceeds the rate of processing jobs, it will cause job backlog in the job queue, and even OOM (Out of Memory). `yr.wait()` allows backpressure and can limit the total number of pending jobs, preventing the job queue from expanding infinitely and avoiding OOM.

Note that this method is mainly used to limit the number of jobs allowed to execute at the same time. This method can also be used to limit the number of concurrent jobs, but this will reduce job distribution performance, so it is not recommended. openYuanrong will automatically distribute and adjust the number of concurrent jobs based on the amount of resources and the resource size required by jobs.

## Usage Example

```python
import yr
import time

# Initialize yr
yr.init()


@yr.invoke
def heavy_computation_task(i):
    # Simulate time-consuming operations, such as image processing or model inference
    time.sleep(1)
    return f"Result from task {i}"


# --- Configuration Parameters ---
TOTAL_TASKS = 100
MAX_CONCURRENT_TASKS = 20  # Maximum parallel/in-flight tasks to prevent OOM
TIMEOUT = 10
WAIT_NUM = 1

# Store executing task handles (Object Refs)
pending_refs = []
results = []

print(f"Start submitting tasks, limiting maximum in-flight tasks to: {MAX_CONCURRENT_TASKS}")

for i in range(TOTAL_TASKS):
    # [Core Logic] If the current running tasks reach the upper limit
    if len(pending_refs) >= MAX_CONCURRENT_TASKS:
        # Use yr.wait to block until at least one task completes
        # timeout=None means infinite waiting until results return
        ready_refs, pending_refs = yr.wait(pending_refs, wait_num=WAIT_NUM, timeout=TIMEOUT)

        # Process completed results
        for ref in ready_refs:
            result = yr.get(ref)
            results.append(result)
            # print(f"Completed and freed memory: {result}")

    # Submit new task
    task_ref = heavy_computation_task.invoke(i)
    pending_refs.append(task_ref)

    if i % 10 == 0:
        print(f"Submitted task {i}, current queue load: {len(pending_refs)}")

# --- Wrap-up ---
# After submitting all tasks, wait for the remaining tasks to complete
print("All tasks submitted, waiting for remaining tasks...")
final_results = yr.get(pending_refs)
results.extend(final_results)

print(f"All completed! Successfully processed {len(results)} tasks.")
yr.finalize()
```
