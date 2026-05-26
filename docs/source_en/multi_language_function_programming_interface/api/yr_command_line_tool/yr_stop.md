# stop

Stop openYuanrong processes in the current session.

## Usage

```shell
yr stop [OPTIONS]
```

## Parameters

* `--force`: Force stop, using `SIGKILL`.
* `-f, --file SESSION_FILE`: Specify the session file path (defaults to `/tmp/yr_sessions/latest/session.json`).

## Description

* Default behavior: Send `SIGTERM` to the daemon, which then executes the graceful shutdown process for components.
* `--force` behavior: First `SIGKILL` the daemon, then forcibly stop component PIDs based on the session file.
* If the session file does not exist or is invalid, the command will return failure.

## Examples

```shell
yr stop
```

```shell
yr stop --force
```

```shell
yr stop -f /tmp/yr_sessions/20260209_020054/session.json
```
