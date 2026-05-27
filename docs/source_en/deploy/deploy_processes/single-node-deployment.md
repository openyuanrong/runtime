# Getting Started

This section will demonstrate deploying openYuanrong on one or more Linux hosts using default configuration parameters, recommended for learning and development. For production deployment, please refer to [User Guide](production/index.md).

## Deploy openYuanrong

First, refer to [Installation Guide](../installation.md) to install openYuanrong command-line tool yr on all deployment hosts, we will use it to deploy openYuanrong.

Choose any host and use the following command to deploy the [master node](glossary-master-node).

```bash
yr start --master
```

After successful deployment, the terminal will print recommended commands for worker nodes to join the cluster, in the following format:

```text
To join an existing cluster, execute the following commands in your shell on worker nodes:

yr start -s 'values.etcd.address=[{ip="x.x.x.x",peer_port="xxxx",port="xxxx"}]' -s 'values.ds_master.ip="x.x.x.x"' -s 'values.ds_master.port="xxxx"' -s 'values.function_master.ip="x.x.x.x"' -s 'values.function_master.global_scheduler_port="xxxx"'

OR

mkdir -p /etc/yuanrong/ && cat << EOF > /etc/yuanrong/config.toml && yr start
[values.etcd]
...
EOF

OR

yr start --master_address http://x.x.x.x:xxxx
```

At this point, openYuanrong services are ready to use. When multi-node cluster deployment is needed, directly execute the recommended commands printed by the master node on the remaining hosts to deploy [worker nodes](glossary-agent-node).

```bash
# Join worker node through automatic discovery mode, replace x.x.x.x:xxxx with function master address
yr start --master_address http://x.x.x.x:xxxx
```

Execute `yr status` command on the master node to view cluster status. Under normal circumstances, `ReadyAgentsCount` should be consistent with the actual number of deployed nodes.

```bash
yr status
```

```text
Cluster Status:
  ...
  ReadyAgentsCount: 2
  ...
```

You can run [Simple Example](../../multi_language_function_programming_interface/examples/simple-function-template.md) to further verify the deployment result.

## Delete openYuanrong Cluster

Use command-line tool yr to execute the following command on **all deployment nodes**:

```bash
yr stop
```

:::{note}
The `yr stop` command will send SIGTERM to the daemon process and wait for it to exit gracefully (up to 40 seconds). If timeout, you can use `yr stop --force` to force termination.
:::
