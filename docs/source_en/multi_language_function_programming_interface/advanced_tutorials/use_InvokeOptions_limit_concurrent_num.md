# Using Resource Limits to Control Task Concurrency

Jobs (stateless and stateful) occupy 500 millicores of `cpu` and 500 MiB of `memory` by default. When a job requires computing power exceeding the configured resources, it is likely to be affected by other concurrently executing jobs, causing slower execution, but it can still complete stably.

The memory footprint of these jobs will grow proportionally, which can cause the total resource usage to exceed the node's memory threshold or lead to OOM. To avoid this problem, you can configure larger resource amounts to reduce the number of concurrent jobs on a single node, providing resource redundancy for job execution. openYuanrong's scheduling mechanism ensures that the total amount of resources requested by all concurrently executing jobs does not exceed the node's threshold.

## Usage Example

The example demonstrates a method of using openYuanrong's distributed calls to load large files into memory and process data. When the concurrency of such jobs is too high, it can lead to OOM. Use openYuanrong to explicitly set `memory` and `cpu` resource sizes for each function to limit function concurrency.

:::{note}

Memory resources are logically defined, not physically defined. openYuanrong does not force job execution memory usage to be within the `memory` requested amount.

:::

```python
import yr
import time

# Initialize yr
yr.init()

# 1GB = 1024 * 1024 * 1024 bytes
# Simulate a function that processes large files
@yr.invoke
def process_large_file(file_name):
    print(f"Processing file: {file_name}, expected memory usage > 3GB...")

    # Simulate loading 3GB data into heap memory
    # In real scenarios, this might be pd.read_csv() or np.load()
    data = "X" * (3 * 1024 * 1024 * 1024)

    time.sleep(5)  # Simulate computation time
    result_size = len(data)

    # Release large object reference to help GC reclaim promptly
    del data
    return f"File {file_name} processing completed, size: {result_size // (1024**3)} GB"

# --- Scheduling Logic ---
file_list = [f"file_{i}.dat" for i in range(10)]
pending_refs = []
max_in_flight = 3

# We set logical requirement to 4GB, leaving room for memory jitter
opt = yr.InvokeOptions()
opt.memory = 4 * 1024

print("Start batch processing large files...")

for file in file_list:
    # If there are too many in-flight tasks, wait for one to complete first to free up logical resource slots
    if len(pending_refs) >= max_in_flight:
        ready, pending_refs = yr.wait(pending_refs, wait_num=1)
        print(f"Completed task: {yr.get(ready[0])}")

    # Submit task
    # yr will check if the current node's remaining 'memory' resource meets the 4GB requirement
    # If not met, this task will remain in Pending state until other tasks release resources
    ref = process_large_file.options(opt).invoke(file)
    pending_refs.append(ref)

# Wait for remaining tasks
results = yr.get(pending_refs)
print("All large file processing completed!")
yr.finalize()
```
