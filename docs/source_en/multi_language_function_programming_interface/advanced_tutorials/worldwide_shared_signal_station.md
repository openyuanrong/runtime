# Using Stateful Functions as Global Signal Stations

In distributed systems, since different tasks may run on different physical nodes, Python's native asyncio.Event cannot work across processes. Through openYuanrong stateful functions, we can create a globally shared signal station, allowing hundreds or thousands of distributed tasks to simultaneously "listen" for the occurrence of a single event.

## Core Principle

- Stateful function as a central node: Maintains a real asyncio.Event object.
- Method subscription: Other tasks call the stateful function's wait() method, which awaits the internal event.
- Broadcast trigger: When a task calls the stateful function's set(), all awaiting tasks are simultaneously awakened.

## Scenarios

- Synchronized startup (Barrier Synchronization): For example, in distributed training, ensuring all nodes have loaded the model before starting training simultaneously.
- Configuration updates: When configuration changes occur, notify all running stateful functions to reload configuration via Event.
- Dependency trigger: Task B must wait for a certain intermediate step of Task A to complete before continuing, but Task A has not ended (cannot use `yr.get()` result as a trigger point).

## Usage Example

```python
import yr
import asyncio
import time


# 1. Define distributed event center
@yr.instance
class SharedEvent:
    def __init__(self):
        # Core: Use asyncio.Event to drive asynchronous non-blocking waiting
        self.event = asyncio.Event()

    async def wait(self):
        """Distributed tasks suspend here waiting"""
        print("Signal center: Received a wait request...")
        await self.event.wait()
        return "SIGNAL_RECEIVED"

    async def set(self):
        """Trigger event, all waiting tasks will launch simultaneously"""
        self.event.set()
        print("Signal center: Broadcast signal sent!")

    async def clear(self):
        """Reset signal for next round of synchronization"""
        self.event.clear()


# 2. Define distributed Worker
@yr.invoke
def heavy_worker(worker_id, event_actor):
    print(f"Worker {worker_id}: Preparing basic environment (e.g., loading model)...")
    time.sleep(1)  # Simulate preparation work

    print(f"Worker {worker_id}: Ready, blocking and waiting for global startup signal...")
    # This will initiate an asynchronous wait to the Actor over the network
    yr.get(event_actor.wait.invoke())

    print(f"Worker {worker_id}: Received signal, starting parallel task execution!")
    return f"Worker {worker_id} Success"


# --- 3. Execution Flow ---
yr.init()

# Create a globally unique signal center
event_center = SharedEvent.invoke()

# Launch 5 Workers distributed across the cluster
worker_ids = range(5)
futures = [heavy_worker.invoke(i, event_center) for i in worker_ids]

print("\n--- Main program: Wait 3 seconds then release all at once ---")
time.sleep(3)

# Trigger signal, wake up all Workers blocked at wait()
event_center.set.invoke()

# View execution results
results = yr.get(futures)
print("\nAll tasks completed:", results)

yr.finalize()
```
