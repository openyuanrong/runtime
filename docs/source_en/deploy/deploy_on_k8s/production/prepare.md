# Environment Preparation

This section introduces the requirements for openYuanrong deployment environment.

## Operating System and Hardware

- Host operating system platform is Linux X86_64 or ARM_64.
- Single host has at least 16 CPUs, 32G or more memory, total cluster has at least 32 CPUs, 64G or more memory.
- Host disk available space is greater than 40G, used for downloading openYuanrong component images.

## Dependent Services

Before deploying openYuanrong, you need to install and deploy the following tools and services first.

- K8s cluster and kubectl tool version 1.19.4 or above. For learning and development, you can deploy through [kind](https://kind.sigs.k8s.io/docs/user/quick-start/){target="_blank"}, for production environment, it is recommended to deploy through [Kubeadm](https://kubernetes.io/docs/reference/setup-tools/kubeadm/){target="_blank"}.
- MinIO without version restriction, for deployment refer to [MinIO Kubernetes](https://minio.org.cn/docs/minio/kubernetes/upstream/){target="_blank"}.
- ETCD v3 version, for deployment refer to [Run etcd clusters as a Kubernetes StatefulSet](https://etcd.io/docs/v3.5/op-guide/kubernetes/){target="_blank"}.
- helm tool version 3.2 or above, used for deploying openYuanrong, for installation refer to [Installing Helm](https://helm.sh/zh/docs/intro/install/){target="_blank"}.
 