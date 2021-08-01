#ifndef KEYS_H
#define KEYS_H

#include "main.h"

#define KEY(x) (!(keyNew & (x)))
#define KEYPRESS(x) (!(keyNew & (x)) && (keyOld & (x)))

extern u16 keyNew, keyOld;

#endif
