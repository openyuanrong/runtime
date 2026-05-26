# Submit Job

## Function Introduction

Submit single job, can specify tenant, affinity labels and other information through request parameters.

## URI

POST `/api/jobs`

## Request Parameters

Request Header Parameters

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ------------ | ------------ | ------------ | ------------ |
| Content-Type | Yes | string | Message body type.<br> **Value:** It is recommended to fill in application/json. |
| X-Tenant-Id | No | string | Tenant to which job belongs. |
| X-Pool-Label | No | string | Affinity resource pool label required by job instance. |

<br> Request Body Parameters

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ------------ | ------------ | ------------ | ------------ |
| entrypoint | Yes | string | Entry command used to start job. |
| metadata | No | map[string]string | User-defined metadata, used for tracking or tagging. |
| runtime_env | Yes | map[string]interface{} | Job runtime environment configuration, such as dependencies, working directory, etc.<br> Among them, **working_dir** is required, optional parameters such as env_vars can be filled. |
| entrypoint_num_cpus | No | float64 | Number of CPUs allocated to job, range: [0.3,16] cores. |
| entrypoint_memory | No | int | Amount of memory allocated to job, range: [134217728,1125899906842624] bytes. |
| entrypoint_num_gpus | No | float64 | Number of gpus allocated to job. |
| entrypoint_resources | No | map[string]float64 | Other custom resource configurations, such as npu, etc. |
| submission_id | No | string | Job instance id. If request does not carry it, backend will automatically generate one, and prefix defaults to app-. If not present, backend will automatically complete it. Format requirement: `^[a-z0-9-]{1,64}$`. |

<br> runtime_env Optional Parameters

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ------------ | ------------ | ------------ | ------------ |
| working_dir | Yes | string | Entry command used to start job.<br> There are two formats:<br> • One is to pass a local zip package file path: `file:///opt/openyuanrong/code/file.zip`, backend will decompress the zip package file under that path to a temporary directory and then reference it;<br> • The other is to pass a directory address: `/opt/openyuanrong/code`, backend will directly reference the code package under that path. |
| env_vars | No | map[string]string | User-defined metadata, used for tracking or tagging. |

## Response Parameters

| **Parameter** | **Parameter Type** | **Description** |
| ------------ | ------------ | ------------ |
| submission_id | string | Job id. |

## Request Example

POST {[frontend endpoint](api-frontend-endpoint)}/api/jobs

```json
{
  "entrypoint": "python script.py",
  "metadata": {
    "autoscenes_ids": "auto_1-test",
    "task_type": "task_1",
    "ttl": "1250"
  },
  "runtime_env": {
    "working_dir": "file:///usr1/deploy/file.zip",
    "env_vars": {
      "SOURCE_REGION": "suzhou_std",
      "DEPLOY_REGION": "suzhou_std"
    },
    "pip": ["numpy==1.24", "scipy==1.11.0"]
  },
  "entrypoint_num_cpus": 3,
  "entrypoint_memory": 134217728,
  "entrypoint_resources": {
    "NPU": 0
  },
  "submission_id": "app-1234"
}
```

## Response Example

Creation successful

```json
{
  "submission_id": "app-1234"
}
```

Creation failed, affinity node mismatch error

```text
"create app failed, submissionId:[app-xxxxx], err: \nthe instance cannot be scheduled within 18000 ms. which has been scheduled for 2 times. The latest failure: no available resource that meets the request requirements, The reasons are as follows:\n\t1 unit with [Affinity can't be Satisfied] requirements: [resource { aff { condition { subConditions { expressions { key: \"abc\" op { exits { } } } weight: 100 } orderPriority: true } } antiAff { } } ].\n"
```

## Error Codes

Please refer to [Error Codes](error-code-rest-api).
