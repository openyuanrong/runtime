# Logs

This section introduces openYuanrong's logging system.

## Logs on Host Deployment Environment

The default log paths and files for master and worker nodes are as follows. Each deployment creates a new log directory using a timestamp, such as `20250520091445`, where the log directory stores component and function logs. You can also refer to the [Deployment Parameters Table](../deploy/deploy_processes/parameters.md) to customize log paths and log levels.

Master node:

```bash
/tmp/yr_sessions
├── 20250520091445
├── 20250520091612
└── latest
    ├── deploy_std.log
    └── log
```

Worker node:

```bash
/tmp/yr_sessions
├── 20250520091445
├── 20250520091612
└── latest
    ├── deploy_std.log
    └── {node_id}
        └── log
```

### Deployment Logs

The deployment log file default path is `/tmp/yr_sessions/latest/deploy_std.log`. You can view deployment parameters and processes through this log to locate deployment issues.

### Function Logs

Function logs are in the log directory. Function standard output is merged into a single file `{node_id}-user_func_std.log` by default. When you configure the `--enable_separated_redirect_runtime_std=true` option during deployment, each function execution's standard output will generate a separate `runtime-{runtime_id}.out` file.

Function processes also contain some platform logic such as code loading, outputting logs as `job-{job_id}-runtime-{runtime_id}.log` and `runtime-{runtime_id}.log`. When function instances fail to start normally, you can check these logs to locate configuration errors such as function package path issues.

### Component Logs

Component logs are in the log directory:

- function master component: Contains `{node_id}-function_master.log` and `{node_id}-function_master_std.log` files, only exists on master node.
- function scheduler component: Contains `{node_id}-scheduler_libruntime.log`, `faasscheduler.so-run.{timestamp}.log`, and `{node_id}-scheduler_std.log` files, only exists on master node.
- frontend component: Contains `{node_id}-faas_frontend_libruntime.log`, `faasfrontend.so-run.{timestamp}.log`, and `{node_id}-faas_frontend_std.log` files, only exists on master node.
- meta service component: Contains `meta-service-run.{timestamp}.log` and `{node_id}-metaservice_std.log` files, only exists on master node.
- dashboard component: Contains `dashboard-run.{timestamp}.log` and `{node_id}-dashboard_std.log` files, only exists on master node.
- function proxy component: Contains `{node_id}-function_proxy.log` and `{node_id}-function_proxy_std.log` files.
- function agent and runtime manager component: Deployed in the same process by default, contains `{node_id}-function_agent.log` file.
- data worker component: `worker.{INFO|WARNING}.log` and `ds_worker_std.log` files exist on both master and worker nodes. `ds_master_std.log` and `master/worker.{INFO|WARNING}.log` files only exist on master node.
- collector component: Contains `{node_id}-collector_std.log` file.
- etcd component: Contains `etcd-run.log` file, only exists on master node.

## Logs on K8s Deployment Environment

openYuanrong component and function logs are mounted to the host by default. The [Function System](glossary-functionsystem) mount path is `/var/paas/sys/log/cff/default`, and the [Data System](glossary-datasystem) mount path is `/home/sn/datasystem/logs`.

Function system logs:

```bash
/var/paas/sys/log/cff/default
├── componentlogs
├── processrouters
│   └── stdlogs
└── servicelogs
```

Data system logs:

```bash
/home/sn/datasystem
└── logs
```

### Mounted Function Logs

The function log default path is `/var/paas/sys/log/cff/default/processrouters/stdlogs`. Function standard output is generated in the `function-agent-{pools_id}-xxx/{node_id}-user-func_std.log` file under the directory corresponding to the resource pool Pod name. Functions running in the same Pod have merged logs into a single file.

Function processes also contain some platform logic such as code loading, outputting logs with default path `/var/paas/sys/log/cff/default/servicelogs`. In the same resource pool Pod name directory, each function run generates files named `job-{job_id}-runtime-{runtime_id}.log` and `runtime-{runtime_id}.log`. When function instances fail to start normally, you can check these logs to locate configuration errors such as function package path issues.

### Mounted Component Logs

- function scheduler component: Includes files in `componentlogs` and `servicelogs` under Pod name `function-agent-xxx-faasscheduler-xxx` subdirectory.
- function manager component: Includes files in `componentlogs` and `servicelogs` under Pod name `function-agent-xxx-faasmanager-xxx` subdirectory.
- frontend component: Includes files in `componentlogs` and `servicelogs` under Pod name `function-agent-xxx-faasfrontend-xxx` subdirectory.
- function master component: Contains files in `componentlogs` under Pod name `function-master-xxx` subdirectory.
- meta service component: Contains files in `componentlogs` under Pod name `meta-service-xxx` subdirectory.
- IAM adaptor component: Contains files in `componentlogs` under Pod name `iam-adaptor-xxx` subdirectory.
- function proxy component: Contains files in `componentlogs` under Pod name `function-proxy-xxx` subdirectory.
- function agent and runtime manager component: Deployed in the same Pod. In each resource pool Pod name corresponding subdirectory `function-agent-{pools_id}-xxx` under `componentlogs`, contains `{node_id}-function_agent.log` and `{node_id}-runtime_manager.log` two files.
- data worker component: Data system component, contains files in `worker` subdirectory under default path `/home/sn/datasystem/logs`.

## Using openYuanrong's Logger

Function services support using openYuanrong's logger. You can print logs through the function's context method `context.getLogger()`, which will produce the same output format as openYuanrong component logs. Taking Java function services as an example, the code and log output are as follows.

```java
package org.yuanrong.demo;

import org.yuanrong.services.runtime.Context;
import org.yuanrong.services.runtime.RuntimeLogger;
import com.google.gson.JsonObject;

public class Demo {
    // Function execution entry point, executed on each request, where input parameter and function return type can be customized
    public String handler(JsonObject event, Context context) {
        RuntimeLogger log = context.getLogger();
        log.info("received request,event content:" + event);

        return event.toString();
    }

    // Function initialization entry point, executed once when function instance starts
    public void initializer(Context context) {
        RuntimeLogger log = context.getLogger();
        log.info("function instance initialization completed");
    }
}
```

If the request body is set to {"name":"yuanrong"}, during host deployment, the following log content can be seen in the default log file `/tmp/yr_sessions/latest/log/runtime-{runtime_id}/user-log.log`.

```bash
[16:41:35:820] | [INFO] | org.yuanrong.services.logger.UserFunctionLogger.info(UserFunctionLogger.java:68) | Thread-1 | userLog | function instance initialization completed
[16:41:35:820] | [INFO] | org.yuanrong.executor.FaaSHandler.faasInitHandler(FaaSHandler.java:288) | Thread-1 | userLog | faas init handler complete.
[16:41:35:827] | [INFO] | org.yuanrong.executor.FaaSHandler.execute(FaaSHandler.java:174) | Thread-2 | userLog | executing udf methods, current type: InvokeFunctionStateless
[16:41:35:828] | [INFO] | org.yuanrong.executor.FaaSHandler.faasCallHandler(FaaSHandler.java:300) | Thread-2 | userLog | faas call handler called.
[16:41:35:832] | [INFO] | org.yuanrong.services.logger.UserFunctionLogger.info(UserFunctionLogger.java:68) | Thread-2 | userLog | received request,event content:{"name":"yuanrong"}
```
