.. _get_histories:

yr.ManagedSessionObj.get_histories
------------------------------------

.. py:method:: ManagedSessionObj.get_histories() -> List[str]

    获取历史列表。

    返回的仅为快照副本，严禁作为可写的内部引用对外暴露。若需修改数据，必须在操作后调用 set_histories() 接口进行回写，以便 SDK 将变更同步至 libruntime。

    返回：
        历史列表的浅拷贝，数据类型：List[str]。
