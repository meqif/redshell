TEST_CFLAGS := $(CFLAGS) -Isrc -Ibuild/cmockery/include/google

TEST_OBJ = build/test_all.o \
           build/test_alias.o \
	   build/test_helper.o

build/test_%.o: tests/test_%.c
	@@echo "CC   test_$*.c"
	@$(CC) -c -o $@ $(TEST_CFLAGS) $<

build/cmockery/lib/libcmockery.a:
	sh build-test-deps.sh

tests: all build/cmockery/lib/libcmockery.a $(TEST_OBJ)
	$(LINK.cc) -o run_tests $(TEST_OBJ) src/alias.o src/helper.o $(LIBS) build/cmockery/lib/libcmockery.a
