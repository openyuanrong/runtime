# Subscribe to Stream Service

## Description

Subscribe to the stream produced by a function service. The interface uses HTTP Server Sent Events to stream responses to the client, and adds a delimiter `\n` at the end of each streamed message to control message separation.

It is recommended that after producing all messages in the stream production function, send a stream production end marker `\xE0\xFF\xE0\xFF` provided by openYuanrong to notify the frontend service to stop receiving message subscriptions for this stream.

## URI

GET /serverless/v1/stream/subscribe

## Request Parameters

Request Header Parameters

| **Parameter** | **Required** | **Type** | **Description** |
| ---------- | ----- | ---------- | ------------------------- |
| X-Stream-Name | Yes | string | Stream name. |
| X-Expect-Num  | No | string | Number of packets to receive per stream read. |
| X-Timeout-Ms  | No | string | Timeout for each stream read. |

<br> Request Body Parameters

None

## Response Parameters

User-defined stream data.

## Request Example

GET {[frontend_endpoint](api-frontend-endpoint)}/serverless/v1/stream/subscribe

## Response Example

Status Code: 200

Indicates successful stream subscription.

```text
aaa

bbb
```

## Error Codes

See [Error Codes](error-code-rest-api)
