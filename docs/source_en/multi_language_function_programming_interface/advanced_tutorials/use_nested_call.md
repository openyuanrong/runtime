# Nested Calls

Nested calls help with the concurrency of sub-jobs. It works by automatically invoking nested jobs remotely, enabling concurrent execution of nested jobs.

Nested jobs also consume certain resources at runtime, including increased load management from the master node to function agents and runtime management of function instances by function agents. There should be a trade-off between job volume and runtime performance; overly fine-grained jobs will reduce execution speed.

## Usage Example

```python
# demo.py
import yr

yr.init()

@yr.invoke
def recursive_process(data_list):
    # Recursion termination condition
    if len(data_list) <= 10:
        return sum(data_list)

    # Split the task into two subtasks (divide and conquer)
    mid = len(data_list) // 2
    left_part = data_list[:mid]
    right_part = data_list[mid:]

    # Dynamically submit subtasks: this is "nested parallelism"
    left_ref = recursive_process.invoke(left_part)
    right_ref = recursive_process.invoke(right_part)

    # Wait for subtask results and merge
    # Note: In large-scale applications, using yr.wait here can further optimize memory
    results = yr.get([left_ref, right_ref])
    return sum(results)

# Launch top-level task
data = list(range(100))
final_result = yr.get(recursive_process.invoke(data))
print(f"Final calculation result: {final_result}")

yr.finalize()
```
