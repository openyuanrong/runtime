# Delete Reserved Instance Configuration

## Description

This API is used to delete reserved instance configuration with specific labels for function services in the openYuanrong cluster by calling the meta_service interface.

## Constraints

None.

## URI

`DELETE /serverless/v1/functions/reserve-instance`

## Request Parameters

### Request Header Parameters

| **Parameter**      | **Required** | **Type**                   | **Description**              |
|-------------|----------| ------------------------- |---------------------|
| X-Tenant-Id | No        | string | Tenant ID, default is ``default``. |
| X-Trace-Id  | No        | string | traceID.             |

### Request Body Parameters

| **Name**              | **Type**                    | **Required** | **Description**                                      |
|---------------------|---------------------------|----------|---------------------------------------------|
| funcName            | String                    | Yes        | Function service uses the format 0@{serviceName}@{funcName}.<br> **Constraints**: ServiceName must be 1-16 letters and numbers; funcName must start with a lowercase letter, can use lowercase letters, numbers, and hyphens, with length not exceeding 127 characters.|
| version             | String                    | Yes        | Function version number.                                       |
| instanceLabel       | String                    | No        | Label name, when label is empty, deletes all labels for all function versions.<br> **Constraints**: Cannot exceed 63 characters, can only contain uppercase and lowercase letters, hyphens, and dots.                      |

## Response Parameters

| **Name**     | **Type**                    | **Required**  | **Description**                 |
| ----------- |---------------------------| ------------------------- |------------------------|
| code | int                       | Yes | Return code, ``0`` indicates successful deletion, non-``0`` indicates deletion failure. For more information, see [Error Codes](error-code-rest-api). |
| message | String                    | Yes | Return error message.                 |

## Status Codes

| **Status Code** | **Description** |
|-----|----------|
| 200 | Request successful (ok).   |
| 400 | Bad Request. |
| 500 | Internal Server Error. |

## Request Example

DELETE {[meta service endpoint](api-meta-service-endpoint)}/serverless/v1/functions/reserve-instance

```json
{
  "funcName": "0@faaspy@hello",
  "version": "latest",
  "instanceLabel": "label001"
}
```

## Response Example

### Successful Response

```json
{
  "code": 0,
  "message": ""
}
```

### Error Response

```json
{
  "code": 4115,
  "message": "function [0@faaspy@hello] is not found. check input parameters"
}
```
