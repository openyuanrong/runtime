.. _notify:

yr.ManagedSessionObj.notify
-------------------------------

.. py:method:: ManagedSessionObj.notify(data: Dict[str, Any]) -> None

    向会话 `wait/notify` 通道发送通知载荷（将 UTF-8 JSON 字节转为原生格式）。

    参数：
        - **data** (dict) - JSON 对象的字典（必须非空）。

    返回：
        None。

    异常：
        - **RuntimeError** - 如果原生调用失败或数据非法，抛出此异常。
