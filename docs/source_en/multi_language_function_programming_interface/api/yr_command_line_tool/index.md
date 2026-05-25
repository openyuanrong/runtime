# CLI - the yr command

`yr` is the openYuanrong Python deployment and operations CLI, used for component startup, shutdown, status checking, and configuration rendering in host mode.

## Lifecycle Commands

| Command | Description |
| ---- | ---- |
| [`start`](./yr_start.md) | Start the cluster (`master` or `agent` mode). |
| [`launch`](./yr_launch.md) | Start a single component only, typically used for container entrypoint scenarios. |
| [`health`](./yr_health.md) | Read the session file to check the health status of components on the current node. |
| [`status`](./yr_status.md) | View cluster status (only queryable in `master` mode for cluster resource views). |
| [`stop`](./yr_stop.md) | Stop daemon/component processes recorded in the session. |

## Configuration Commands

| Command | Description |
| ---- | ---- |
| [`config`](./yr_config.md) | Output merged configuration or built-in configuration templates. |

## Global Options

| Option | Description |
| ---- | ---- |
| `-c, --config PATH` | Specify the `config.toml` path; defaults to `/etc/yuanrong/config.toml` if not specified. |
| `-v, --verbose` | Enable DEBUG logging. |
| `--version` | Output the `yr` version and exit. |
| `-h, --help` | View help information. |

```{eval-rst}
.. toctree::
  :hidden:

  yr_start
  yr_launch
  yr_health
  yr_status
  yr_stop
  yr_config
  yr_version
```

## Get Installation Path

```shell
python3 -c "import yr;print(yr.__path__[0])"
# /usr/local/lib/python3.9/site-packages/yr
```
