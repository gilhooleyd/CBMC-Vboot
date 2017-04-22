#include <stdint.h>
#include "tlcl.h"
#include "utility.h"
#include "vboot_api.h"
#include "tpm.h"

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
extern uint32_t fifo_state;
extern uint32_t fifo_in_amt;
extern uint32_t fifo_in_cmdsize;


uint32_t read8(uint32_t addr) {
    printf("DEBUG: Reading: %d \n", addr);
    return tpm_update(1, addr, 0); 
}

void write8(uint8_t data, uint32_t addr) {
    printf("DEBUG: Writing: %d %d \n", data, addr);
   tpm_update(2, addr, data); 
}

/*
 * Sees if there is data available at a particular locality.
 * Returns 1 if data is available, 0 otherwise.
 * Valid function for localities 0-5
 */
int is_data_aval(int locality) {
    int status;

    // Input error checking 
    if (locality < 0 || locality > 5) {
//        terminal_writestring("Error: Wrong locality\n");
        return -1;
    }

    // Read and parse status data
    status = read8(STS(locality));
    if ((status & (STS_DATA_AVAIL | STS_VALID))
            == (STS_DATA_AVAIL | STS_VALID))
        return 1;
    else
        return 0;
}

int readBurstCount() {
    return 256;
}

void delay() {
}

/*
 * Sends len number of bytes over to the TPM.
 * Returns the number of bytes it sent succcessfully
 */
uint32_t send(const unsigned char *buf, uint32_t len)
{
    int status, burstcnt = 0;
    uint32_t count = 0;

    // tell the TPM that we will be writing a command now
    write8(STS_COMMAND_READY, STS(locality));

    // loop to write command
    while (count < len - 1) {
        // get the burst count
        burstcnt = readBurstCount();
        printf("DEBUG: Burstcount %d", burstcnt);

        if (burstcnt == 0){
            delay(); /* wait for FIFO to drain */
        } else {
            for (; burstcnt > 0 && count < len - 1; burstcnt--) {
                write8(buf[count],
                        DATA_FIFO(locality));
                count++;
            }

            // wait while we are not in a valid state (overflow)
            while ( ( (status = read8(STS(locality)))
                        & STS_VALID) == 0) {
                printf("DEBUG: IN 126 LOOP");
            }

            // break if the TPM is not expecting more data
            if ((status & STS_DATA_EXPECT) == 0) {
                return -1;
            }
        }
    }

    /* write last byte */
    write8(buf[count], DATA_FIFO(locality));

    // wait until valid state
    while ( ( (status = read8(STS(locality)))
                & STS_VALID) == 0) {}
    // check that no more data is expected
    if ((status & STS_DATA_EXPECT) != 0) {
//        terminal_writestring("Expecting too much data\n");
        return -1;
    }
    assert(len == fifo_in_amt);
    assert(fifo_in_amt == fifo_in_cmdsize);
    // Tell the TPM to execute the command
    write8(STS_GO, STS(locality));

    return len;
}
 
/*
 * Receive helper function
 * It will copy received data into buf.
 * It *attempts* to read count bytes but it
 * will return early if the TPM has no data
 * Returns: number of bytes received
 */
int recv_helper(unsigned char *buf, int count) {
     int size = 0, burstcnt = 0; 
 
     while (is_data_aval(locality)
             && size < count) {
         // Get the burst count (amount we can read at once)
         if (burstcnt == 0){
             burstcnt = readBurstCount();
         }
 
         // if burst count is zero then there is no data to read
         if (burstcnt == 0) {
             delay(); 
         }
         // otherwise read the data
         else {
             for (; burstcnt > 0 && size < count; burstcnt--) {
                 buf[size] = read8(DATA_FIFO(locality));
                 size++;
             }
         }
     }
     return size;
 }
 
/*
 * Recieve takes a buffer of size count.
 * It receives one command return from the TPM,
 * as long as command_size < count
 */
int recv(unsigned char *buf, int count)
 {
     int command_size;
     int size = 0;
 
     if (count < 6)
         return -1;
//    terminal_writestring("Receiving ");
 
     // Check that data is available
     if (!is_data_aval(locality)) {
//         terminal_writestring("No data aval at beginning\n");
         return -2;
     }
 
     // Read first 6 bytes
     // (All commands are larger than 6 bytes and
     //   these bytes include the command size 
     //   and the tpm tag)
     if ((size = recv_helper(buf, 6)) < 6) {
//         terminal_writestring("First 6 bytes Fail\n");
         return -1;
     }

     // Get the command size
     command_size = (*(buf + 5));
     if (command_size > count) {
//         terminal_writestring("Command Size too large \n");
         return -1;
     }
 
     /* read all data, except last byte */
     if ((size += recv_helper(&buf[6], command_size - 6 - 1))
             < command_size - 1) {
//         terminal_writestring("Reading all data failed\n");
         return -1;
     }
 
     /* check for receive underflow */
     if (!is_data_aval(locality)){ 
//         terminal_writestring("Underflow\n");
         return -1;
     }

     /* read last byte */
     if ((size += recv_helper(&buf[size], 1)) != command_size) {
//         terminal_writestring("Last byte failed\n");
         return -1;
     }

     // Make sure no data is left
     if (is_data_aval(locality)) {
//         terminal_writestring("Everything failed\n");
         return -1;
     }
 
     write8(STS_COMMAND_READY, STS(locality));
 
     return 0;
 }

/*
 * Sends and recieves data from the TPM
 * Request is a buffer of request_length that sends all bytes to the TPM
 * Response is a buffer of reponse_length that receives AT MOST those
 * bytes (resonse_length is often greater than bytes recieved)
 */
VbError_t VbExTpmSendReceive(const uint8_t* request, uint32_t request_length,
                             uint8_t* response, uint32_t* response_length) {

    uint32_t ret;
    // send the request to the TPM, check that all bytes send 
    ret = send(request, request_length);
    if (ret != request_length) {
//        terminal_writestring("Send Failed :(\n");
        return -1;
    } else {
        // TPM Fifo in correct state
        assert(fifo_state == FIFO_SENDING);
    }

    // get a response from the TPM, check that recv passed
    ret = recv(response, *response_length);
    if (ret != 0) {
        return -1;
    }
    else {
        // TPM Data receive assertion
        assert(is_data_aval(locality) == 0);
    }
    return 0;
}

VbError_t VbExTpmInit(void) {
    return VBERROR_SUCCESS;
}

VbError_t VbExTpmClose(void) {
    return VBERROR_SUCCESS;
}
