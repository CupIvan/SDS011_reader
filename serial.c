#include <stdio.h>  // perror
#include <fcntl.h>  // open
#include <unistd.h> // read

#include "serial.h"

int close_serial_port(int fd)
{
	close(fd);
}

// Opens the specified serial port, sets it up for binary communication,
// configures its read timeouts, and sets its baud rate.
// Returns a non-negative file descriptor on success, or -1 on failure.
int open_serial_port(const char * device, speed_t baud_rate)
{
	int fd = open(device, O_RDWR | O_NOCTTY);
	if (fd == -1)
	{
		perror(device);
		return -1;
	}

	// Flush away any bytes previously read or written.
	int result = tcflush(fd, TCIOFLUSH);
	if (result)
	{
		perror("tcflush failed");  // just a warning, not a fatal error
	}

	// Get the current configuration of the serial port.
	struct termios options;
	result = tcgetattr(fd, &options);
	if (result)
	{
		perror("tcgetattr failed");
		close(fd);
		return -1;
	}

	// Turn off any options that might interfere with our ability to send and
	// receive raw binary bytes.
	options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
	options.c_oflag &= ~(ONLCR | OCRNL);
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

	// Set up timeouts: Calls to read() will return as soon as there is
	// at least one byte available or when 100 ms has passed.
	options.c_cc[VTIME] = 1;
	options.c_cc[VMIN] = 0;

	cfsetospeed(&options, baud_rate);
	cfsetispeed(&options, cfgetospeed(&options));

	result = tcsetattr(fd, TCSANOW, &options);
	if (result)
	{
		perror("tcsetattr failed");
		close(fd);
		return -1;
	}

	return fd;
}

// Writes bytes to the serial port, returning 0 on success and -1 on failure.
int write_serial_port(int fd, uint8_t * buffer, size_t size)
{
	ssize_t result = write(fd, buffer, size);
	if (result != (ssize_t)size)
	{
		perror("failed to write to port");
		return -1;
	}
	return 0;
}

// Reads bytes from the serial port.
// Returns after all the desired bytes have been read, or if there is a
// timeout or other error.
// Returns the number of bytes successfully read into the buffer, or -1 if
// there was an error reading.
ssize_t read_serial_port(int fd, uint8_t * buffer, size_t size)
{
	size_t received = 0;
	while (received < size)
	{
		ssize_t r = read(fd, buffer + received, size - received);
		if (r < 0)
		{
		perror("failed to read from port");
		return -1;
		}
		if (r == 0)
		{
			// Timeout
			usleep(300);
			continue;
		}
		received += r;
	}
	return received;
}
