#include <stdio.h>
#include <string.h>
#include "CUnit-2.1-3/CUnit/Basic.h"

//The max limit of the text field for messages
void testMessageSize() {

    CU_ASSERT_EQUAL(sendmessage("192.168.1.122", "4444", "1",
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), 1)// this message is too long and will return error code 1
    CU_ASSERT_EQUAL(sendmessage("192.168.1.122", "4444", "1",
                             "This is a properly sized message"), 2)
    // this message is correctly sized and will return error code 2
}
