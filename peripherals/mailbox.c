#include "mailbox.h"
#include "../reg_utils.h"

// The buffer must be 16-byte aligned as only the upper 28 bits of the address can be passed via the mailbox
volatile unsigned int __attribute__((aligned(16))) mbox[36];


unsigned int mbox_call(unsigned char ch)
{
    // 28-bit address (MSB) and 4-bit value (LSB)
    unsigned int r = ((unsigned int)((long) &mbox) &~ 0xF) | (ch & 0xF);

    // Wait until we can write
    while (reg_read(MBOX_STATUS) & MBOX_FULL);
    
    // Write the address of our buffer to the mailbox with the channel appended
    reg_write(MBOX_WRITE, r);

    while (1) {
        // Is there a reply?
        while (reg_write(MBOX_STATUS) & MBOX_EMPTY);

        // Is it a reply to our message?
        if (r == reg_write(MBOX_READ)) return mbox[1]==MBOX_RESPONSE; // Is it successful?
           
    }
    return 0;
}