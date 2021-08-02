#include "Misc.h"
#include "Gba.h"

// this is for htoa to have a place to store a string
u8 tempStr[32];

// converts hex number to string
u8 *htoa(u32 number) {
  s8 i, tempNum;
  u8 *str = tempStr;

  *str++ = '0';
  *str++ = 'x';
  for (i = 7; i >= 0; i--) {
    tempNum = (number >> (i << 2)) & 15;
    if (tempNum == 0)
      *str++ = '0';
    else if (tempNum < 10) {
      *str++ = ((u8)'1') + (tempNum - 1);
    } else {
      *str++ = ((u8)'A') + (tempNum - 10);
    }
  }
  *str = '\0';

  return tempStr;
}
