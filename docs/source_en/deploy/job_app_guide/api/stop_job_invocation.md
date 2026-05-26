# Stop Job

## Function Introduction

When job is in running state, stop the current job.

## URI

POST `/api/jobs/{submissionId}/stop`

Path Parameters:

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ------------ | ------------ | ------------ | ------------- |
| submissionId | Yes | String | Job instance id. |

## Response Parameters

Returns a string. true is returned when job stop is successful, false is returned when stop fails.

## Request Example

POST {[frontend endpoint](api-frontend-endpoint)}/api/jobs/{submissionId}/stop

## Response Example

Stop successful

```text
true
```

Stop failed

```text
false
```

## Error Codes

Please refer to [Error Codes](error-code-rest-api).
