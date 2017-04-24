#include <stdint.h>
#include <stdlib.h>

int main(void) {
    uint32_t req_size = nondet_int() % 256; 
    uint32_t resp_size = nondet_int() % 256; 

    uint8_t *request = malloc(req_size);
    uint8_t *response = malloc(resp_size);

    uint8_t *resp_len;

	uint8_t indigest[20];
	uint8_t outdigest[20];

    for (int i = 0; i < 20; i++)
        indigest[i] = nondet_int();

    tpm_init();
    VbExTpmSendReceive(request, req_size, response, resp_len);
//    TlclExtend(0, &indigest, &outdigest); 
}
