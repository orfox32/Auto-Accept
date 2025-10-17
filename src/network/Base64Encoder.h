#pragma once
#include <string>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

class Base64Encoder {
public:
    static inline std::string encode(const std::string& input) {
        BIO *bio, *b64;
        BUF_MEM *bufferPtr;

        b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        bio = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);

        BIO_write(bio, input.c_str(), input.length());
        BIO_flush(bio);

        BIO_get_mem_ptr(bio, &bufferPtr);
        BIO_set_close(bio, BIO_NOCLOSE);
        BIO_free_all(bio);

        return std::string(bufferPtr->data, bufferPtr->length);
    }
};
