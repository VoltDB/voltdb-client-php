/*
 *
 * Copyright 2016, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#ifndef PHP7_WRAPPER_VOLTDB_H
#define PHP7_WRAPPER_VOLTDB_H

#if PHP_MAJOR_VERSION < 7

#define php_voltdb_int int
#define php_voltdb_long long
#define php_voltdb_ulong ulong
#define php_voltdb_zend_object zend_object_value
#define php_voltdb_zend_resource zend_rsrc_list_entry
#define php_voltdb_zend_resource_id int
#define php_voltdb_add_property_string(arg, name, context, b) \
  add_property_string(arg, name, context, b)
#define php_voltdb_add_property_stringl(res, name, str, len, b) \
  add_property_stringl(res, name, str, len, b)
#define php_voltdb_add_next_index_stringl(data, str, len, b) \
  add_next_index_stringl(data, str, len, b)

#define PHP_VOLTDB_RETURN_STRING(val, dup) RETURN_STRING(val, dup)
#define PHP_VOLTDB_MAKE_STD_ZVAL(pzv) MAKE_STD_ZVAL(pzv)
#define PHP_VOLTDB_FREE_STD_ZVAL(pzv)
#define PHP_VOLTDB_DELREF(zv) Z_DELREF_P(zv)

#define PHP_VOLTDB_RETURN_DESTROY_ZVAL(val) \
  RETURN_ZVAL(val, false /* Don't execute copy constructor */, \
              true /* Dealloc original before returning */)

#define PHP_VOLTDB_WRAP_OBJECT_START(name) \
  typedef struct name { \
    zend_object std;
#define PHP_VOLTDB_WRAP_OBJECT_END(name) \
  } name;

#define PHP_VOLTDB_FREE_WRAPPED_FUNC_START(class_object) \
  static void free_##class_object(class_object *wrapped_obj TSRMLS_DC) {
#define PHP_VOLTDB_FREE_WRAPPED_FUNC_END() \
    zend_object_std_dtor(&wrapped_obj->std TSRMLS_CC); \
    efree(wrapped_obj); \
  }

#define PHP_VOLTDB_ALLOC_CLASS_OBJECT(class_object, class_entry) \
  class_object *intern; \
  zend_object_value retval; \
  intern = (class_object *)emalloc(sizeof(class_object)); \
  memset(intern, 0, sizeof(class_object));

