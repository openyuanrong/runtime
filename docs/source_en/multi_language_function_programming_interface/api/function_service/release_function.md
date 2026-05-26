# Publish Function

## Description

This API is used to publish a function in the openYuanrong cluster by calling the meta_service interface.

## Constraints

When using the default generated version number for publishing, the interval between multiple publications must be greater than ``1 s``. The current default version number generation is based on the current time. Multiple publications in a short time will generate duplicate version numbers and will return a duplicate version number error.

## URI

`POST /serverless/v1/functions/{function_id}/versions`

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| function_id | String | Yes | Function name. |

## Request Parameters

### Request Header Parameters

| **Parameter**     | **Required**             | **Type**                   | **Description** |
| ----------- | -------------------- | ------------------------- | ----------- |
| Content-Type | Yes | string | Message body type; it is recommended to use ``application/json``. |

### Request Body Parameters

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| revisionId | String| Yes | Function revisionId, must be the latest version. |
| versionNumber | String | No | Version number.<br> **Constraints**: Must start and end with letters or numbers, can contain letters, numbers, hyphens (-), underscores (_), and dots (.), length not exceeding 42. When empty, the default generated format is like "v20060102-150405".|
| versionDesc | String | No | Version description.  |
| kind | String | No | Function type, values ``faas`` or ``yrlib``.<br> It is recommended to fill in according to the kind when the function was registered. |

## Response Parameters

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| code | int | Yes | Return code, ``0`` indicates successful update, non-``0`` indicates update failure. For more information, see [Error Codes](error-code-rest-api).  |
| message | String | Yes | Return error message. |
| function | PublishFunctionResult | Yes | Update result. |

### PublishFunctionResult Introduction

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| id | int | Yes | Function ID.  |
| functionVersionUrn | String | Yes | Function version URN, used for invoking the function. |
| revisionId | String| Yes | Function revisionId, used for publishing the function. |

## Status Codes

| **Status Code** | **Description** |
|-----|----------|
| 200 | Request successful (ok). |
| 500 | Internal Server Error. |

## Request Example

POST {[meta service endpoint](api-meta-service-endpoint)}/serverless/v1/functions/{function_id}/versions

```json
{
    "revisionId": "20240302084750473",
    "versionDesc": "try-mod-version2",
    "versionNumber": "B.2233",
    "kind": "yrlib"
}
```

## Response Example

### Successful Response

```json
{
    "code": 0,
    "message": "SUCCESS",
    "function": {
        "id": "sn:cn:yrk:default:function:0-f-a:B.2233",
        "createTime": "2024-04-16 12:45:31.639 UTC",
        "updateTime": "",
        "functionUrn": "sn:cn:yrk:default:function:0-f-a",
        "name": "0-f-a",
        "tenantId": "default",
        "businessId": "yrk",
        "productId": "",
        "reversedConcurrency": 0,
        "description": "this is func",
        "tag": null,
        "functionVersionUrn": "sn:cn:yrk:default:function:0-f-a:B.2233",
        "revisionId": "20240416124531639",
        "codeSize": 3131,
        "codeSha256": "557ed9f661be0020c8a3243fa362a1aaccd0f5a4bb6cc8040a26bfd3844ccf14",
        "bucketId": "bucket-test-log1",
        "objectId": "a-1713271531641",
        "handler": "",
        "layers": null,
        "cpu": 500,
        "memory": 500,
        "runtime": "cpp11",
        "timeout": 500,
        "versionNumber": "B.2233",
        "versionDesc": "try-mod-version2",
        "environment": {},
        "customResources": {},
        "statefulFlag": 0,
        "lastModified": "2024-04-16 12:45:31.639 UTC",
        "Published": "2024-04-16 12:45:38.004 UTC",
        "minInstance": 0,
        "maxInstance": 100,
        "concurrentNum": 100,
        "funcLayer": null,
        "status": "",
        "instanceNum": 0,
        "device": {},
        "created": "2024-04-16 12:45:31.639 UTC"
    }
}
```

### Error Response

```json
{
    "code": 4134,
    "message": "revisionId is non latest version"
}
```
