# Secure Communication

openYuanrong supports TLS 1.2, TLS 1.3 protocols for secure communication between external and components. This section will introduce how to generate related certificates.

Required certificate files and directory planning are as follows. If dashboard is not deployed, you can skip the dashboard and prometheus certificate generation process.

```text
${Your_Workspace}
├── etcd // etcd certificate directory
│   ├── ca.crt
│   ├── client.crt
│   ├── client.key
│   ├── server.crt
│   └── server.key
├── yr // openYuanrong function system component certificate directory
│   ├── ca.crt
│   ├── module.crt
│   └── module.key
├── curve // openYuanrong data system curve certificate directory
│   ├── worker.key
│   ├── worker.key_secret
│   ├── client.key
│   ├── client.key_secret
│   └── worker_authorized_clients
│       ├── client.key
│       └── worker.key
├── dashboard // openYuanrong dashboard component one-way authentication certificate directory
│   ├── server.crt
│   └── server.key
└── prometheus // prometheus certificate directory
    ├── ca.crt
    ├── client.crt
    ├── client.key
    ├── server.crt
    └── server.key
```

## Environment Preparation

A Linux host.

### Install openssl

We use openssl to generate etcd and openYuanrong function system related certificates.

```shell
yum install openssl -y

# Check installation version.
openssl version
```

### Install ZeroMQ

openYuanrong data system components use ZeroMQ as RPC communication framework. Public and private keys depend on interfaces provided by ZeroMQ. At the same time, ZeroMQ's security functions depend on libsodium.

