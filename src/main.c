/*
 * Copyright 2015, Wink Saville
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 */

#include <sel4/sel4.h>
#include <sel4/string.h>
#include <sel4/printf.h>
#include <sel4/debug_printf.h>
#include <sel4/assert.h>
#include <sel4/debug_assert.h>
#include <sel4/putchar.h>
#include <sel4/benchmark.h>
#include <sel4/bootinfo.h>
#include <sel4/stop.h>

volatile seL4_Uint32 volatile0 = 0;


typedef struct {
    int idx;
    char buff[256];
} Buffer;

/**
 * Write a character using seL4_PutChar
 */
static void writeBeg(seL4_Writer* this) {
    Buffer *pBuffer = (Buffer*)this->data;
    pBuffer->idx = 0;
}

/**
 * Write a character using seL4_PutChar
 */
static void writeEnd(seL4_Writer* this) {
    Buffer *pBuffer = (Buffer*)this->data;
    pBuffer->buff[pBuffer->idx] = 0;
}

/**
 * Write a character using seL4_PutChar
 */
static void writeChar(seL4_Writer* this, void* param) {
    Buffer *pBuffer = (Buffer*)this->data;
    char ch = ((char)(((int)param) & 0xff));
    pBuffer->buff[pBuffer->idx++] = ch;
    if (pBuffer->idx >= sizeof(pBuffer->buff)) {
        pBuffer->idx = 0;
    }
}

/**
 * No parameters are passed to main, the return
 * value is ignored and the program hangs.
 */
