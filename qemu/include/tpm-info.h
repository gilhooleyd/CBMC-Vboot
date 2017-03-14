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

#define ADDR FED40000
