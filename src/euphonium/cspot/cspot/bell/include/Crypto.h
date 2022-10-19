#ifndef BELL_CRYPTO_H
#define BELL_CRYPTO_H

#include <vector>
#include <string>

#ifdef BELL_USE_MBEDTLS
#include "CryptoMbedTLS.h"
#define Crypto CryptoMbedTLS
#else
#include "CryptoOpenSSL.h"
#define Crypto CryptoOpenSSL
#endif
#endif