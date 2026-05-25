# Implementing MapReduce Based on openYuanrong

MapReduce decomposes computational tasks into two main phases: Map and Reduce, thereby simplifying parallel processing of large-scale datasets. The distributed programming interface provided by openYuanrong can easily implement the MapReduce programming model for big data processing.

This example demonstrates how to implement MapReduce based on openYuanrong through a simple word frequency counting example, including:

- How to parallelize multiple Map tasks using stateless functions.
- How to use data objects to pass results from multiple Map tasks for the Reduce process.

## Solution Overview

Main phases of word frequency counting:

1. Map Phase
   Input data is split into multiple chunks (Splits), each Map task processes one data chunk, separately counting word frequencies

2. Shuffle Phase
   Intermediate results from Map output are sorted and grouped, aggregating values with the same key together for Reduce task processing

3. Reduce Phase
   The Reduce function performs reduction operations on values with the same key, for example, in word frequency counting, merging the frequencies of the same word

## Prerequisites

Deploy openYuanrong cluster: [Deploy on Hosts](../../deploy/deploy_processes/index.md)

## Implementation Process

### Text Loading and Splitting

We load the Zen of Python text content through "import this" and split it into a specified number of partitions.

```python
import sys
import subprocess

zen_of_python = subprocess.check_output([sys.executable, "-c", "import this"])
corpus = zen_of_python.decode('utf-8').strip().split()

num_partitions = 3
chunk = len(corpus) // num_partitions
partitions = [
    corpus[i * chunk: (i + 1) * chunk] for i in range(num_partitions)
]
```

### Mapping

We define a stateless function using the `@yr.invoke` decorated ordinary function `apply_map` to process data chunks in distributed parallel.

```python
@yr.invoke
def apply_map(corpus, num_partitions=3):
    map_results = [list() for _ in range(num_partitions)]
    for document in corpus:
        for result in map_function(document):
            first_letter = result[0].decode("utf-8")[0]
            word_index = ord(first_letter) % num_partitions
            map_results[word_index].append(result)
    return map_results
```

### Shuffling and Reducing

The goal of the reduce phase is to aggregate all results from return values to the same node. We create a dictionary to sum up all words appearing on each partition.

```python
@yr.invoke
def apply_reduce(results):
    reduce_results = dict()
    for res in results:
        for _, value_list in enumerate(yr.get(res)):
           for _, value_tuple in enumerate(value_list):
                key = value_tuple[0]
                value = value_tuple[1]
                if key not in reduce_results:
                    reduce_results[key] = 0
                reduce_results[key] += value

    return reduce_results
```

### Define Main Flow

The main flow initializes the openYuanrong runtime context through `yr.init()`. You can adjust the number of tasks according to your cluster size.

Finally, aggregate and output the results, and call `yr.finalize()` to clean up the context.

```python
import sys
import subprocess
import yr

if __name__ == '__main__':
    yr.init()

    # Load zen of python data
    zen_of_python = subprocess.check_output([sys.executable, "-c", "import this"])
    corpus = zen_of_python.split()

    # Split data into 3 chunks
    num_partitions = 3
    chunk = len(corpus) // num_partitions
    partitions = [
        corpus[i * chunk: (i + 1) * chunk] for i in range(num_partitions)
    ]

    # Invoke remote function apply_map to process data in parallel
    map_results = [
        apply_map.invoke(data, num_partitions)
        for data in partitions
    ]

    # Invoke remote function apply_reduce to process mapping results
    reduce_result = apply_reduce.invoke(map_results)
    counts = yr.get(reduce_result)

    # Sort and output results
    sorted_counts = sorted(counts.items(), key=lambda item: item[1], reverse=True)
    for count in sorted_counts:
        print(f"{count[0].decode('utf-8')}: {count[1]}")

    yr.finalize()
```

### Run Program

:::
:::{dropdown} Complete Code
:chevron: down-up
:icon: chevron-down

```python
import sys
import subprocess
import yr

@yr.invoke
def apply_map(corpus, num_partitions=3):
    map_results = [list() for _ in range(num_partitions)]
    for document in corpus:
        for result in map_function(document):
            first_letter = result[0].decode("utf-8")[0]
            word_index = ord(first_letter) % num_partitions
            map_results[word_index].append(result)
    return map_results

@yr.invoke
def apply_reduce(results):
    reduce_results = dict()
    for res in results:
        for _, value_list in enumerate(yr.get(res)):
           for _, value_tuple in enumerate(value_list):
                key = value_tuple[0]
                value = value_tuple[1]
                if key not in reduce_results:
                    reduce_results[key] = 0
                reduce_results[key] += value

    return reduce_results

def map_function(document):
    for word in document.lower().split():
        yield word, 1

if __name__ == '__main__':
    yr.init()

    # Load zen of python data
    zen_of_python = subprocess.check_output([sys.executable, "-c", "import this"])
    corpus = zen_of_python.split()

    # Split data into 3 chunks
    num_partitions = 3
    chunk = len(corpus) // num_partitions
    partitions = [
        corpus[i * chunk: (i + 1) * chunk] for i in range(num_partitions)
    ]

    # Invoke remote function apply_map to process data in parallel
    map_results = [
        apply_map.invoke(data, num_partitions)
        for data in partitions
    ]

    # Invoke remote function apply_reduce to process mapping results
    reduce_result = apply_reduce.invoke(map_results)
    counts = yr.get(reduce_result)

    # Sort and output results
    sorted_counts = sorted(counts.items(), key=lambda item: item[1], reverse=True)
    for count in sorted_counts:
        print(f"{count[0].decode('utf-8')}: {count[1]}")

    yr.finalize()
```

:::

Running the above Python program on an environment with openYuanrong deployed produces the following output:

```bash
is: 10
better: 8
than: 8
the: 6
to: 5
of: 3
although: 3
be: 3
special: 2
unless: 2
one: 2
should: 2
do: 2
may: 2
never: 2
way: 2
if: 2
implementation: 2
idea.: 2
a: 2
explain,: 2
ugly.: 1
implicit.: 1
complex.: 1
complex: 1
complicated.: 1
flat: 1
readability: 1
counts.: 1
cases: 1
rules.: 1
python,: 1
peters: 1
simple: 1
sparse: 1
dense.: 1
aren't: 1
zen: 1
by: 1
tim: 1
beautiful: 1
explicit: 1
nested.: 1
enough: 1
break: 1
in: 1
face: 1
refuse: 1
one--: 1
only: 1
--obvious: 1
it.: 1
obvious: 1
first: 1
practicality: 1
purity.: 1
pass: 1
silently.: 1
silenced.: 1
ambiguity,: 1
guess.: 1
and: 1
preferably: 1
at: 1
you're: 1
dutch.: 1
beats: 1
errors: 1
explicitly: 1
temptation: 1
there: 1
that: 1
not: 1
now: 1
often: 1
*right*: 1
it's: 1
it: 1
idea: 1
--: 1
let's: 1
good: 1
are: 1
great: 1
more: 1
never.: 1
now.: 1
hard: 1
bad: 1
easy: 1
namespaces: 1
honking: 1
those!: 1

```
