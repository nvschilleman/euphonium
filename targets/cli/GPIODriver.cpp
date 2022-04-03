#include "GPIODriver.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


void gpioDigitalWrite(int pin, int value) {
    int fd = open("/sys/class/gpio/gpio24/value", O_WRONLY);
    if (fd == -1) {
        printf("error\n");
    }

    char char_val[2];
    if(value == 1){
        char_val[0] = '1';
    }
    else
    {
        char_val[0] = '0';
    }
    char_val[1] = '\0';

    if (write(fd, char_val, 1) != 1) {
        printf("error setting gpio\n");
    }
    close(fd);
}

int gpioDigitalRead(int pin) {
    return 0;
//    return gpio_get_level((gpio_num_t) pin);
}

// @TODO
int gpioAnalogRead(int pin) {
    return 0;
}

//helper function, check if exported
bool isExported(int pin){
    std::string gpio_path = "/sys/class/gpio/gpio" + std::to_string(pin);
    struct stat st;
    if (stat(gpio_path.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
    {
        return false;
    }
    return true;
}

void exportPIN(int pin){
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1) {
        printf("Unable to open /sys/class/gpio/export\n");
    }

    char char_pin[6];
    snprintf(char_pin, 6, "%d", pin);

    int char_pin_len = strlen(char_pin);
    if (write(fd, char_pin, char_pin_len) != char_pin_len) {
        printf("Error writing to /sys/class/gpio/export\n");
    }

    close(fd);
}

void unexport(int pin){
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1) {
        printf("Unable to open /sys/class/gpio/unexport\n");
    }

    char char_pin[6];
    snprintf(char_pin, 6, "%d", pin);

    int char_pin_len = strlen(char_pin);
    if (write(fd, char_pin, char_pin_len) != char_pin_len) {
        printf("Error writing to /sys/class/gpio/unexport\n");
    }

    close(fd);
}

#define PIN_DIR_IN 0
#define PIN_DIR_OUT 1

void set_dir(int pin, int dir){
    std::string gpio_path = "/sys/class/gpio/gpio" + std::to_string(pin) + "/direction";
    int fd = open(gpio_path.c_str(), O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/gpio24/direction");
    }

    char char_dir[4];

    if(dir == PIN_DIR_IN){
        strcpy(char_dir, "in");
    }
    else
    {
        strcpy(char_dir, "out");
    }

    int char_dir_len = strlen(char_dir);
    if (write(fd, char_dir, char_dir_len) != char_dir_len) {
        printf("Error writing to %s\n", gpio_path.c_str());
    }

    close(fd);
}

void gpioPinMode(int pin, int mode) {
    int selectedMode = 0;

    if(isExported(pin)){
        unexport(pin);
    }
    exportPIN(pin);

// todo: more modes
    switch (mode) {
    case 1:
//        gpio_set_direction((gpio_num_t) pin, GPIO_MODE_INPUT);
        break;
    case 2:
//        gpio_set_direction((gpio_num_t) pin, GPIO_MODE_INPUT);
//        gpio_set_pull_mode((gpio_num_t) pin, GPIO_PULLUP_ONLY);
        break;
    case 3:
//        gpio_set_direction((gpio_num_t) pin, GPIO_MODE_INPUT);
//        gpio_set_pull_mode((gpio_num_t) pin, GPIO_PULLDOWN_ONLY);
        break;
    case 4:
        set_dir(pin, PIN_DIR_OUT);
        break;
    }
}

void exportGPIODriver(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("digital_write", &gpioDigitalWrite, "gpio");
    berry->export_function("digital_read", &gpioDigitalRead, "gpio");
    berry->export_function("pin_mode", &gpioPinMode, "gpio");
    berry->export_function("analog_read", &gpioAnalogRead, "gpio");
}
