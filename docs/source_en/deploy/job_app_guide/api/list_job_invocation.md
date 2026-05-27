# Query Job List

## Function Introduction

Query current job list, obtain detailed information of multiple jobs' driver programs, job status and other content.

## URI

GET `/api/jobs`

## Response Parameters

| **Parameter** | **Parameter Type** | **Description** |
| ------------ | ------------ | ------------ |
| type | string | Used to identify job category. Currently: SUBMISSION. |
| submission_id | string | Job id. |
| driver_info | [DriverInfo](api-data-struct-list-driverinfo) object | Detailed information of driver program associated with job. |
| status | string | Current running status of job. |
| start_time | string | Job start timestamp, unit in milliseconds. |
| end_time | string | Job end timestamp, unit in milliseconds. |
| metadata | map[string]string | Job metadata. |
| runtime_env | map[string]interface{} | Job runtime environment. |
| driver_agent_http_address | string | Not enabled yet. |
| driver_node_id | string | Id of node where driver program runs. |
| driver_exit_code | int32 | Exit code of driver program. |
| error_type | string | Class and identification of problem caused by internal error or user-provided script error. |

(api-data-struct-list-driverinfo)=

<br> DriverInfo Type Parameters

| **Parameter** | **Parameter Type** | **Description** |
| ------------ | ------------ | ------------ |
| id | string | Unique identifier of driver program. |
| node_ip_address | string | IP address of node where driver program is located. |
| pid | string | Process id of driver program process. |

## Request Example

GET {[frontend endpoint](api-frontend-endpoint)}/api/jobs

## Response Example

Query list successful

```json
[
  {
    "type": "SUBMISSION",
    "entrypoint": "python script_yr_sleep.py",
    "submission_id": "app-fe4ee1e5-d24f-4236-be15-7654e3c2229b",
    "driver_info": {
      "id": "app-fe4ee1e5-d24f-4236-be15-7654e3c2229b",
      "node_ip_address": "7.183.38.81",
      "pid": "748250"
    },
    "status": "SUCCEEDED",
    "start_time": "1755484077",
    "end_time": "1755484140",
    "metadata": {
      "autoscenes_ids": "auto_1-test",
      "task_type": "task_1",
      "ttl": "1250"
    },
    "runtime_env": {
      "envVars": "{\"DEPLOY_REGION\":\"suzhou_std\",\"SOURCE_REGION\":\"suzhou_std\"}",
      "pip": "pip3.9 install numpy==1.24 scipy==1.11.0 && pip3.9 check",
      "working_dir": "file:///home/disk/tk/file.zip"
    },
    "driver_agent_http_address": "",
    "driver_node_id": "node2-744947",
    "driver_exit_code": 0,
    "error_type": ""
  }
]
```

Query list is empty

```json
[]
```

## Error Codes

Please refer to [Error Codes](error-code-rest-api).
