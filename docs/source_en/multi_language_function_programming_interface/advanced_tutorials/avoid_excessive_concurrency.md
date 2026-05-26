# Avoid Excessive Concurrency

Compared to ordinary function calls, concurrent or distributed invocations require more workload. Concurrently calling overly fine-grained jobs may be less efficient than ordinary function calls. To avoid this issue, you need to carefully design functions and plan concurrent jobs. If you need to concurrently call a fine-grained job, you can use batch processing to let each invocation handle more effective work.

## Usage Example

```python
# demo.py
import time
import numpy as np
import yr

yr.init()


def process_ordinary(batch):
    time.sleep(0.00001)
    # Use numpy vectorized computation, extremely fast
    return batch * 2


@yr.invoke
def process_batch(batch):
    # Use numpy vectorized computation, extremely fast
    return process_ordinary(batch)


# Split data into larger chunks
datas = np.arange(1000)

start_time = time.time()
results = [process_ordinary(data) for data in datas]
end_time = time.time()
print(f"Ordinary function call takes {end_time - start_time} seconds")
# Ordinary function call takes 0.07148480415344238 seconds

start_time = time.time()
results = yr.get([process_batch.invoke(data) for data in datas])
end_time = time.time()
print(f"Parallelizing tasks takes {end_time - start_time} seconds")
# Parallelizing tasks takes 6.002047777175903 seconds

chunks = np.array_split(datas, 10)  # Split into 10 large tasks
start_time = time.time()
results = yr.get([process_batch.invoke(chunk) for chunk in chunks])  # Only 10 scheduling overhead, extremely high computation efficiency
end_time = time.time()
print(f"Parallelizing tasks with batching takes {end_time - start_time} seconds")
# Parallelizing tasks with batching takes 0.016678571701049805 seconds

yr.finalize()
```

The above example demonstrates that the efficiency of concurrent scheduling is not as good as ordinary calls. However, through batch processing, each invocation in the concurrency handles more jobs, thereby achieving the expected processing efficiency.
