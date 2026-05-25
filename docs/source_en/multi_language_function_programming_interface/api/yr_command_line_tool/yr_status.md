# status

Query cluster status and resource views.

## Usage

```shell
yr status [OPTIONS]
```

## Parameters

* `-f, --file SESSION_FILE`: Specify the session file path (defaults to `/tmp/yr_sessions/latest/session.json`).

## Description

* `status` relies on cluster connection information in the session file.
* Only `master` mode will output the complete cluster status; if the session is from `agent` mode, it will return an error.
* When function master has TLS enabled, `status` will automatically perform HTTPS queries according to the session configuration.

## Examples

```shell
yr status
```

```shell
yr status -f /tmp/yr_sessions/20260209_020054/session.json
```
