#ifndef __I2C_H__
#define __I2C_H__

void I2C_init();

void I2C_push(unsigned char b);

void I2C_cat(const unsigned char* d, unsigned len);

void I2C_conset_start();

void I2C_block();

#endif // __I2C_H__
