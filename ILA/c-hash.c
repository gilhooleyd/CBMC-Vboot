 #include <openssl/sha.h>
 #include <stdint.h>
 #include <stdio.h>

 unsigned char *SHA1(const unsigned char *d, unsigned long n,
                           unsigned char *md);

// compile with 'gcc c-hash.c -lcrypto'

// This main function shows that the sw-tpm function 
// acts the same way as the SHA1-Hash function.
//
// The PCR extend takes the concatenation of the original 
// PCR value and the value to be extended, then
// saves the SHA1-Hash as the output
int main(void) {
    uint8_t data[40];
    uint8_t out[20];
    int i;
    // The initial PCR value
    // TCG claims it can be either 0x00 or 0xFF
    // but we have seen it to be 0x00 in SWTPMLIBS
    for (i = 0; i < 20; i++) {
        data[i] = 0x00;
    }
    // The extend data
    // I extend it with bytes values starting at 0 
    // and counting upwards
    // (same as the ../tpm/test_tpm function)
    for (i = 20; i < 40; i++) {
        data[i] = i - 20;
    }

    // actually take the SHA1 Hash
    SHA1(data, 40, out);

    // We can see that the output matches
    for (i = 0; i < 20; i++) {
        printf("%x\n", out[i]);
    }

 }
