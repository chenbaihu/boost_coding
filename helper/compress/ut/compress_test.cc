#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

#include "gtest_common.h"
#include "base64.h"
#include "compressor.h"
#include "bzip2.h"

static uint64_t timems() {
    struct timeval time;
    ::gettimeofday(&time, NULL);
    return time.tv_sec*1000 + time.tv_usec/1000; 
}

#define TimeConsumeMs(begms)  \
    (timems()-begms) 

TEST_UNIT(compress_nocompress_test1) 
{
    std::string input_data = "123456789"; 

    util::CompressorPtr compressPtr = util::CompressorFactory::CreateCompressor(util::kNoCompress); 
    H_TEST_ASSERT(compressPtr!=NULL);

    std::string compress_data; 
    H_TEST_ASSERT(compressPtr->Compress(input_data.data(), input_data.size(), compress_data)); 
    H_TEST_ASSERT(compress_data==input_data);

    std::string uncompress_data; 
    H_TEST_ASSERT(compressPtr->Uncompress(compress_data.data(), compress_data.size(), uncompress_data)); 
    H_TEST_ASSERT(uncompress_data==input_data);

    //H_TEST_ASSERT(1==1);
    //H_TEST_EQUAL(1, 1);
}

TEST_UNIT(compress_huffmancode_test1) 
{
    std::string input_data = "123456789"; 

    util::CompressorPtr compressPtr = util::CompressorFactory::CreateCompressor(util::kTestHuffman); 
    H_TEST_ASSERT(compressPtr!=NULL);

    std::string compress_data; 
    H_TEST_ASSERT(compressPtr->Compress(input_data.data(), input_data.size(), compress_data)); 

    std::string uncompress_data; 
    H_TEST_ASSERT(compressPtr->Uncompress(compress_data.data(), compress_data.size(), uncompress_data)); 
    printf("compress_data.size=%ld\tuncompress_data.size=%ld\tinput_data.size=%ld\n", compress_data.size(), uncompress_data.size(), input_data.size()); 
    H_TEST_ASSERT(memcmp(uncompress_data.data(), input_data.data(), input_data.size())==0); 
} 

TEST_UNIT(compress_huffmancode_test2) 
{
    std::string input_data_b64 = "CgomCgoKCgoKbQoK4AoKCgpICgoKCgoKCgoKCgAKCgoKCgoKQQoKCgoKAAoACmkKCgoKCgoKCgoKCgoKCgIKCgoKCgoKCgoKCgoKCgoKCgoKCgAKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKAAoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgAKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoCCgoKCgoKCgoKCgoKCgoKCgoKCgoACgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgAKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgAKAApjCgAKAAoKCgoKSgo=";  
    std::string input_data = util::Base64::decode(input_data_b64);

    util::CompressorPtr compressPtr = util::CompressorFactory::CreateCompressor(util::kTestHuffman); 
    H_TEST_ASSERT(compressPtr!=NULL);

    std::string compress_data; 
    H_TEST_ASSERT(compressPtr->Compress(input_data.data(), input_data.size(), compress_data)); 

    std::string uncompress_data; 
    H_TEST_ASSERT(compressPtr->Uncompress(compress_data.data(), compress_data.size(), uncompress_data)); 
    printf("compress_data.size=%ld\tuncompress_data.size=%ld\tinput_data.size=%ld\n", compress_data.size(), uncompress_data.size(), input_data.size()); 
    H_TEST_ASSERT(memcmp(uncompress_data.data(), input_data.data(), input_data.size())==0); 
    H_TEST_ASSERT(input_data_b64==util::Base64::encode(uncompress_data)); 
}

//TEST_UNIT(compress_huffmancode_test3) 
//{
//    std::string input_data_b64 = "AQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEB";
//    std::string input_data = util::Base64::decode(input_data_b64);
//
//    util::CompressorPtr compressPtr = util::CompressorFactory::CreateCompressor(util::kTestHuffman); 
//    H_TEST_ASSERT(compressPtr!=NULL);
//
//    std::string compress_data; 
//    H_TEST_ASSERT(compressPtr->Compress(input_data.data(), input_data.size(), compress_data)); 
//
//    std::string uncompress_data; 
//    H_TEST_ASSERT(compressPtr->Uncompress(compress_data.data(), compress_data.size(), uncompress_data)); 
//    printf("uncompress_data.size=%ld\tinput_data.size=%ld\n", uncompress_data.size(), input_data.size()); 
//    H_TEST_ASSERT(memcmp(uncompress_data.data(), input_data.data(), input_data.size())==0); 
//    H_TEST_ASSERT(input_data_b64==util::Base64::encode(uncompress_data)); 
//}

TEST_UNIT(compress_zlibhuffmancode_test1) 
{
    std::string input_data = "123456789"; 

    util::CompressorPtr compressPtr = util::CompressorFactory::CreateCompressor(util::kHuffman); 
    H_TEST_ASSERT(compressPtr!=NULL);

    std::string compress_data; 
    H_TEST_EQUAL(compressPtr->Compress(input_data.data(), input_data.size(), compress_data), false); // 因为膨胀 

    //std::string uncompress_data; 
    //H_TEST_ASSERT(compressPtr->Uncompress(compress_data.data(), compress_data.size(), uncompress_data)); 
    //printf("uncompress_data.size=%ld\tinput_data.size=%ld\n", uncompress_data.size(), input_data.size()); 
    //H_TEST_ASSERT(memcmp(uncompress_data.data(), input_data.data(), input_data.size())==0); 
} 

