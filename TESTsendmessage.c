#include <stdio.h>
#include <string.h>
#include "CUnit-2.1-3/CUnit/Basic.h"
#include <testablechat.h>
//peers can receive message
void testSendMessage() {

    CU_ASSERT_EQUAL(sendmessage())
}
