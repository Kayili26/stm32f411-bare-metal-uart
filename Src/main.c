#include <stdint.h>

#define REG32(addr) (*(volatile uint32_t *)(addr))

#define RCC_AHB1ENR REG32(0x40023830) /* RCC — Reset and Clock Control */

#define GPIOA_MODER REG32(0x40020000) /* mode register */
#define GPIOA_ODR REG32(0x40020014) /* output data register */

int main(void)
{

RCC_AHB1ENR |= (1u << 0); /* 1. Enable the clock to GPIOA.*/

GPIOA_MODER &= ~(3u << 10); /* 2. Making PA5 an output. */
GPIOA_MODER |= (1u << 10);

while (1) /* 3. Blink forever. ODR bit 5 drives PA5. */
{
GPIOA_ODR ^= (1u << 5); /* toggle LED */
for (volatile uint32_t i = 0; i < 400000; i++);
}
}
