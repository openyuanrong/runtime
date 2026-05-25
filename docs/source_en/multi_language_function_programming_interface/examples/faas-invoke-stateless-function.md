# Running Distributed Jobs in Function Service

Function services are commonly used for developing service-type applications, while stateless and stateful functions are often used for developing job-type applications. In practical business scenarios, a complex service request may need to complete multiple distributed jobs. A common approach is to use a function service to receive and parse external requests, then invoke stateless or stateful functions to process business logic. We introduce the implementation of such approach through an example of calculating π using the Monte Carlo method.

## Prerequisites

We use Python for development, defining a stateless function to handle point-counting tasks and a function service to receive external requests with customizable task counts. The example is deployed on an openYuanrong host cluster.

- [Deployed openYuanrong on hosts](../../deploy/deploy_processes/index.md) and configured to support function services.
- Created the same code package directory on all nodes, e.g., `/opt/code`, to store the built executable function code.

## Implementation Process

### Define Stateless Function to Handle Point-counting Tasks

Create a new file `monte_carlo.py` in the code package directory with the following content. Define a stateless function `compute_pi` to handle point-counting tasks and count the number of points falling within the circle.

```python
import yr
import random

@yr.invoke
def compute_pi(total_points: int) -> int:
    circle_points = 0
    for i in range(total_points):
        rand_x = random.uniform(-1, 1)
        rand_y = random.uniform(-1, 1)

        origin_dist = rand_x**2 + rand_y**2
        if origin_dist <= 1:
            circle_points += 1

    return circle_points
```

Use curl tool to register this stateless function. For parameter meanings, see [API documentation](../api/function_service/register_function.md):

```bash
# Replace /opt/code with your code package directory
META_SERVICE_ENDPOINT=<meta service component endpoint, default http://{master node ip}:31182>
curl -H "Content-type: application/json" -X POST -i ${META_SERVICE_ENDPOINT}/serverless/v1/functions -d '{"name":"0-baas-task","runtime":"python3.9","kind":"yrlib","cpu":600,"memory":512,"timeout":60,"storageType":"local","codePath":"/opt/code"}'
```

Record the value of the `id` field in the return format for use in the FaaS function, which corresponds to `sn:cn:yrk:default:function:0-baas-task:$latest`.

```bash
{"code":0,"message":"SUCCESS","function":{"id":"sn:cn:yrk:default:function:0-baas-task:$latest","createTime":"2025-04-28 11:31:20.986 UTC","updateTime":"","functionUrn":"sn:cn:yrk:default:function:0-baas-task","name":"0-baas-task","tenantId":"default","businessId":"yrk","productId":"","reversedConcurrency":0,"description":"","tag":null,"functionVersionUrn":"sn:cn:yrk:default:function:0-baas-task:$latest","revisionId":"20250428113120986","codeSize":0,"codeSha256":"","bucketId":"","objectId":"","handler":"","layers":null,"cpu":600,"memory":512,"runtime":"python3.9","timeout":60,"versionNumber":"$latest","versionDesc":"$latest","environment":{},"customResources":null,"statefulFlag":0,"lastModified":"","Published":"2025-04-28 11:31:20.986 UTC","minInstance":0,"maxInstance":100,"concurrentNum":100,"funcLayer":[],"status":"","instanceNum":0,"device":{},"created":""}}
```

### Define Function Service to Receive External Requests

Create a new file `service_entry.py` in the code package directory with the following content. The `handler` interface of this function service parses the `tasksNumber` parameter configured in external requests, sets the number of point-counting tasks, and invokes the stateless function `compute_pi` to run the tasks.

```python
import yr
import monte_carlo

def handler(event, context):
    tasks_number = event.get("tasksNumber")

    POINTS_PER_TASK = 10000000
    TOTAL_POINTS = tasks_number * POINTS_PER_TASK

    # Dynamically specify resources used when invoking stateless function compute_pi. If not specified, the resources configured when registering the function metadata will be used (cpu 600 millicores, memory 512 MiB)
    opt = yr.InvokeOptions(cpu=1000, memory=1000)
    results = [
        monte_carlo.compute_pi.options(opt).invoke(POINTS_PER_TASK)
        for i in range(tasks_number)
    ]

    # Calculate final result
    circle_points = sum(yr.get(results))
    pi = (circle_points * 4) / TOTAL_POINTS
    print(f"π is: {pi}")

    return pi

def init(context):
    # Configure registration id of stateless function compute_pi
    conf = yr.Config(function_id="sn:cn:yrk:default:function:0-baas-task:$latest")
    yr.init(conf)
```

Use curl tool to register the function. For parameter meanings, see [API documentation](../api/function_service/register_function.md):

```bash
# Replace /opt/code with your code package directory
META_SERVICE_ENDPOINT=<meta service component endpoint, default http://{master node ip}:31182>
curl -H "Content-type: application/json" -X POST -i ${META_SERVICE_ENDPOINT}/serverless/v1/functions -d '{"name":"0@faas@demo","runtime":"python3.9","handler":"service_entry.handler","kind":"faas","cpu":600,"memory":512,"timeout":60,"extendedHandler":{"initializer":"service_entry.init"},"extendedTimeout":{"initializer":30},"storageType":"local","codePath":"/opt/code"}'
```

Record the value of the `functionVersionUrn` field in the return format for invocation, which corresponds to `sn:cn:yrk:default:function:0@faas@demo:latest`.

```bash
{"code":0,"message":"SUCCESS","function":{"id":"sn:cn:yrk:default:function:0@faas@demo:latest","createTime":"2025-04-28 12:02:21.930 UTC","updateTime":"","functionUrn":"sn:cn:yrk:default:function:0@faas@demo","name":"0@faas@demo","tenantId":"default","businessId":"yrk","productId":"","reversedConcurrency":0,"description":"","tag":null,"functionVersionUrn":"sn:cn:yrk:default:function:0@faas@demo:latest","revisionId":"2025042812022193","codeSize":0,"codeSha256":"","bucketId":"","objectId":"","handler":"service_entry.handler","layers":null,"cpu":600,"memory":512,"runtime":"python3.9","timeout":60,"versionNumber":"latest","versionDesc":"latest","environment":{},"customResources":null,"statefulFlag":0,"lastModified":"","Published":"2025-04-28 12:02:21.930 UTC","minInstance":0,"maxInstance":100,"concurrentNum":100,"funcLayer":[],"status":"","instanceNum":0,"device":{},"created":""}}
```

### Test Application

Use curl tool to invoke the function service `0@faas@demo`. For parameter meanings, see [API documentation](../api/function_service/function_invocation.md):

```bash
FRONTEND_ENDPOINT=<frontend component endpoint, default http://{master node ip}:8888>
FUNCTION_VERSION_URN=<functionVersionUrn recorded in previous step>
curl -H "Content-type: application/json" -X POST -i ${FRONTEND_ENDPOINT}/serverless/v1/functions/${FUNCTION_VERSION_URN}/invocations -d '{"tasksNumber":2}'
```

Result output:

```bash
3.1412376
```
