# Error Codes

## SDK Error Codes

```{csv-table}
:header: >
:    "Error Code", "Description", "Solution"
:widths: 15, 20, 40
:align: left

1001, "Parameter validation failed.", "Restore the abnormal parameters according to the message, and call the function again."
1002, "Insufficient resources.", "Check whether the requested resources exceed the resources managed by the kernel."
1003, "Instance not found.", "Check whether the instance has been properly scaled out."
1004, "Duplicate request.", "Contact us."
1005, "Request limit reached.", "Retry after a certain interval."
1006, "Resource error, unable to schedule.", "Check whether there is an error in the configured resources, and whether the requested resources exceed the resources managed by each data plane or the allowed range."
1007, "Instance exited after executing exit or kill 15.", "Check the failure reason of the failed runtime and fix it."
1008, "ExtensionMeta configuration error.", "Check the ExtensionMeta configuration and modify it according to the prompt message."
1010, "Failed to create grouped instances.", "Check the InvokeOptions configuration for grouped instance creation parameters to see if there are any configuration issues."
1011, "Failed to exit grouped instances.", "Check the InvokeOptions configuration for grouped instance creation parameters to see if there are any configuration issues."
1012, "Local rate-limited instance creation failed.", "Check the local scheduler traffic limit creation."
1013, "Instance evicted.", "Check the failure reason of the failed runtime and fix it."
1014, "Authentication failed.", "Check the IAM configuration verification policy."
1015, "Function metadata does not exist.", "Check whether the function has been deployed."
1016, "Instance information is incorrect.", "Contact us."
1017, "Instance scheduling cancelled.", "The create request scheduling was cancelled. Possible reasons include: no available resources within the scheduling timeout period, calling Finalize during execution, etc."
2001, "User code cannot be loaded.", "Check whether the user so is normal, whether permissions are configured correctly, etc."
2002, "User code has core dump or exception.", "Check the user function error and fix it according to the relevant stack and msg."
3001, "Failed to connect to runtime.", "Contact us."
3002, "Communication error.", "Check whether the environment network is normal."
3003, "Internal error.", "Contact us."
3004, "Frontend cannot connect to busproxy.", "Check whether the busproxy on the node where the frontend is located is running normally."
3005, "etcd access exception.", "Collect environment information and contact us."
3006, "busproxy connection broken.", "Collect environment information and contact us."
3007, "redis access exception.", "Collect environment information and contact us."
3008, "K8S unreachable.", "Collect environment information and contact us."
3009, "function agent execution failed.", "Collect environment information and contact us."
3010, "State machine execution failed.", "Collect environment information and contact us."
3011, "Local scheduler execution failed.", "Collect environment information and contact us."
3012, "runtime manager execution failed.", "Collect environment information and contact us."
3013, "instance manager execution failed.", "Collect environment information and contact us."
3014, "Local scheduler exception.", "Collect environment information and contact us."
4001, "Incorrect use of init.", "Modify the usage of init."
4002, "init connection failed.", "Check whether the init connection address is correct and whether the environment network is normal."
4003, "Deserialization failed.", "Contact us."
4004, "The instance called in local mode does not exist.", "Check whether the instance is created correctly."
4005, "get retrieval failed.", "Determine whether the get timeout is reasonable. If confirmed, contact us."
4006, "Incorrect function usage.", "Modify the function usage according to the prompt msg."
4007, "create error.", "Modify the relevant create parameters according to the prompt msg."
4008, "invoke error.", "Modify the relevant invoke parameters according to the prompt msg."
4009, "kill error.", "Modify the relevant kill parameters according to the prompt msg."
4201, "Failed to operate rockdb.", "Collect environment information and contact us."
4202, "The read/write data exceeds the shared memory size.", "Check whether the configured shared memory meets expectations."
4203, "Failed to operate disk files.", "Collect environment information and contact us."
4204, "Insufficient disk space.", "Collect environment information and contact us."
4205, "Data system connection failed.", "Collect environment information and contact us."
4299, "Other data system errors.", "Collect environment information and contact us."
4306, "Dependent request failed.", "Modify the relevant create parameters according to the prompt msg."
9000, "Instance exited after executing finalize.", "Check whether the code initiates call requests after using the Finalize or Terminate interface."
```

:::{note}

The following types of exceptions can be retried later. If unresolved, contact us.

- Internal Server error.
- timeout exceptions.

:::

(error-code-rest-api)=

## REST API Error Codes 

