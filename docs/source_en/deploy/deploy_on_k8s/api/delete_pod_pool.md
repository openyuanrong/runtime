# Delete Pod Resource Pool

## Function Introduction

When deploying openYuanrong cluster on K8s, delete created resource pools.

## URI

DELETE /serverless/v1/podpools?id={id}&group={group}

Query Parameters

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ---------- | ---------- | ---------- | -------------------- |
| id    | No, if not set, then `group` parameter must be set. | String | Pod resource pool ID. |
| group | No, if not set, then `id` parameter must be set.    | String | Pod resource pool group.<br> **Constraint:** When only this parameter is set, all resource pools in the group will be deleted. |

## Request Parameters

None

## Response Parameters

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ---------- | ---------- | ---------- | -------------------- |
| code    | Yes | int    | Return code. 0 indicates success, non-0 indicates failure. |
| message | Yes | String | Error message. |

## Request Example

DELETE {[meta service endpoint](api-meta-service-endpoint)}/serverless/v1/podpools?id=pool1

## Response Example

Status Code: 200

Indicates deletion success.

```json
{
    "code": 0,
    "message": ""
}
```

## Error Codes

Please refer to [Error Codes](error-code-rest-api).
