#include <stdint.h>  // uint8_t
#include <stdio.h>
#include "serial.h"

// Laser PM2.5 Sensor
// Product model: SDS011, version: V1.3
// Datasheet: https://nettigo.pl/attachments/398

typedef struct
{
	uint8_t head;    // 0xAA Message header
	uint8_t no;      // Commander No.
	uint8_t pm25_lo; // PM2.5 Low byte
	uint8_t pm25_hi; // PM2.5 High byte
	uint8_t pm10_lo; // PM10 Low byte
	uint8_t pm10_hi; // PM10 High byte
	uint8_t id1;     // ID byte 1
	uint8_t id2;     // ID byte 2
	uint8_t crc;     // Check-sum
	uint8_t tail;    // 0xAB Message tail
} Tdata;

int main()
{
	Tdata data;
	unsigned char* ptr = (char*)&data;

	int fd = open_serial_port("/dev/ttyUSB0", B9600);
	if (fd < 0) { return 1; }

	printf("ID PM2.5 PM10\n");

	int n;

	while (1)
	{
		n = read_serial_port(fd, (uint8_t*)&data, sizeof(data));
//		int i; for (i=0; i<n; i++) printf("%.2x ", ptr[i]);
		if (n == sizeof(data))
		if (data.head == 0xAA)
		if (data.tail == 0xAB)
		if (data.no   == 0xC0)
			printf("%.4x %.1f %.1f\n",
				(int)data.id1 * 256 + data.id2,
				((float)data.pm25_hi*256 + data.pm25_lo)/10,
				((float)data.pm10_hi*256 + data.pm10_lo)/10
			);
	}

	close_serial_port(fd);
	return 0;
}
