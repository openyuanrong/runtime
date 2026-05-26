# Version Management

openYuanrong supports publishing multiple versions of functions, enabling continuous integration and release in the software development lifecycle, ensuring function stability and reliability. A version is equivalent to a function snapshot, including function code and function configuration. When you publish a version, openYuanrong generates a snapshot for the function and automatically assigns a version number associated with it for subsequent use.

When a function is registered, a latest version is generated, and updates to the function are also made on the latest version. You can publish function versions after testing is stable, using stable versions to serve online requests, and continue development and testing on the latest version.

Before any version is published, the latest version is the only function version you have, and the latest version cannot be deleted. After a version is published, the published version cannot be modified. Version numbers are monotonically increasing and will not be reused.

Related REST APIs:

- [Release Function](../../api/function_service/release_function.md): Publish a function version. 
- [Query Specific Version of Function](../../api/function_service/get_function_by_version.md): Query information about a specific version of a function.
- [Query All Versions of Function](../../api/function_service/get_function_by_name.md): Query information about all versions of a function.
- [Delete Function](../../api/function_service/delete_function.md): Delete a function version.