int main(void) {
    Buffer buffer = {
        .idx = 0
    };
    seL4_Writer writer = {
        .writeBeg = writeBeg,
        .writeEnd = writeEnd,
        .writeParam = writeChar,
        .data = &buffer
    };

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

    //seL4_BenchmarkResetLog();
    //seL4_BenchmarkDumpFullLog();

    char *str1 = "str1";
    char *str2 = "str2";
    seL4_Assert(seL4_StrNCmp(seL4_Null, seL4_Null, 0) == 0);
    seL4_Assert(seL4_StrNCmp(str1, seL4_Null, 0) == 0);
    seL4_Assert(seL4_StrNCmp(seL4_Null, str1, 0) == 0);
    seL4_Assert(seL4_StrNCmp(str1, str1, 4) == 0);
    seL4_Assert(seL4_StrNCmp(str1, str2, 3) == 0);
    seL4_Assert(seL4_StrNCmp(str1, str2, 4) < 0);
    seL4_Assert(seL4_StrNCmp(str2, str1, 4) > 0);

    seL4_PutChar('h');
    seL4_PutChar('i');
    seL4_PutChar('\n');
    seL4_Printf("Hello, World\n");
    seL4_DebugPrintf("Hello, World: via seL4_DebugDrintf\n");

    seL4_WPrintf(&writer, "Hello");
    seL4_Printf("seL4_WPrintf: buffer.buff=%s\n", buffer.buff);
    seL4_Assert(buffer.idx == 5);
    seL4_Assert(seL4_StrNCmp("Hello", buffer.buff, 5) == 0);

    seL4_Printf("%"); seL4_Printf("\n");
    seL4_Printf("expect=<empty>\n");
    seL4_Printf("       %1"); seL4_Printf("\n");
    seL4_Printf("expect=%1\n");
    seL4_Printf("       %%"); seL4_Printf("\n");
    seL4_Printf("expect=%s\n", "%");
    seL4_Printf("   str=%s\n", "string");
    seL4_Printf("expect=string\n");
    seL4_Printf("binary=%b\n",0);
    seL4_Printf("expect=0\n");
    seL4_Printf("binary=%b\n", 0x87654321);
    seL4_Printf("expect=10000111011001010100001100100001\n");
    seL4_Printf("binary=%b\n", 0xFFFFFFFF);
    seL4_Printf("expect=11111111111111111111111111111111\n");
    seL4_Printf("int   =%d\n", 0);
    seL4_Printf("expect=0\n", 0);
    seL4_Printf("int=%d\n", 1);
    seL4_Printf("expect=1\n");
    seL4_Printf("int   =%d\n", 0x7FFFFFFF);
    seL4_Printf("expect=2147483647\n");
    seL4_Printf("int   =%d\n", 0x80000000);
    seL4_Printf("expect=-2147483648\n");
    seL4_Printf("int   =%d\n", 0x80000001);
    seL4_Printf("expect=-2147483647\n");
    seL4_Printf("int   =%d\n", -1);
    seL4_Printf("expect=-1\n");
    seL4_Printf("uint  =%u\n", 0x0);
    seL4_Printf("expect=0\n");
    seL4_Printf("uint  =%u\n", 0x7FFFFFFF);
    seL4_Printf("expect=2147483647\n");
    seL4_Printf("uint  =%u\n", 0x80000000);
    seL4_Printf("expect=2147483648\n");
    seL4_Printf("uint  =%u\n", 0x80000001);
    seL4_Printf("expect=2147483649\n");
    seL4_Printf("uint  =0x%x\n", 0x1234567);
    seL4_Printf("expect=0x1234567\n");
    seL4_Printf("uint  =0x%x\n", 0x89ABCDEF);
    seL4_Printf("expect=0x89ABCDEF\n");
    seL4_Printf("uint  =0x%x\n", 0xFFFFFFFF);
    seL4_Printf("expect=0xFFFFFFFF\n");
    seL4_Printf("       %l"); seL4_Printf("\n");
    seL4_Printf("expect=%l\n");
    seL4_Printf("       %la"); seL4_Printf("\n");
    seL4_Printf("expect=%la\n");
    seL4_Printf("       %ll"); seL4_Printf("\n");
    seL4_Printf("expect=%ll\n");
    seL4_Printf("       %llz"); seL4_Printf("\n");
    seL4_Printf("expect=%llz\n");
    seL4_Printf("       %llz1"); seL4_Printf("\n");
    seL4_Printf("expect=%llz1\n");

    seL4_Printf("uint64=0x%llx\n", 0xFEDCBA9876543210ll);
    seL4_Printf("expect=0xFEDCBA9876543210\n");
    void *pv = (void *)0x87654321;
    seL4_Printf("    pv=0x%p\n", pv);
    seL4_Printf("expect=0x87654321\n");

    seL4_Assert(0 == 0);
    seL4_DebugAssert(0 == 0);
    seL4_CompileTimeAssert(1 == 1);
    seL4_Bool b = seL4_True;
    seL4_Assert(b != seL4_False);
  
    seL4_Int8 i8 = 1;
    seL4_Assert(i8 == 1);
    seL4_Assert(sizeof(i8) == 1);
    seL4_Int16 i16 = 2;
    seL4_Assert(i16 == 2);
    seL4_Assert(sizeof(i16) == 2);
    seL4_Int32 i32 = 3;
    seL4_Assert(i32 == 3);
    seL4_Assert(sizeof(i32) == 4);
    seL4_Int64 i64 = 4;
    seL4_Assert(i64 == 4);
    seL4_Assert(sizeof(i64) == 8);

    seL4_Uint8 u8 = 1;
    seL4_Assert(u8 == 1);
    seL4_Assert(sizeof(u8) == 1);
    seL4_Uint16 u16 = 2;
    seL4_Assert(u16 == 2);
    seL4_Assert(sizeof(u16) == 2);
    seL4_Uint32 u32 = 3;
    seL4_Assert(u32 == 3);
    seL4_Assert(sizeof(u32) == 4);
    seL4_Uint64 u64 = 4;
    seL4_Assert(u64 == 4);
    seL4_Assert(sizeof(u64) == 8);

    seL4_Uint8 *p = seL4_Null;
    seL4_Assert(p == seL4_Null);

    seL4_BootInfo *bi = seL4_GetBootInfo();
    seL4_Printf("bi=%p\n", bi);
    seL4_Printf("bi->nodeId=%d\n", bi->nodeID);

    seL4_Printf("Test nolibc done, Stopping...\n");
    seL4_Stop();

    return 0;
}
