# Delete Job

## Function Introduction

When job is in final state Failed/Succeeded/Stopped, delete the current job.

## URI

DELETE `/api/jobs/{submissionId}`

Path Parameters:

| **Parameter** | **Required** | **Parameter Type** | **Description** |
| ------------ | ------------ | ------------ | ------------- |
| submissionId | Yes | String | Job instance id. |

## Response Parameters

Returns a string. true is returned when job deletion is successful, false is returned when deletion fails.

## Request Example

DELETE {[frontend endpoint](api-frontend-endpoint)}/api/jobs/{submissionId}

## Response Example

Deletion successful

```text
true
```

Deletion failed

```text
false
```

## Error Codes

Please refer to [Error Codes](error-code-rest-api).
