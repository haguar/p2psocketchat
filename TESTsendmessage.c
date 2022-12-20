#include <stdio.h>
#include <string.h>
#include "CUnit-2.1-3/CUnit/Basic.h"
#include <testablechat.h>
//peers can receive message
void testSendMessage() {

    CU_ASSERT_EQUAL(sendmessage("192.168.1.122", "4444", 1, "hope this message finds you well"), 2)//message is sent via code 2 indicating execution reached past the error check for message NOT sent
}
