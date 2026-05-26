# Configure and Obtain Metrics

Various metrics reporting modes and export methods can be set through deployment configuration, allowing you to integrate according to your needs.

## Configure Metrics Export

Monitoring is disabled by default in openYuanrong. When deploying on K8s, to enable it, you need to configure the parameter `observer.metrics.enable` to `true`, and set the corresponding exporter in `observer.metrics.metricsConfig`. When deploying on a host, to enable it, you need to add `--enable_metrics=true --metrics_config_file={file_name}.json` in the startup parameters. For the configuration file format, refer to the [Configuration Example](metrics-config-example).

### Export Modes

openYuanrong supports two export modes: immediatelyExport and batchExport.

- `immediatelyExport`: Exports data to the receiving backend immediately after collection. The advantage is good reporting timeliness, but it consumes more system resources.
- `batchExport`: Exports data to the receiving backend when data collection reaches a certain time or when metric data accumulates to a certain quantity. The advantage is less system resource consumption and reduced backend receiving pressure, but timeliness is poor.

Different export modes can configure specific exporters. Related parameters are as follows:

- `name`: User-defined export mode name.
- `enable`: Whether the export mode is enabled.
- `exporters`: Exporters used in this export mode; multiple can be configured.

### Exporters

openYuanrong supports three exporters: fileExporter, [Prometheus](https://prometheus.io/){target="_blank"} exporter (prometheusPushExporter), and [OpenTelemetry](https://opentelemetry.io/){target="_blank"} exporter (opentelemetryExporter).

Exporters include the following configurations:

- `enable`: Whether the exporter is enabled.
- `enabledInstruments`: Metrics allowed to be exported by the exporter. Multiple metric names can be filled in, separated by commas `,`. For metric names, refer to [System Metrics](system-metrics.md) and [Alarm Metrics](alarm-metrics.md).
- `failureQueueMaxSize`: Maximum capacity of metric memory storage queue when export fails. When exceeding the set value, data will be written to disk. Default value is 1000 entries.
- `failureDataDir`: Metric storage file path when export fails, default is `/home/sn/metrics/failure`. When deploying on k8s, this path is mounted by default to the host `/var/paas/sys/metrics/cff/default/failureMetrics/` directory.
- `failureDataFileMaxCapacity`: Maximum capacity of metric storage file when export fails. When file size exceeds this capacity, the file will be overwritten. Default value is 20MB.
- `batchSize`: Batch export entry count. When the number of stored metrics exceeds the set value, export is triggered. Default value is 512 entries. This configuration only takes effect when export mode is `batchExport`.
- `batchIntervalSec`: Batch export interval. Exports metrics once every certain time interval. Default value is 15 seconds. This configuration only takes effect when export mode is `batchExport`.
- `initConfig`: Initialization parameters that need to be set for the exporter, see the following table for details.
  - fileExporter Exporter Initialization Parameters:

    | Initialization Parameter | Description | Constraint |
    | ---------- | -------------------- | -------------------- |
    | fileDir | File storage path. | Optional, uses log storage path when empty. |
    | fileName | File name. | Optional, defaults to `{nodeID}-{componentName}-metrics.data` when empty. |
    | rolling | File rolling configuration, includes the following items: <br>`enable`: File rolling switch, default `false`.<br>`maxFiles`: Maximum number of files to retain, default value `3`.<br>`maxSize`: Maximum file capacity, default value `100MB`.<br>`compress`: Whether to enable file compression, default `false`, only takes effect when rolling is enabled. | Optional, file rolling is disabled when empty. |
    | contentType | Metric content format, choose one of the following two options. <br>`STANDARD`: Standard format, retains all information.<br>`LABELS`: Label mode, retains only metric labels. | Optional, defaults to standard mode. |

  - prometheusPushExporter Exporter Initialization Parameters:

    | Initialization Parameter | Description | Constraint |
    |-------------------|------------------------------------ |------------------|
    | ip | Prometheus push gateway address. | Required, e.g.: `127.0.0.1` |
    | port | Prometheus push gateway port. | Required, e.g.: `9091` |
    | heartbeatUrl | Prometheus push gateway heartbeat URL. | Required, e.g.: `/healthy` |
    | heartbeatInterval | Prometheus push gateway heartbeat interval, unit (ms). | Required, e.g.: `5000` |

  - opentelemetryExporter Exporter Initialization Parameters:

    | Initialization Parameter | Description | Constraint |
    |-------------------|------------------------------------ |------------------|
    | endpoint | OTLP receiver URL. | Required, e.g.: `http://localhost:4318/v1/metrics` |

(metrics-config-example)=

### Configuration Examples

The following are two metrics configuration file examples.

- Configure both immediate reporting and batch reporting export modes, using file exporter to export metrics.

  ```json
  {
    "backends": [
      {
        "immediatelyExport": {
          "name": "your name",
          "enable": true,
          "exporters": [{
            "fileExporter": {
              "enable": true,
              "enabledInstruments": ["yr_alarm"],
              "failureQueueMaxSize": 1000,
              "failureDataDir": "/home/sn/metrics/failure",
              "failureDataFileMaxCapacity": 20,
              "initConfig": {
                "fileDir": "/home/sn/metrics/file",
                "rolling": {
                  "enable": true,
                  "maxFiles": 3,
                  "maxSize": 100,
                  "compress": false
                },
                "contentType": "STANDARD"
              }
            }
          }]
        }
      },
      {
        "batchExport": {
          "name": "your name",
          "enable": true,
          "exporters": [
            {
              "fileExporter": {
                "enable": true,
                "enabledInstruments": ["yr_app_instance_billing_invoke_latency"],
                "batchSize": 2,
                "batchIntervalSec": 10,
                "failureQueueMaxSize": 3,
                "failureDataDir": "/home/sn/metrics/failure",
                "failureDataFileMaxCapacity": 20,
                "initConfig": {
                  "fileDir": "/home/sn/metrics/file",
                  "rolling": {
                    "enable": true,
                    "maxFiles": 3,
                    "maxSize": 100,
                    "compress": false
                  },
                  "contentType": "STANDARD"
                }
              }
            }
          ]
        }
      }
    ]
  }
  ```

- Configure batch reporting export mode, using Prometheus exporter to export metrics.

  ```json
  {
    "backends": [
      {
        "batchExport": {
          "name": "your name",
          "enable": true,
          "exporters": [
            {
              "prometheusPushExporter": {
                "enable": true,
                "enabledInstruments": ["yr_node_cpu_usage","yr_node_memory_usage","yr_etcd_alarm"],
                "batchSize": 10,
                "batchIntervalSec": 5,
                "failureQueueMaxSize": 100,
                "failureDataDir": "/home/sn/metrics/failure",
                "failureDataFileMaxCapacity": 20,
                "initConfig": {
                  "ip": "your prometheus pushgateway ip",
                  "port": 9091,
                  "heartbeatUrl": "/healthy",
                  "heartbeatInterval": 5000
                }
              }
            }
          ]
        }
      }
    ]
  }
  ```

- Configure batch reporting export mode, using OpenTelemetry exporter to export metrics to otel-collector.

  ```json
  {
    "backends": [
      {
        "batchExport": {
          "name": "otel-collector",
          "enable": true,
          "exporters": [
            {
              "opentelemetryExporter": {
                "enable": true,
                "enabledInstruments": ["yr_node_cpu_usage", "yr_node_memory_usage", "yr_etcd_alarm"],
                "batchSize": 100,
                "batchIntervalSec": 5,
                "failureQueueMaxSize": 100,
                "failureDataDir": "/home/sn/metrics/failure",
                "failureDataFileMaxCapacity": 20,
                "initConfig": {
                  "endpoint": "http://localhost:4318/v1/metrics"
                }
              }
            }
          ]
        }
      }
    ]
  }
  ```

## Obtaining Exported Metrics Data

You can obtain metrics data by configuring different types of exporters.

### File Exporter

The export directory for fileExporter can be passed through configuration, using the log directory by default. File name formats are as follows:

- Application metrics file name format: `yr_metrics_xxx.data`.
- System metrics file name format: `{nodeName}-{moduleName}-metrics.data`, for example: `pekphis355665-3445437-function_master-metrics.data`.

Export file content has two formats: `STANDARD` and `LABELS`.

- `STANDARD` mode: Taking collection of function instance process memory usage and the most recent call request as an example, file content is as follows:

    ```text
    {"name":"runtime_memory_usage_vm_size","description":"","type":"Gauge","unit":"KB","value":"11000000","timestamp_ms":1691056024621,"labels":{"job_id":"job01","instance_id":"ins01"}}

    {"name":"runtime_memory_usage_vm_rss","description":"","type":"Gauge","unit":"KB","value":"11000000","timestamp_ms":1691056024621,"labels":{"job_id":"job01","instance_id":"ins01"}}

    {"name":"runtime_memory_usage_rss_anon","description":"","type":"Gauge","unit":"KB","value":"11000000","timestamp_ms":1691056024621,"labels":{"job_id":"job01","instance_id":"ins01"}}
    ```

- `LABELS` mode: Taking collection of function instance process memory usage and the most recent call request as an example, file content is as follows:

    ```text
    {"job_id":"job01","instance_id": "ins01"}
    ```

    Among them, the meaning of file content fields is as follows:

    | Field | Description |
    | ---------- | ---- |
    | name | Metric name |
    | description | Metric description |
    | type | Metric type |
    | unit | Metric unit |
    | value | Collected value |
    | timestamp_ms | Collection timestamp |
    | labels | Label attributes |
