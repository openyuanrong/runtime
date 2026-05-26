# config

View or export the effective CLI configuration.

## Subcommands

* `yr config dump`: Output the merged final configuration (TOML).
* `yr config template`: Output the built-in `values.toml` and `config.toml.jinja` templates.

## Usage

```shell
yr config dump [OPTIONS]
yr config template
```

## Parameters

`yr config dump` supports:

* `-s, --set KEY=VALUE`: Override configuration from the command line; can be specified multiple times. `VALUE` must be a valid TOML literal.

## Description

The configuration merge priority for `config dump` (high to low):

1. `-s/--set` command-line overrides
2. User configuration file specified via `-c/--config` or at the default path
3. Built-in default templates

## Customize Configuration Based on Built-in config.toml.jinja

The final effective configuration for `yr` is rendered and merged from the following two built-in files:

- `api/python/yr/cli/values.toml`
- `api/python/yr/cli/config.toml.jinja`

If you want to adjust the startup parameters of a component, it is recommended to use a custom `config.toml` to override the corresponding fields, rather than modifying the built-in template files.

Recommended steps:

1. Execute `yr config template` to view the built-in template structure.
2. Create a new `config.toml`, writing only the fields you want to override.
3. First execute `yr -c /path/to/config.toml config dump` to verify the rendered result.
4. Execute `yr -c /path/to/config.toml start --master` (or `start`) to start.

Example (override component startup parameters):

```toml
[mode.master]
frontend = true
dashboard = true

[values]
log_level = "DEBUG"

[values.function_proxy]
ip = "10.88.0.4"
port = "22772"
grpc_listen_port = "22773"

[function_proxy.args]
enable_metrics = false
litebus_thread_num = 25
runtime_recover_enable = true

[ds_worker.args]
heartbeat_interval_ms = 120000
node_dead_timeout_s = 120
```

Verification and startup:

```shell
yr -c /path/to/config.toml config dump
yr -c /path/to/config.toml start --master
```

If you only want to temporarily change a single parameter, you can also avoid editing the file and directly use `-s/--set`, for example:

```shell
yr start --master -s 'function_proxy.args.enable_metrics=true'
```

## values.toml Field Descriptions

Built-in default values file path: `api/python/yr/cli/values.toml`.

:::{Note}

Fields containing `{{ ... }}` template variables in `values.toml` **will be automatically populated by the CLI at startup, requiring no manual configuration from the user**. The CLI will automatically retrieve and calculate these values based on the runtime environment (such as IP, timestamp, CPU, memory, ports, etc.).

Port fields commonly written as `{{ 32379|check_port() }}` indicate that the given port is preferred; if it is occupied, an available port will be automatically selected.

To override automatically calculated values, specify them via the `-s/--set` parameter, for example: `-s 'values.host_ip="10.0.0.1"'`.

:::

### [values] Core Fields

:::{tip}
Default values with `{{ ... }}` in the table below indicate that the field will be automatically populated at runtime, requiring no manual configuration.
:::

| Field | Default Value | Description |
| ---- | ---- | ---- |
| `host_ip` | `{{ ip }}` | Node IP. |
| `deploy_path` | `{{ deploy_path }}` | Deployment directory for this startup (typically under `/tmp/yr_sessions/<timestamp>`). |
| `yr_package_path` | `{{ yr_package_path }}` | `openyuanrong` Python package path. |
| `cpu_num` | `{{ cpu_millicores }}` | Available CPU (millicores). |
| `memory_num` | `{{ memory_num_mb }}` | Available memory (MB). |
| `shared_memory_num` | `{{ memory_num_mb // 3 }}` | Shared memory (MB). |
| `node_id` | `{{ node_id }}` | Unique node ID (defaults based on hostname + pid). |
| `log_level` | `INFO` | Global log level. |
| `pid` | `{{ pid }}` | Current `yr` process PID. |
| `ld_library_path` | `{{ ld_library_path }}` | `LD_LIBRARY_PATH` inherited into the template rendering context at startup. |
| `python_path` | `{{ python_path }}` | `PYTHONPATH` inherited into the template rendering context at startup. |

### [values.meta_store]

| Field | Default Value | Description |
| ---- | ---- | ---- |
| `enable` | `false` | Whether to enable meta store. |
| `address` | `""` | Meta store address. |
| `mode` | `local` | Meta store mode. |

### [values.fs] Function System Related Fields

| Field | Default Value | Description |
| ---- | ---- | ---- |
| `schedule_plugins` | `["Label","ResourceSelector","Default","Heterogeneous","NUMA"]` | Scheduling plugin chain. |

#### [values.fs.log]

| Field | Default Value | Description |
| ---- | ---- | ---- |
| `also_log_to_stderr` | `false` | Whether to also output to stderr. |
| `async_log_buf_secs` | `30` | Log buffer seconds. |
| `async_log_max_queue_size` | `51200` | Async log queue limit. |
| `async_log_thread_count` | `1` | Async log thread count. |
| `level` | `{{ values.log_level }}` | Function system log level. |
| `path` | `{{ values.deploy_path }}/logs/function_system` | Function system log directory. |
| `rolling_max_files` | `10` | Number of files to retain during log rotation. |
| `rolling_max_size` | `40` | Maximum size per file (MB). |
| `rolling_retention_days` | `30` | Log retention days. |
| `compress_enable` | `false` | Whether to compress logs. |