[Download](https://github.com/jedisct1/libsodium/releases/tag/1.0.18-RELEASE){target="_blank"} version 1.0.18 package `libsodium-1.0.18.tar.gz`, refer to the following commands to install libsodium.

```shell
tar -zxvf libsodium-1.0.18.tar.gz
cd libsodium-1.0.18
./configure
make && make install
```

[Download](https://github.com/zeromq/libzmq/releases/tag/v4.3.5){target="_blank"} version 4.3.5 package `zeromq-4.3.5.tar.gz`, refer to the following commands to install ZeroMQ.

```shell
tar -zxvf zeromq-4.3.5.tar.gz
cd zeromq-4.3.5
mkdir build
cd build
cmake .. -DENABLE_CURVE:BOOL=ON -DWITH_LIBSODIUM:BOOL=ON -DWITH_LIBSODIUM_STATIC:BOOL=ON
make && make install
```

### Create Working Directory

Create a working directory to save generated certificate files, for example `/opt/security_deployment`. Subsequent operations will use `${WorkSpace}` to represent your working directory.

```shell
mkdir -p /opt/security_deployment
export WorkSpace=/opt/security_deployment
```

## Generate etcd Certificate Files

The following steps will generate 5 files: etcd CA certificate, server certificate, server private key, client certificate and client private key.

### Generate etcd CA Certificate

Execute the following commands to generate CA certificate file `ca.crt`.

```shell
mkdir -m 700 -p ${WorkSpace}/etcd
cd ${WorkSpace}/etcd
openssl genrsa -out ca.key 2048

# Certificate CN can be modified according to actual configuration, default is etcd-ca. days is certificate validity period (days), certificates need to be regenerated after expiration.
openssl req -x509 -new -nodes -key ca.key -subj "/CN=etcd-ca" -days 10000 -out ca.crt
```

### Generate etcd Server Private Key and Certificate

Create `etcd.conf` file, where alt_names configuration item value `127.0.0.1` should be replaced with your openYuanrong cluster master node IP.

```shell
cd ${WorkSpace}/etcd
cat >etcd.conf << EOF
[req]
distinguished_name = req_distinguished_name
req_extensions = v3_req
prompt = no
[req_distinguished_name]
CN = etcd-ca
[v3_req]
keyUsage = keyEncipherment, dataEncipherment
extendedKeyUsage = serverAuth, clientAuth
subjectAltName = @alt_names
[alt_names]
IP.1 = 127.0.0.1
DNS.1 = 127.0.0.1
EOF
```

Execute commands in sequence to generate server private key `server.key` and server certificate `server.crt` files.

```shell
cd ${WorkSpace}/etcd
openssl genrsa -out server.key 2048

# Certificate CN can be modified according to actual configuration, here default is etcd-server.
openssl req -new -key server.key -subj "/CN=etcd-server" -out server.csr -config etcd.conf

# days is certificate validity period (days), certificates need to be regenerated after expiration.
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 10000 -extfile etcd.conf -extensions v3_req
```

### Generate etcd Client Private Key and Certificate

Execute commands in sequence to generate client private key `client.key` and client certificate `client.crt` files.

```shell
cd ${WorkSpace}/etcd
openssl genrsa -out client.key 2048

# Certificate CN can be modified according to actual configuration, here default is etcd-client.
openssl req -new -key client.key -subj "/CN=etcd-client" -out client.csr

# days is certificate validity period (days), certificates need to be regenerated after expiration.
openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 10000
```

### Save etcd Certificate Files

At this point, certificate files required by etcd have been generated. Execute the following commands to save these files to our planned directory.

```shell
mkdir -p ${WorkSpace}/cert/etcd
cd ${WorkSpace}/etcd
cp -ar ca.crt client.crt client.key  server.key server.crt ${WorkSpace}/cert/etcd
```

## Generate openYuanrong Function System Component Certificate Files

The following steps will generate 3 files: openYuanrong function system component CA certificate, private key and certificate.

### Create Configuration File

Execute the following commands to create configuration file, where alt_names configuration item value `127.0.0.1` should be replaced with your openYuanrong cluster master node IP.

```shell
mkdir -p ${WorkSpace}/yr
cd ${WorkSpace}/yr
rm -rf demoCA 2>/dev/null
mkdir -p ./demoCA/newcerts/
touch ./demoCA/index.txt
touch ./demoCA/index.txt.attr
echo 01 > ./demoCA/serial
if [ ! -f  ./demoCA/v3.ext ];
then
cat >./demoCA/v3.ext << EOF
[ v3_req ]
basicConstraints = CA:FALSE
keyUsage = nonRepudiation, digitalSignature, keyEncipherment
subjectAltName = @alt_names
[ v3_ca ]
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid:always,issuer
basicConstraints = critical,CA:true
[ alt_names ]
DNS.1 = test
DNS.2 = 127.0.0.1
IP.1 = 127.0.0.1
EOF
fi
```

### Generate openYuanrong Function System Component CA Certificate

Execute the following commands to generate CA certificate file `ca.crt`.

```shell
cd ${WorkSpace}/yr
openssl genrsa -out ca.key 2048

# subj can be modified according to actual configuration.
openssl req -new -key ca.key -out ca.csr -subj "/C=CN/ST=zhejiang/L=hangzhou/O=ha/OU=Personal/CN=rootCA"

# days is certificate validity period (days), certificates need to be regenerated after expiration.
openssl x509 -req -days 10000 -extfile ./demoCA/v3.ext -extensions v3_ca -signkey ca.key -in ca.csr -out ca.crt
```

### Generate openYuanrong Function System Component Private Key and Certificate

Execute commands in sequence to generate private key `module.key` and certificate `module.crt` files.

```shell
cd ${WorkSpace}/yr
openssl genrsa -out module.key 2048

# subj can be modified according to actual configuration
openssl req -new -key module.key -out module.csr -subj "/C=CN/ST=zhejiang/L=hangzhou/O=ha/OU=Personal/CN=test"
openssl ca -extfile ./demoCA/v3.ext -days 10000 -extensions v3_req -in module.csr -out module.crt -cert ca.crt -keyfile ca.key -notext -batch
```

### Save openYuanrong Function System Component Certificate Files

At this point, certificate files required by openYuanrong function system components have been generated. Execute the following commands to save these files to our planned directory.

```shell
mkdir -p ${WorkSpace}/cert/yr
cd ${WorkSpace}/yr
cp -ar ca.crt module.crt module.key ${WorkSpace}/cert/yr
```

## Generate openYuanrong Data System Component Curve Certificate Files

### Generate worker and client Public and Private Keys

We will use ZeroMQ's interface `zmq_curve_keypair` to generate public and private keys. For detailed interface introduction, refer to [Zmq Interface Documentation](http://api.zeromq.org/4-0:zmq-curve-keypair){target="_blank"}.

Create working directory.

```shell
mkdir -p ${WorkSpace}/yr_data_system
cd ${WorkSpace}/yr_data_system
```

Create `create_cert.cpp` file in current directory, content as follows.

```c++
extern "C" {
#include <zmq.h>
}
#include <iostream>
#include <fstream>

bool GenerateCurveKeyPair(std::pair<std::string, std::string> &outKeyPair)
{
    char publicKey[41];
    char secretKey[41];
    int ret = zmq_curve_keypair(publicKey, secretKey);
    if (ret != 0) {
        return true;
    }
    outKeyPair.first = publicKey;
    outKeyPair.second = secretKey;
    return false;
}

static void SaveFile(const std::string &fileName, const std::string &fileContent)
{
    std::cout << "Saving file: " << fileName << std::endl;
    std::ofstream public_file(fileName);
    public_file << fileContent;
    public_file.close();
}

int main(int argc, char **argv)
{
    std::pair<std::string, std::string> keys;
    bool ret = GenerateCurveKeyPair(keys);
    if (ret) {
        std::cout << "create cert for worker failed" << std::endl;
        return -1;
    }

    // Please do not modify file names
    SaveFile("./worker.key", keys.first);
    SaveFile("./worker.key_secret", keys.second);

    std::pair<std::string, std::string> clientKeys;
    ret = GenerateCurveKeyPair(clientKeys);
    if (ret) {
        std::cout << "create cert for client failed" << std::endl;
        return -1;
    }

    // Please do not modify file names
    SaveFile("./client.key", clientKeys.first);
    SaveFile("./client.key_secret", clientKeys.second);
}
```

Execute the following commands in sequence:

```shell
g++ create_cert.cpp -lzmq -o create_cert
export LD_LIBRARY_PATH=/usr/local/lib64:/usr/local/lib
./create_cert
```

Confirm no error messages are printed and `worker.key`, `worker.key_secret`, `client.key` and `client.key_secret` 4 files are generated in the directory.

### Save openYuanrong Data System Component Certificate Files

At this point, certificate files required by openYuanrong data system components have been generated. Execute the following commands to save these files to our planned directory.

```shell
mkdir -p ${WorkSpace}/cert/curve
cd ${WorkSpace}/yr_data_system
cp -ar worker.key worker.key_secret client.key client.key_secret ${WorkSpace}/cert/curve
mkdir -p ${WorkSpace}/cert/curve/worker_authorized_clients
cp ${WorkSpace}/cert/curve/worker.key ${WorkSpace}/cert/curve/worker_authorized_clients/worker.key
cp ${WorkSpace}/cert/curve/client.key ${WorkSpace}/cert/curve/worker_authorized_clients/client.key
```

## Generate dashboard Certificate Files

The following steps will generate 2 files: dashboard server certificate and server private key for one-way authentication communication.

```shell
mkdir -p ${WorkSpace}/cert/dashboard
cd ${WorkSpace}/cert/dashboard
openssl req -x509 -newkey rsa:2048 -keyout server.key -out server.crt -days 10000 -nodes -subj "/C=CN/ST=zhejiang/L=hangzhou/O=ha/OU=Personal/CN=dashboard"
```

## Generate prometheus Certificate Files

The following steps will generate 5 files: prometheus CA certificate, server certificate, server private key, client certificate and client private key.

### Create Configuration File

Execute the following commands to create configuration file, where alt_names configuration item value `127.0.0.1` should be replaced with the IP of the machine running prometheus.

```shell
mkdir -p ${WorkSpace}/prometheus
cd ${WorkSpace}/prometheus
rm -rf demoCA 2>/dev/null
mkdir -p ./demoCA/newcerts/
touch ./demoCA/index.txt
touch ./demoCA/index.txt.attr
echo 01 > ./demoCA/serial
if [ ! -f  ./demoCA/v3.ext ];
then
cat >./demoCA/v3.ext << EOF
[ v3_req ]
basicConstraints = CA:FALSE
keyUsage = nonRepudiation, digitalSignature, keyEncipherment
subjectAltName = @alt_names
[ v3_ca ]
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid:always,issuer
basicConstraints = critical,CA:true
[ alt_names ]
DNS.1 = prometheus
DNS.2 = 127.0.0.1
IP.1 = 127.0.0.1
EOF
fi
```

### Generate prometheus CA Certificate

Execute the following commands to generate CA certificate file `ca.crt`.

```shell
cd ${WorkSpace}/prometheus
openssl genrsa -out ca.key 2048

# subj can be modified according to actual configuration.
openssl req -new -key ca.key -out ca.csr -subj "/C=CN/ST=zhejiang/L=hangzhou/O=ha/OU=Personal/CN=rootCA"

# days is certificate validity period (days), certificates need to be regenerated after expiration.
openssl x509 -req -days 10000 -extfile ./demoCA/v3.ext -extensions v3_ca -signkey ca.key -in ca.csr -out ca.crt
```

### Generate prometheus Server Private Key and Certificate

Execute commands in sequence to generate server private key `server.key` and server certificate `server.crt` files.

```shell
cd ${WorkSpace}/prometheus
openssl genrsa -out server.key 2048

# subj can be modified according to actual configuration
openssl req -new -key server.key -out server.csr -subj "/C=CN/ST=zhejiang/L=hangzhou/O=ha/OU=Personal/CN=prometheus"
openssl ca -extfile ./demoCA/v3.ext -days 10000 -extensions v3_req -in server.csr -out server.crt -cert ca.crt -keyfile ca.key -notext -batch
```

### Generate prometheus Client Private Key and Certificate

Execute commands in sequence to generate client private key `client.key` and client certificate `client.crt` files.

```shell
cd ${WorkSpace}/prometheus
openssl genrsa -out client.key 2048

# Certificate CN can be modified according to actual configuration, here default is prometheus-client.
openssl req -new -key client.key -subj "/CN=prometheus-client" -out client.csr

# days is certificate validity period (days), certificates need to be regenerated after expiration.
openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 10000
```

### Save prometheus Certificate Files

At this point, certificate files required by prometheus have been generated. Execute the following commands to save these files to our planned directory.

```shell
mkdir -p ${WorkSpace}/cert/prometheus
cd ${WorkSpace}/prometheus
cp -ar ca.crt client.crt client.key server.key server.crt ${WorkSpace}/cert/prometheus
```

### Secure Communication prometheus Configuration and Startup

**Step1: Basic Configuration**

Please complete prometheus basic configuration first, refer to [prometheus Configuration and Startup](observability-prometheus).

**Step2: Security Configuration**

In the same directory as prometheus program, add `web-config.yml` file, file content as follows:

  ```bash
  tls_server_config:
  cert_file: ${WorkSpace}/cert/prometheus/server.crt
  key_file: ${WorkSpace}/cert/prometheus/server.key
  client_auth_type: "RequireAndVerifyClientCert"
  client_ca_file: ${WorkSpace}/cert/prometheus/ca.crt
  ```

**Step3: Start prometheus**

Command to start secure communication prometheus is as follows:

  ```shell
  nohup ./prometheus --web.config.file=./web-config.yml > ./prometheus.log 2>&1 & # prometheus default port is 9090
  ```

## Configure Secure Communication When Deploying openYuanrong

Create custom configuration file `config.toml`, content as follows, replace `/opt/ssl` in it with `${WorkSpace}` path.

- Deploy basic components only

    ```toml
    [values.fs.tls]
    enable = true
    base_path = "/opt/ssl/cert/yr"
    ca_file = "ca.crt"
    cert_file = "module.crt"
    key_file = "module.key"

    [values.ds.curve]
    enable = true
    base_path = "/opt/ssl/cert/curve"
    cache_storage_auth_type = "ZMQ"
    cache_storage_auth_enable = true

    [values.etcd]
    auth_type = "TLS"

    [values.etcd.auth]
    base_path = "/opt/ssl/cert/etcd"
    ca_file = "ca.crt"
    cert_file = "server.crt"
    key_file = "server.key"
    client_cert_file = "client.crt"
    client_key_file = "client.key"
    ```

- Deploy basic components and Dashboard, configure `metrics_config_file` and `address` parameters according to actual situation

    ```toml
    [mode.master]
    dashboard = true
    collector = true
    frontend = true

    [mode.agent]
    collector = true

    [function_agent.args]
    enable_metrics = true
    metrics_config_file = "/home/metrics/config.json"

    [values]
    log_level = "DEBUG"

    [values.fs.tls]
    enable = true
    base_path = "/opt/ssl/cert/yr"
    ca_file = "ca.crt"
    cert_file = "module.crt"
    key_file = "module.key"

    [values.ds.curve]
    enable = true
    base_path = "/opt/ssl/cert/curve"
    cache_storage_auth_type = "ZMQ"
    cache_storage_auth_enable = true

    [values.etcd]
    auth_type = "TLS"

    [values.etcd.auth]
    base_path = "/opt/ssl/cert/etcd"
    ca_file = "ca.crt"
    cert_file = "server.crt"
    key_file = "server.key"
    client_cert_file = "client.crt"
    client_key_file = "client.key"

    [values.dashboard.auth]
    enable = true
    cert_file = "/opt/ssl/cert/dashboard/server.crt"
    key_file = "/opt/ssl/cert/dashboard/server.key"

    [values.dashboard.prometheus]
    address = "10.88.0.3:9090"

    [values.dashboard.prometheus.auth]
    enable = true
    base_path = "/opt/ssl/cert/prometheus"
    ca_file = "ca.crt"
    cert_file = "client.crt"
    key_file = "client.key"
    ```

Use the following command to deploy master node:

```bash
yr -c ${YOUR_FILE_PATH}/config.toml start --master
```

Refer to the following command to deploy worker node:

```bash
yr -c ${YOUR_FILE_PATH}/config.toml start --master_address http://x.x.x.x:xxxx
```
