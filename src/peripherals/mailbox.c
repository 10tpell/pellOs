#include "peripherals/mailbox.h"
#include "reg_utils.h"

// The buffer must be 16-byte aligned as only the upper 28 bits of the address can be passed via the mailbox
volatile unsigned int __attribute__((aligned(16))) mbox[36];


unsigned int mailbox_call(unsigned char ch)
{
    // 28-bit address (MSB) and 4-bit value (LSB)
    unsigned int r = ((unsigned int)((long) &mbox) &~ 0xF) | (ch & 0xF);

    // Wait until we can write
    while (reg_read((void*) MBOX_STATUS) & MBOX_FULL);
    
    // Write the address of our buffer to the mailbox with the channel appended
    reg_write((void*) MBOX_WRITE, r);

    while (1) {
        // Is there a reply?
        while (reg_read((void*) MBOX_STATUS) & MBOX_EMPTY);

        // Is it a reply to our message?
        if (r == reg_read((void*) MBOX_READ)) return mbox[1]==MBOX_RESPONSE; // Is it successful?
           
    }
    return 0;
}

// 0xffff0000fe00b898
// TODO THIS NEEDS TO BE MAPPED