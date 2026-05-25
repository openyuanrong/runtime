# Delete Function

## Description

This API is used to delete a function in the openYuanrong cluster by calling the meta_service interface. If the function has multiple versions, all will be deleted.

## Constraints

None

## URI

`DELETE /serverless/v1/functions/{name}?versionNumber={version}`

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| name | String | Yes | Function name. |
| version | String | No | Function version. |

## Request Parameters

None

## Response Parameters

| **Name**     | **Type**  | **Required**  | **Description**               |
| ----------- | -------------------- | ------------------------- | ------------------------- |
| code | int | No | Return code, ``0`` indicates successful deletion, non-``0`` indicates deletion failure. For more information, see [Error Codes](error-code-rest-api).  |
| message | String | No | Return error message. |

## Status Codes

| **Status Code** | **Description** |
|-----|----------|
| 200 | Request successful (ok).   |
| 400 | Bad Request. |
| 500 | Internal Server Error. |

## Request Example

```shell
curl -X DELETE  http://X.X.X.X:31182/serverless/v1/functions/0@faaspy@hello3
```

## Response Example

### Successful Response

```json
{}
```

### Error Response

```json
{
    "code": 4115,
    "message": "function [0@faaspy@hello3] is not found. check input parameters"
}
```
