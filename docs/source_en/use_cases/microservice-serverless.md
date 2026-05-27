# Migrate SpringBoot Container Microservices to openYuanrong

openYuanrong is compatible with mainstream Java microservice frameworks and supports deploying microservices as openYuanrong functions for unified operation, enjoying the elasticity and maintenance-free advantages brought by Serverless.

## Solution Introduction

In a SpringBoot microservice project, introduce the adapter SDK provided by openYuanrong as a dependency and modify some build configurations, then package and deploy as an openYuanrong function service, achieving near-zero code modification migration to the openYuanrong platform.

## Preparation

1. [Deploy openYuanrong cluster on K8s](../deploy/deploy_on_k8s/index.md) and use the kubectl tool on the K8s cluster master node to obtain the openYuanrong service endpoint.

    Meta service, responsible for functions and resource pool management, etc.

    ```bash
    echo "http://$(kubectl get nodes -o jsonpath='{.items[0].status.addresses[?(@.type=="InternalIP")].address}'):$(kubectl get svc meta-service -o jsonpath='{.spec.ports[0].nodePort}')"
    ```

    Frontend service, responsible for accessing traffic and undertaking function calls, etc.

    ```bash
    echo "http://$(kubectl get nodes -o jsonpath='{.items[0].status.addresses[?(@.type=="InternalIP")].address}'):$(kubectl get svc frontend-lb -o jsonpath='{.spec.ports[0].nodePort}')"
    ```

2. Install [MinIO Client](tools-minio-client) for uploading code packages to the MinIO service in the openYuanrong cluster.
3. Install openYuanrong Microservice Adapter SDK

    [Download openYuanrong release package](../reference/releases.md) openyuanrong-*.tar.gz and execute the following commands in sequence:

    ```bash
    tar -xzf openyuanrong-*.tar.gz
    cd openyuanrong
    WORKSPACE=$(pwd)

    cd ${WORKSPACE}/datasystem/sdk/
    mvn install:install-file -Dfile=datasystem-${VERSION}_${ARCH}.jar -DartifactId=datasystem -DgroupId=org.yuanrong.datasystem -Dversion=${VERSION} -Dpackaging=jar

    cd ${WORKSPACE}/runtime/sdk/java/
    mvn install:install-file -Dfile=faas-function-sdk-${VERSION}.jar -DartifactId=faas-function-sdk -DgroupId=org.yuanrong -Dversion=${VERSION} -Dpackaging=jar

    cd ${WORKSPACE}/spring-adapter/sdk/
    bash install-adapter.sh -v ${VERSION}
    ```

## Implementation Process

### Create SpringBoot Microservice Project

Create a SpringBoot microservice project, or refer to modifications based on your existing SpringBoot project. The directory structure is as follows, containing four files: pom.xml, zip_file.xml, Application.java, and MyController.java.

- pom.xml: Maven configuration file, adding openYuanrong microservice-function-yuanrong dependency and build plugin configuration.
- zip_file.xml: Code packaging configuration.
- Application.java: Microservice main class.
- MyController.java: Handles web requests, implements `/rest/hello` interface.

``` bash
microservice-demo
├── pom.xml
└── src
    └── main
        ├── assembly
        │   └── zip_file.xml
        └── java
            └── com
                └── example
                    └── microservicedemo
                        ├── Application.java
                        └── MyController.java
```

:::{dropdown} pom.xml file content, **must correspondingly modify microservice-function-yuanrong version number to your installed version**
:chevron: down-up
:icon: chevron-down

