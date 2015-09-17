/*
   RainbowCrack - a general propose implementation of Philippe Oechslin's faster time-memory trade-off technique.

   Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
*/

#ifndef _HASHALGORITHM_H
#define _HASHALGORITHM_H

void HashLM(unsigned char* pPlain, int nPlainLen, unsigned char* pHash);
void HashMD5(unsigned char* pPlain, int nPlainLen, unsigned char* pHash);
void HashMD4(unsigned char* pPlain, int nPlainLen, unsigned char* pHash);
void HashMD2(unsigned char* pPlain, int nPlainLen, unsigned char* pHash);
void HashSHA1(unsigned char* pPlain, int nPlainLen, unsigned char* pHash);
void HashSHA256(unsigned char* pPlain, int nPlainLen, unsigned char* pHash);
void HashSHA512(unsigned char* pPlain, int nPlainLen, unsigned char* pHash);
void HashSHA224(unsigned char* pPlain, int nPlainLen, unsigned char* pHash);
void HashSHA384(unsigned char* pPlain, int nPlainLen, unsigned char* pHash);
void HashCrypt(unsigned char* pPlain, int nPlainLen, unsigned char* pHash);
#endif