| **Error Code** | **Error Message**                     | **Description**                                     |
| ---------- | -------------------------------- | -------------------------------------------- |
| 4037       | session config invalid           | The user-specified instance session is invalid.                     |
| 4101       | FunctionNameExist                | The function name already exists.                               |
| 4102       | AliasNameAlreadyExists           | The alias already exists.                                 |
| 4103       | TotalRoutingWeightNotOneHundred  | The total routing weight is not 100.                         |
| 4104       | InvalidUserParam                 | The user input parameter is invalid.                           |
| 4105       | FunctionVersionDeletionForbidden | The function version has an alias and cannot be deleted.               |
| 4106       | LayerVersionSizeOutOfLimit       | The layer version exceeds the limit.                             |
| 4107       | TenantLayerSizeOutOfLimit        | The tenant layer size exceeds the limit.                         |
| 4108       | LayerVersionNumOutOfLimit        | The layer version number exceeds the limit.                           |
| 4109       | TriggerNumOutOfLimit             | The number of function triggers exceeds the limit.                     |
| 4110       | FunctionVersionOutOfLimit        | The function version exceeds the limit.                           |
| 4111       | AliasOutOfLimit                  | The number of function aliases exceeds the limit.                         |
| 4112       | LayerIsUsed                      | If the function uses this layer, it cannot be deleted.               |
| 4113       | LayerVersionNotFound             | The layer version has not been created.                               |
| 4114       | RepeatedPublishmentError         | There is no difference between two releases.                           |
| 4115       | FunctionNotFound                 | The function has not been created or has been deleted.                               |
| 4116       | FunctionVersionNotFound          | The function version has not been created or has been deleted.                             |
| 4117       | AliasNameNotFound                | The alias has not been created or has been deleted.                             |
| 4118       | TriggerNotFound                  | The trigger has not been created or has been deleted.                               |
| 4119       | LayerNotFound                    | The layer has not been created or has been deleted.                               |
| 4120       | PoolNotFound                     | The pool has not been created or has been deleted.                               |
| 4123       | PageInfoError                    | The page information exceeds the list boundary.                               |
| 4124       | TriggerPathRepeated              | The trigger path is used by another trigger.                               |
| 4125       | DuplicateCompatibleRuntimes      | Duplicate items found in the compatible runtimes list.                               |
| 4126       | CompatibleRuntimeError           | Some compatible runtimes are not in the configuration list.                             |
| 4127       | ZipFileCountError                | The number of files in the zip file exceeds the configured limit.                               |
| 4128       | ZipFilePathError                 | Some file paths in the zip file are invalid.                               |
| 4129       | ZipFileUnzipSizeError            | The decompressed file size exceeds the configured limit.                                 |
| 4130       | ZipFileSizeError                 | The compressed package size exceeds the configured limit.                                 |
| 4134       | RevisionIDError                  | The requested revision ID does not match the entry ID operated in storage. |
| 121016     | SaveFileError                    | Error saving temporary file when processing the package zip file.    |
| 121017     | UploadFileError                  | Error processing package zip file when uploading.              |
| 121018     | EmptyAliasAndVersion             | The alias name and version are empty.                         |
| 121019     | ReadingPackageTimeout            | Timeout when reading the package.                               |
| 121026     | BucketNotFound                   | Bucket information for a specific business ID not found.                 |
| 121029     | ZipFileError                     | Error processing the package zip file.                    |
| 121030     | DownloadFileError                | Error obtaining pre-signed download URL from package storage.        |
| 121032     | DeleteFileError                  | Error deleting package storage object.                       |
| 121036     | InvalidFunctionLayer             | The layer tenant information does not match its requested context.       |
| 121046     | InvalidQueryURN                  | The URN in the query request is invalid.                              |
| 121047     | InvalidQualifier                 | The qualifier in the request is invalid; it represents a version or alias.       |
| 121048     | ReadBodyError                    | Error processing the body of the HTTP request.             |
| 121049     | AuthCheckError                   | Error when authenticating the HTTP request.             |
| 121052     | InvalidJSONBody                  | Error parsing the body of the HTTP request as JSON format.     |
| 121057     | TriggerIDNotFound                | The trigger ID in the request cannot be found in storage.           |
| 121058     | FunctionNameFormatErr            | The function name format in storage is incorrect.                     |
| 122001     | KVNotFound                       | No such kv in etcd.                         |
| 122002     | EtcdError                        | Error in etcd.                              |
| 122003     | TransactionFailed                | etcd transaction error.                              |
| 122004     | UnmarshalFailed                  | Error deserializing to JSON.                     |
| 122005     | MarshalFailed                    | Error serializing to string.                       |
| 122006     | VersionOrAliasEmpty              | The version or alias is an empty string.             |
| 122007     | ResourceIDEmpty                  | The resource ID is an empty string.                         |
| 122008     | NoTenantInfo                     | Tenant information does not exist.                             |
| 122009     | NoResourceInfo                   | Resource information does not exist.                             |
| 130600     | InvalidParamErrorCode            | Error parsing parameters of the HTTP request.                 |
| 150424     | function metadata not found      | Function metadata not found.                           |
| 150500     | internal system error                      | Internal system error.                               |
| 150444     | instance label not found         | The specified instance label does not exist.                      |
| 330404     | subcribe stream failed             | Failed to subscribe to stream.                                 |
