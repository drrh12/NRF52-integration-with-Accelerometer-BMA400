#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_twi.h"
#include <stdio.h>

#define ACCELEROMETER_I2C_ADDRESS 0x15 // described on the sensor
#define ACC_X_LSB 0x04
#define ACC_Y_LSB 0x00
#define ACC_Z_LSB 0x00
#define ACC_MSB_DATA_X 
#define ACC_MSB_DATA_Y
#define ACC_MSB_DATA_Z 

#define SCL_PIN   26
#define SDA_PIN   27

#define TWI_INSTANCE_ID     0
static const nrf_drv_twi_t twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

void twi_init(void) {
    ret_code_t err_code;
    const nrf_drv_twi_config_t twi_config = {
        .scl                = SCL_PIN,
        .sda                = SDA_PIN,
        .frequency          = NRF_DRV_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&twi);
}

void read_accel_data(int16_t* x, int16_t* y, int16_t* z) {
    uint8_t accel_data[6];
    uint8_t reg_address_x = ACC_X_LSB;
    uint8_t reg_address_y = ACC_Y_LSB;
    uint8_t reg_address_z = ACC_Z_LSB;

    ret_code_t err_code;

    // Read X-axis data
    err_code = nrf_drv_twi_tx(&twi, ACCELEROMETER_I2C_ADDRESS, &reg_address_x, sizeof(reg_address_x), true);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_twi_rx(&twi, ACCELEROMETER_I2C_ADDRESS, &accel_data[0], sizeof(accel_data[0]));
    APP_ERROR_CHECK(err_code);

    // Read Y-axis data
    err_code = nrf_drv_twi_tx(&twi, ACCELEROMETER_I2C_ADDRESS, &reg_address_y, sizeof(reg_address_y), true);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_twi_rx(&twi, ACCELEROMETER_I2C_ADDRESS, &accel_data[2], sizeof(accel_data[2]));
    APP_ERROR_CHECK(err_code);

    // Read Z-axis data
    err_code = nrf_drv_twi_tx(&twi, ACCELEROMETER_I2C_ADDRESS, &reg_address_z, sizeof(reg_address_z), true);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_twi_rx(&twi, ACCELEROMETER_I2C_ADDRESS, &accel_data[4], sizeof(accel_data[4]));
    APP_ERROR_CHECK(err_code);

    // Convert the raw data to 16-bit signed values
    *x = (int16_t)((accel_data[1] << 8) | accel_data[0]);
    *y = (int16_t)((accel_data[3] << 8) | accel_data[2]);
    *z = (int16_t)((accel_data[5] << 8) | accel_data[4]);
}

int main(void) {
    twi_init();

    int16_t accel_x, accel_y, accel_z;

    printf("Accelerometer example started:\r\n");

    while (1) {
        read_accel_data(&accel_x, &accel_y, &accel_z);

        printf("Accelerometer Data:\r\n");
        printf("X: %d\r\n", accel_x);
        printf("Y: %d\r\n", accel_y);
        printf("Z: %d\r\n", accel_z);
        printf("\r\n");

        nrf_delay_ms(50);
    }
}