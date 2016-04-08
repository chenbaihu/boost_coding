#include <string.h>
#include <iostream>

#include "gtest_common.h"
#include "base64.h"

TEST_UNIT(base64_test1) 
{
    std::string input_data = "123456789"; 

    std::string b64 = util::Base64::encode(input_data); 
    std::string d64 = util::Base64::decode(b64);
    
    H_TEST_ASSERT(d64==input_data); 

    std::cout << "input_data=" << input_data << std::endl;
    std::cout << "b64="        << b64        << std::endl;
    std::cout << "d64="        << d64        << std::endl;

    //H_TEST_ASSERT(1==1);
    //H_TEST_EQUAL(1, 1);
} 

int main(int argc, char * argv[]) 
{
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}


