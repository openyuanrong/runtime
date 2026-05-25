# Delete Session

## Description

This API is used to delete the persistent data of an AI Agent session in the data system.

## URI

`DELETE /serverless/v1/functions/{function-urn}/sessions/{sessionId}`

| **Parameter** | **Required** | **Type** | **Description** |
| :--- | :--- | :--- | :--- |
| `function-urn` | Yes | string | Function version URN. |
| `sessionId` | Yes | string | Session ID. |

## Request Parameters

### Request Header Parameters

| **Parameter** | **Required** | **Type** | **Description** |
| :--- | :--- | :--- | :--- |
| Authorization | Yes | string | Authentication information. |

## Response Parameters

| **Name** | **Type** | **Description** |
| :--- | :--- | :--- |
| `code` | int | Return code. Returned only on failure. |
| `message` | String | Return message. Returned only on failure. |

## Response Examples

### Successful Response

- **HTTP Status Code**: ``200``
- **Body**: ``(empty)``

### Error Response

```json
{
    "code": 200400,
    "message": "sessionId is empty"
}
```

or

```json
{
    "code": 200404,
    "message": "session not found"
}
```
