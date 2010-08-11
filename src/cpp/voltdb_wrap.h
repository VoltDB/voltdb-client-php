/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.0
 * 
 * This file is not intended to be easily readable and contains a number of 
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG 
 * interface file instead. 
 * ----------------------------------------------------------------------------- */

#ifndef SWIG_VOLTDB_WRAP_H_
#define SWIG_VOLTDB_WRAP_H_

class SwigDirector_c_Exception : public voltdb::Exception, public Swig::Director {

public:
    SwigDirector_c_Exception(zval *self);
    virtual ~SwigDirector_c_Exception() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_NullPointerException : public voltdb::NullPointerException, public Swig::Director {

public:
    SwigDirector_NullPointerException(zval *self);
    virtual ~SwigDirector_NullPointerException() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_InvalidColumnException : public voltdb::InvalidColumnException, public Swig::Director {

public:
    SwigDirector_InvalidColumnException(zval *self);
    virtual ~SwigDirector_InvalidColumnException() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_OverflowUnderflowException : public voltdb::OverflowUnderflowException, public Swig::Director {

public:
    SwigDirector_OverflowUnderflowException(zval *self);
    virtual ~SwigDirector_OverflowUnderflowException() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_IndexOutOfBoundsException : public voltdb::IndexOutOfBoundsException, public Swig::Director {

public:
    SwigDirector_IndexOutOfBoundsException(zval *self);
    virtual ~SwigDirector_IndexOutOfBoundsException() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_NonExpandableBufferException : public voltdb::NonExpandableBufferException, public Swig::Director {

public:
    SwigDirector_NonExpandableBufferException(zval *self);
    virtual ~SwigDirector_NonExpandableBufferException() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_UninitializedParamsException : public voltdb::UninitializedParamsException, public Swig::Director {

public:
    SwigDirector_UninitializedParamsException(zval *self);
    virtual ~SwigDirector_UninitializedParamsException() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_ParamMismatchException : public voltdb::ParamMismatchException, public Swig::Director {

public:
    SwigDirector_ParamMismatchException(zval *self);
    virtual ~SwigDirector_ParamMismatchException() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_NoMoreRowsException : public voltdb::NoMoreRowsException, public Swig::Director {

public:
    SwigDirector_NoMoreRowsException(zval *self);
    virtual ~SwigDirector_NoMoreRowsException() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_StringToDecimalException : public voltdb::StringToDecimalException, public Swig::Director {

public:
    SwigDirector_StringToDecimalException(zval *self);
    virtual ~SwigDirector_StringToDecimalException() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_ConnectException : public voltdb::ConnectException, public Swig::Director {

public:
    SwigDirector_ConnectException(zval *self);
    virtual ~SwigDirector_ConnectException() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_NoConnectionsException : public voltdb::NoConnectionsException, public Swig::Director {

public:
    SwigDirector_NoConnectionsException(zval *self);
    virtual ~SwigDirector_NoConnectionsException() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_LibEventException : public voltdb::LibEventException, public Swig::Director {

public:
    SwigDirector_LibEventException(zval *self);
    virtual ~SwigDirector_LibEventException() throw ();
    virtual char const *what() const throw();
};
class SwigDirector_ByteBuffer : public voltdb::ByteBuffer, public Swig::Director {

public:
    SwigDirector_ByteBuffer(zval *self, char *buffer, int32_t capacity);
    SwigDirector_ByteBuffer(zval *self, voltdb::ByteBuffer const &other);
    SwigDirector_ByteBuffer(zval *self);
    virtual bool isExpandable();
    virtual void ensureRemaining(int32_t remaining) throw(voltdb::NonExpandableBufferException);
    virtual void ensureRemainingExact(int32_t remaining) throw(voltdb::NonExpandableBufferException);
    virtual void ensureCapacity(int32_t capacity) throw(voltdb::NonExpandableBufferException);
    virtual void ensureCapacityExact(int32_t capacity) throw(voltdb::NonExpandableBufferException);
    virtual ~SwigDirector_ByteBuffer();
};
class SwigDirector_ExpandableByteBuffer : public voltdb::ExpandableByteBuffer, public Swig::Director {

public:
    SwigDirector_ExpandableByteBuffer(zval *self, voltdb::ExpandableByteBuffer const &other);
    SwigDirector_ExpandableByteBuffer(zval *self);
    SwigDirector_ExpandableByteBuffer(zval *self, char *data, int32_t length);
    virtual bool isExpandable();
    virtual void ensureRemaining(int32_t amount) throw(voltdb::NonExpandableBufferException);
    virtual void ensureRemainingExact(int32_t amount) throw(voltdb::NonExpandableBufferException);
    virtual void ensureCapacity(int32_t capacity) throw(voltdb::NonExpandableBufferException);
    virtual void ensureCapacityExact(int32_t capacity) throw(voltdb::NonExpandableBufferException);
    virtual ~SwigDirector_ExpandableByteBuffer();
    virtual void resetRef(char *data);
};
class SwigDirector_SharedByteBuffer : public voltdb::SharedByteBuffer, public Swig::Director {

public:
    SwigDirector_SharedByteBuffer(zval *self, voltdb::SharedByteBuffer const &other);
    SwigDirector_SharedByteBuffer(zval *self);
    SwigDirector_SharedByteBuffer(zval *self, char *data, int32_t length);
    SwigDirector_SharedByteBuffer(zval *self, boost::shared_array< char > data, int32_t length);
    virtual bool isExpandable();
    virtual void ensureRemaining(int32_t amount) throw(voltdb::NonExpandableBufferException);
    virtual void ensureRemainingExact(int32_t amount) throw(voltdb::NonExpandableBufferException);
    virtual void ensureCapacity(int32_t capacity) throw(voltdb::NonExpandableBufferException);
    virtual void ensureCapacityExact(int32_t capacity) throw(voltdb::NonExpandableBufferException);
    virtual ~SwigDirector_SharedByteBuffer();
    virtual void resetRef(char *data);
    virtual void resetRefSwigPublic(char *data) {
      voltdb::SharedByteBuffer::resetRef(data);
    }
};
class SwigDirector_ScopedByteBuffer : public voltdb::ScopedByteBuffer, public Swig::Director {

public:
    SwigDirector_ScopedByteBuffer(zval *self, int32_t capacity);
    SwigDirector_ScopedByteBuffer(zval *self, char *data, int32_t length);
    virtual bool isExpandable();
    virtual void ensureRemaining(int32_t amount) throw(voltdb::NonExpandableBufferException);
    virtual void ensureRemainingExact(int32_t amount) throw(voltdb::NonExpandableBufferException);
    virtual void ensureCapacity(int32_t capacity) throw(voltdb::NonExpandableBufferException);
    virtual void ensureCapacityExact(int32_t capacity) throw(voltdb::NonExpandableBufferException);
    virtual ~SwigDirector_ScopedByteBuffer();
    virtual void resetRef(char *data);
    virtual void resetRefSwigPublic(char *data) {
      voltdb::ScopedByteBuffer::resetRef(data);
    }
};
class SwigDirector_ProcedureCallbackNative : public voltdb::ProcedureCallback, public Swig::Director {

public:
    SwigDirector_ProcedureCallbackNative(zval *self);
    virtual bool callback(voltdb::InvocationResponse response) throw(voltdb::Exception);
    virtual ~SwigDirector_ProcedureCallbackNative();
};
class SwigDirector_StatusListenerNative : public voltdb::StatusListener, public Swig::Director {

public:
    SwigDirector_StatusListenerNative(zval *self);
    virtual bool uncaughtException(std::exception exception, boost::shared_ptr< voltdb::ProcedureCallback > callback);
    virtual bool connectionLost(std::string hostname, int32_t connectionsLeft);
    virtual bool backpressure(bool hasBackpressure);
    virtual ~SwigDirector_StatusListenerNative();
};
class SwigDirector_ConnectionPool : public voltdb::ConnectionPool, public Swig::Director {

public:
    SwigDirector_ConnectionPool(zval *self);
    virtual ~SwigDirector_ConnectionPool();
};

#endif
