# Function Service Connecting to Backend Storage

In service application development, business logic often needs to connect to backend services such as MySQL, Redis for data storage, or Kafka, RabbitMQ for message passing. When developing applications based on openYuanrong function services, you can use the connection methods provided natively by the backend service, such as client SDKs, to establish connections with the server.

## Solution Overview

Use openYuanrong function service to develop an application in Java that provides a REST API to connect to a Redis cluster for reading and writing data. The function implements two methods: an initialization method for establishing connection with the Redis cluster, and an execution method for processing external requests.

Note that the reason for handling connection establishment and connection pool creation logic in the function service's initialization method is that the initialization method is executed only once when the function instance starts, and the created connections can be reused in the execution method when processing requests, avoiding performance issues caused by repeated connection establishment.

## Prerequisites

1. [Deploy openYuanrong cluster on K8s](../../deploy/deploy_on_k8s/index.md). Use kubectl tool on the K8s cluster master node to obtain openYuanrong server endpoints.

    Meta service, which is responsible for function and resource pool management.

    ```bash
    echo "http://$(kubectl get nodes -o jsonpath='{.items[0].status.addresses[?(@.type=="InternalIP")].address}'):$(kubectl get svc meta-service -o jsonpath='{.spec.ports[0].nodePort}')"
    ```

    Frontend service, which is responsible for receiving traffic and handling function invocation.

    ```bash
    echo "http://$(kubectl get nodes -o jsonpath='{.items[0].status.addresses[?(@.type=="InternalIP")].address}'):$(kubectl get svc frontend-lb -o jsonpath='{.spec.ports[0].nodePort}')"
    ```

2. Install the function service [Java SDK](install-yuanrong-java-sdk) faas-function-sdk and [MinIO Client](tools-minio-client). MinIO Client is used to upload code packages to the MinIO service in the openYuanrong cluster.
3. Redis cluster has been deployed.

## Implementation Process

### Create Function Service Application Project

Create a Java language function service application project with the following directory structure, including five files: pom.xml, zip_file.xml, FaaSHandler.java, Scenario.java, and Utils.java.

- pom.xml: Maven configuration file, introducing dependencies on openYuanrong SDK, Redis, etc., and build plugin configuration.
- zip_file.xml: Code packaging configuration.
- FaaSHandler.java: Core business processing class, including `initializer` initialization method and `handler` execution method.
- Scenario.java: Format external request parameters.
- Utils.java: Redis connection configuration.

``` bash
access-reids
├── pom.xml
└── src
    └── main
        ├── assembly
        │   └── zip_file.xml
        └── java
            └── com
                └── yuanrong
                    └── demo
                        ├── FaaSHandler.java
                        ├── Scenario.java
                        └── Utils.java
```

:::{dropdown} pom.xml file content, **must update faas-function-sdk version number to your installed version**
:chevron: down-up
:icon: chevron-down

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <groupId>org.example</groupId>
    <artifactId>access-redis</artifactId>
    <version>1.0.0</version>

    <properties>
        <project.build.sourceEncoding>UTF-8</project.build.sourceEncoding>
        <maven.compiler.source>1.8</maven.compiler.source>
        <maven.compiler.target>1.8</maven.compiler.target>
        <maven.build.timestamp.format>yyyyMMddHHmmss</maven.build.timestamp.format>
        <package.finalName>access-redis</package.finalName>
        <package.outputDirectory>target</package.outputDirectory>
    </properties>

    <dependencies>
        <dependency>
            <groupId>org.apache.commons</groupId>
            <artifactId>commons-pool2</artifactId>
            <version>2.12.0</version>
        </dependency>
        <dependency>
            <groupId>redis.clients</groupId>
            <artifactId>jedis</artifactId>
            <version>5.2.0</version>
            <!-- Exclude slf4j-api to avoid conflicts with openYuanrong's log4j dependency -->
            <exclusions>
                <exclusion>
                    <groupId>org.slf4j</groupId>
                    <artifactId>slf4j-api</artifactId>
                </exclusion>
            </exclusions>
        </dependency>
        <dependency>
            <groupId>org.yuanrong</groupId>
            <artifactId>faas-function-sdk</artifactId>
            <!-- Update version number to your actual version -->
            <version>${VERSION}</version>
        </dependency>
        <dependency>
            <groupId>com.google.code.gson</groupId>
            <artifactId>gson</artifactId>
            <version>2.9.0</version>
        </dependency>
    </dependencies>

    <build>
        <plugins>
            <!-- Configure packaging as follows -->
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-assembly-plugin</artifactId>
                <version>2.2</version>
                <configuration>
                    <archive>
                        <manifest>
                            <addDefaultImplementationEntries>true</addDefaultImplementationEntries>
                        </manifest>
                    </archive>
                    <appendAssemblyId>false</appendAssemblyId>
                </configuration>
                <executions>
                    <execution>
                        <id>auto-deploy</id>
                        <phase>package</phase>
                        <goals>
                            <goal>single</goal>
                        </goals>
                        <configuration>
                            <descriptors>
                                <descriptor>src/main/assembly/zip_file.xml</descriptor>
                            </descriptors>
                            <finalName>${package.finalName}</finalName>
                            <outputDirectory>${package.outputDirectory}</outputDirectory>
                            <archiverConfig>
                                <directoryMode>0700</directoryMode>
                                <fileMode>0600</fileMode>
                                <defaultDirectoryMode>0700</defaultDirectoryMode>
                            </archiverConfig>
                        </configuration>
                    </execution>
                </executions>
            </plugin>
        </plugins>
    </build>
