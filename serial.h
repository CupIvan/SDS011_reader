#include <termios.h> // speed_t
#include <stdint.h>  // uint8_t

int     open_serial_port(const char* device, speed_t baud_rate);
int    close_serial_port(int fd);
int    write_serial_port(int fd, uint8_t* buffer, size_t size);
ssize_t read_serial_port(int fd, uint8_t* buffer, size_t size);
