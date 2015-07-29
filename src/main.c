/*
 * Copyright 2015, Wink Saville
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 */

#include <autoconf.h>

#include <sel4/simple_types.h>
#include <sel4/sel4.h>
#include <sel4/string.h>
#include <sel4/printf.h>
#include <sel4/debug_printf.h>
#include <sel4/assert.h>
#include <sel4/debug_assert.h>
#include <sel4/putchar.h>
#include <sel4/benchmark.h>

volatile seL4_Uint32 volatile0 = 0;

/**
 * Return seL4_True if the expr was not true signifying a failure
 */
static inline seL4_Bool test_failure(seL4_Bool expr, const char* exprStrg,
                                     const char* function, const char* file, const int lineNumber) {
    seL4_Bool failure = !expr;
    if (failure) {
        if (seL4_StrLen(exprStrg) == 0) {
            seL4_Printf("TEST was not true. func=%s file=%s:%d\n",
                        function, file, lineNumber);
        } else {
            seL4_Printf("TEST '%s' was not true. func=%s file=%s:%d\n",
                        exprStrg, function, file, lineNumber);
        }

    }
    return failure;
}

/**
 * Macro which returns seL4_True if the expr did NOT evaluate to true
 */
#define TEST(expr) test_failure((seL4_Bool)(expr), #expr, __FUNCTION__, __FILE__, __LINE__)

seL4_Bool test_strlen(void) {
    seL4_Bool failure = seL4_False;
    failure |= TEST(seL4_StrLen(seL4_Null) == 0);
    failure |= TEST(seL4_StrLen("") == 0);
    failure |= TEST(seL4_StrLen("1") == 1);
    failure |= TEST(seL4_StrLen("\n") == 1);
    failure |= TEST(seL4_StrLen("12") == 2);
    return failure;
}

seL4_Bool test_strncmp(void) {
    seL4_Bool failure = seL4_False;
    char *str1 = "str1";
    char *str2 = "str2";
    failure |= TEST(seL4_StrNCmp(seL4_Null, seL4_Null, 0) == 0);
    failure |= TEST(seL4_StrNCmp(str1, seL4_Null, 0) == 0);
    failure |= TEST(seL4_StrNCmp(seL4_Null, str1, 0) == 0);
    failure |= TEST(seL4_StrNCmp(str1, str1, 4) == 0);
    failure |= TEST(seL4_StrNCmp(str1, str2, 3) == 0);
    failure |= TEST(seL4_StrNCmp(str1, str2, 4) < 0);
    failure |= TEST(seL4_StrNCmp(str2, str1, 4) > 0);
    return failure;
}

seL4_Bool test_putchar(void) {
    // Just verify we can call it.
    seL4_PutChar('h');
    seL4_PutChar('i');
    seL4_PutChar('\n');
    return seL4_False;
}

seL4_Bool test_simple_types(void) {
    seL4_Bool failure = seL4_False;
    seL4_Int8 i8 = 1;
    failure |= TEST(i8 == 1);
    failure |= TEST(sizeof(i8) == 1);
    seL4_Int16 i16 = 2;
    failure |= TEST(i16 == 2);
    failure |= TEST(sizeof(i16) == 2);
    seL4_Int32 i32 = 3;
    failure |= TEST(i32 == 3);
    failure |= TEST(sizeof(i32) == 4);
    seL4_Int64 i64 = 4;
    failure |= TEST(i64 == 4);
    failure |= TEST(sizeof(i64) == 8);

    seL4_Uint8 u8 = 1;
    failure |= TEST(u8 == 1);
    failure |= TEST(sizeof(u8) == 1);
    seL4_Uint16 u16 = 2;
    failure |= TEST(u16 == 2);
    failure |= TEST(sizeof(u16) == 2);
    seL4_Uint32 u32 = 3;
    failure |= TEST(u32 == 3);
    failure |= TEST(sizeof(u32) == 4);
    seL4_Uint64 u64 = 4;
    failure |= TEST(u64 == 4);
    failure |= TEST(sizeof(u64) == 8);

    seL4_Uint8 *p = seL4_Null;
    failure |= TEST(p == seL4_Null);

    return failure;
}

