# Use Cases

```{eval-rst}
.. toctree::
   :glob:
   :maxdepth: 1
   :hidden:

   llm_on_multiple_machines
   accelerate_llm_instance_scaling
   microservice-serverless
```

openYuanrong can serve as infrastructure for AI intelligent computing, used to develop AI inference, reinforcement learning, and other applications. It can also serve as infrastructure for general-purpose computing, developing big data analytics, HPC (High Performance Computing), and other applications.

## openYuanrong as AI Infrastructure

openYuanrong provides commonly used AI application frameworks through open and compatible approaches, simplifying end-to-end AI application development. Built-in heterogeneous distributed multi-level caching capabilities support efficient LLM training and inference. On-demand component-based flexible deployment facilitates integration with existing business.

1. **Ecosystem Openness**: Provides open application frameworks for different domains, and adapts commonly used frameworks like vLLM, Verl, etc. through adaptors, enabling zero-code modification access for existing applications.

2. **Efficient Inference and Training**: Built-in heterogeneous distributed multi-level caching capabilities allow inference to quickly transfer model parameters and KV Cache data, and training to switch between training and inference parameters with zero redundancy.

3. **Flexible Deployment**: Supports full or lightweight deployment, supports deployment on open-source Kubernetes or cloud clusters, facilitating integration with existing business systems.

### AI Intelligent Computing Use Cases

- [Deploy Multi-Machine PD Separation Service Based on vLLM, Long Sequence Inference TTFT Reduced by 20%](llm_on_multiple_machines.md)
- [Inference Instance Model Loading Speed 10x Improvement, Fast Elastic Response to Business Traffic Changes](accelerate_llm_instance_scaling.md)

## openYuanrong as General-Purpose Computing Infrastructure

Microservices, big data analytics, and HPC are common distributed applications in general-purpose computing scenarios. openYuanrong's multi-language function programming interfaces provide distributed parallelization capabilities for standalone programs, simplifying development complexity. Function-granularity application instances respond elastically to business at extreme speeds. Different application workloads support co-resource pool deployment, ensuring high performance while significantly improving resource utilization.

1. **Simplified Development**: openYuanrong's multi-language function programming interfaces support commonly used development languages like Python, C++, and Java. By abstracting resources, adaptive dynamic scheduling hides complexities like elasticity and distributed scheduling, simplifying distributed application development.

2. **Extreme Elasticity**: Snapshot-based cold start acceleration capabilities allow function application instances to automatically scale at millisecond-level to respond to business traffic, eliminating the need to pre-allocate large resources for business peak demands, significantly improving resource utilization.

3. **Diverse Workload Co-Pooling**: openYuanrong clusters support co-resource pool deployment of distributed applications like microservices, big data analytics, and HPC, with efficient communication and data exchange, achieving high performance.

### General-Purpose Computing Use Cases

- [Migrate SpringBoot Container Microservices to openYuanrong](microservice-serverless.md)
