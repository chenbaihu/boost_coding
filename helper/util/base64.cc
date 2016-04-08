#include <ctype.h>
#include <stdio.h>
#include "base64.h"

namespace util {

std::string base64_encode(const unsigned char* input) 
{
    if (input == NULL) {
        return std::string();
    }
    return base64_encode(std::string((const char*)input));
}

std::string base64_encode(const std::string& input)
{
    std::string output;
    if (input.empty()) {
        return output;
    }

    if (base64_encode(input, output)) {
        return output;
    }
    output.clear();
    return output;
}

bool base64_encode(const std::string& input, std::string& output)
{
    output.clear();
    if (input.empty()) {
        return true;
    }

    int result_length = ((input.size()+2)/3)*4;;
    output.resize(result_length);
    if (base64_encode((const unsigned char*)input.data(), (int)input.size(), (char*)&output[0], result_length, &result_length)!=0) {
        fprintf(stderr, "base64_encode failed\n");
        output.clear();
        return false;
    }
    return true;
}

std::string base64_decode(const char* input)
{
    if (input==NULL) {
        return std::string();
    }
    return base64_decode(std::string(input));
}

std::string base64_decode(const std::string& input)
{
    std::string output;
    if (input.empty()) {
        return output;
    }

    if (base64_decode(input, output)) {
        return output;
    }
    output.clear();
    return output;
}

bool base64_decode(const std::string& input, std::string& output)
{
    output.clear();
    if (input.empty()) {
        return true;
    }

    int result_length = input.size()*3/4;
    output.resize(result_length);
    if (base64_decode((const char*)input.data(), (int)input.size(), (unsigned char*)&output[0], result_length, &result_length)!=0) {
        output.clear();
        fprintf(stderr, "base64_decode failed\n");
        return false;
    }
    return true;
}

#define XX 127
/*
 * Table for decoding hexadecimal in quoted-printable
 */
static const unsigned char index_hex[256] = {
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
     0, 1, 2, 3,  4, 5, 6, 7,  8, 9,XX,XX, XX,XX,XX,XX,
    XX,10,11,12, 13,14,15,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,10,11,12, 13,14,15,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX
};
#define HEXCHAR(c)  (index_hex[(unsigned char)(c)])

/*
 * Table for decoding base64
 */
static const unsigned char index_64[256] = {
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,62,XX,XX,
    52,53,54,55, 56,57,58,59, 60,61,XX,XX, XX,XX,XX,XX,
    XX, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
    15,16,17,18, 19,20,21,22, 23,24,25,XX, XX,XX,XX,63,
    XX,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
    41,42,43,44, 45,46,47,48, 49,50,51,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX
};
#define CHAR64(c)  (index_64[(unsigned char)(c)])

static const char base64_char[64] =
{
    'A','B','C','D',  'E','F','G','H',
    'I','J','K','L',  'M','N','O','P',
    'Q','R','S','T',  'U','V','W','X',
    'Y','Z','a','b',  'c','d','e','f',
    'g','h','i','j',  'k','l','m','n',
    'o','p','q','r',  's','t','u','v',
    'w','x','y','z',  '0','1','2','3',
    '4','5','6','7',  '8','9','-','_'
};

void base64_encode_group(const unsigned char *input, int grp_len, char* output)
{
    if (grp_len == 3) {
    	output[0] = base64_char[ input[0]>>2 ];
    	output[1] = base64_char[ ((input[0]&0x03)<<4) + (input[1]>>4) ];
    	output[2] = base64_char[ ((input[1]&0x0f)<<2) + (input[2]>>6) ];
    	output[3] = base64_char[ input[2]&0x3f ];
        return;
    }
	
    if (grp_len == 2) {
        output[0] = base64_char[ input[0]>>2 ];
        output[1] = base64_char[ ((input[0]&0x03)<<4) + (input[1]>>4) ];
        output[2] = base64_char[ ((input[1]&0x0f)<<2) ];
        output[3] = '=';
        return;
    }
	
    if (grp_len == 1) {
	output[0] = base64_char[ input[0]>>2 ];
	output[1] = base64_char[ ((input[0]&0x03)<<4) ];
	output[2] = '=';
	output[3] = '=';
    }
}

void base64_decode_group(const char input[4], unsigned char output[3], int* p_write)
{
	if (input[2]=='=' && input[3]=='=') { 
		output[0] = CHAR64(input[0])<<2;
		output[0] |= CHAR64(input[1])>>4;
		*p_write = 1;
	} else if (input[3]=='=') {
		output[0] = CHAR64(input[0])<<2;
		output[0] |= CHAR64(input[1])>>4;
		output[1] = CHAR64(input[1])<<4;
		output[1] |= CHAR64(input[2])>>2;
		*p_write = 2;
	} else {
		output[0] = CHAR64(input[0])<<2;
		output[0] |= CHAR64(input[1])>>4;
		output[1] = CHAR64(input[1])<<4;
		output[1] |= CHAR64(input[2])>>2;
		output[2] = CHAR64(input[2])<<6;
		output[2] |= CHAR64(input[3]);
		*p_write = 3;
	}
}

int base64_encode(const unsigned char* input, int input_len, char* output, int output_size, int* result_length)
{
    int encode_length;
    
    if (!output) {
        return -1;
    }
    
    encode_length = ((input_len+2)/3)*4;
    if (output_size < encode_length) {
        return -1;
    }
    *result_length = encode_length;
    
    int grp_count = (input_len+2)/3;
    for(int i=0; i<grp_count; i++) {
        if (i==grp_count-1) {
        	base64_encode_group(input+i*3, input_len-i*3, output+i*4);
            continue;
        }
        base64_encode_group(input+i*3, 3, output+i*4);
    }
    return 0;
}

int base64_decode(const char* input, int input_len, unsigned char* output, int output_size, int* result_length)
{
    int count;
    int written;
    
    if (!output) {
        return -1;
    }
    
    if ((input_len%4) != 0) {
        return -1;
    }
    
    *result_length = count = 0;
    while (count<input_len){
        if(*result_length>=output_size) {
        	return -1;
        }
        
        int bad_data = 0;
        if (input[count+2]=='=' && input[count+3]=='=') { 
            if(CHAR64(input[count])==XX 
                    || CHAR64(input[count+1])==XX) {
                bad_data = 1;
            }
        } else if(input[count+3]=='=') {
            if(CHAR64(input[count])==XX 
                    || CHAR64(input[count+1])==XX 
                    || CHAR64(input[count+2])==XX) {
                bad_data = 1;
            }
        } else {
            for(int i=0; i<4 && !bad_data; i++) {
                if(CHAR64(input[count+i])==XX || input[count+i]=='=') {
                    bad_data = 1;
                }
            }
        }
        
        if(bad_data) {
            return -1;
        }
        
        base64_decode_group(input+count, (unsigned char*)output + *result_length, &written);
        *result_length += written;
        count += 4;
    }
    return 0;
}

} // end of namespace util


#ifdef _TEST_

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

int main(int argc, char* argv[]) 
{
    std::string src = "helll base64";
    std::string b64 = util::base64_encode(src);
    std::string d64 = util::base64_decode(b64);
    std::cout << "b64[" << b64 << "] d64[" << d64 << "]" << std::endl;
    return 0;
}

#endif




