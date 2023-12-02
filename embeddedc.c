#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

uint8_t portA[4];

#define TRISA &portA[0]
#define LATA &portA[1]
#define PORTA &portA[2]
#define BSRA &portA[3]

struct bits_region {
	unsigned int bit0 : 1;
	unsigned int bit1 : 1;
	unsigned int bit2 : 1;
	unsigned int bit3 : 1;
	unsigned int bit4 : 1;
	unsigned int bit5 : 1;
	unsigned int bit6 : 1;
	unsigned int bit7 : 1;
}__attribute__((packed));

union reg {
	unsigned char data;
	struct bits_region bits;
};

typedef struct {
	union reg TRIS;
	union reg LAT;
	union reg PORT;
	union reg BSR;
}GPIO_t;

GPIO_t *GPIO_A = (GPIO_t *)portA;

int main(void)
{
	*TRISA = 255;
	printf("%d\n", GPIO_A->TRIS.data);

	*LATA = 0b01010101;
	printf("%d\n", GPIO_A->LAT.bits.bit0);
	printf("%d\n", GPIO_A->LAT.bits.bit7);
	printf("%d\n", GPIO_A->LAT.bits.bit6);

	while(1) {
		sleep(10);
	}
}
