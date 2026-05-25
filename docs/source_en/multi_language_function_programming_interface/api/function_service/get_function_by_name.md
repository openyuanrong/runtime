# Query All Versions of Function

## Description

This API is used to query the function version list in the openYuanrong cluster by calling the meta_service interface.

## Constraints

None

## URI

GET /serverless/v1/functions/{name}/versions?pageIndex={pageIndex}&pageSize={pageSize}

### Query Parameters

| **Name**     | **Type**  | **Required**  | **Constraints** | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- | ------------------------- |
| pageSize | int | No |   | Page size for query |
| pageIndex | int | No |   | Starting page number for query |

## Request Parameters

### Request Header Parameters

None

### Request Body Parameters

None

## Response Parameters

| **Name**     | **Type**  | **Required**  | Constraints | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- | ------------------------- |
| functions | FunctionVersionInfo Array | No |  | Function version list  |
| pageInfo | PageInfo | Yes |  | Pagination information |

### PageInfo

| **Name**     | **Type**  | **Required**  | Constraints | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- | ------------------------- |
| total | int | Yes |  | Total number of functions matching the query conditions |
| pageSize | int | No |   | Page size for query |
| pageIndex | int | No |   | Starting page number for query |

### FunctionVersionInfo Object

| **Name**     | **Type**  | **Required**  | Constraints | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- | ------------------------- |
| id | String | Yes |  | Function ID  |
| functionVersionUrn | String | Yes |  | Function version URN, used for invoking the function |
| revisionId | String| Yes | | Function revisionId, used for publishing the function |
| name | String | Yes | | Function name|
| createTime | Date | Yes | | Function creation time|
| updateTime | Date | Yes | | Function update time|
| versionNumber | String | Yes | | Version number|
| runtime | String | Yes | | Function runtime type |
| description | String | No |  | Function description  |
| handler | String | Yes |  | Call handler |
| cpu | int | Yes |  | Function CPU size, unit: m (millicores) |
| memory | int | Yes |  | Function MEM size, unit: MB |
| timeout | int | Yes |  | Function invocation timeout |
| customResources | map | No | key-value format, key is string, value is float type| Function custom resources |
| environment | map | No | key-value format, both key and value are string | Function environment variables |
| minInstance | int | No | | Minimum number of instances (used by function service)|
| maxInstance | int | No | | Maximum number of instances (used by function service) |
| concurrentNum | int | No | | Instance concurrency (used by function service) |
| storageType | String | Yes | Values: local, s3 | Code package storage type |
| codePath | String | No | | Code package local path, effective when storageType is configured as local |
| bucketId | String | No | | Bucket name|
| objectId | String | No | | Storage object ID|

## Status Codes

| **Status Code** | **Description** |
|-----|----------|
| 200 | ok |
| 500 | Internal Server Error |

## Request Example

```shell
curl -X GET http://x.x.x.x:31182/serverless/v1/functions/0@faaspy@hello2/versions?pageIndex=1&pageSize=1
```

## Response Example

Successful Response

```json
{
    "functions": [
        {
            "id": "sn:cn:yrk:default:function:0@faaspy@hello2:1",
            "createTime": "2024-03-02 08:31:12.724 UTC",
            "updateTime": "2024-03-02 08:47:50.473 UTC",
            "functionUrn": "sn:cn:yrk:default:function:0@faaspy@hello2",
            "name": "0@faaspy@hello2",
            "tenantId": "default",
            "businessId": "yrk",
            "productId": "",
            "reversedConcurrency": 0,
            "description": "this is a function",
            "tag": null,
            "functionVersionUrn": "sn:cn:yrk:default:function:0@faaspy@hello2:1",
            "revisionId": "20240302084750473",
            "codeSize": 0,
            "codeSha256": "",
            "bucketId": "",
            "objectId": "",
            "handler": "handler.my_handler",
            "layers": null,
            "cpu": 600,
            "memory": 512,
            "runtime": "python3.9",
            "timeout": 600,
            "versionNumber": "1",
            "versionDesc": "version2",
            "environment": {},
            "customResources": null,
            "statefulFlag": 0,
            "lastModified": "",
            "Published": "2024-03-02 09:07:53.390 UTC",
            "minInstance": 0,
            "maxInstance": 10,
            "concurrentNum": 10,
            "funcLayer": [],
            "status": "",
            "instanceNum": 0,
            "device": {},
            "created": ""
        }
    ],
    "pageInfo": {
        "pageIndex": "1",
        "pageSize": "1",
        "total": 2
    }
}
```

Error Response

```json
{
    "functions": null,
    "pageInfo": {
        "pageIndex": "1",
        "pageSize": "1",
        "total": 0
    }
}
```