seL4_Bool test_sel4_assert() {
    seL4_Bool failure = seL4_False;

    /*
     * Manually test these compile time errors by enabling both
     * and the compilation should fail.
     */
    //seL4_CompileTimeAssert(1 == 0);
    //seL4_CompileTimeAssert(volatile0 == 0);

    /*
     * Manually test these runtime errors, enable one at a time
     * and compile and run, each statement should fail.
     */
    //seL4_Fail("failing");
    //seL4_Assert(0 == 1);
    //seL4_Assert(volatile0 == 1);
    //seL4_DebugAssert(1 == 2);

    /*
     * Manually uncomment the statement below and in .config
     * set CONFIG_USER_DEBUG_BUILD=y this causes NDEBUG to NOT
     * be defined. Compile and run and you should see a runtime
     * error.
     *
     * Next set CONFIG_USER_DEBUG_BUILD=n recompile and rerun.
     * The statement will NOT fail because NDEBUG is now defined.
     * This verifies seL4_DebugAssert compiles to a NOP when
     * NDEBUG is defined.
     */
    //seL4_DebugAssert(volatile0 == 2);

    // None of this should fail
    seL4_Assert(0 == 0);
    seL4_Assert(volatile0 == 0);
    seL4_DebugAssert(0 == 0);
    seL4_DebugAssert(volatile0 == 0);
    seL4_CompileTimeAssert(0 == 0);
    seL4_Bool b = seL4_True;
    seL4_Assert(b != seL4_False);

    return failure;
}

#if defined(CONFIG_BENCHMARK)

seL4_Bool test_benchmark(void) {
    seL4_BenchmarkResetLog();
    seL4_BenchmarkDumpFullLog();
    return seL4_False;
}

#else // CONFIG_BENCHMARK is not defined

#pragma message "In " __FILE__ " CONFIG_BENCHMARK is not defined. Edit .config setting 'CONFIG_BENCHMARK=y'"

#endif // CONFIG_BENCHMARK

// Test BootInfo if it gets initialized, currently it isn't
seL4_Bool test_bootinfo() {
    seL4_Bool failure = seL4_False;
    seL4_BootInfo *bi = seL4_GetBootInfo();
    failure |= TEST(bi != seL4_Null);
    failure |= TEST(bi->nodeID == 0);
    return failure;
}

typedef struct {
    int idx;
    char buff[256];
} Buffer;

/**
 * writeBeg called before first writeCharToBuff
 */
static void writeBeg(seL4_Writer* this) {
    Buffer *pBuffer = (Buffer*)this->data;
    pBuffer->idx = 0;
}

/**
 * writeEnd called after the last writeCharToBuff
 */
static void writeEnd(seL4_Writer* this) {
    Buffer *pBuffer = (Buffer*)this->data;
    pBuffer->buff[pBuffer->idx] = 0;
}

/**
 * Write a character to the buffer
 */
static void writeCharToBuff(seL4_Writer* this, void* param) {
    Buffer *pBuffer = (Buffer*)this->data;
    char ch = ((char)(((int)param) & 0xff));
    pBuffer->buff[pBuffer->idx++] = ch;
    if (pBuffer->idx >= sizeof(pBuffer->buff)) {
        pBuffer->idx = 0;
    }
}

static inline seL4_Bool testPrintingResult(const char* expected, Buffer *pBuffer,
                                           const char* function, const char* file, const int lineNumber) {
    seL4_Bool failure = seL4_False;
    if (pBuffer->idx != seL4_StrLen(expected)) {
        seL4_Printf("buff:%s != expected:%s idx=%d\n", pBuffer->buff, expected, pBuffer->idx);
        failure |= test_failure(pBuffer->idx == seL4_StrLen(expected), "", function, file, lineNumber);
    } else if (seL4_StrNCmp(expected, pBuffer->buff, pBuffer->idx) != 0) {
        seL4_Printf("buff:%s != expected:%s idx=%d\n", pBuffer->buff, expected, pBuffer->idx);
        failure |= test_failure(seL4_StrNCmp(expected, pBuffer->buff, pBuffer->idx) == 0l,
                                "", function, file, lineNumber);
    }
    return failure;
}

#define TEST_PRINTING_NO_PARAM(formatting, expectedVal) \
    ({  seL4_Bool failure; \
        seL4_WPrintf(&writer, formatting); \
        failure = testPrintingResult(expectedVal, (Buffer*)(writer.data), __FUNCTION__, __FILE__, __LINE__); \
        failure; })

#define TEST_PRINTING(formatting, param, expectedVal) \
    ({  seL4_Bool failure; \
        seL4_WPrintf(&writer, formatting, param); \
        failure = testPrintingResult(expectedVal, (Buffer*)(writer.data), __FUNCTION__, __FILE__, __LINE__); \
        failure; })


