# Distributed Fusion Call Stack

In openYuanrong's distributed function system, a single call may span multiple instances, containers, or physical nodes. To help developers quickly locate the source of anomalies, openYuanrong provides the **Distributed Fusion Call Stack** feature.

This feature transparently chains Java call paths across nodes and preserves complete exception types and stack trace information when anomalies occur. Specifically, when capturing a `YRException` exception, users can use the `YRException.getCause()` method to obtain the **real Java exception object** (such as `NullPointerException`, `IllegalArgumentException`, etc.) thrown on the remote node, without parsing logs or relying on external monitoring.

## Background and Value

In a pure Java distributed function scenario, one function may call another instance deployed on a different node. If the called function throws an exception, traditional RPC frameworks typically return only generic error codes or vague messages, making it difficult for the caller to determine the root cause.

openYuanrong's Distributed Fusion Call Stack solves this problem:

- **End-to-End Java Exception Propagation**: The original exception thrown by the remote JVM is serialized and completely passed back to the caller.
- **Seamless Integration with Java Exception Mechanism**: Callers can directly use standard methods like `getCause()`, `printStackTrace()` to handle exceptions.
- **Complete Stack Context Preservation**: Including class names, method names, file names, and line numbers for precise problem code location.

## Limitations and Considerations

- Only supports **Java to Java** function calls. If calling non-Java functions, `getCause()` may return `null` or a wrapped exception.
- Chain calls support a maximum of 10 layers currently; higher layers are not supported.
- Exceptions thrown by called functions must implement `java.io.Serializable`, otherwise they cannot be propagated.
- Custom exception classes must exist in both the caller's and callee's classpath and be version-compatible.
- Custom exceptions must implement a constructor with a String parameter.
- Sensitive information (such as local file paths) is automatically desensitized during cross-node transmission.

## Usage

### Multi-Layer Java Call Chain Example

Assume there is a call chain:  
`Driver → InstanceA → InstanceB`

- If `InstanceB` throws an `IOException`
- `InstanceA` does not catch this exception
- `Driver` catches `YRException` and calls `e.getCause()`, it will directly obtain the `IOException` instance

openYuanrong automatically propagates the exception from `InstanceB` to `Driver`, and intermediate nodes do not change the exception type.

The following code demonstrates how to safely chain call remote Java functions and obtain real exceptions:

```java
package com.example;

import org.yuanrong.Config;
import org.yuanrong.api.YR;
import org.yuanrong.call.InstanceHandler;
import org.yuanrong.exception.YRException;
import org.yuanrong.runtime.client.ObjectRef;

import java.io.IOException;
import java.nio.file.NoSuchFileException;
import java.util.List;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Main {

    public static class MyYRApp {
        public static String failedSmallCall() throws YRException, IOException {
            String filePath = "not_exist.txt";
            // Real exception
            List<String> lines = Files.readAllLines(Paths.get(filePath));
            for (String line : lines) {
                System.out.println(line);
            }
            return "";
        }

        public static String remoteChainingCall() throws YRException, IOException {
            // Chain call, exception will propagate layer by layer
            InstanceHandler instance = YR.instance(Main.MyYRApp::new).invoke();
            ObjectRef ref1 = instance.function(MyYRApp::failedSmallCall).invoke();
            String res = (String) YR.get(ref1, 10000);
            return res + "-remoteChainingCall";
        }
    }

    public static void main(String[] args) throws YRException {
        YR.init(new Config());
        InstanceHandler instance = YR.instance(Main.MyYRApp::new).invoke();
        try {
            ObjectRef ref1 = instance.function(MyYRApp::remoteChainingCall).invoke();
            YR.get(ref1, 10000);
        }  catch (YRException e) {
            // Key: Get the real Java exception thrown remotely through getCause()
            Throwable cause = e.getCause();
            if (cause != null) {
                System.err.println("Captured remote exception:");
                System.err.println("  Type: " + cause.getClass().getName());
                System.err.println("  Message: " + cause.getMessage());
            } else {
                System.err.println("Call failed but did not carry specific exception: " + e.getMessage());
            }
        } finally {
            YR.Finalize();
        }
    }
}
```

**Remote Exception Capture Output Example:**

```shell
Captured remote exception:
  Type: java.nio.file.NoSuchFileException
  Message: null
java.nio.file.NoSuchFileException
        at com.example.MyYRApp.failedSmallCall(MyYRApp.java:11)
        at sun.reflect.NativeMethodAccessorImpl.invoke(NativeMethodAccessorImpl.java:62)
        at sun.reflect.DelegatingMethodAccessorImpl.invoke(DelegatingMethodAccessorImpl.java:43)
        at java.lang.reflect.Method.invoke(Method.java:498)
        at org.yuanrong.executor.FunctionHandler.invoke(FunctionHandler.java:361)
        at org.yuanrong.executor.FunctionHandler.execute(FunctionHandler.java:122)
        at org.yuanrong.codemanager.CodeExecutor.execute(CodeExecutor.java:81)
        at org.yuanrong.Entrypoint.runtimeEntrypoint(Entrypoint.java:102)
        at org.yuanrong.runtime.server.RuntimeServer.main(RuntimeServer.java:51)
        at com.example.MyYRApp.remoteChainingCall(MyYRApp.java:23)
```

## Technical Implementation Details

- **Exception Serialization**: All serializable Java exceptions (implementing `Serializable`) can be propagated.
- **Stack Enhancement**: Remote exception stack trace information is completely preserved and call path metadata (such as target node ID) is appended.
- **Type Safety**: Deserialized exception objects maintain original class type, supporting `instanceof` judgment.

For example:

```java
if (e.getCause() instanceof IllegalArgumentException) {
    // Can safely perform type-specific handling
}
```

## Related API

| Class/Method | Description |
|  |  |
| `org.yuanrong.exception.YRException` | Wrapped exception thrown when remote call fails |
| `YRException.getCause()` | Get the wrapped real Java exception (may be `NullPointerException`, `IOException`, etc.) |

Through the Distributed Fusion Call Stack, openYuanrong enables Java developers to enjoy the same exception debugging experience in distributed environments as local development—**"What you see is what you get, exception is the root cause"**.
