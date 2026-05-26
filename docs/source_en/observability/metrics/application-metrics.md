# Application Metrics

## Function Instance Dimension

| Metric Name | Metric Meaning | Collection Cycle | Unit | Label Attributes |
|---------------------------|---------|-------|-------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------|
| yr_app_instance_status | Instance status information | Collected per instance | state (enum); Enum values: 1 (scheduling), 2 (creating), 3 (Running), 4 (Failed), 5 (Exited), 6 (Fatal), 7 (ScheduleFailed) | instance_id, node_id, ip, function_key, timestamp, agent_id, parent_id |
| yr_instance_exit_latency | Instance exit time | Collected per instance | ms | instance_id, status_code, agent_id, start_ms, end_ms |
| yr_instance_memory_usage | Instance runtime memory usage | Collected per instance | KB | instance_id, node_id, timestamp, agent_id |

### Function Instance Dimension Label Attributes Description

| Label Attribute | Description | Example |
|--------------|------------|----------------------------------------------------|
| instance_id | Instance ID | 317431e-e910-4000-8000-0000003bf404 |
| node_id | Node name | pekphis355665 |
| ip | Node IP address | 127.0.0.1 |
| timestamp | Timestamp when metric was obtained | 1694413250917 |
| function_key | Function key | 1234567890123456/0-yr-test38/$lastest |
| agent_id | Agent where function instance is located | function-agnet-large-pool-8000-16384-9dff909-zpz54 |
| parent_id | ID of object that created the instance | fhdisa1-eer0-5748-67934-7r589433bf694 |
| status_code | Execution step result status value | |
| start_ms | Start time when metric was obtained (timestamp) | 1694413250917 |
| end_ms | End time when metric was obtained (timestamp) | 1694413250999 |

## Billing Dimension

| Metric Name | Metric Meaning | Collection Cycle | Unit | Metric Attributes |
|----------------------------------------|--------|----------------|------|---------------------------------------------------------------------------------------------------------------|
| yr_app_instance_billing_invoke_latency | Function call duration | Collected per invocation | ms | request_id, function_name, status_code, start_ms, end_ms, interval_ms, pool_label, cpu_type |
| yr_instance_running_duration | Instance running duration | Periodically collected during instance runtime (15s) | ms | instance_id, cpu_type, xpu_type, init_ms, last_report_ms, report_ms, pool_label, agent_id, required_resources |

### Billing Dimension Label Attributes Description

| Label Attribute | Description | Example |
|--------------------|-----------------|------------------------------------------------------------------------------------------------------------------------------------|
| cpu_type | Processor type information | Intel(R) Xeon Gold 6278C CPU @ 2.60GHz |
| xpu_type | NPU or GPU type information | 910B4 |
| pool_label | Function instance Pod label | ["NODE_ID:pekphis355665", "app:function-agent-runtime-pools-2000-4000","resource.owner:default",reuse:true,"runtimepool5:value1"] |
| status_code | Invoke request success/failure code | 0 |
| start_ms | Function execution start time (timestamp), unit ms (milliseconds) | 1694413250950 |
| end_ms | Function execution end time (timestamp), unit ms (milliseconds) | 1694413250989 |
| interval_ms | Function execution time interval, unit ms (milliseconds) | 1360 |
| agent_id | Agent where function instance is located | function-agent-large-pool-8000-16384-9dfd976f6-zpz54 |
| request_id | Request ID | 4e524b77a5b9352100 |
| function_name | Function name | 12345678901234567890123456/0@faas001@hello/lastest |
| instance_id | Instance ID | r673481e-e910-4000-8000-0000003bf404 |
| init_ms | Instance creation time (timestamp), unit ms (milliseconds) | 1694467803989 |
| last_report_ms | Last report time (timestamp), unit ms (milliseconds) | 1694469803989 |
| report_ms | Current report time (timestamp), unit ms (milliseconds) | 1694469903989 |
| required_resources | Resources requested by instance | {"CPU":"500.000000","Memory":"500.000000","NPU/.+/count":"1.000000"} |
