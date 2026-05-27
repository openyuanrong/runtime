.. _wait_for_notify:

yr.ManagedSessionObj.wait_for_notify
--------------------------------------

.. py:method:: ManagedSessionObj.wait_for_notify(timeout_ms: int) -> Dict[str, Any] | None

    阻塞等待，直至收到该会话的 `notify` 通知或触发超时。

    参数：
        - **timeout_ms** (int) - 等待超时时间（毫秒）。``-1`` 表示无限等待。

    返回：
        解析后的 JSON 对象（dict），若超时则为 ``None``。
        数据类型：Dict[str, Any]。

    异常：
        - **RuntimeError** - 如果发生中断或其他原生错误，抛出此异常。
