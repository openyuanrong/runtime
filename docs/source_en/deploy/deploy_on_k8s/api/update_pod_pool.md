# Update Pod Resource Pool

## Function Introduction

When deploying openYuanrong cluster on K8s, update the size of a single resource pool.

## URI

PUT /serverless/v1/podpools/{id}

Path Parameters

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ---------- | ---------- | ---------- | -------------------- |
| id | Yes | String | Pod resource pool ID. |

## Request Parameters

Request Header Parameters

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ---------- | ---------- | ---------- | -------------------- |
| Content-Type | Yes | String | Message body type.<br> **Value:** It is recommended to fill in `application/json`. |

<br> Request Body Parameters

| **Parameter** | **Required** | **Parameter Type** | **Description**  |
| ---------- | ---------- | ---------- | -------------------- |
| size                           | No | int    | Pool size.<br> **Value:** Greater than or equal to 0, default value is `0`. |
| max_size                       | No | int    | Pool maximum allowed replica count. For pools without automatic scaling enabled, ignore this configuration.<br> **Value:** Greater than or equal to 0 and greater than or equal to size. |
| horizontal_pod_autoscaler_spec | No | String | Pool HPA declaration. When parameter `max_size` is greater than `size`, this configuration is not supported.<br> **Value:** Follows k8s specification and constraints. |

## Response Parameters

| **Parameter** | **Required**  | **Parameter Format** | **Description** |
| ---------- | ---------- | ---------- | -------------------- |
| code    | Yes | int    | Return code. 0 indicates success, non-0 indicates failure. |
| message | Yes | String | Error message. |

## Request Example

PUT {[meta service endpoint](api-meta-service-endpoint)}/serverless/v1/podpools/pool1

```json
{
    "minReplicas": -1,
    "maxReplicas": 2,
    "metrics": [
        {
            "resource": {
                "name": "cpu",
                "target": {
                    "averageUtilization": -20,
                    "type": "Utilization"
                }
            },
            "type": "Resource"
        },
        {
            "resource": {
                "name": "memory",
                "target": {
                    "averageUtilization": 50,
                    "type": "Utilization"
                }
            },
            "type": "Resource"
        }
    ]
}
```

## Response Example

Status Code: 200

Indicates update success.

```json
{
    "code": 0,
    "message": ""
}
```

## Error Codes

Please refer to [Error Codes](error-code-rest-api).