seL4_Bool test_printf(void) {
    seL4_Bool failure = seL4_False;
    seL4_Printf("Hello, World\n");
    seL4_DebugPrintf("Hello, World: via seL4_DebugDrintf\n");

    // Define a buffer and a writer for testing printf
    Buffer buffer;
    seL4_Writer writer = {
            .writeBeg = writeBeg,
            .writeParam = writeCharToBuff,
            .writeEnd = writeEnd,
            .data = &buffer
    };

    failure |= TEST_PRINTING_NO_PARAM("Hello", "Hello");
    failure |= TEST_PRINTING_NO_PARAM("%", "");
    failure |= TEST_PRINTING_NO_PARAM("%1", "%1");
    failure |= TEST_PRINTING_NO_PARAM("%%", "%");

    failure |= TEST_PRINTING("%s", "string", "string");
    failure |= TEST_PRINTING("%b", 0, "0");
    failure |= TEST_PRINTING("%b", 0x87654321, "10000111011001010100001100100001");
    failure |= TEST_PRINTING("%b", 0xFFFFFFFF, "11111111111111111111111111111111");

    failure |= TEST_PRINTING("%d", 1, "1");
    failure |= TEST_PRINTING("%d", 0x7FFFFFFF, "2147483647");
    failure |= TEST_PRINTING("%d", 0x80000000, "-2147483648");
    failure |= TEST_PRINTING("%d", 0x80000001, "-2147483647");
    failure |= TEST_PRINTING("%d", 0xFFFFFFFF, "-1");
    failure |= TEST_PRINTING("%d", -1, "-1");

    failure |= TEST_PRINTING("%u", 2, "2");
    failure |= TEST_PRINTING("%u", 0x7FFFFFFF, "2147483647");
    failure |= TEST_PRINTING("%u", 0x80000000, "2147483648");
    failure |= TEST_PRINTING("%u", 0x80000001, "2147483649");
    failure |= TEST_PRINTING("%u", 0xFFFFFFFF, "4294967295");
    failure |= TEST_PRINTING("%u", -1, "4294967295");

    failure |= TEST_PRINTING("%x", 0, "0");
    failure |= TEST_PRINTING("%x", 9, "9");
    failure |= TEST_PRINTING("%x", 10, "a");
    failure |= TEST_PRINTING("%x", 15, "f");
    failure |= TEST_PRINTING("%x", 16, "10");
    failure |= TEST_PRINTING("0x%x", 0x12345678, "0x12345678");
    failure |= TEST_PRINTING("0x%x", 0x9abcdef0, "0x9abcdef0");

    failure |= TEST_PRINTING_NO_PARAM("%l", "%l");
    failure |= TEST_PRINTING_NO_PARAM("%la", "%la");
    failure |= TEST_PRINTING_NO_PARAM("%ll", "%ll");
    failure |= TEST_PRINTING_NO_PARAM("%llz1", "%llz1");

    failure |= TEST_PRINTING("%llx", 0ll, "0");
    failure |= TEST_PRINTING("%llx", 9ll, "9");
    failure |= TEST_PRINTING("%x", 10ll, "a");
    failure |= TEST_PRINTING("%x", 15ll, "f");
    failure |= TEST_PRINTING("%llx", 16ll, "10");

    // Test we are "filling" zeros correctly as we print using writeUint32 twice
    failure |= TEST_PRINTING("%llx", 0x80000000ll, "80000000");
    failure |= TEST_PRINTING("%llx", 0x800000000ll, "800000000");
    failure |= TEST_PRINTING("%llx", 0x800000000000ll, "800000000000");
    failure |= TEST_PRINTING("%llx", 0x80000000000000ll, "80000000000000");
    failure |= TEST_PRINTING("%llx", 0x8000000000000000ll, "8000000000000000");
    failure |= TEST_PRINTING("%llx", 0x8000000000000001ll, "8000000000000001");

    failure |= TEST_PRINTING("%llx", 0xFEDCBA9876543210ll, "fedcba9876543210");
    failure |= TEST_PRINTING("%llx", 0x7fffffffffffffffll, "7fffffffffffffff");
    failure |= TEST_PRINTING("%llx", -1ll,                 "ffffffffffffffff");

    void *pv = (void *)0xf7654321;
    failure |= TEST_PRINTING("%p", pv, "f7654321");

    return failure;
}

/**
 * No parameters are passed to main, the return
 * value is ignored and the program hangs.
 */
int main(void) {
    seL4_Bool failure = seL4_False;
    seL4_Printf("\ntest-nolibc:\n");

    failure |= test_strlen();
    failure |= test_strncmp();
    failure |= test_putchar();
    failure |= test_simple_types();
    failure |= test_sel4_assert();
    #if defined(CONFIG_BENCHMARK)
    failure |= test_benchmark();
    #endif
    failure |= test_bootinfo();
    failure |= test_printf();

    seL4_Printf(failure ? "-FAILURE-\n" : "+SUCCESS+\n");
    return 0;
}
