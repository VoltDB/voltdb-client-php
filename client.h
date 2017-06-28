/* This file is part of VoltDB.
 * Copyright (C) 2008-2017 VoltDB Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef VOLT_CLIENT_H
#define VOLT_CLIENT_H

#include <map>
#include <boost/make_shared.hpp>
#include "Client.h"
#include "Procedure.hpp"
#include "InvocationResponse.hpp"

#include "common.h"

// VoltClient
PHP_METHOD(VoltClient, __construct);
PHP_METHOD(VoltClient, connect);
PHP_METHOD(VoltClient, invoke);
PHP_METHOD(VoltClient, invokeAsync);
PHP_METHOD(VoltClient, getResponse);
PHP_METHOD(VoltClient, drain);
PHP_METHOD(VoltClient, close);

#define VOLT_RESPONSE_RES_NAME "VoltDB Response"
struct voltresponse_object;
// Result of asynchronously called invocations
typedef struct _voltresponse_res {
    voltdb::InvocationResponse *resp;
    bool retrieved;
} voltresponse_res;

PHP_VOLTDB_WRAP_OBJECT_START(voltclient_object)
    voltdb::Client *client;
    std::map<const char *, voltdb::Procedure *> procedures;
PHP_VOLTDB_WRAP_OBJECT_END(voltclient_object)

#if PHP_MAJOR_VERSION < 7

#define Z_VOLTCLIENT_OBJECT_P(zv) \
  (voltclient_object *)zend_object_store_get_object(zv TSRMLS_CC)

#else

static inline voltclient_object
*voltclient_object_from_obj(zend_object *obj) {
    return (voltclient_object *)((char*)(obj) -
                                 XtOffsetOf(voltclient_object, std));
}

#define Z_VOLTCLIENT_OBJECT_P(zv) voltclient_object_from_obj(Z_OBJ_P((zv)))

#endif /* PHP_MAJOR_VERSION */

void create_voltclient_class(int module_number TSRMLS_DC);

// A custom allocator which uses Zend MM allocation API internally
template<typename T>
struct ZMMAllocator
{
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template<typename U>
    struct rebind {typedef ZMMAllocator<U> other;};

    ZMMAllocator() throw() {};
    ZMMAllocator(const ZMMAllocator& other) throw() {};

    template<typename U>
    ZMMAllocator(const ZMMAllocator<U>& other) throw() {};

    template<typename U>
    ZMMAllocator& operator = (const ZMMAllocator<U>& other) { return *this; }
    ZMMAllocator<T>& operator = (const ZMMAllocator& other) { return *this; }
    ~ZMMAllocator() {}

    pointer allocate(size_type n, const void* hint = 0)
    {
        return static_cast<T*>(emalloc(n * sizeof(T)));
    }

    void deallocate(T* ptr, size_type n)
    {
        efree(ptr);
    }
};

template <typename T, typename U>
inline bool operator == (const ZMMAllocator<T>&, const ZMMAllocator<U>&)
{
    return true;
}

template <typename T, typename U>
inline bool operator != (const ZMMAllocator<T>& a, const ZMMAllocator<U>& b)
{
    return !(a == b);
}

#endif  // VOLT_CLIENT_H
