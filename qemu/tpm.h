#include <stdint.h>
#include "tlcl.h"
#include "utility.h"
#include "vboot_api.h"

VbError_t VbExTpmSendReceive(const uint8_t* request, uint32_t request_length,
                             uint8_t* response, uint32_t* response_length);
VbError_t VbExTpmInit(void);
VbError_t VbExTpmClose(void);
