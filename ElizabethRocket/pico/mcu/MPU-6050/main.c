#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#define M_PI 3.14159265358979323846

#define MPU_ADDR 0x68
#define MPU_PWR  0x6B
#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG  0x1B
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H  0x43

#define I2C i2c0
#define SDA_PIN 4
#define SCL_PIN 5 

int16_t read_reg(uint8_t reg) {
    uint8_t buffer[2];
    i2c_write_blocking(I2C, MPU_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C, MPU_ADDR, buffer, 2, false);
    return (int16_t)((buffer[0] << 8) | buffer[1]);
}

void write_reg(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    i2c_write_blocking(I2C, MPU_ADDR, buf, 2, false);
}

void I2C_init() {
    i2c_init(I2C, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}

void MPU_init() {
    write_reg(MPU_PWR, 0x00);
    sleep_ms(100);
    
    write_reg(ACCEL_CONFIG, 0x00);  // Акселерометр: ±2g
    write_reg(GYRO_CONFIG, 0x00);   // Гироскоп: ±250 град/с
    sleep_ms(100);
}

int main() {
    stdio_init_all();
    I2C_init();
    MPU_init();
    
    while (1) {
        int16_t ax = read_reg(ACCEL_XOUT_H + 0);
        int16_t ay = read_reg(ACCEL_XOUT_H + 2);
        int16_t az = read_reg(ACCEL_XOUT_H + 4);
        
        int16_t gx = read_reg(GYRO_XOUT_H + 0);
        int16_t gy = read_reg(GYRO_XOUT_H + 2);
        int16_t gz = read_reg(GYRO_XOUT_H + 4);
        
        double ax_g = ax / 16384.0;
        double ay_g = ay / 16384.0;
        double az_g = az / 16384.0;
        
        double gx_ds = gx / 131.0;
        double gy_ds = gy / 131.0;
        double gz_ds = gz / 131.0;
        
        float roll  = atan2(ay_g, az_g) * 180 / M_PI;
        float pitch = atan2(-ax_g, sqrt(ay_g*ay_g + az_g*az_g)) * 180 / M_PI;
        
        printf("==================================\n");
        printf("Accel (g):   X=%6.2f Y=%6.2f Z=%6.2f\n", ax_g, ay_g, az_g);
        printf("Gyro (°/s):  X=%6.2f Y=%6.2f Z=%6.2f\n", gx_ds, gy_ds, gz_ds);
        printf("----------------------------------\n");
        printf("Углы наклона:\n");
        printf("Угол крена:  %6.1f°\n", roll);
        printf("Угол тангажа: %6.1f°\n", pitch);
        
        sleep_ms(1000);
    }
    
    return 0;
}