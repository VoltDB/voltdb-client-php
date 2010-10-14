# warnings
CPPFLAGS += -fmessage-length=0
# weird crap
CPPFLAGS += -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS
# code output
CPPFLAGS += -O3 -arch x86_64
# headers
CPPFLAGS += -I./cppinclude -I../src/cpp
CPPFLAGS += -I/opt/local/include/php -I/opt/local/include/php/main -I/opt/local/include/php/TSRM -I/opt/local/include/php/Zend -I/opt/local/include/php/ext -I/opt/local/include/php/ext/date/lib
# libraries
CPPFLAGS += -pthread

LDFLAGS += -shared -dynamic -dylib -arch x86_64 -lpthread
LDFLAGS += -undefined suppress -flat_namespace

# All Target
all: voltdb.so

# Tool invocations
voltdb_wrap.o: ../src/cpp/voltdb_wrap.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CCACHE) $(COMPILE.cpp) -c -o $@ $?
	@echo 'Finished building: $<'
	@echo ' '
	
voltdb.so: voltdb_wrap.o libvoltcpp.a
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(LINK.cpp) $(LDFLAGS) -o $@ $^
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *.o voltdb.so
	-@echo ' '
	
.PHONY: all clean