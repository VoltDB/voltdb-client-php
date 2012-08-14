#ifndef VOLT_CLIENT_H
#define VOLT_CLIENT_H

// VoltClient
PHP_METHOD(VoltClient, __construct);
PHP_METHOD(VoltClient, __destruct);
PHP_METHOD(VoltClient, connect);
PHP_METHOD(VoltClient, addStoredProc);
PHP_METHOD(VoltClient, removeStoredProc);
PHP_METHOD(VoltClient, getStoredProcs);
PHP_METHOD(VoltClient, invoke);
PHP_METHOD(VoltClient, invokeAsync);
PHP_METHOD(VoltClient, drain);

struct voltclient_object {
    zend_object std;
    voltdb::Client client;
    std::map<const char *, voltdb::Procedure *> procedures;
};

void create_voltclient_class(void);

#endif  // VOLT_CLIENT_H
