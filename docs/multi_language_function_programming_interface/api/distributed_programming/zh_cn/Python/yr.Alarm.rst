yr.Alarm
=========

.. py:class:: yr.Alarm(name: str, description: str)

    基类：``Metrics``

    用于设置和管理告警信息。

    .. note::
        如果启动 yr 时未配置 `runtime_metrics_config`，则样例无法使用。

    参数：
        - **name** (str) - 名称。
        - **description** (str) - 描述。

    **方法**：

    .. list-table::
       :header-rows: 0
       :widths: 30 70

       * - :ref:`__init__ <init_Alarm>`
         -
       * - :ref:`set <Alarm_set>`
         - 设置值。

.. toctree::
    :maxdepth: 1
    :hidden:

    yr.Alarm.__init__
    yr.Alarm.set
