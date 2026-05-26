# Installation and Deployment FAQ

## 1. Installation Questions

### Successfully installed openYuanrong, but when running Python Driver program, error occurs: "ModuleNotFoundError: No module named 'yr'"

Cause: Multiple Python versions exist in current environment.

Solution: Use the Python version corresponding to when you installed openYuanrong to run the program.

## 2. Deployment Questions

### Deploy openYuanrong on host, check cluster status through `yr status` command, worker node has not joined cluster

- Cause 1: Host firewall is enabled, ports used by openYuanrong components are not accessible.

  Solution: In non-production environments, you can temporarily disable firewall using the following commands and redeploy. In production environments, you need to open ports used by openYuanrong.

  ```shell
  systemctl stop firewalld
  systemctl disable firewalld
  ```

- Cause 2: openYuanrong versions installed on master and worker nodes are inconsistent. You can check installed version through `pip show yr` command.

  Solution: Reinstall the same openYuanrong version on each node.

### Deploy openYuanrong on host, when checking cluster status through `yr status` command, error occurs: "Connect to etcd server failed.context deadline exceeded."

Cause: http_proxy/https_proxy proxy is configured in current environment.

Solution: Disable proxy configuration.
