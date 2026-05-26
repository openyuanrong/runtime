# launch

Start a single component only, typically used for container entrypoint scenarios.

## Usage

```shell
yr launch [OPTIONS] COMPONENT
```

## Parameters

* `COMPONENT`: Component name, for example `etcd`, `ds_master`, `ds_worker`, `function_master`, `function_proxy`, `function_agent`, `frontend`, `dashboard`, `collector`, `meta_service`, `function_scheduler`; also supports internal components for K8s deployment such as `iam_server`, `function_manager`, `runtime_manager`.
* `--inherit-env`: Inherit parent process environment variables (default is not to inherit).
* `--env-subst KEY`: Replace `{KEY}` in the configuration file with the value of the environment variable `KEY`. Can be specified multiple times, or use comma separation (e.g., `--env-subst A,B`).

## Description

* `launch` only starts a single component and will not automatically start dependent components.
* For production deployment, please prefer using `yr start` to obtain complete dependency orchestration, health check, and session management capabilities.

## Examples

```shell
yr launch etcd
```

```shell
yr launch --inherit-env --env-subst POD_IP,HOST_IP function_proxy
```
