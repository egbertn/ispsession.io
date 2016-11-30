 //here come the messages
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: MSG_OUTOF_LICENSES
//
// MessageText:
//
// Nierop Webconsultancy: ISP Session, class %1 was not returned to the object pool. Please consider upgrading! Thanks! Performance will suffer if the object pool is too small. Upgrading can be done here http://www.nieropwebconsult.nl/asp_session_manager.htm?page=order
//
#define MSG_OUTOF_LICENSES               0x00000001L

//
// MessageId: MSG_ERROR
//
// MessageText:
//
// Nierop Webconsultancy: ISP Session, failure with error: %2
//
#define MSG_ERROR                        0x00000002L

