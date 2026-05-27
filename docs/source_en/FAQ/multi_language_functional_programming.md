# Multi-language Function Programming FAQ

## 1. Stateless Function and Stateful Function Questions

### Correctly configured http_proxy/https_proxy proxy address in Driver, but `yr.init` interface fails to connect to openYuanrong cluster, error: "ValueError: failed to init, code: 4002, module code 20, msg: failed to connect to all addresses"

Cause: openYuanrong does not enable http_proxy/https_proxy proxy configuration by default.

Solution: Configure the following environment variable to enable http_proxy/https_proxy proxy configuration.

  ```shell
  export YR_ENABLE_HTTP_PROXY=true
  ```