TEST_UNIT(compress_zlibhuffmancode_test2) 
{
    std::string input_data_b64 = "CgomCgoKCgoKbQoK4AoKCgpICgoKCgoKCgoKCgAKCgoKCgoKQQoKCgoKAAoACmkKCgoKCgoKCgoKCgoKCgIKCgoKCgoKCgoKCgoKCgoKCgoKCgAKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKAAoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgAKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoCCgoKCgoKCgoKCgoKCgoKCgoKCgoACgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgAKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgAKAApjCgAKAAoKCgoKSgo=";  
    std::string input_data = util::Base64::decode(input_data_b64);

    util::CompressorPtr compressPtr = util::CompressorFactory::CreateCompressor(util::kHuffman); 
    H_TEST_ASSERT(compressPtr!=NULL);

    std::string compress_data; 
    H_TEST_ASSERT(compressPtr->Compress(input_data.data(), input_data.size(), compress_data)); 

    std::string uncompress_data; 
    H_TEST_ASSERT(compressPtr->Uncompress(compress_data.data(), compress_data.size(), uncompress_data)); 
    printf("------compress_data.size=%ld\tuncompress_data.size=%ld\tinput_data.size=%ld\n", compress_data.size(), uncompress_data.size(), input_data.size()); 
    H_TEST_ASSERT(memcmp(uncompress_data.data(), input_data.data(), input_data.size())==0); 
    H_TEST_ASSERT(input_data_b64==util::Base64::encode(uncompress_data)); 
}

TEST_UNIT(compress_zlibhuffmancode_test3) 
{
    std::string input_data_b64 = "AQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEB";
    std::string input_data = util::Base64::decode(input_data_b64);

    util::CompressorPtr compressPtr = util::CompressorFactory::CreateCompressor(util::kHuffman); 
    H_TEST_ASSERT(compressPtr!=NULL);

    std::string compress_data; 
    H_TEST_ASSERT(compressPtr->Compress(input_data.data(), input_data.size(), compress_data)); 

    std::string uncompress_data; 
    H_TEST_ASSERT(compressPtr->Uncompress(compress_data.data(), compress_data.size(), uncompress_data)); 
    printf("-------compress_data.size=%ld\tuncompress_data.size=%ld\tinput_data.size=%ld\n", compress_data.size(), uncompress_data.size(), input_data.size()); 
    H_TEST_ASSERT(memcmp(uncompress_data.data(), input_data.data(), input_data.size())==0); 
    H_TEST_ASSERT(input_data_b64==util::Base64::encode(uncompress_data)); 
}

TEST_UNIT(compress_bzip2_test1)
{
    std::string input_data = "123456789"; 

    util::CompressorPtr compressPtr = util::CompressorFactory::CreateCompressor(util::kBZIP2); 
    H_TEST_ASSERT(compressPtr!=NULL);

    std::string compress_data; 
    H_TEST_EQUAL(compressPtr->Compress(input_data.data(), input_data.size(), compress_data), false);  //因为膨胀，所以失败

    //std::string uncompress_data; 
    //H_TEST_ASSERT(compressPtr->Uncompress(compress_data.data(), compress_data.size(), uncompress_data, input_data.size())); 
    //H_TEST_ASSERT(memcmp(uncompress_data.data(), input_data.data(), input_data.size())==0); 
} 

TEST_UNIT(compress_bzip2_test2) 
{
    std::string input_data_b64 = "CgomCgoKCgoKbQoK4AoKCgpICgoKCgoKCgoKCgAKCgoKCgoKQQoKCgoKAAoACmkKCgoKCgoKCgoKCgoKCgIKCgoKCgoKCgoKCgoKCgoKCgoKCgAKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKAAoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgAKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoCCgoKCgoKCgoKCgoKCgoKCgoKCgoACgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgAKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgAKAApjCgAKAAoKCgoKSgo=";  
    std::string input_data = util::Base64::decode(input_data_b64);

    util::CompressorPtr compressPtr = util::CompressorFactory::CreateCompressor(util::kBZIP2); 
    H_TEST_ASSERT(compressPtr!=NULL);

    std::string compress_data; 
    uint64_t begms = timems();
    H_TEST_ASSERT(compressPtr->Compress(input_data.data(), input_data.size(), compress_data)); 
    fprintf(stdout, "Compress usedtime=%lu\tinput_data.size=%lu\tcompress_data.size=%lu\n", 
            TimeConsumeMs(begms), 
            input_data.size(), 
            compress_data.size());

    compress_data.resize(input_data.size());
    size_t compressLen=input_data.size();
    begms = timems();
    int ret = util::Bzip2::Compress(input_data.data(), input_data.size(), &compress_data[0], &compressLen);
    fprintf(stdout, "Compress usedtime=%lu\tinput_data.size=%lu\tcompress_data.size=%lu\n", 
            TimeConsumeMs(begms), 
            input_data.size(), 
            compressLen);
    H_TEST_EQUAL(ret, 0); 

    std::string uncompress_data; 
    begms = timems();
    H_TEST_ASSERT(compressPtr->Uncompress(compress_data.data(), compress_data.size(), uncompress_data, input_data.size())); 
    fprintf(stdout, "Uncompress usedtime=%lu\tcompress_data.size=%lu\tuncompress_data.size=%lu\n", 
            TimeConsumeMs(begms), 
            compress_data.size(), 
            uncompress_data.size());
    H_TEST_ASSERT(memcmp(uncompress_data.data(), input_data.data(), input_data.size())==0); 
    H_TEST_ASSERT(input_data_b64==util::Base64::encode(uncompress_data)); 
}

TEST_UNIT(compress_gzip_test1) 
{
    H_TEST_ASSERT(1==1);
    H_TEST_EQUAL(1, 1);
}

TEST_UNIT(compress_zlib_test1) 
{
    H_TEST_ASSERT(1==1);
    H_TEST_EQUAL(1, 1);
}

int main(int argc, char * argv[]) 
{
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}


