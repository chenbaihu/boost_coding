#include "compressor.h"

#include "bzip2.h"
#include "huffmancode.h"

//#include <zlib/zlib.h>
//#include "zlib.h"
//#include "gzip.h"

namespace util {

CompressorPtr CompressorFactory::CreateCompressor(int compress_method)
{
    switch(compress_method)
    {
        case kNoCompress:
            return std::tr1::shared_ptr<Compressor>(new FakeCompressor());
            break;
        case kHuffman:
            return std::tr1::shared_ptr<Compressor>(new HuffManCode());
            break;
        case kBZIP2:
            return std::tr1::shared_ptr<Compressor>(new Bzip2());
            break;
        //case kZlibCompress:
        //    return std::tr1::shared_ptr<Compressor>(new ZLib());
        //    break;
        //case kGZipCompress:
        //    return std::tr1::shared_ptr<Compressor>(new GZip());
        //    break;
        default:
            break;
    }
    return std::tr1::shared_ptr<Compressor>();
}

//size_t CompressorFactory::GetCompressBound(int compress_method, size_t original_data_len)
//{
//    switch(compress_method)
//    {
//        case kNoCompress:
//            return original_data_len;
//            break;
//        case kZlibCompress:
//            return ZLib::GetCompressBound(original_data_len);
//        case kGZipCompress:
//            return GZip::GetCompressBound(original_data_len);
//        default:
//            return original_data_len;
//    }
//}

} // end of namespace util 


