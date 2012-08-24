PHP VoltDB Client Library API
=============================

Introduction
------------
VoltDB is a high throughput, low latency, clustering database capable of running
hundreds of thousands transactions per second. The VoltDB PHP client library
allows a PHP application to connect to and query from a VoltDB cluster.

Installation
------------
The client library uses a C extension and a special version VoltDB C++
driver. Both must be built and compiled on the targeted deployment
platform. Please see the README for further instructions on how to build and
install the extension.

If you have downloaded the extension as a bundle from the VoltDB website, then
the C++ driver is already included and only requires the build and installation
processes.

Note: The extension is not supported on Microsoft Windows.

Classes
-------
- VoltClient: Connects to and queries from the VoltDB cluster.
- VoltInvocationResponse: A collection of VoltTables resulting from a query.
- VoltTable: A collection of rows resulting accessed through the VoltInvocationResponse.

Constants
---------
VoltInvocationResponse::SUCCESS
VoltInvocationResponse::USER_ABORT
VoltInvocationResponse::GRACEFUL_FAILURE
VoltInvocationResponse::UNEXPECTED_FAILURE
VoltInvocationResponse::CONNECTION_LOST

VoltClient
----------
public __construct( void )
- Description: Constructor

- Parameters: None

- Return Value: An unconnected VoltClient

public bool connect( string $hostname, string $username, string $password [, int
$port] )
- Description: Connects to a single host within a cluster. Call multiple times
  to connect to all the cluster nodes.

- Parameters:
hostname: The host or IP address of the VoltDB server.
username: The username to authenticate with. Use "" if not set on the server.
password: The password to authenticate with. Use "" if not set on the server.
port: Set the port if you are not using the default port number of 21212.

- Return Value: TRUE if a connection was made.

public VoltInvocationResponse invoke( string $query [, array $parameters] )
- Description: Invokes a synchronous query.

- Parameters:
query: A string calling either the stored procedure or an ad hoc query.
parameters: Zero or more string parameters to pass to the query or stored
procedure through an array. A non-string parameter will be coerced into a
string. Note that ad hoc queries must call the @adhoc system procedure and pass
the query as a parameter.

- Return Value: A VoltInvocationResponse object or NULL. The
  VoltInvocationResult will have a status code to indicate whether the
  invocation was successful. The result will also contain the data resulting
  form the query.

public resource invokeAsync( string $query [, array $parameters] )
- Description: Invokes an asynchronous query.

- Parameters:
query: A string calling either the stored procedure or an ad hoc query.
parameters: Zero or more string parameters to pass to the query or stored
procedure through an array. A non-string parameter will be coerced into a
string. Note that ad hoc queries must call the @adhoc system procedure and pass
the query as a parameter.

- Return Value: A resource that is retrieved by calling drain() to wait for the
  results and getResponse() to get the VoltInvoationResponse object.

public VoltInvocationResponse getResponse( resource $resource )
- Description: Gets an asynchronous query result from the resource.

- Parameters:
resource: The resource returned by calling invokeAsync().

- Return Value: A VoltInvocationResponse object or NULL. The
  VoltInvocationResult will have a status code to indicate whether the
  invocation was successful. If the query is successful, then the object will
  contain the results of the query.

public bool drain( void )
- Description: A blocking call that will not return until VoltDB responds.

- Parameters: None

- Return Value: TRUE if data was retrieved and no error occurred.



VoltInvocationResponse
----------------------
public int appStatusCode( void )
- Description: Gets the application status code.

- Parameters: None

- Return Value: The developer of the stored procedure sets this value. This
  allows the developer to create customer error codes when the procedure's logic
  goes awry.

public string appStatusString( void )
- Description: Gets a human readable string version of the application status
  code.

- Parameters: None

- Return Value: A human readable status code.

public bool hasMoreResults( void )
- Description: Checks if there are more VoltTable objects available.

- Parameters: None

- Return Value: TRUE if there are more VoltTable objects.

public VoltTable nextResult( void )
- Description: Advances to the next VoltTable.

- Parameters: None

- Return Value: Returns the next VoltTable.

public int resultCount( void )
- Description: Returns the number of VoltTables resulting from the query.

- Parameters: None

- Return Value: The number of VoltTables resulting from the query.

public int statusCode( void )
- Description: Returns a VoltInvocationResponse constant indicating the
  successfulness of the query.

- Parameters: None

- Return Value: A VoltInvocationResponse constant indicating the successfulness
  of the query.

public string statusString( void )
- Description: Returns a human readable version of the query status code.

- Parameters: None

- Return Value: A human readable status code.

VoltTable
---------
public int columnCount( void )
- Description: Returns the number of columns in the VoltTable. This will remain
  constant across all rows.

- Parameters: None

- Return Value: The number of columns in the VoltTable.

public bool hasMoreRows( void )
- Description: Checks if there are more row arrays available.

- Parameters: None

- Return Value: TRUE if there are more row arrays.

public array nextRow( void )
- Description: Gets the next associative array representing a row.

- Parameters: None

- Return Value: The next associative array representing the row. Each array will
  use a key value that is the name of the columns returned.

public int rowCount( void )
- Description: Returns the number of rows in the VoltTable.

- Parameters: None

- Return Value: The number of rows in the VoltTable.

public int statusCode( void )
- Description: Returns a status code for the particular VoltTable row.

- Parameters: None

- Return Value: This value is set by the developer of a stored procedure and is
  user code specific. It is not a value produced by VoltDB itself.
