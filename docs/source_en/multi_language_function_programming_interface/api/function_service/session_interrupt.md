# Interrupt Session

## Description

This API is used to interrupt an executing or pending AI Agent session.

## URI

`POST /serverless/v1/functions/{function-urn}/sessions/{sessionId}/interrupt`

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
| `message` | String | Return message. |

## Response Examples

### Successful Response

```json
{
    "message": "Interrupted Success"
}
```

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
