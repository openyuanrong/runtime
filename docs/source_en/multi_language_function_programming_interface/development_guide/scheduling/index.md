# Scheduling

```{eval-rst}
.. toctree::
   :glob:
   :maxdepth: 1
   :hidden:

   logical_resource
   affinity
```

When selecting the running node for function instances, openYuanrong makes decisions based on the following factors.

## Resources

Each stateless function or stateful function can specify the resources it needs. Nodes available for deployment have the following states:

- Optional: The node has the resources specified by the openYuanrong function and the resources are idle and available.
- Not optional: The node does not have the required resources or the resources are being used by other openYuanrong functions.

Resource requirements are hard requirements. Stateful functions or stateless functions will only run when there are optional nodes. When there are multiple optional nodes, openYuanrong will select an appropriate node through scheduling strategies.

Refer to the [Resources](./logical_resource.md) section to learn how to specify node and openYuanrong function resources.

## Scheduling Strategies

openYuanrong will select the best node to run openYuanrong functions based on the following scheduling strategies.

### Local First

Requests to run openYuanrong functions initiated from a node in the openYuanrong cluster will be preferentially scheduled on that local node. Even if the local node's resource utilization is high, locality is still prioritized.

### Same Tenant First

In scenarios with multiple tenants in the cluster, priority is given to allocating openYuanrong functions to nodes where tasks from the same tenant are already running, improving resource utilization for the same tenant.

### Maximum Remaining Resources First

When local node resources are insufficient, priority is given to nodes with more remaining resources, optimizing task load on nodes.

### Affinity

You can set labels on nodes or openYuanrong functions to achieve deployment of newly scheduled openYuanrong functions on nodes with specific labels, or deploy them on the same nodes as openYuanrong functions with specific labels. Through affinity, you can customize strategies to achieve more flexible scheduling to meet business needs.

Refer to the [Affinity](./affinity.md) section to learn how to configure openYuanrong function affinity properties.
