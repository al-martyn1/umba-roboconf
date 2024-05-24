#pragma once

// Спёрто из проекта umba-2c.
// Вообще надо бы сделать ещё и декодинг base64 и выделить в отдельную либу

// https://tools.ietf.org/html/rfc2045#page-24
// https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp
// https://ru.wikipedia.org/wiki/Base64

inline
const char* getBase64StandartChars()
{
    static const char* base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/="; // last symbol - filler
    return base64_chars;
}

inline
const char* getBase64UrlChars()
{
    static const char* base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789-_="; // last symbol - filler
    return base64_chars;
}

inline
const char* getBase64UrlAlterChars()
{
    static const char* base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789*-="; // last symbol - filler
    return base64_chars;
}

inline
const char* getBase64RegexChars()
{
    static const char* base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789!-="; // last symbol - filler
    return base64_chars;
}



static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode( unsigned char const* bytes_to_encode
                         , unsigned int in_len
                         , bool         add_filling  = true
                         , unsigned int max_line_len = 0
                         , const char* base64_chars = getBase64StandartChars()
                         )
{
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  //const char* base64_chars = getBase64Chars();

  unsigned int output_line_len = 0; // unsigned int max_line_len

  while (in_len--)
  {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3)
    {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
      {
        ret += base64_chars[char_array_4[i]];
        ++output_line_len;
        if (max_line_len && output_line_len>=max_line_len)
        {
            ret += "\r\n";
            output_line_len = 0;
        }
      }
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    if (add_filling)
    {
        while((i++ < 3))
          ret += base64_chars[64];
          //ret += '=';
    }
      

  }

  return ret;

}