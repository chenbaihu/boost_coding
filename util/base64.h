#ifndef __BASE64_H_
#define __BASE64_H_

#include <string>

namespace util 
{

std::string base64_encode(const unsigned char* input);
std::string base64_encode(const std::string& input);
bool base64_encode(const std::string& input, std::string& output);

std::string base64_decode(const unsigned char* input);
std::string base64_decode(const std::string& input);
bool base64_decode(const std::string& input, std::string& output);

/*
 *result_length --- 编码后的数据块长度。
 *返回值：0 成功
 *        -1失败
 *注：编码后的数据长度为：((input_len+2)/3)*4  
 **/
int base64_encode(const unsigned char* input, int input_len, char* output, int output_size, int* result_length);

/*
 *result_length --- 解码后的数据块长度。
 *返回值：0 成功
 *       -1失败
 *	 output_size >= (input_len*3)/4.
 **/
int base64_decode(const char* input, int input_len, unsigned char* output, int output_size, int* result_length);

}

#endif /* __BASE64_H_ */

