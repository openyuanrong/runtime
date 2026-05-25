# Query Reserved Instance Configuration

## Description

This API is used to query reserved instance configuration with specific labels for function services in the openYuanrong cluster by calling the meta_service interface.

## Constraints

None.

## URI

GET /serverless/v1/functions/reserve-instance

## Request Parameters

### Request Header Parameters

| **Parameter**      | **Required** | **Type**                   | **Description**              |
|-------------|----------| ------------------------- |---------------------|
| X-Tenant-Id | No        | string | Tenant ID, default is default |
| X-Trace-Id  | No        | string | traceID             |

### Request Body Parameters

| **Name**              | **Type**                    | **Required** | Constraints                                                                              | **Description**                                      |
|---------------------|---------------------------|----------|---------------------------------------------------------------------------------|---------------------------------------------|
| funcName            | String                    | Yes        | ServiceName must be 1-16 letters and numbers; funcName must start with a lowercase letter, can use lowercase letters, numbers, and hyphens, with length not exceeding 127 characters. | Function service uses the format 0@{serviceName}@{funcName} |
| version             | String                    | Yes        |                                                                                 | Function version number                                       |

## Response Parameters

| **Name**  | **Type**                     | **Required**  | Constraints | **Description**   |
|---------|----------------------------| ------------------------- | ---------------------- |----------|
| total   | int                        | Yes |  | Total count       |
| results| Array[GetReserveInsResult] | Yes |  | Reserved instance configuration array |

### GetReserveInsResult

| **Name**     | **Type**  | **Required** | Constraints                       | **Description** |
| ----------- | -------------------- |----------|--------------------------|--------|
| instanceLabel       | String                    | No        |                                         | Label name                                   |
| instanceConfigInfos | Array[InstanceConfigInfo] | Yes        |           | Reserved instance configuration                      |

### InstanceConfigInfo

| **Name**     | **Type**  | **Required** | Constraints                       | **Description** |
| ----------- | -------------------- |----------|--------------------------|--------|
| clusterId | String | No        |  | Cluster ID  |
| maxInstance | String | No        |                  | Maximum number of instances  |
| minInstance | String | No        |                 | Minimum number of instances  |

## Status Codes

| **Status Code** | **Description** |
|-----|----------|
| 200 | ok |
| 400 | Bad Request |
| 500 | Internal Server Error |

## Request Example

```json
{
  "funcName": "0@faaspy@hello",
  "version": "latest"
}
```

## Response Example

Successful Response

```json
{
  "total": 2,
  "results": [
    {
      "instanceLabel": "label001",
      "instanceConfigInfos": [
        {
          "clusterId": "cluster001",
          "maxInstance": 101,
          "minInstance": 0
        }
      ]
    },
    {
      "instanceLabel": "",
      "instanceConfigInfos": [
        {
          "clusterId": "cluster001",
          "maxInstance": 100,
          "minInstance": 0
        }
      ]
    }
  ]
}
```

Error Response, for more information see [Error Codes](error-code-rest-api)

```json
{
  "code":4115,
  "message": ""
}
```
