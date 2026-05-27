# Specifications and Restrictions

Contains requirements and restrictions for software, network, and hardware environments, for example, deployment on Windows environment is not supported.

## 1. ds-worker Startup Parameter Specifications and Restrictions

| Name | Meaning | Default Value | Constraint |
|--|--|--|--|
| unix_domain_socket_dir | Specifies the path where unix domain socket files are created | ~/.datasystem/unix_domain_socket_dir/ | Valid path format & length not exceeding 80 characters |

## 2. function-agent K8S POD pids Resource Specifications and Restrictions

| Name | Meaning | Default Value | Constraint |
|--|--|--|--|
| Average pids resources required by each CppRuntime | Based on function-agent K8S POD pids resource specifications (`/sys/fs/cgroup/pids/kubepods/{burstable,besteffort,}/pod${uuid}/pids.max`), the maximum number of CppRuntime instances that a single POD can start can be calculated | 51 | function-agent K8S POD internal function system components occupy 60 pids resources, in addition, each runtime needs to reserve more than 27 pids resources for CppRuntime (without data system), and more than 51 pids resources for each runtime (with data system). To modify K8S POD pids resource specifications, modify the `--pod-max-pids` startup parameter of kubelet; to temporarily modify existing POD pids cgroups resource specifications, modify `/sys/fs/cgroup/pids/kubepods/{burstable,besteffort,}/pod${uuid}/pids.max` |

## 3. function-master K8S POD Resource Specifications and Restrictions

| Name | Meaning | Default Value | Constraint |
|--|--|--|--|
| .Values.global.resources.functionMaster.limits.memory | 3 Gi | When cluster instance scale is greater than 10000, it is recommended that this parameter be set greater than 5 Gi, otherwise function-master may experience insufficient memory oom. (Current maximum specification supported by k8s deployment does not exceed 20000 instances) |

## 4. openYuanrong runtime and runtime-manager Resource Occupancy Description

Within openYuanrong resource pool pool pod, runtime and runtime-manager will run. When runtime instances are started, these two components themselves also occupy memory. Therefore, the memory resources that can be provided to user code are less than the pool's own resource quota. Specifically, the RSS occupancy of different language runtime processes is as follows. Data fluctuates with actual scenarios and software version iterations, for reference only (unit MB).

| Test Content | Java             | Python | Cpp |
| --- | -----------------| --- | --- |
| Sleep 60s | 175 | 107 | 95 |
| 100 invocations | 176 –> 181 | 107 | 95 |

Note: The above data comes from MultiLanguageRuntime version runtime.

runtime-manager occupies 25 MB - 27 MB of memory.

Taking a running idle cpp runtime instance as an example, the sum of RSS memory occupied by runtime and runtime-manager is about 110 MB. For a resource pool pod with a quota of 128 MB, the memory available to users is no more than 18 MB. This can easily cause runtime and runtime-manager to be killed due to OOM.

Users need to evaluate reasonable resource pool memory size configuration based on the memory occupied by their code execution and the memory occupied by loading their code's dynamic link library .so files.

## 5. etcd Deployment Specifications Description

### etcd Configuration Description

| Parameter Name | Meaning                                                             | Default Value        | Parameter Impact                                                   |
|------|----------------------------------------------------------------|------------|--------------------------------------------------------|
| `--snapshot-count`     | Refers to how many write requests trigger a snapshot and compress Raft log entries.                          | 100000     | Before executing a snapshot, data needs to be saved in memory. If the value is too large, it will consume more memory. If the value is too small, it may trigger snapshots frequently, affecting write performance. |
| `--quota-backend-bytes`     | Maximum storage byte limit for backend storage, unit B                                             | 8589934592 | When written data exceeds this limit, etcd will stop accepting new write operations                          |
| `--auto-compaction-mode`     | Automatic compaction strategy, configuring periodic means regular compaction, configuring revision means compaction by revision number            | revision   | If etcd writes data non-periodically, it is recommended to configure revision strategy                       |
| `--auto-compaction-retention`     | Specifies the retention value for automatic compaction. For periodic strategy, it represents the retention time window. For revision strategy, it represents the number of retained revisions | 1000       | Automatic compaction cleans up old revisions, reducing etcd storage space occupancy, but may have some impact on performance    |

### Performance Baseline Configuration

| Cpu | Memory | quota-backend-bytes         | snapshot-count | auto-compaction-mode | auto-compaction-retention | Cluster Node Count | Maximum Concurrent Instance Creation Count | Registered Function Count | Maximum Running Instance Count |
|-----|--------|----------------|----------------|---------------|---------------------------|-------|-----------|--------|----------|
| 4C  | 8G     | 8589934592 (8G) | 20000          | revision      | 5000                      | <100  | <1000     | <1000  | <2000    |
| 8C  | 16G    | 8589934592 (8G) | 50000          | revision      | 10000                     | <500  | <2000     | <5000  | <5000    |
| 16C | 32G    | 8589934592 (8G) | 100000         | revision      | 25000                     | <1000 | <5000     | <10000 | <10000   |

Cpu and Memory reference configuration (modify etcd deployment StatefulSet):

```yaml
resources:
  limits:
    cpu: "4"
    memory: 8Gi
  requests:
    cpu: "4"
    memory: 8Gi
```

Compaction parameters and DB Size reference configuration (modify etcd deployment StatefulSet):

```yaml
# Add the following environment variables
# Configure auto-compaction-mode compaction mode
- name: ETCD_AUTO_COMPACTION_MODE
  value: "revision"
#  Configure auto-compaction-retention compaction parameters
- name: ETCD_AUTO_COMPACTION_RETENTION
  value: "10000"
# Configure quota-backend-bytes DB Size (configuration unit is bytes)
- name: ETCD_QUOTA_BACKEND_BYTES
  value: "8589934592"
# Configure snapshot-count
- name: ETCD_SNAPSHOT_COUNT
  value: "20000"
```

### Special Instructions

1. To increase maximum concurrent instance creation count, please increase Cpu, Memory, and automatic compaction parameters.
2. To increase registered function count/maximum running instance count, please increase Memory, DB Size.
3. To increase cluster node count, please increase Cpu, Memory.
4. When memory is insufficient, please reduce the number of saved snapshots to prevent etcd from experiencing OOM.
5. etcd performance depends on disk, please use high-performance disks as much as possible, refer to official website [etcd](https://etcd.io/docs/v3.5/op-guide/hardware/).