#define PHP_VOLTDB_FREE_CLASS_OBJECT(class_object, handler) \
  retval.handle = zend_objects_store_put( \
    intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, \
    (zend_objects_free_object_storage_t)free_##class_object, NULL TSRMLS_CC); \
  retval.handlers = &handler; \
  return retval;

#define PHP_VOLTDB_HASH_FOREACH_VAL_START(ht, data) \
  zval **tmp_data = NULL; \
  for (zend_hash_internal_pointer_reset(ht); \
       zend_hash_get_current_data(ht, (void**)&tmp_data) == SUCCESS; \
       zend_hash_move_forward(ht)) { \
    data = *tmp_data;

#define PHP_VOLTDB_HASH_FOREACH_STR_KEY_VAL_START(ht, key, key_type, data) \
  zval **tmp##key = NULL; \
  ulong index##key; \
  uint len##key; \
  for (zend_hash_internal_pointer_reset(ht); \
       zend_hash_get_current_data(ht, (void**)&tmp##key) == SUCCESS; \
       zend_hash_move_forward(ht)) { \
    key_type = zend_hash_get_current_key_ex(ht, &key, &len##key, &index##key,\
                                         0, NULL); \
    data = *tmp##key;

#define PHP_VOLTDB_HASH_FOREACH_LONG_KEY_VAL_START(ht, key, key_type, index,\
                                                 data) \
  zval **tmp##key = NULL; \
  uint len##key; \
  for (zend_hash_internal_pointer_reset(ht); \
       zend_hash_get_current_data(ht, (void**)&tmp##key) == SUCCESS; \
       zend_hash_move_forward(ht)) { \
    key_type = zend_hash_get_current_key_ex(ht, &key, &len##key, &index,\
                                         0, NULL); \
    data = *tmp##key;

#define PHP_VOLTDB_HASH_FOREACH_END() }

static inline int php_voltdb_zend_hash_find(HashTable *ht, char *key, int len,
                                          void **value) {
  zval **data = NULL;
  if (zend_hash_find(ht, key, len, (void **)&data) == SUCCESS) {
    *value = *data;
    return SUCCESS;
  } else {
    *value = NULL;
    return FAILURE;
  }
}

#define php_voltdb_zend_hash_del zend_hash_del

#define PHP_VOLTDB_GET_CLASS_ENTRY(object) zend_get_class_entry(object TSRMLS_CC)

#define PHP_VOLTDB_INIT_HANDLER(class_object, handler) \
  handler = *zend_get_std_object_handlers(); \
  handler.clone_obj = NULL;

#define PHP_VOLTDB_RETURN_STRING(str, duplicate) \
  RETURN_STRING(str, duplicate);

#define PHP_VOLTDB_ADD_ASSOC_STRING(arg, key, str, duplicate) \
  add_assoc_string(arg, key, str, duplicate);

#define PHP_VOLTDB_REGISTER_RESOURCE(rsrc_id, zv, rsrc, rsrc_type) \
  rsrc_id = ZEND_REGISTER_RESOURCE(zv, rsrc, rsrc_type);

#define PHP_VOLTDB_LIST_ADD_REF(rsrc_id) zend_list_addref(rsrc_id);

#define PHP_VOLTDB_LIST_FIND(rsrc_object, rsrc_id) \
  int type; \
  rsrc_object *ptr = (rsrc_object *)zend_list_find(rsrc_id, &type);

#define PHP_VOLTDB_FETCH_RESOURCE(rsrc_obj, rsrc_object, rsrc_zv, rsrc_type_name, rsrc_type) \
  ZEND_FETCH_RESOURCE(rsrc_obj, rsrc_object *, &rsrc_zv, -1, \
                      (char *)rsrc_type_name, rsrc_type);

#else

#define php_voltdb_int size_t
#define php_voltdb_long zend_long
#define php_voltdb_ulong zend_ulong
#define php_voltdb_zend_object zend_object*
#define php_voltdb_zend_resource zend_resource
#define php_voltdb_zend_resource_id zend_resource*
#define php_voltdb_add_property_string(arg, name, context, b) \
  add_property_string(arg, name, context)
#define php_voltdb_add_property_stringl(res, name, str, len, b) \
  add_property_stringl(res, name, str, len)
#define php_voltdb_add_next_index_stringl(data, str, len, b) \
  add_next_index_stringl(data, str, len)

#define PHP_VOLTDB_RETURN_STRING(val, dup) RETURN_STRING(val)
#define PHP_VOLTDB_MAKE_STD_ZVAL(pzv) \
  pzv = (zval *)emalloc(sizeof(zval));
#define PHP_VOLTDB_FREE_STD_ZVAL(pzv) efree(pzv);
#define PHP_VOLTDB_DELREF(zv)

#define PHP_VOLTDB_RETURN_DESTROY_ZVAL(val) \
  RETVAL_ZVAL(val, false /* Don't execute copy constructor */, \
              true /* Dealloc original before returning */); \
  efree(val); \
  return

#define PHP_VOLTDB_WRAP_OBJECT_START(name) \
  typedef struct name {
#define PHP_VOLTDB_WRAP_OBJECT_END(name) \
    zend_object std; \
  } name;

#define WRAPPED_OBJECT_FROM_OBJ(class_object, obj) \
  class_object##_from_obj(obj);

#define PHP_VOLTDB_FREE_WRAPPED_FUNC_START(class_object) \
  static void free_##class_object(zend_object *object) { \
    class_object *wrapped_obj = WRAPPED_OBJECT_FROM_OBJ(class_object, object)
#define PHP_VOLTDB_FREE_WRAPPED_FUNC_END() \
    zend_object_std_dtor(&wrapped_obj->std); \
  }

#define PHP_VOLTDB_ALLOC_CLASS_OBJECT(class_object, class_entry) \
  class_object *intern; \
  intern = (class_object *)ecalloc(1, \
    sizeof(class_object) + zend_object_properties_size(class_entry));

#define PHP_VOLTDB_FREE_CLASS_OBJECT(class_object, handler) \
  intern->std.handlers = &handler; \
  return &intern->std;

#define PHP_VOLTDB_HASH_FOREACH_VAL_START(ht, data) \
  ZEND_HASH_FOREACH_VAL(ht, data) {

#define PHP_VOLTDB_HASH_FOREACH_STR_KEY_VAL_START(ht, key, key_type, data) \
  zend_string *(zs_##key); \
  ZEND_HASH_FOREACH_STR_KEY_VAL(ht, (zs_##key), data) { \
    if ((zs_##key) == NULL) {key = NULL; key_type = HASH_KEY_IS_LONG;} \
    else {key = (zs_##key)->val; key_type = HASH_KEY_IS_STRING;}

#define PHP_VOLTDB_HASH_FOREACH_LONG_KEY_VAL_START(ht, key, key_type, index, \
                                                 data) \
  zend_string *(zs_##key); \
  ZEND_HASH_FOREACH_KEY_VAL(ht, index, zs_##key, data) { \
    if ((zs_##key) == NULL) {key = NULL; key_type = HASH_KEY_IS_LONG;} \
    else {key = (zs_##key)->val; key_type = HASH_KEY_IS_STRING;}

#define PHP_VOLTDB_HASH_FOREACH_END() } ZEND_HASH_FOREACH_END();

static inline int php_voltdb_zend_hash_find(HashTable *ht, char *key, int len,
                                          void **value) {
  zval *value_tmp = zend_hash_str_find(ht, key, len -1);
  if (value_tmp == NULL) {
    return FAILURE;
  } else {
    *value = (void *)value_tmp;
    return SUCCESS;
  }
}

static inline int php_voltdb_zend_hash_del(HashTable *ht, char *key, int len) {
  return zend_hash_str_del(ht, key, len - 1);
}

#define PHP_VOLTDB_GET_CLASS_ENTRY(object) Z_OBJ_P(object)->ce

#define PHP_VOLTDB_INIT_HANDLER(class_object, handler) \
  handler = *zend_get_std_object_handlers(); \
  handler.offset = XtOffsetOf(class_object, std); \
  handler.clone_obj = NULL; \
  handler.free_obj = free_##class_object;

#define PHP_VOLTDB_RETURN_STRING(str, duplicate) \
  RETURN_STRING(str); \
  if (!duplicate) { efree((char *)str); }

#define PHP_VOLTDB_ADD_ASSOC_STRING(arg, key, str, duplicate) \
  add_assoc_string(arg, key, str); \
  if (!duplicate) { efree((char *)str); }

#define PHP_VOLTDB_REGISTER_RESOURCE(rsrc_id, zv, rsrc, rsrc_type) \
  rsrc_id = zend_register_resource(rsrc, rsrc_type); \
  RETURN_RES(rsrc_id);

#define PHP_VOLTDB_LIST_ADD_REF(rsrc_id) rsrc_id->gc.refcount++;

#define PHP_VOLTDB_LIST_FIND(rsrc_object, rsrc_id) \
  int type = rsrc_id->type; \
  rsrc_object *ptr = (rsrc_object *)rsrc_id->ptr;

#define PHP_VOLTDB_FETCH_RESOURCE(rsrc_obj, rsrc_object, rsrc_zv, rsrc_type_name, rsrc_type) \
  rsrc_obj = (rsrc_object *)zend_fetch_resource(Z_RES_P(rsrc_zv), rsrc_type_name, rsrc_type);

#endif /* PHP_MAJOR_VERSION */

#if PHP_VERSION_ID < 50399

#define PHP_VOLTDB_INIT_CLASS_OBJECT_PROPERTIES() \
  zval *tmp; \
  zend_hash_copy(intern->std.properties, &ce->default_properties, \
                 (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));

#else

#define PHP_VOLTDB_INIT_CLASS_OBJECT_PROPERTIES() \
  object_properties_init(&intern->std, ce);

#endif /* PHP_VERSION_ID */

#endif /* PHP7_WRAPPER_VOLTDB_H */
