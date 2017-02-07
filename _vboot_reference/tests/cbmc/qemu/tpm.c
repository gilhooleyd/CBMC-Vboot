#include <stdint.h>
#include "tlcl.h"
#include "utility.h"
#include "vboot_api.h"

/* macros to access registers at locality ''l'' */
#define ACCESS(l)                       (0x0000 | ((l) << 12))
#define STS(l)                          (0x0018 | ((l) << 12))
#define DATA_FIFO(l)                    (0x0024 | ((l) << 12))
#define DID_VID(l)                      (0x0F00 | ((l) << 12))
/* access bits */
#define ACCESS_ACTIVE_LOCALITY          0x20 /* (R)*/
#define ACCESS_RELINQUISH_LOCALITY      0x20 /* (W) */
#define ACCESS_REQUEST_USE              0x02 /* (W) */
/* status bits */
#define STS_VALID                       0x80 /* (R) */
#define STS_COMMAND_READY               0x40 /* (R) */
#define STS_DATA_AVAIL                  0x10 /* (R) */
#define STS_DATA_EXPECT                 0x08 /* (R) */
#define STS_GO                          0x20 /* (W) */

int locality = 0;

uint8_t read8(uint32_t addr) {
   uint8_t *ptr = (uint8_t *) addr;
   return *ptr;
}

void write8(uint32_t addr, uint8_t data) {
   uint8_t *ptr = (uint8_t *) addr;
   *ptr = data;
}

uint32_t read32(uint32_t addr) {
   uint32_t *ptr = (uint32_t *) addr;
   return *ptr;
}

void write32(uint32_t addr, uint32_t data) {
    uint32_t * ptr = (uint32_t *) addr;
    *ptr = data;
}

 int request_locality(int l)
 {
     write8(ACCESS_RELINQUISH_LOCALITY, ACCESS(locality));
 
     write8(ACCESS_REQUEST_USE, ACCESS(l));
     /* wait for locality to be granted */
     if (read8(ACCESS(l) & ACCESS_ACTIVE_LOCALITY))
         return locality = l;
 
     return -1;
 }
// int send(unsigned char *buf, int len)
// {
//     int status, burstcnt = 0;
//     int count = 0;
// 
//     if (request_locality(locality) == -1)
//         return -1;
//     write8(STS_COMMAND_READY, STS(locality));
// 
//     while (count < len - 1) {
//         burstcnt = read8(STS(locality) + 1);
//         burstcnt += read8(STS(locality) + 2) << 8;
// 
//         if (burstcnt == 0){
//             delay(); /* wait for FIFO to drain */
//         } else {
//             for (; burstcnt > 0 && count < len - 1;
//                     burstcnt—) {
//                 write8(buf[count],
//                         DATA_FIFO(locality));
//                 count++;
//             }
// 
//             /* check for overflow */
//             for (status = 0; (status & STS_VALID)
//                     == 0; )
//                 status = read8(STS(locality));
//             if ((status & STS_DATA_EXPECT) == 0)
//                 return -1;
//         }
//     }
// 
//     /* write last byte */
//     write8(buf[count], DATA_FIFO(locality));
// 
//     /* make sure it stuck */
//     for (status = 0; (status & STS_VALID) == 0; )
//         status = read8(STS(locality));
//     if ((status & STS_DATA_EXPECT) != 0)
//         return -1;
// 
//     /* go and do it */
//     write8(STS_GO, STS(locality));
// 
//     return len;
// }
// 
// int recv_data(unsigned char *buf, int count)
// {
// 
//     int size = 0, burstcnt = 0, status;
// 
//     status = read8(STS(locality));
//     while ((status & (STS_DATA_AVAIL | STS_VALID))
//             == (STS_DATA_AVAIL | STS_VALID)
//             && size < count) {
//         if (burstcnt == 0){
//             burstcnt = read8(STS(locality) + 1);
//             burstcnt += read8(STS(locality) + 2) << 8;
//         }
// 
//         if (burstcnt == 0) {
//             delay(); /* wait for the FIFO to fill */
//         } else {
//             for (; burstcnt > 0 && size < count;
//                     burstcnt—) {
//                 buf[size] = read8(DATA_FIFO
//                         (locality));
//                 size++;
//             }
//         }
//         status = read8(STS(locality));
//     }
// 
//     return size;
// }
// 
// int recv(unsigned char *buf, int count)
// {
//     int expected, status;
//     int size = 0;
// 
//     if (count < 6)
//         return 0;
// 
//     /* ensure that there is data available */
//     status = read8(STS(locality));
//     if ((status & (STS_DATA_AVAIL | STS_VALID))
//             != (STS_DATA_AVAIL | STS_VALID))
//         return 0;
// 
//     /* read first 6 bytes, including tag and paramsize */
//     if ((size = recv_data(buf, 6)) < 6)
//         return -1;
//     expected = be32_to_cpu(*(unsigned *) (buf + 2));
// 
//     if (expected > count)
//         return -1;
// 
//     /* read all data, except last byte */
//     if ((size += recv_data(&buf[6], expected - 6 - 1))
//             < expected - 1)
//         return -1;
// 
//     /* check for receive underflow */
//     status = read8(STS(locality));
//     if ((status & (STS_DATA_AVAIL | STS_VALID))
//             != (STS_DATA_AVAIL | STS_VALID))
//         return -1;
// 
//     /* read last byte */
//     if ((size += recv_data(&buf[size], 1)) != expected)
//         return -1;
//     /* make sure we read everything */
//     status = read8(STS(locality));
//     if ((status & (STS_DATA_AVAIL | STS_VALID))
//             == (STS_DATA_AVAIL | STS_VALID)) {
//         return -1;
//     }
// 
//     write8(STS_COMMAND_READY, STS(locality));
// 
//     return expected;
// }

VbError_t VbExTpmSendReceive(const uint8_t* request, uint32_t request_length,
                             uint8_t* response, uint32_t* response_length) {

    return 0;
}

VbError_t VbExTpmInit(void) {
    unsigned vendor;
    int i;

    for (i = 0 ; i < 5 ; i++)
        write8(ACCESS_RELINQUISH_LOCALITY, ACCESS(i));
    if (request_locality(0) < 0)
        return 0;

    vendor = read32(DID_VID(0));
    if ((vendor & 0xFFFF) == 0xFFFF)
        return 0;

    return 1;
}

VbError_t VbExTpmClose(void) {
    return VBERROR_SUCCESS;
}