```xml
<?xml version="1.0" encoding="UTF-8"?>

<project xmlns="http://maven.apache.org/POM/4.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>
    <parent>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-parent</artifactId>
        <version>2.6.7</version>
        <relativePath/>
    </parent>

    <groupId>com.example</groupId>
    <artifactId>microservice-example</artifactId>
    <version>1.0.0</version>

    <properties>
        <package.finalName>microservice-demo</package.finalName>
        <package.outputDirectory>target</package.outputDirectory>
    </properties>

    <dependencies>
        <dependency>
            <groupId>org.yuanrong.m2s</groupId>
            <artifactId>microservice-function-yuanrong</artifactId>
            <!-- Modify version number to your actual used version -->
            <version>${VERSION}</version>
        </dependency>
        <dependency>
            <!-- Exclude spring-boot-starter-logging to avoid conflict with log4j-->
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-web</artifactId>
                <exclusions>
                    <exclusion>
                        <groupId>org.springframework.boot</groupId>
                        <artifactId>spring-boot-starter-logging</artifactId>
                    </exclusion>
                </exclusions>
        </dependency>
    </dependencies>
    <build>
        <plugins>
            <!-- Configure packaging method as follows -->
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
    <baseDirectory></baseDirectory>
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
            <fileMode>0660</fileMode>
            <directoryMode>0760</directoryMode>
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
:::{dropdown} Application.java file content
:chevron: down-up
:icon: chevron-down

```java
package com.example.microservicedemo;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class Application {

    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }

}
```

:::
:::{dropdown} MyController.java file content
:chevron: down-up
:icon: chevron-down

```java
package com.example.microservicedemo;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RequestMapping("/rest")
@RestController
public class MyController {
    @GetMapping(value = "/hello")
    public String helloWorld(String name) {
        return "Hello World, " + name;
    }
}
```

:::

### Package Microservice

In the microservice-demo directory, execute the maven packaging command.

```bash
mvn clean package
```

The deployment package microservice-demo.zip is generated in the target directory.

### Deploy Microservice

Refer to the "Preparation" section to install [MinIO Client](tools-minio-client) steps for common commands, upload microservice-demo.zip to the MinIO service in the openYuanrong cluster.

```bash
mc mb mys3/this-bucket
mc cp microservice-demo.zip mys3/this-bucket/microservice-demo.zip
```

Create a `create_func.json` file in the `microservice-demo` directory with the following content (**replace fields in s3CodePath according to actual situation**) as the request parameters for registering the function service. For parameter meanings, see [API Description](../multi_language_function_programming_interface/api/function_service/register_function.md).

```json
{
    "name": "0@microservice@demo",
     "handler": "org.yuanrong.m2s.function.Handler.handleRestRequest",
    "runtime": "java8",
    "description": "",
    "cpu": 600,
    "memory": 512,
    "timeout": 30,
    "concurrentNum": "100",
    "minInstance": "1",
    "maxInstance": "100",
    "environment": {
        "spring_start_class": "com.example.microservicedemo.Application"
    },
    "layers": [],
    "kind": "faas",
    "storageType": "s3",
    "s3CodePath": {
        "bucketId": "this-bucket",
        "objectId": "microservice-demo.zip",
        "bucketUrl": "http://x.x.x.x:30110/"
    },
    "codePath": "",
    "schedulePolicies": [
    ],
    "extendedHandler": {
         "initializer": "org.yuanrong.m2s.function.Handler.initializer"
    },
    "extendedTimeout": {
        "initializer": 600
    },
    "customResources": {}
}
```

Use the curl tool to register the microservice as a function service.

```shell
META_SERVICE_ENDPOINT=<Meta service endpoint obtained in "Preparation" step>
curl -X POST -i ${META_SERVICE_ENDPOINT}/serverless/v1/functions -H 'Content-Type: application/json' -H 'x-storage-type: local' -d @create_func.json
```

The result returns in the following format. Record the value of the `functionVersionUrn` field for invocation, here corresponding to `sn:cn:yrk:12345678901234561234567890123456:function:0@microservice@demo:latest`

```bash
{"code":0,"message":"SUCCESS","function":{"id":"sn:cn:yrk:12345678901234561234567890123456:function:0@microservice@demo:latest","createTime":"2025-02-28 07:36:09.088 UTC","updateTime":"","functionUrn":"sn:cn:yrk:12345678901234561234567890123456:function:0@microservice@demo","name":"0@microservice@demo","tenantId":"12345678901234561234567890123456","businessId":"yrk","productId":"","reversedConcurrency":0,"description":"","tag":null,"functionVersionUrn":"sn:cn:yrk:12345678901234561234567890123456:function:0@microservice@demo:latest","revisionId":"20250228073609088","codeSize":0,"codeSha256":"","bucketId":"","objectId":"","handler":"org.yuanrong.m2s.function.Handler.handleRestRequest","layers":null,"cpu":2000,"memory":2048,"runtime":"java8","timeout":30,"versionNumber":"latest","versionDesc":"latest","environment":{"spring_start_class":"com.example.microservicedemo.Application"},"customResources":null,"statefulFlag":0,"lastModified":"","Published":"2025-02-28 07:36:09.088 UTC","minInstance":1,"maxInstance":100,"concurrentNum":100,"funcLayer":[],"status":"","instanceNum":0,"device":{},"created":""}}
```

### Invoke Microservice

#### Prepare Resource Pool

Before running the microservice, you need to initialize a service resource (cpu, memory, etc.) in the openYuanrong cluster. If you have already created a resource pool when deploying openYuanrong, you can skip this step.

Create a `create_pool.json` file in the `microservice-demo` directory with the following content as the request parameters for creating a resource pool. For parameter meanings, see [API Description](../deploy/deploy_on_k8s/api/create_pod_pool.md)

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

Use the curl tool to create a resource pool.

```shell
META_SERVICE_ENDPOINT=<Meta service endpoint obtained in "Preparation" step>
curl -X POST -i ${META_SERVICE_ENDPOINT}/serverless/v1/podpools -H 'Content-Type: application/json' -d @create_pool.json
```

The result returns as follows:

```bash
{"code":0,"message":"","result":{"failed_pools":null}}
```

#### Service Invocation

Create an `invoke_func.json` file in the `microservice-demo` directory with the following content as the request parameters for invoking the function service. For parameter meanings, see [API Description](../multi_language_function_programming_interface/api/function_service/function_invocation.md)

```json
{
  "isBase64Encoded": false,
  "httpMethod": "GET",
  "body": "",
  "path":"/rest/hello",
  "headers": {
      "Content-Type": "application/json"
  },
  "pathParameters":{},
  "queryStringParameters": {
    "name": "yuanrong"
  }
}
```

Use the curl tool to invoke the microservice.

```shell
FUNCTION_VERSION_URN=<functionVersionUrn recorded in "Deploy Microservice" step>
FRONTEND_ENDPONT=<Frontend service address obtained in "Preparation" step>
curl -X POST -i ${FRONTEND_ENDPONT}/serverless/v1/functions/${FUNCTION_VERSION_URN}/invocations -H 'Content-Type: application/json' -d @invoke_func.json
```

Result output:

```json
{
    "body": "Hello World, yuanrong",
    "headers": {
      "Content-Length": "21",
      "Content-Language": "en",
      "Content-Type": "text/plain"
    },
    "statusCode": 200,
    "isBase64Encoded": false
}
```
