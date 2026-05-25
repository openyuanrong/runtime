# start

Start the openYuanrong cluster, supporting `master` and `agent` modes.

## Usage

```shell
yr start [OPTIONS]
```

## Parameters

* `--master`: Start in master mode (defaults to agent mode without this parameter).
* `-s, --set KEY=VALUE`: Override configuration from the command line; can be specified multiple times. `VALUE` must be a valid TOML literal.
* `--master_address http(s)://host:port`: Only available in agent mode. Before startup, pulls service discovery information from the specified `function_master` and automatically converts it to `-s` overrides.

:::{Note}

Common `-s/--set` examples:

* String: `-s 'values.fs.log.level="DEBUG"'`
* Number: `-s 'values.ds_master.port=12123'`
* Array of tables: `-s 'values.etcd.address=[{ip="10.88.0.9",peer_port="32380",port="32379"}]'`

`--master_address` usage restrictions:

* Only supported by `yr start` (agent mode).
* Using it together with `--master` will result in an error and exit.
* If service discovery fails, it will exit directly.
* The parameter value must include a protocol prefix, i.e., `http://` or `https://`.
* When using `https://`, you need to provide `values.fs.tls.ca_file`, `values.fs.tls.cert_file`, `values.fs.tls.key_file` via `--config` or `-s` (can be used together with `values.fs.tls.base_path`).

:::

## Configuration Merge Rules

When `yr start` starts, the final configuration is merged according to the following priority (high to low):

1. Command-line overrides: `-s/--set`
2. User configuration file: defaults to `/etc/yuanrong/config.toml` (can be specified via `-c`)
3. Built-in defaults: `api/python/yr/cli/values.toml` + `api/python/yr/cli/config.toml.jinja`

## Startup Behavior

After executing `yr start`, the CLI will:

1. Create a deployment directory `/tmp/yr_sessions/<timestamp>/`
2. Update the symbolic link `/tmp/yr_sessions/latest -> /tmp/yr_sessions/<timestamp>/`
3. Start the components corresponding to the mode and perform health checks
4. Write the session file `/tmp/yr_sessions/latest/session.json`

Where:

* `master` starts by default: `etcd`, `ds_master`, `ds_worker`, `function_master`, `function_proxy`, `function_agent`
* `agent` starts by default: `ds_worker`, `function_proxy`, `function_agent`

## Examples

Start the master node:

```shell
yr start --master
```

After successful startup, the terminal will print the recommended command for worker nodes to join the cluster (including `-s 'values.*=...'` overrides).

On worker nodes, you can directly execute this command to join the cluster, for example:

```shell
yr start -s 'values.etcd.address=[{ip="10.88.0.4",peer_port="32380",port="32379"}]' \
  -s 'values.ds_master.ip="10.88.0.4"' \
  -s 'values.ds_master.port="12123"' \
  -s 'values.function_master.ip="10.88.0.4"' \
  -s 'values.function_master.global_scheduler_port="22770"'
```

Or use the auto-discovery mode (agent):

```shell
yr start --master_address http://10.88.0.4:22770
```
