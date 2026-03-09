#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

// Адрес MPU6050 на шине I2C
#define MPU6050_ADDR 0x68

// Регистры MPU6050
#define PWR_MGMT_1   0x6B
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H  0x43

#define I2C_PORT i2c0
#define PIN_SDA 4
#define PIN_SCL 5

// Функция для чтения 2 байт из регистра
int16_t read_word(uint8_t reg) {
    uint8_t buffer[2];
    
    // Указываем регистр для чтения
    i2c_write_blocking(i2c_default, MPU6050_ADDR, &reg, 1, true);
    
    // Читаем 2 байта
    i2c_read_blocking(i2c_default, MPU6050_ADDR, buffer, 2, false);
    
    // Преобразуем в 16-битное значение (Big Endian -> Little Endian)
    return (int16_t)((buffer[0] << 8) | buffer[1]);
}

int main() {
    stdio_init_all();
    
    i2c_init(I2C_PORT, 500 * 1000); // 100 кГц
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);
    
    // Небольшая задержка для стабилизации
    sleep_ms(100);
    
    // Пробуждаем MPU6050 (записываем 0 в регистр PWR_MGMT_1)
    uint8_t config[2] = {PWR_MGMT_1, 0x00};
    i2c_write_blocking(i2c_default, MPU6050_ADDR, config, 2, false);
    
    printf("\nMPU6050 Reader Started!\n");
    printf("======================\n\n");
    
    while (1) {
        // Читаем акселерометр
        int16_t ax = read_word(ACCEL_XOUT_H);
        int16_t ay = read_word(ACCEL_XOUT_H + 2);
        int16_t az = read_word(ACCEL_XOUT_H + 4);
        
        // Читаем гироскоп
        int16_t gx = read_word(GYRO_XOUT_H);
        int16_t gy = read_word(GYRO_XOUT_H + 2);
        int16_t gz = read_word(GYRO_XOUT_H + 4);
        
        // Выводим данные
        printf("ACCEL: X=%6d Y=%6d Z=%6d  |  GYRO: X=%6d Y=%6d Z=%6d\n", 
               ax, ay, az, gx, gy, gz);
        
        sleep_ms(100); // Задержка 100 мс
    }
    
    return 0;
}