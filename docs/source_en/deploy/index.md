# Installation and Deployment

```{eval-rst}
.. toctree::
   :glob:
   :maxdepth: 1
   :hidden:

   installation
   deploy_processes/index
   deploy_on_k8s/index
   job_app_guide/index
   service_app_guide
```

## Install openYuanrong

Before developing applications, you need to [install openYuanrong](installation.md), including the installation of openYuanrong SDK and command-line tool yr.

## Deploy openYuanrong Cluster

openYuanrong supports deployment as processes on Linux hosts, and also supports deployment as containers on Kubernetes (K8s). You can deploy openYuanrong on a single Linux host or single-node Kubernetes cluster for learning and development. In production environments, it is recommended to deploy multiple nodes through a cluster to bring better performance and reliability guarantees. For detailed introduction, refer to:

* [Deploy on Hosts](deploy_processes/index.md)
* [Deploy on K8s](deploy_on_k8s/index.md)

## Deploy openYuanrong Applications

openYuanrong supports using Python, C++, and Java multi-language to develop distributed applications. To learn how to run openYuanrong applications, refer to:

* [Deploy Job Applications](job_app_guide/index.md): Deploy applications developed using single-machine program distributed parallelization interfaces.
* [Deploy Service Applications](service_app_guide.md): Deploy applications developed using function service interfaces.
