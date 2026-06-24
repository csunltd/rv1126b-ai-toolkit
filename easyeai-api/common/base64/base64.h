/**
 *
 * Copyright 2021 by Guangzhou Easy EAI Technologny Co.,Ltd.
 * website: www.easy-eai.com
 *
 * Author: Jiehao.Zhong <zhongjiehao@easy-eai.com>
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * License file for more details.
 * 
 */

 
#ifndef BASE64_H
#define BASE64_H

#include <stdint.h>


#if defined(__cplusplus)
extern "C" {
#endif

//注意：通常、エンコード後のデータはエンコード前より少なくとも 1/3 大きくなります。具体的なサイズは、入力データサイズを 3 バイト境界にアラインした後の 4/3 です
//例：
//	入力データ長が 2 の場合、3 バイト境界にアラインすると 3 になり、その後 *4/3 します
extern int32_t base64_encode(char *out_data, const char* in_data, unsigned int in_len);
extern int32_t base64_decode(char *out_data, unsigned int out_len, const char* encoded_string);

#if defined(__cplusplus)
}
#endif
#endif // BASE64_H