# Query Specific Version of Function

## Description

This API is used to query a function in the openYuanrong cluster by calling the meta_service interface.

## Constraints

None

## URI

`GET /serverless/v1/functions/{name}?versionNumber={version}`

### Path Parameters

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| name | String | Yes | Function name. |

### Query Parameters

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| version | String | Yes | Version name, exact match. |

## Request Parameters

### Request Header Parameters

None

### Request Body Parameters

None

## Response Parameters

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| code | int | Yes | Return code, ``0`` indicates successful query, non-``0`` indicates query failure. For more information, see [Error Codes](error-code-rest-api).  |
| message | String | Yes | Return error message. |
| function | GetFunctionResult | Yes | Query result. |

### GetFunctionResult Object

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| id | int | Yes | Function ID.  |
| functionVersionUrn | String | Yes | Function version URN, used for invoking the function. |
| revisionId | String| Yes | Function revisionId, used for publishing the function. |
| name | String | Yes | Function name.|
| createTime | Date | Yes | Function creation time.|
| updateTime | Date | Yes | Function update time.|
| versionNumber | String | Yes | Version number.|
| runtime | String | Yes | Function runtime type. |
| description | String | No | Function description.  |
| handler | String | Yes | Call handler. |
| cpu | int | Yes | Function CPU size, unit: `m (millicores)`. |
| memory | int | Yes | Function MEM size, unit: `MB`. |
| timeout | int | Yes | Function invocation timeout. |
| customResources | map | No | Function custom resources.<br> **Constraints**: key-value format, key is string, value is float type. |
| environment | map | No | Function environment variables.<br> **Constraints**: key-value format, both key and value are string.|
| minInstance | int | No | Minimum number of instances (used by function service).|
| maxInstance | int | No | Maximum number of instances (used by function service). |
| concurrentNum | int | No | Instance concurrency (used by function service). |
| storageType | String | Yes | Code package storage type.<br> Values: ``local``, ``s3``. |
| codePath | String | No | Code package local path, effective when storageType is configured as ``local``. |
| bucketId | String | No | Bucket name.|
| objectId | String | No | Storage object ID.|

## Status Codes

| **Status Code** | **Description** |
|-----|----------|
| 200 | Request successful (ok).   |
| 400 | Bad Request. |
| 500 | Internal Server Error. |

## Request Example

```shell
curl -X GET http://x.x.x.x:31182/serverless/v1/functions/0-f-a?versionNumber=v20240508-154704
```

## Response Example

### Successful Response

```json
{
    "code": 0,
    "message": "SUCCESS",
    "function": {
        "id": "sn:cn:yrk:default:function:0-f-a:v20240508-154704",
        "createTime": "2024-05-09 02:25:49.477 UTC",
        "updateTime": "",
        "functionUrn": "sn:cn:yrk:default:function:0-f-a",
        "name": "0-f-a",
        "tenantId": "default",
        "businessId": "yrk",
        "productId": "",
        "reversedConcurrency": 0,
        "description": "this is func",
        "tag": null,
        "functionVersionUrn": "sn:cn:yrk:default:function:0-f-a:v20240508-154704",
        "revisionId": "20240509022549477",
        "codeSize": 3753,
        "codeSha256": "39267d6c674f7ae5f99a284323d1f7f036b68e055e55f23a1fdcaa14c6e267e0",
        "bucketId": "bucket-test-log1",
        "objectId": "a-1715221549479",
        "handler": "",
        "layers": null,
        "cpu": 500,
        "memory": 500,
        "runtime": "cpp11",
        "timeout": 500,
        "versionNumber": "v20240508-154704",
        "versionDesc": "try-mod-version2",
        "environment": {},
        "customResources": {},
        "statefulFlag": 0,
        "lastModified": "2024-05-09 02:25:49.477 UTC",
        "Published": "2024-05-09 02:25:49.536 UTC",
        "minInstance": 0,
        "maxInstance": 100,
        "concurrentNum": 100,
        "funcLayer": null,
        "status": "unavailable",
        "instanceNum": 0,
        "device": {},
        "created": "2024-05-09 02:25:49.477 UTC"
    }
}
```

### Error Response

```json
{
    "code":4115,
    "message":"function [a] is not found. check input parameters"
}
```
