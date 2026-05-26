# Create Reserved Instance Configuration

## Description

This API is used to create reserved instance configuration with specific labels for function services in the openYuanrong cluster by calling the meta_service interface.

## Constraints

- The function version must exist.

- A maximum of 100 Labels can be created for the same function version.

## URI

`POST /serverless/v1/functions/reserve-instance`

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
| instanceLabel       | String                    | Yes        | Label name, cannot be empty.<br> **Constraints**: Cannot exceed 63 characters, can only contain uppercase and lowercase letters, hyphens, and dots.                                    |
| instanceConfigInfos | Array[InstanceConfigInfo] | Yes        | Reserved instance configuration, duplicate cluster configurations are not allowed.                      |

#### InstanceConfigInfo

| **Name**     | **Type**  | **Required** | **Description** |
| ----------- | -------------------- |----------|--------|
| clusterId | String | No        | Cluster ID. Default is ``cluster001``, cannot exceed 64 characters. The cluster ID must exist, specified through deployment configuration (refer to deployment document parameter configuration).  |
| maxInstance | String | No        | Maximum number of instances. Values: ``0-1000``.  |
| minInstance | String | No        | Minimum number of instances. Values: ``0-1000``.  |

## Response Parameters

| **Name**     | **Type**                    | **Required**  | **Description**                 |
| ----------- |---------------------------| ------------------------- | ------------------------|
| code | int                       | Yes | Return code, ``0`` indicates successful creation, non-``0`` indicates creation failure. For more information, see [Error Codes](error-code-rest-api). |
| message | String                    | Yes | Return error message.                 |
| instanceConfigInfos | Array[InstanceConfigInfo] | Yes        | Reserved instance configuration.                 |
| reserveInsBaseInfo | ReserveInsBaseInfo        | Yes        | Basic configuration information.                 |

### ReserveInsBaseInfo

| **Name**     | **Type**  | **Required**  | **Description** |
| ----------- | -------------------- | ------------------------- | --------|
| funcName            | String                    | Yes        | Function name.   |
| version             | String                    | Yes        | Function version number.  |
| instanceLabel       | String                    | No        | Label name.   |

## Status Codes

| **Status Code** | **Description** |
|-----|----------|
| 200 | Request successful (ok).   |
| 400 | Bad Request. |
| 500 | Internal Server Error. |

## Request Example

POST {[meta service endpoint](api-meta-service-endpoint)}/serverless/v1/functions/reserve-instance

```json
{
  "funcName": "0@faaspy@hello",
  "version": "latest",
  "instanceLabel": "label001",
  "instanceConfigInfos": [
    {
      "clusterId": "cluster001",
      "maxInstance": 101,
      "minInstance": 0
    }
  ]
}
```

## Response Example

### Successful Response

```json
{
  "code": 0,
  "message": "",
  "instanceConfigInfos": [
    {
      "clusterId": "cluster001",
      "maxInstance": 101,
      "minInstance": 0
    }
  ],
  "reserveInsBaseInfo": {
    "funcName": "0@faaspy@hello",
    "version": "latest",
    "instanceLabel": "label001"
  }
}
```

### Error Response

```json
{
  "code": 4115,
  "message": "function [0@faaspy@hello1] is not found. check input parameters"
}
```
