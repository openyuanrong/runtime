# Configuring Functions

This section introduces the available configuration items for function services.

## Configuring Environment Variables

By configuring environment variables, different code snippets can be executed without modifying the code. Environment variables are stored as string key-value pairs as part of the function configuration, and different functions have independent environment variables.

Environment variables are encrypted and stored using AES256 (Advanced Encryption Standard 256) standard in the openYuanrong system. When initializing function instances, environment variables are decrypted and injected into the function instance context.

Environment variable keys must start with uppercase or lowercase letters and can only contain uppercase letters, lowercase letters, and numbers.

Environment variables are configured through the [Register Function](../../api/function_service/register_function.md) and [Update Function](../../api/function_service/update_function.md) APIs, with the request parameter `environment`, as shown in the following example.

```json
{
    "environment": {
        "key1": "value1"
    }
}
```

Retrieve environment variables through the `getUserData` interface of the `context`. 

```python
context.getUserData('key1')
```

## Configuring Single Instance Concurrency

Concurrency is the maximum number of threads allocated by openYuanrong to a single function instance, that is, the number of requests each instance can handle simultaneously.

By default, a function instance can only process one request at a time, processing the next request after the current one completes. By configuring single instance concurrency, a function instance can concurrently handle multiple requests, reducing the number of instances needed to process the same number of requests, and to some extent reducing function cold start latency.

Single instance concurrency is configured through the [Register Function](../../api/function_service/register_function.md) and [Update Function](../../api/function_service/update_function.md) APIs, with the request parameter `concurrentNum`, as shown in the following example.

```json
{
    "concurrentNum": "2"
}
```

## Configuring Maximum Instance Count

openYuanrong will scale out based on function instance load, up to the configured maximum function instance count. By configuring the maximum instance count, you can control traffic and the maximum resource usage of functions.

Maximum instance count is configured through the [Register Function](../../api/function_service/register_function.md) and [Update Function](../../api/function_service/update_function.md) APIs, with the request parameter `maxInstance`, as shown in the following example.

```json
{
    "maxInstance": "10"
}
```

## Configuring Reserved Instance Count

Reserved instances are the minimum number of instances allocated by openYuanrong to a function, representing the minimum number of function instances that must run. After function creation, the corresponding number of function instances are pre-started. openYuanrong will scale in function instances when request volume decreases, but will maintain at least the minimum number of instances to handle user requests. Although reserved instances will occupy a fixed portion of resources, they reduce the process of starting instances when handling requests, thereby reducing latency.

Reserved instance count is configured through the [Register Function](../../api/function_service/register_function.md) and [Update Function](../../api/function_service/update_function.md) APIs, with the request parameter `minInstance`, as shown in the following example.

```json
{
    "minInstance": "3"
}
```

## Configuring Reserved Instance Labels

You can configure labels for reserved instances and specify request scheduling to instances with those labels during invocation. By configuring different numbers of reserved instances with labels for different use cases, you can achieve finer-grained request scheduling strategies.

Related REST APIs:

- [Create Reserved Instance Configuration](../../api/function_service/create_reserve_instance.md). 
- [Update Reserved Instance Configuration](../../api/function_service/update_reserve_instance.md).
- [Query Reserved Instance Configuration](../../api/function_service/get_reserve_instance.md).
- [Delete Reserved Instance Configuration](../../api/function_service/delete_reserve_instance.md).
