.. _load_session:

yr.SessionService.load_session
-------------------------------

.. py:method:: SessionService.load_session() -> ManagedSessionObj | None

    加载当前请求关联的会话对象。

    返回：
        会话对象，如果会话 ID 为空则返回 ``None``。
        数据类型：ManagedSessionObj。
    
    异常：
        - **RuntimeError** - 如果 bridge 调用失败，抛出此异常。