#### [values.fs.tls]

| Field | Default Value | Description |
| ---- | ---- | ---- |
| `enable` | `false` | Whether to enable function system TLS. |
| `base_path` | `""` | Certificate root directory. |
| `ca_file` | `ca.crt` | CA filename. |
| `cert_file` | `module.crt` | Certificate filename. |
| `key_file` | `module.key` | Private key filename. |

#### [values.fs.metrics]

| Field | Default Value | Description |
| ---- | ---- | ---- |
| `metrics_config` | `""` | Metrics configuration string. |
| `metrics_config_file` | `{{ values.yr_package_path }}/functionsystem/config/metrics/metrics_config.json` | Metrics configuration file path. |

### [values.ds.curve]

| Field | Default Value | Description |
| ---- | ---- | ---- |
| `enable` | `false` | Whether to enable Curve/ZMQ security features. |
| `base_path` | `""` | Curve key directory. |
| `cache_storage_auth_type` | `Noauth` | Cache authentication method (`Noauth`/`ZMQ`/`AK/SK`). |
| `cache_storage_auth_enable` | `false` | Whether to enable cache authentication. |

### [values.etcd] and etcd Security Configuration

| Field | Default Value | Description |
| ---- | ---- | ---- |
| `enable_multi_master` | `false` | Whether to enable multi-master (multi-etcd node) mode. |
| `auth_type` | `Noauth` | etcd authentication type (e.g., `Noauth`, `TLS`). |
| `table_prefix` | `""` | etcd table prefix. |

#### [values.etcd.auth]

| Field | Default Value | Description |
| ---- | ---- | ---- |
| `base_path` | `""` | etcd certificate directory. |
| `ca_file` | `ca.crt` | etcd CA filename. |
| `cert_file` | `server.crt` | etcd server certificate filename. |
| `key_file` | `server.key` | etcd server private key filename. |
| `client_cert_file` | `client.crt` | etcd client certificate filename. |
| `client_key_file` | `client.key` | etcd client private key filename. |

#### [[values.etcd.address]]

| Field | Default Value | Description |
| ---- | ---- | ---- |
| `ip` | `{{ values.host_ip }}` | etcd node IP. |
| `peer_port` | `{{ 32380\|check_port() }}` | etcd peer port. |
| `port` | `{{ 32379\|check_port() }}` | etcd client port. |

### Component Address and Port Fields

| Configuration Section | Field | Default Value | Description |
| ---- | ---- | ---- | ---- |
| `[values.ds_master]` | `ip` | `{{ values.host_ip }}` | ds_master address. |
| `[values.ds_master]` | `port` | `{{ 12123\|check_port() }}` | ds_master port. |
| `[values.ds_worker]` | `ip` | `{{ values.host_ip }}` | ds_worker address. |
| `[values.ds_worker]` | `port` | `{{ 31501\|check_port() }}` | ds_worker port. |
| `[values.function_master]` | `ip` | `{{ values.host_ip }}` | function_master address. |
| `[values.function_master]` | `global_scheduler_port` | `{{ 22770\|check_port() }}` | global scheduler port. |
| `[values.function_proxy]` | `ip` | `{{ values.host_ip }}` | function_proxy address. |
| `[values.function_proxy]` | `port` | `{{ 22772\|check_port() }}` | function_proxy HTTP port. |
| `[values.function_proxy]` | `grpc_listen_port` | `{{ 22773\|check_port() }}` | function_proxy gRPC port. |
| `[values.function_agent]` | `ip` | `{{ values.host_ip }}` | function_agent address. |
| `[values.function_agent]` | `port` | `{{ 58866\|check_port() }}` | function_agent port. |

### Dashboard Related Fields

| Configuration Section | Field | Default Value | Description |
| ---- | ---- | ---- | ---- |
| `[values.dashboard]` | `ip` | `{{ values.host_ip }}` | dashboard address. |
| `[values.dashboard]` | `port` | `{{ 9080\|check_port() }}` | dashboard HTTP port. |
| `[values.dashboard]` | `grpc_port` | `{{ 9081\|check_port() }}` | dashboard gRPC port. |
| `[values.dashboard.auth]` | `enable` | `false` | dashboard TLS switch. |
| `[values.dashboard.auth]` | `cert_file` | `""` | dashboard certificate path. |
| `[values.dashboard.auth]` | `key_file` | `""` | dashboard private key path. |
| `[values.dashboard.prometheus]` | `address` | `""` | prometheus address. |
| `[values.dashboard.prometheus.auth]` | `enable` | `false` | prometheus TLS switch. |
| `[values.dashboard.prometheus.auth]` | `base_path` | `""` | prometheus certificate directory. |
| `[values.dashboard.prometheus.auth]` | `ca_file` | `""` | prometheus CA file. |
| `[values.dashboard.prometheus.auth]` | `cert_file` | `""` | prometheus client certificate. |
| `[values.dashboard.prometheus.auth]` | `key_file` | `""` | prometheus client private key. |

## Examples

```shell
yr config dump
```

```shell
yr config dump -s 'values.ds_master.port=12123' -s 'values.fs.log.level="DEBUG"'
```

```shell
yr config template
```
