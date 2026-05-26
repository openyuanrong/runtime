# Alarm Metrics

openYuanrong provides alarm capabilities through metrics. When failures (internal or external components) occur, openYuanrong can export failure information to designated platforms for timely user operations and maintenance.

| Metric Name | Metric Meaning | Trigger Condition |
| ---------- | -------------------- | -------------------- |
| yr_proxy_alarm | Scheduler exception. | Reports alarm when heartbeat detection (period is `systemTimeout/12`, unit is ms, `systemTimeout` default value is `60000`) fails, and no longer detects heartbeat afterwards. |
| yr_etcd_alarm | etcd exception. | Reports alarm when heartbeat detection (period is `metaStoreCheckHealthIntervalMs`, unit is ms, default value is `5000`) fails, and continues to detect heartbeat afterwards. If detection fails and alarms continuously, clears alarm after recovery. |
| yr_election_alarm | Leader election exception. | When openYuanrong component (function-master) enables leader election, reports alarm if election fails (currently only supports alarm scenarios using txn leader election mode), clears alarm after recovery. |
