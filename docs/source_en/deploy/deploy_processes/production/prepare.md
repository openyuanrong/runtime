# Environment Preparation

This section introduces the requirements for openYuanrong deployment environment.

## Operating System and Hardware

- Host is Linux system on X86_64 or ARM_64 platform. We have fully tested on openEuler 22.03 LTS/24.03 LTS, Ubuntu 22.04 LTS and Debian 12/13. If you encounter deployment problems on other Linux systems, please contact us by raising an issue.
- Single host has at least 2 CPUs, 4G or more memory.
- All hosts in the cluster have network connectivity. Refer to [Deployment Parameters](../parameters.md) port related configurations, open up ports required for communication by openYuanrong components, open source etcd, etc.

    You can use `ping` command to detect whether two hosts are network connected, and use tools like [netcat](https://netcat.sourceforge.net/){target="_blank"} to check whether ports are open, for example:

    ```shell
    nc -vz 127.0.0.1 8888
    ```

    Test environment can run the following command to stop and disable firewall, ensure ports are available.

    ```shell
    systemctl stop firewalld
    systemctl disable firewalld

    # Verify operation result, if Active property shows inactive, it means closed.
    systemctl status firewalld
    ```

## Dependent Services and Tools

Deploy and use openYuanrong command-line tool `yr`. It depends on [net-tools](https://net-tools.sourceforge.io/){target="_blank"} to automatically obtain host IP, and depends on [netcat](https://netcat.sourceforge.net/){target="_blank"} to monitor whether there are port conflicts when randomly assigning component ports. You can install using the following commands:

```shell
yum install -y net-tools
# Verify whether installation is successful.
ifconfig

yum install -y nc
# Verify whether installation is successful.
nc -vz 127.0.0.1 8888
```

:::{Note}

net-tools and netcat are both open source tools, there is network port sniffing risk, please use with caution.

:::

## Development Dependencies and Restrictions

openYuanrong supports developing applications using C++ (version: 14 and above), Java (version: 8, 17, 21), Python (version: 3.9, 3.10, 3.11). You need to install corresponding language runtime environment on deployment hosts. Particularly, when installing multiple Java and Python language versions, you need to create corresponding soft links in /usr/bin, refer to commands as follows.

```shell
# java
ln -s ${JAVA8_INSTALL_HOME}/jre/bin/java /usr/bin/java1.8
ln -s ${JAVA17_INSTALL_HOME}/jre/bin/java /usr/bin/java17
ln -s ${JAVA21_INSTALL_HOME}/jre/bin/java /usr/bin/java21
# python
ln -sf $PYTHON_PATH_3_9/bin/python3.9 /usr/bin/python3.9
ln -sf $PYTHON_PATH_3_10/bin/python3.10 /usr/bin/python3.10
ln -sf $PYTHON_PATH_3_11/bin/python3.11 /usr/bin/python3.11
```

Other constraints:

- Maximum deployment of 1000 worker nodes in a single cluster.
- Maximum running of 10000 function instances in a single cluster.
- Single data object size cannot exceed configured node shared memory.
