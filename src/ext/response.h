#ifndef VOLT_INVOCATION_RESPONSE_H
#define VOLT_INVOCATION_RESPONSE_H

// VoltInvocationResponse
PHP_METHOD(VoltInvocationResponse, statusCode);
PHP_METHOD(VoltInvocationResponse, statusString);
PHP_METHOD(VoltInvocationResponse, appStatusCode);
PHP_METHOD(VoltInvocationResponse, appStatusString);
PHP_METHOD(VoltInvocationResponse, hasMoreResults);
PHP_METHOD(VoltInvocationResponse, nextResult);

struct voltresponse_object {
    zend_object std;
    voltdb::InvocationResponse *response;
    std::vector<voltdb::Table>::const_iterator it;
};

void create_voltresponse_class(void);
struct voltresponse_object *instantiate_voltresponse(zval *return_val, voltdb::InvocationResponse &resp);

#endif  // VOLT_INVOCATION_RESPONSE_H