</project>
```

:::
:::{dropdown} zip_file.xml file content
:chevron: down-up
:icon: chevron-down

```xml
<?xml version="1.0" encoding="UTF-8"?>

<assembly xmlns="http://maven.apache.org/plugins/maven-assembly-plugin/assembly/1.1.2">

    <id>auto-deploy</id>
    <baseDirectory/>
    <formats>
        <format>zip</format>
    </formats>

    <fileSets>
        <fileSet>
            <directory>src/main/resources/</directory>
            <outputDirectory>config</outputDirectory>
            <includes>
                <include>**</include>
            </includes>
            <fileMode>0600</fileMode>
            <directoryMode>0700</directoryMode>
        </fileSet>

    </fileSets>

    <dependencySets>
        <dependencySet>
            <outputDirectory>lib</outputDirectory>
            <scope>runtime</scope>
        </dependencySet>
    </dependencySets>
</assembly>
```

:::
:::{dropdown} FaaSHandler.java file content
:chevron: down-up
:icon: chevron-down

```java
package com.yuanrong.demo;

import org.yuanrong.services.runtime.Context;
import org.yuanrong.services.runtime.RuntimeLogger;

import com.google.gson.Gson;
import com.google.gson.JsonObject;

import org.apache.commons.pool2.impl.GenericObjectPoolConfig;
import redis.clients.jedis.Connection;
import redis.clients.jedis.HostAndPort;
import redis.clients.jedis.JedisCluster;

import java.time.Duration;
import java.util.Set;
import java.util.HashSet;

public class FaaSHandler {
    // Reusable Redis cluster client
    private JedisCluster jedisCluster = null;

    // Initialization method, executed once when the instance starts to establish redis connection pool
    public void initializer(Context context) {
        // Use openYuanrong built-in logging module to print logs
        RuntimeLogger log = context.getLogger();

        // Get redis cluster address from environment variables, addresses separated by commas, e.g.: 10.x.x.1:6379,10.x.x.2:6379,10.x.x.3:6379
        String redisClusterNodes = context.getUserData("redis_ip_address");
        log.info("redis cluster address:" + redisClusterNodes);

        // Get redis password from environment variables
        String pwd = context.getUserData("redis_password");
        Set<HostAndPort> jedisClusterNodes = new HashSet<>();
        for (String clusterNodeStr : redisClusterNodes.split(",")) {
            String[] nodeInfo = clusterNodeStr.split(":");
            jedisClusterNodes.add(new HostAndPort(nodeInfo[0], Integer.parseInt(nodeInfo[1])));
        }
        GenericObjectPoolConfig<Connection> config = Utils.getJedisPoolConfig();
        jedisCluster = new JedisCluster(jedisClusterNodes, Utils.REDIS_CONNECT_TIMEOUT,
                Utils.REDIS_READ_TIMEOUT, 3,
                pwd, "faas-redis-client", config);
    }

