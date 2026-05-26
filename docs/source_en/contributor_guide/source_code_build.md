
# Compile openYuanrong from Source Code

## Overview

This chapter introduces how to compile openYuanrong from source code.

## Environment Preparation

A host with the following configuration:

* Hardware

    | Item       | Requirement                  |
    |------------|---------------------|
    | CPU        | 4 cores or more, 16 cores recommended        |
    | Memory        | 10GB or more, 32GB recommended     |
    | Disk Space    | 50GB or more             |
    | OS Platform | Linux x64/Linux ARM |

* Operating System

    | Operating System             | Version                             |
    |---------------------|----------------------------------|
    | openEuler (x86_64)  | openEuler 22.03 LTS or above    |
    | openEuler (aarch64) | openEuler 22.03 LTS or above    |

Install the following tools used for compilation on the host and keep the versions consistent:

| Tool Name           | Tool Type  | Function        | x86_64 Version    | aarch64 Version   |
|-------------------|-----------|------------|----------------|---------------|
| AdoptOpenJDK      | Open Source Third Party | Build    | 8              | 8             |
| Apache Maven      | Open Source Third Party | Build    | 3.9.11         | 3.9.11        |
| Golang            | Open Source Third Party | Build    | 1.24.1         | 1.24.1        |
| CMake             | Open Source Third Party | Build    | 3.22.0         | 3.22.0        |
| Python            | Open Source Third Party | Build    | 3.9/3.10/3.11  | 3.9/3.10/3.11 |
| ninja-build/ninja | Open Source Third Party | Build    | 1.13.1         | 1.13.1        |
| bazelbuild/bazel  | Open Source Third Party | Build    | 6.5.0          | 6.5.0         |
| Node.js           | Open Source Third Party | Build    | 20.19.0        | 20.19.0         |

:::{tip}
We provide scripts for use on the openeuler_22.03_LTS_sp4 environment to facilitate quick installation of compilation tools. Refer to the following commands to download and use.

```bash
yum install -y wget
wget https://openyuanrong.obs.cn-southwest-2.myhuaweicloud.com/build_tools/openeuler_22.03_LTS/check_tools.sh
wget https://openyuanrong.obs.cn-southwest-2.myhuaweicloud.com/build_tools/openeuler_22.03_LTS/install_tools.sh
bash install_tools.sh
source /etc/profile.d/buildtools.sh
```

:::

## Compilation

openYuanrong is divided into four code repositories:

