.. _set_histories:

yr.ManagedSessionObj.set_histories
------------------------------------

.. py:method:: ManagedSessionObj.set_histories(histories: List[str]) -> None

    设置历史列表并立即同步到 libruntime。

    参数：
        - **histories** (List[str]) - 历史列表。
    
    返回：
        None。
    
    异常：
        - **RuntimeError** - 如果 bridge 调用失败，抛出此异常。