# Query Pod Resource Pool

## Function Introduction

When deploying openYuanrong cluster on K8s, query created pod resource pools.

## URI

GET /serverless/v1/podpools?id={id}&group={group}&offset={offset}&limit={limit}

Query Parameters

| **Parameter** | **Required**  | **Parameter Type**  | **Description** |
| ---------- | ---------- | ---------- | -------------------- |
| id     | No | String | Pod resource pool ID, exact match. |
| group  | No | String | Pod resource pool group, exact match. |
| offset | No | int    | Query start page.<br> **Value:** Greater than or equal to 0, returns empty when less than 0. When not filled or filled with 0, default value is `1`.|
| limit  | No | int    | Query page size.<br> **Value:** Greater than 0, returns empty when less than or equal to 0. When not filled, default value is `10`. |

## Request Parameters

None

## Response Parameters

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ---------- | ---------- | ---------- | -------------------- |
| code    | Yes | int                                               | Return code, 0 indicates success, non-0 indicates failure. |
| message | Yes | String                                            | Error message. |
| result  | Yes | [QueryResult](api-data-struct-queryresult) object | Query result. |

(api-data-struct-queryresult)=

<br> QueryResult Type Parameters

| **Parameter** | **Required**  | **Parameter Type** | **Description** |
| ---------- | ---------- | ---------- | -------------------- |
| count | Yes | int                                                   | Number of resource pools matching query conditions. |
| pools | No | Array of [PoolInfo](api-data-struct-poolinfo) objects | Pool information list. |

(api-data-struct-poolinfo)=

<br> PoolInfo Type Parameters

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ---------- | ---------- | ---------- | -------------------- |
| id                             | Yes | String                   | Pool ID. |
| group                          | Yes | String                   | Group. |
| reuse                          | No | bool                     | Whether pool is reusable. When pod is used by function instance exclusively, if reuse is false, pool will immediately replenish new pod, otherwise need to wait until instance pod is destroyed before replenishing new pod. |
| status                         | Yes | int                      | Resource pool status.<br> **Value:** 0 (Pending), 1 (Creating), 2 (Pending Update), 3 (Updating), 4 (Running), 5 (Creation Failed), 6 (Pending Deletion). |
| msg                            | Yes | String                   | Resource pool status information. |
| ready_count                    | Yes | int                      | Number of pods in running status. |
| size                           | Yes | int                      | Pool size. |
| max_size                       | Yes | int                      | Pool maximum replica count. |
| image                          | No | String                   | runtime-manager component image tag. |
| init_image                     | No | String                   | function-agent-init component image tag. |
| labels                         | No | Map[String,String]       | Custom pool labels. |
| environment                    | No | Map[String,String]       | Custom runtime-manager container environment variables. |
| volumes                        | No | String                   | Volume declarations needed by pool, supports HostPath and PVC. |
| volume_mounts                  | No | String                   | Volume mount declarations needed by pool. |
| resources                      | Yes | [ResourceRequirement](api-data-struct-resourcerequirement-get) object | Pool resource declaration. |
| affinities                     | No | String                   | Pool affinity declaration. |
| node_selector                  | No | Map[String,String]       | Node label matching, pool will select specific nodes for scheduling. |
| runtime_class_name             | No | String                   | Container runtime class name. |
| tolerations                    | No | String                   | Taints that pool can tolerate. |
| horizontal_pod_autoscaler_spec | No | String                   | Pool HPA declaration. |
| topology_spread_constraints    | No | String                   | Pool topology spread constraints. |
| idle_recycle_time              | No | [IdleRecyclePolicy](api-data-struct-idlerecyclepolicy-get) object | Configured automatic scaling pod idle recycling time. |

(api-data-struct-idlerecyclepolicy-get)=

<br> IdleRecyclePolicy Type Parameters

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ---------- | ----- | ---------- | ------------------------- |
| reserved | No | int | Reserved pod idle recycling time. Unit in seconds. -1 means never recycle, 0 means idle recycling not configured. |
| scaled   | No | int | Scaled pod idle recycling time. Unit in seconds. -1 means never recycle, 0 means idle recycling not configured. |

(api-data-struct-resourcerequirement-get)=

<br> ResourceRequirement Type Parameters

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ---------- | ----- | ---------- | ------------------------- |
| requests | Yes | Map[String,String] | Pod request resource definition. cpu unit m, represents milli-core; mem unit Mi or Gi. |
| limits   | Yes | Map[String,String] | Pod limit resource definition. cpu unit m, represents milli-core; mem unit Mi or Gi. |

## Request Example

GET {[meta service endpoint](api-meta-service-endpoint)}/serverless/v1/podpools?id=pool1&group=rg1

## Response Example

Status Code: 200

Query created successfully.

```json
{
    "code": 0,
    "message": "",
    "result": {
        "count": 1,
        "pools": [
            {
                "id": "pool1",
                "group": "rg1",
                "size": 1,
                "max_size": 3,
                "ready_count": 1,
                "status": 4,
                "msg": "Running",
                "image": "cd-docker-hub.szxy5.artifactory.cd-cloud-artifact.tools.xxx.com/yuanrong_euleros_x86/runtime-manager:201.2.0.B992.20240301163827-zyw-1",
                "init_image": "",
                "reuse": true,
                "labels": null,
                "environment": null,
                "resources": {
                    "requests": {
                        "cpu": "600m",
                        "memory": "512Mi"
                    },
                    "limits": {
                        "cpu": "600m",
                        "memory": "512Mi"
                    }
                },
                "volumes": "",
                "volume_mounts": "",
                "pod_anti_affinities": "",
                "horizontal_pod_autoscaler_spec": "",
                "topology_spread_constraints": "",
                "idle_recycle_time": {
                    "reserved": 80,
                    "scaled": 20
                }
            }
        ]
    }
}
```

## Error Codes

Please refer to [Error Codes](error-code-rest-api).