    // Business processing method, triggered by each external request, body parameters and return value types can be customized
    public JsonObject handler(String body, Context context){
        RuntimeLogger log = context.getLogger();
        log.info("request body:" + body);

        Gson gson = new Gson();
        Scenario s = gson.fromJson(body, Scenario.class);
        if (s == null) {
            log.info("parse object failed");
        }
        int size = s.getRecords().size();
        if (size <= 0 || size > 1000) {
            log.error("Records number must [1,1000],current:" + size);
            return new Gson().fromJson("{\"success\":false,\"message\":\"records must range from 1 to 1000\"}", JsonObject.class);
        }

        long  startTime = System.currentTimeMillis();
        for(String record:s.getRecords()){
            try {
                jedisCluster.lpush(s.getScenarioName(), record);
            }catch (Exception e){
                log.error("Redis error:" + e.getMessage());
                return new Gson().fromJson("{\"success\":false,\"message\":\""+e.getMessage()+"\"}", JsonObject.class);
            }
        }
        long endTime = System.currentTimeMillis();
        log.info("write records cost:" + (endTime - startTime));

        long count = jedisCluster.llen(s.getScenarioName());
        log.info("total records:" + count);
        for (long i = 0; i < count; i++) {
            String record = jedisCluster.lpop(s.getScenarioName());
            log.info("Pop record:" + record);
        }
        log.info("all is ok");
        return new Gson().fromJson("{\"success\":true, \"message\":\"\"}", JsonObject.class);
    }
}
```

:::
:::{dropdown} Scenario.java file content
:chevron: down-up
:icon: chevron-down

```java
package com.yuanrong.demo;

import java.util.ArrayList;

public class Scenario {
    private String scenarioName;
    private ArrayList<String> records;

    public Scenario(String scenarioName, ArrayList<String> records) {
        super();
        this.scenarioName = scenarioName;
        this.records = records;
    }

    public Scenario() {
        super();
    }

    @Override
    public String toString() {
        return "scenarioName:" + scenarioName + ";records:" + records;
    }

    public void setScenarioName(String scenarioName) {
        this.scenarioName = scenarioName;
    }

    public String getScenarioName() {
        return scenarioName;
    }

    public void setRecords(ArrayList<String> records) {
        this.records = records;
    }

    public ArrayList<String> getRecords() {
        return records;
    }
}
```

:::
:::{dropdown} Utils.java file content
:chevron: down-up
:icon: chevron-down

```java
package com.yuanrong.demo;

import org.apache.commons.pool2.impl.GenericObjectPoolConfig;

import java.time.Duration;

public class Utils {
    public static final Integer REDIS_CONNECT_TIMEOUT = 3000;
    public static final Integer REDIS_READ_TIMEOUT = 2000;
    public static final Integer REDIS_DEFAULT_MIN_IDLE = 5;
    public static final Integer REDIS_DEFAULT_MAX_TOTAL = 50;
    public static final Integer REDIS_DEFAULT_MAX_IDLE = 50;

    public static <T> GenericObjectPoolConfig<T> getJedisPoolConfig() {
        GenericObjectPoolConfig<T> config = new GenericObjectPoolConfig<>();
        config.setTestOnBorrow(false);
        config.setTestOnReturn(false);
        config.setTestOnCreate(false);
        config.setTestWhileIdle(true);
        config.setBlockWhenExhausted(true);
        config.setMaxWait(Duration.ofSeconds(3));
        int minIdle = getEnvValueInt("redis_minIdle", REDIS_DEFAULT_MIN_IDLE);
        int maxTotal = getEnvValueInt("redis_maxTotal", REDIS_DEFAULT_MAX_TOTAL);
        int maxIdle = getEnvValueInt("redis_maxIdle", REDIS_DEFAULT_MAX_IDLE);
        config.setMinIdle(minIdle);
        config.setMaxTotal(maxTotal);
        config.setMaxIdle(maxIdle);
        config.setTimeBetweenEvictionRuns(Duration.ofSeconds(600));
        return config;
    }