* [yuanrong](https://atomgit.com/openeuler/yuanrong){target="_blank"}: Runtime and dashboard repository, where runtime compilation depends on release packages from yuanrong-functionsystem and yuanrong-datasystem repositories.
* [yuanrong-functionsystem](https://atomgit.com/openeuler/yuanrong-functionsystem){target="_blank"}: Function system repository, compilation depends on release packages from yuanrong-datasystem repository.
* [yuanrong-datasystem](https://atomgit.com/openeuler/yuanrong-datasystem){target="_blank"}: Data system repository, can be compiled independently.
* [ray-adapter](https://atomgit.com/openeuler/ray-adapter){target="_blank"}: ray adapter repository, can be compiled independently.

To use the multi-language function programming interface, you need to compile the yuanrong repository; to use the data system interface alone, you need to compile the yuanrong-datasystem repository; to use the ray adapter interface, you need to compile the ray-adapter repository.

### Compile yuanrong-datasystem

First download the source code. Create a code directory, for example `mkdir -p /opt/openyuanrong/`, and execute the following command in the directory.

```bash
# Here downloads the master branch of yuanrong-datasystem repository, replace with your forked personal repository and branch as needed.
git clone -b master https://atomgit.com/openeuler/yuanrong-datasystem.git
```

Execute the following script to compile.

```bash
# Learn more compilation options through bash build.sh -h
cd /opt/openyuanrong/yuanrong-datasystem
bash build.sh -X off
```

:::{Note}

You can set compilation concurrency through the compilation option `-j`. With the compilation host idle CPU and memory 1:2 configuration (e.g., 16U32G), it is recommended to set to: number of CPU cores + 1, to avoid compilation failure due to insufficient memory during the linking phase.

:::

Compilation artifacts are generated in the `yuanrong-datasystem/output` directory, containing the following two files:

* `yr-datasystem-vx.x.x.tar.gz`: Contains SDK and binary files.
* `yr_datasystem-x.x.x.whl`: whl installation package, contains command-line tools and SDK.

### Compile yuanrong-functionsystem

**Step1: Source Code Download**

Download the function system source code through Git or other methods in the compilation directory. The following example will download the function system source code as the `yuanrong-functionsystem` folder in the `/opt/openyuanrong` folder.

```shell
cd /opt/openyuanrong
git clone -b master https://atomgit.com/openeuler/yuanrong-functionsystem.git
```

**Step2: Get Dependencies**

According to the architecture design, the function system compilation process depends on the data system SDK, so you need to copy the data system compilation artifacts or release artifacts to the function system build dependencies. The function system build script will automatically download or introduce dependencies at startup according to the configuration of [vendor/VendorList.csv file](https://atomgit.com/openeuler/yuanrong-functionsystem/blob/master/vendor/VendorList.csv){target="_blank"}. For data system compilation dependencies, it will be introduced by default through local files. That is, use the path descriptor `file://localhost/vendor/src/yr-datasystem.tar.gz` to describe the data system source and will not verify the dependency Hash value.

Assuming the data system compilation artifact path is `/opt/openyuanrong/yuanrong-datasystem/output/yr-datasystem-v0.0.0.tar.gz`, you can copy the data system compilation artifact to the default path configured in `VendorList.csv` through the following command.

```shell 
cd /opt/openyuanrong
mkdir -p yuanrong-functionsystem/vendor/src
cp -a yuanrong-datasystem/output/yr-datasystem-v0.0.0.tar.gz yuanrong-functionsystem/vendor/src/yr-datasystem.tar.gz
```

**Step3: Source Code Compilation**

In the function system source code root directory, we can use the built-in compilation script's `build` command to achieve source code compilation. The compilation process will be divided into three stages: third-party component download compilation, second-party component compilation, system source code compilation. The entire process will be completed automatically, and components that have already been compiled will be automatically skipped.

```shell
cd /opt/openyuanrong/yuanrong-functionsystem
./run.sh build
```

`build` command key parameters:

* `JOB_NUM`, `-j`: Compilation concurrency, sets the number of CPU cores used when compiling the program. Default value: OS number of cores
* `VERSION`, `-v`: Compilation version number, sets the version number recorded in the program when compiling. Default value: 0.0.0

For more commands, refer to: `./run.sh --help`, `./run.sh build --help`

:::{Note}

It is recommended to set compilation concurrency through the compilation option `-j` to avoid compilation failure due to insufficient memory during the compilation linking phase. The peak memory occupancy (GB) is about 1.5~1.8 times the concurrency, i.e., `MEM(GB) ≈ JOB(CPU) * 1.8(GB/CPU)`, it is recommended to set safe concurrency: number of CPU cores / 2

:::

**Step4: Artifact Packaging**

The main compilation artifacts of the function system are multiple binary programs and public dynamic libraries, and will be accompanied by some dependencies and runtime configurations. Therefore, you can use the `pack` command to complete the packaging of function system build artifacts. Packaging artifacts and packaging content will be uniformly stored in the `output` folder.

```bash
cd /opt/openyuanrong/yuanrong-functionsystem
./run.sh pack
```

`pack` command key parameters:

* `VERSION`, `-v`: Compilation version number, sets the version number recorded in the program when compiling. Default value: 0.0.0

For more commands, refer to: `./run.sh --help`, `./run.sh pack --help`

The directory and key artifacts are as follows:

```bash
tree output/
├── functionsystem
│   ├── bin          # Function system compilation artifacts
│   ├── config       # Function system runtime configuration
│   ├── deploy       # Function system deployment configuration
│   ├── lib          # Function system dynamic libraries
│   └── sym          # Function system symbol table
├── metrics
│   ├── config       # Observability configuration
│   ├── include      # Observability interface
│   └── lib          # Observability dynamic libraries
├── metrics.tar.gz   # Observability packaging artifacts
└── yr-functionsystem-v0.0.0.tar.gz # Function system packaging artifacts
```

### Compile Runtime and Dashboard

First download the source code. Create a code directory, for example `mkdir -p /opt/openyuanrong/`, and execute the following command in the directory.

```bash
# Here downloads the master branch of yuanrong repository, replace with your forked personal repository as needed.
git clone -b master https://atomgit.com/openeuler/yuanrong.git
```

#### Compile Runtime

Runtime compilation depends on data system and function system release packages:

* Create directory `/opt/openyuanrong/yuanrong/datasystem/output`, copy the compiled data system release package `yr-datasystem-vx.x.x.tar.gz` to this directory and extract it through `tar -zxf yr-datasystem-vx.x.x.tar.gz --strip-components=1` command. Extract the compiled function system release package `yr-functionsystem-vx.x.x.tar.gz`, copy the `metrics` folder in the package to the `/opt/openyuanrong/yuanrong/` directory.

Execute the following script to compile.

```bash
# Learn more compilation options through bash build.sh -h
cd /opt/openyuanrong/yuanrong
bash build.sh
```

If dependency download failure error occurs, you can try deleting the /opt/openyuanrong/yuanrong/thirdparty/runtime_deps directory and recompiling

:::{Note}

You can set compilation concurrency through the compilation option `-j`. With the compilation host idle CPU and memory 1:2 configuration (e.g., 16U32G), it is recommended to set to: number of CPU cores + 1, to avoid compilation failure due to insufficient memory during the linking phase.

:::

Compilation artifacts are generated in the `yuanrong/output` directory, containing the following files:

* `yr-runtime-vx.x.x.tar.gz`: Runtime release package.

#### Compile Dashboard

Dashboard is an optional component, execute the following script to compile when needed.

```bash
# Learn more compilation options through bash build.sh -h
cd /opt/openyuanrong/yuanrong/go
bash build.sh
```

Compilation artifacts are generated in the `yuanrong/output` directory, containing the following files:

* `yr-dashboard-vx.x.x.tar.gz`: Dashboard release package.

### Compile ray-adapter

First download the source code. Create a code directory, for example `mkdir -p /opt/openyuanrong/`, and execute the following command in the directory.

```bash
# Here downloads the master branch of ray-adapter repository, replace with your forked personal repository as needed.
git clone -b master https://atomgit.com/openeuler/ray-adapter.git
```

Execute the following script to compile.

```bash
# Learn more compilation options through bash build.sh -h
cd /opt/openyuanrong/ray-adapter
bash build.sh
```

Compilation artifacts are generated in the `ray-adapter/output` directory, containing the following files:

* `ray_adapter-x.x.x.whl`: whl installation package.
