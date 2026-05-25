# health

Check the health status of components on the current node.

## Usage

```shell
yr health [OPTIONS]
```

## Parameters

* `-f, --file SESSION_FILE`: Specify the session file path (defaults to `/tmp/yr_sessions/latest/session.json`).

## Description

* `health` reads the component PIDs from the session file and checks whether the processes are still running.
* If the session does not exist or components are not running, it will return a non-zero exit code.

## Examples

```shell
yr health
```

```shell
yr health -f /tmp/yr_sessions/20260209_020054/session.json
```
