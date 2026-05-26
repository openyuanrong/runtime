# Query Function List

## Description

This API is used to query functions in the openYuanrong cluster by calling the meta_service interface.

## Constraints

## URI

`GET /serverless/v1/functions?versionNumber={version}&name={name}&pageIndex={pageIndex}&pageSize={pageSize}`

### Query Parameters

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| name | String | No | Function name, fuzzy match |
| version | String | No | Function version name, exact match |
| pageSize | int | No | Page size for query |
| pageIndex | int | No | Starting page number for query |

## Request Parameters

### Request Header Parameters

None

### Request Body Parameters

None

## Response Parameters

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| code | int | Yes | Return code, 0 indicates successful query, non-0 indicates query failure. For more information, see [Error Codes](error-code-rest-api).  |
| message | String | Yes | Return error message. |
| result | QueryResult Object | Yes | Query result. |

### QueryResult

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| total | int | Yes | Total number of functions matching the query conditions. |
| functions | FunctionInfo Array | Yes | Function information array. |

### FunctionInfo Object

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
| customResources | map | No | Function custom resources.<br> **Constraints**: key-value format, key is string, value is float type.|
| environment | map | No | Function environment variables.<br> **Constraints**: key-value format, both key and value are string.|
| minInstance | int | No | Minimum number of instances (used by function service).|
| maxInstance | int | No | Maximum number of instances (used by function service).|
| concurrentNum | int | No | Instance concurrency (used by function service).|
| storageType | String | Yes | Code package storage type. Values: ``local``, ``s3``. |
| codePath | String | No | Code package local path, effective when storageType is configured as ``local``. |
| bucketId | String | No | Bucket name.|
| objectId | String | No | Storage object ID.|

## Status Codes

| **Status Code** | **Description** |
|-----|----------|
| 200 | Request successful (ok).   |
| 500 | Internal Server Error. |

## Request Example

```shell
curl -X GET http://x.x.x.x:31182/serverless/v1/functions?pageIndex=1&pageSize=1&versionNumber=&name=
```

## Response Example

### Successful Response

```json
{
    "code": 0,
    "message": "",
    "result": {
        "functions": [
            {
                "createTime": "2024-05-09 02:25:49.477 UTC",
                "updateTime": "2024-05-09 02:26:03.267 UTC",
                "functionUrn": "sn:cn:yrk:default:function:0-f-a",
                "name": "0-f-a",
                "tenantId": "default",
                "businessId": "yrk",
                "productId": "",
                "reversedConcurrency": 0,
                "description": "this is func",
                "tag": null,
                "functionVersionUrn": "sn:cn:yrk:default:function:0-f-a:$latest",
                "revisionId": "20240509022603267",
                "codeSize": 3753,
                "codeSha256": "39267d6c674f7ae5f99a284323d1f7f036b68e055e55f23a1fdcaa14c6e267e0",
                "bucketId": "bucket-test-log1",
                "objectId": "a-1715221563270",
                "handler": "",
                "layers": null,
                "cpu": 500,
                "memory": 500,
                "runtime": "cpp11",
                "timeout": 500,
                "versionNumber": "$latest",
                "versionDesc": "$latest",
                "environment": {},
                "customResources": null,
                "statefulFlag": 0,
                "lastModified": "",
                "Published": "2024-05-09 02:25:49.477 UTC",
                "minInstance": 0,
                "maxInstance": 100,
                "concurrentNum": 100,
                "funcLayer": [],
                "status": "",
                "instanceNum": 0,
                "device": {}
            }
        ],
        "total": 3
    }
}
```

### Error Response

```json
{
    "code": 444,
    "message": "xxx error"
}
```
