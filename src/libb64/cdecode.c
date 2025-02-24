/****************************************************************************************************************************
  cdecode.c - c source to a base64 decoding algorithm implementation

  This is part of the libb64 project, and has been placed in the public domain.
  For details, see http://sourceforge.net/projects/libb64

  For ESP8266 using W5x00/ENC8266 Ethernet
   
  AsyncWebServer_Ethernet is a library for the Ethernet with lwIP_5100, lwIP_5500 or lwIP_enc28j60 library
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_Ethernet
  
  Copyright (c) 2016 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
  as published bythe Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <https://www.gnu.org/licenses/>
 *****************************************************************************************************************************/

#include "cdecode.h"

int base64_decode_value(int value_in)
{
  static const char decoding[] =
  { 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1, -1, -1, 0, 1, 2,
    3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1,
    -1, -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
    47, 48, 49, 50, 51
  };

  static const char decoding_size = sizeof(decoding);
  value_in -= 43;

  if (value_in < 0 || value_in > decoding_size)
    return -1;

  return decoding[(int)value_in];
}

void base64_init_decodestate(base64_decodestate* state_in)
{
  state_in->step = step_a;
  state_in->plainchar = 0;
}

int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in)
{
  const char* codechar = code_in;
  char* plainchar = plaintext_out;
  int fragment;

  *plainchar = state_in->plainchar;

  switch (state_in->step)
  {
      while (1)
      {
      case step_a:
        do
        {
          if (codechar == code_in + length_in)
          {
            state_in->step = step_a;
            state_in->plainchar = *plainchar;
            return plainchar - plaintext_out;
          }

          fragment = base64_decode_value(*codechar++);
        } while (fragment < 0);

        *plainchar    = (fragment & 0x03f) << 2;
        
        // fall through

      case step_b:
        do
        {
          if (codechar == code_in + length_in)
          {
            state_in->step = step_b;
            state_in->plainchar = *plainchar;
            return plainchar - plaintext_out;
          }

          fragment = base64_decode_value(*codechar++);
        } while (fragment < 0);

        *plainchar++ |= (fragment & 0x030) >> 4;
        *plainchar    = (fragment & 0x00f) << 4;
        
        // fall through

      case step_c:
        do
        {
          if (codechar == code_in + length_in)
          {
            state_in->step = step_c;
            state_in->plainchar = *plainchar;
            return plainchar - plaintext_out;
          }

          fragment = base64_decode_value(*codechar++);
        } while (fragment < 0);

        *plainchar++ |= (fragment & 0x03c) >> 2;
        *plainchar    = (fragment & 0x003) << 6;
        
        // fall through

      case step_d:
        do
        {
          if (codechar == code_in + length_in)
          {
            state_in->step = step_d;
            state_in->plainchar = *plainchar;
            return plainchar - plaintext_out;
          }

          fragment = base64_decode_value(*codechar++);
        } while (fragment < 0);

        *plainchar++   |= (fragment & 0x03f);
        
        // fall through
      }
  }

  /* control should not reach here */
  return plainchar - plaintext_out;
}

int base64_decode_chars(const char* code_in, const int length_in, char* plaintext_out) {

  base64_decodestate _state;
  base64_init_decodestate(&_state);
  int len = base64_decode_block(code_in, length_in, plaintext_out, &_state);

  if (len > 0)
    plaintext_out[len] = 0;

  return len;
}
