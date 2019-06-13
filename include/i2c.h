#ifndef __I2C_H__
#define __I2C_H__

/*
 * I2C-Initialize
 */

void I2C_init();

/*
 * I2C-Push
 *
 * Pushes an arbitrary byte onto the 
 * data stream that's sent over the I2C state machine
 */

void I2C_push(unsigned char b);

/*
 * I2C-concatenate
 *
 * Take an arbitrary amount of bytes
 * and push it to the data stream
 */

void I2C_cat(const unsigned char* d, unsigned len);

/*
 * I2C-console-set-start
 *
 * Initializes the state machine
 * transfer process
 */

void I2C_conset_start();

/*
 * I2C-Block
 *
 * Waits until the entire transfer process
 * is finished. 
 */

void I2C_block();

#endif // __I2C_H__
