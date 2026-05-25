# API

```{eval-rst}
.. toctree::
   :glob:
   :maxdepth: 1
   :hidden:

   distributed_programming/index
   function_service/index
   yr_command_line_tool/index
   error_code
```

APIs are categorized as follows:

- [Distributed Parallelization API for Single-Machine Programs](./distributed_programming/index.md): Provides interfaces for developing stateless and stateful functions.
- [Function Service API](./function_service/index.md): Provides interfaces for developing, calling, and managing function services.

## How to Call REST APIs

Calling a REST API requires constructing a request URI, which typically consists of three parts: the request protocol, the server endpoint, and the resource path. The default deployed openYuanrong cluster uses the HTTP request protocol, and the server endpoints are provided by the frontend and meta service components. Taking the register function REST API as an example, the complete URI is: `http://{meta service endpoint}/serverless/v1/functions`.

(api-frontend-endpoint)=

### Obtaining the Frontend Component Server Endpoint

The frontend component provides REST APIs such as function invocation services and stream subscription services. The server endpoint can be obtained through the following methods.

- When deploying openYuanrong on a host, in the `/tmp/yr_sessions/latest/session.json` file generated in the default deployment path, combine the `function_master.ip` and `frontend.port` from the `cluster_info` field to obtain the frontend component's server endpoint.

   Example session.json file:

   ```text
   ...
   "cluster_info": {
      "for-join": {
         "function_master.ip": "192.168.2.2",
         "function_master.port": "47029",
         ...
         "frontend.port": "40021"
      }
   }
   ...
   ```

- When deploying openYuanrong on K8s, obtain it via the kubectl command on the K8s master node.

   ```bash
   echo "$(kubectl get nodes -o jsonpath='{.items[0].status.addresses[?(@.type=="InternalIP")].address}'):$(kubectl get svc frontend-lb -o jsonpath='{.spec.ports[0].nodePort}')"
   ```

(api-meta-service-endpoint)=

### Obtaining the Meta Service Component Server Endpoint

The meta service component provides REST APIs such as function management and resource pool management. The server endpoint can be obtained through the following methods.

- When deploying openYuanrong on a host, in the `/tmp/yr_sessions/latest/metaservice_config.json` file generated in the default deployment path, combine the `ip` and `port` from the `server` field to obtain the meta service component's server endpoint.

   Example metaservice_config.json file:

   ```text
   ...
   "server": {
      "ip": "192.168.2.2",
      "port": 34565
   }
   ...
   ```

- When deploying openYuanrong on K8s, obtain it via the kubectl command on the K8s master node.

   ```bash
   echo "$(kubectl get nodes -o jsonpath='{.items[0].status.addresses[?(@.type=="InternalIP")].address}'):$(kubectl get svc meta-service -o jsonpath='{.spec.ports[0].nodePort}')"
   ```
