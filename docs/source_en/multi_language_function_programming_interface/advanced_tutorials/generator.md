# openYuanrong Generators

In Python, a function that uses the `yield` keyword is called a generator. A generator function is a special type of function that can produce results incrementally during iteration, rather than returning all values at once. All generators decorated with `yr.instance` or `yr.invoke` are called openYuanrong generators.

## Use Cases

Generators calculate and return only one value per iteration without loading all values into memory at once, effectively saving memory space. Common use cases include:

- Infinite sequences: Generators can be used to generate infinite sequences without exhausting memory.
- Data stream processing: Generators can be used to process large data streams, such as reading data line by line from a file or receiving data from a network stream.

## Usage Example

openYuanrong generators return an `ObjectRefGenerator` object, through which you can call `_next` and `_anext` methods, or iterate through it.

Iteration operations may return the following exceptions:

| **Type** | **Description** |
| -----------   | ------------------   |
| RuntimeError  | Failed to get yield return value. |
| YRInvokeError | Function execution error. |

:::{note}

Async generators can only be implemented in stateful functions; stateless functions do not support async.

:::

### Stateless Function Generator

```python
import yr

yr.init()

@yr.invoke
def countdown(n):
    while n > 0:
        yield n
        n -= 1

n = 5
# Use next interface
generator = countdown.invoke(n)
print(yr.get(next(generator)))

# Use for loop
for ref in generator:
    print(yr.get(ref))

yr.finalize()
```

### Stateful Function Generator

```python
import yr

yr.init()

@yr.instance
class StatefulFunc():
    def __init__(self, number):
        self.sum = number

    def countdown(self):
        n = self.sum
        while n > 0:
            yield n
            n -= 1

n = 5
instance = StatefulFunc.invoke(n)

# Use next interface
generator = instance.countdown.invoke()
print(yr.get(next(generator)))

# Use for loop
for ref in generator:
    print(yr.get(ref))

instance.terminate()
yr.finalize()
```

### Stateful Function Async Generator

```python
import asyncio
import time
import yr

yr.init()

@yr.instance
class StatefulFunc():
    async def gen(self, n):
        for i in range(n):
            await asyncio.sleep(1)
            yield i

instance = StatefulFunc.invoke()

async def async_gen(n):
    gen = instance.gen.invoke(n)
    async for ref in gen:
        res = await ref
        print(res)

async def multi_gen():
    n = 5
    await asyncio.gather(async_gen(n), async_gen(n), async_gen(n), async_gen(n), async_gen(n))

now = time.time()
asyncio.run(multi_gen())
cost = time.time() - now
# Concurrently execute 5 generator functions, each taking 5s, total time less than 6s
print(cost)

instance.terminate()
yr.finalize()
```