    public static int getEnvValueInt(String key, int defaultValue) {
        String envValue = System.getenv(key);
        if (envValue != null && envValue.length() != 0) {
            return Integer.parseInt(envValue);
        }
        return defaultValue;
    }
}
```

:::

### Package Function Service Application

In the `access-redis` project directory, execute the Maven packaging command.

```bash
mvn clean package
```

The deployment package access-redis.zip is generated in the target directory.

### Deploy Function Service Application

Refer to the common commands in the "Install MinIO Client" step in "Prerequisites" to upload the deployment package access-redis.zip to the Minio service in the openYuanrong cluster.

Create a new file create_func.json in the `access-redis` directory with the following content as the request parameters for registering the function service function. For parameter meanings, see [API documentation](../api/function_service/register_function.md)

:::{Note}

Replace the following fields according to your actual situation:

- environment: Environment variables used by the function, including Redis cluster node addresses and password
- s3CodePath: Location where the code package is stored on minio

:::

```json
{
    "name": "0@faas@access-redis",
     "handler": "com.yuanrong.demo.FaaSHandler.handler",
    "runtime": "java8",
    "description": "",
    "cpu": 600,
    "memory": 512,
    "timeout": 30,
    "concurrentNum": "100",
    "minInstance": "1",
    "maxInstance": "100",
    "environment": {
        "redis_ip_address": "10.x.x.1:6379,10.x.x.2:6379,10.x.x.3:6379",
        "redis_password":"xxx"
    },
    "layers": [],
    "kind": "faas",
    "storageType": "s3",
    "s3CodePath": {
        "bucketId": "code",
        "objectId": "access-redis.zip",
        "bucketUrl": "http://x.x.x.x:30110/"
    },
    "codePath": "",
    "schedulePolicies": [
    ],
    "extendedHandler": {
         "initializer": "com.yuanrong.demo.FaasHandler.initializer"
    },
    "extendedTimeout": {
        "initializer": 600
    },
    "customResources": {}
}
```

Use curl tool to register the function service.

```shell
META_SERVICE_ENDPOINT=<meta service endpoint obtained in "Prerequisites" step>
curl -X POST -i ${META_SERVICE_ENDPOINT}/serverless/v1/functions -H 'Content-Type: application/json' -H 'x-storage-type: local' -d @create_func.json
```

The result returns in the following format, record the value of the `functionVersionUrn` field for invocation, which corresponds to `sn:cn:yrk:default:function:0@faas@access-redis:latest` here.

```bash
{"code":0,"message":"SUCCESS","function":{"id":"sn:cn:yrk:default:function:0@faas@access-redis:latest","createTime":"2025-03-11 07:23:08.339 UTC","updateTime":"","functionUrn":"sn:cn:yrk:default:function:0@faas@access-redis","name":"0@faas@access-redis","tenantId":"default","businessId":"yrk","productId":"","reversedConcurrency":0,"description":"","tag":null,"functionVersionUrn":"sn:cn:yrk:default:function:0@faas@access-redis:latest","revisionId":"20250311072308339","codeSize":0,"codeSha256":"","bucketId":"code","objectId":"access-redis.zip","handler":"com.yuanrong.demo.FaaSHandler.handler","layers":null,"cpu":600,"memory":512,"runtime":"java8","timeout":30,"versionNumber":"latest","versionDesc":"latest","environment":{"redis_ip_address":"10.x.x.1:6379,10.x.x.2:6379,10.x.x.3:6379","redis_password":"xxx"},"customResources":null,"statefulFlag":0,"lastModified":"","Published":"2025-03-11 07:23:08.339 UTC","minInstance":1,"maxInstance":100,"concurrentNum":100,"funcLayer":[],"status":"","instanceNum":0,"device":{},"created":""}}
```

### Invoke Function Service Application

#### Prepare Resource Pool

Before running the application, you need to initialize a resource pool in the openYuanrong cluster that matches the application's resource configuration (cpu, memory, etc.) (see the create_func.json file created in the previous step).

Create a new file create_pool.json in the `access-redis` directory with the following content as the request parameters for creating the resource pool. For parameter meanings, see [API documentation](../../deploy/deploy_on_k8s/api/create_pod_pool.md)

```json
{
    "pools": [
        {
            "id": "pool-600-512",
            "size": 1,
            "group": "rg1",
            "reuse": false,
            "image": "",
            "init_image": "",
            "labels": {
            },
            "environment": {},
            "volumes": "",
           "volume_mounts": "",
            "resources": {
                "limits": {
                    "cpu": "600m",
                    "memory": "512Mi"
                },
                "requests": {
                    "cpu": "600m",
                    "memory": "512Mi"
                }
            }
        }
    ]
}
```

Use curl tool to create the resource pool.

```shell
META_SERVICE_ENDPOINT=<meta service endpoint obtained in "Prerequisites" step>
curl -X POST -i ${META_SERVICE_ENDPOINT}/serverless/v1/podpools -H 'Content-Type: application/json' -d @create_pool.json
```

The result returns as follows:

```bash
{"code":0,"message":"","result":{"failed_pools":null}}
```

#### Invoke Application

Use curl tool to invoke the application.

```shell
FUNCTION_VERSION_URN=<functionVersionUrn recorded in "Deploy Microservice" step>
FRONTEND_ENDPONT=<frontend endpoint obtained in "Prerequisites" step>
curl -X POST -i ${FRONTEND_ENDPONT}/serverless/v1/functions/${FUNCTION_VERSION_URN}/invocations -H 'Content-Type: application/json' -d "{\"scenarioName\":\"snap-up\",\"records\":[\"phone-238,watch-862\",\"phone-858,watch-354\"]}"
```

Result output:

```json
{
    "success": true,
    "message": ""
}
```
