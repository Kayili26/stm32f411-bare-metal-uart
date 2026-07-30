#include <stdint.h>
#define REG32(addr) (*(volatile uint32_t *)(addr))
/* RCC */
#define RCC_AHB1ENR REG32(0x40023830)
#define RCC_APB1ENR REG32(0x40023840)
/* GPIOA */
#define GPIOA_MODER REG32(0x40020000)
#define GPIOA_ODR REG32(0x40020014)
#define GPIOA_AFRL REG32(0x40020020)
/* USART2 */
#define USART2_SR REG32(0x40004400)
#define USART2_DR REG32(0x40004404)
#define USART2_BRR REG32(0x40004408)
#define USART2_CR1 REG32(0x4000440C)
/* Flag bits */
#define SR_TXE (1u << 7) /* transmit data register empty */
#define SR_RXNE (1u << 5) /* receive data register not empty */
static void uart2_init(void)
{
RCC_AHB1ENR |= (1u << 0); /* GPIOA clock */
RCC_APB1ENR |= (1u << 17); /* USART2 clock */

GPIOA_MODER &= ~((3u << 4) | (3u << 6));
GPIOA_MODER |= ((2u << 4) | (2u << 6));

GPIOA_AFRL &= ~((0xFu << 8) | (0xFu << 12));
GPIOA_AFRL |= ((7u << 8) | (7u << 12));
USART2_BRR = 0x8B; /* 115200 baud */

USART2_CR1 = (1u << 13) | (1u << 3) | (1u << 2);
}
static void uart2_putc(char c)
{
while (!(USART2_SR & SR_TXE)) { } /* wait until we can write */
USART2_DR = (uint8_t)c;
}
static void uart2_puts(const char *s)
{
while (*s) uart2_putc(*s++);
}
int main(void)
{

/* LED setup from step 1 which will be used in a shell */
RCC_AHB1ENR |= (1u << 0);
GPIOA_MODER &= ~(3u << 10);
GPIOA_MODER |= (1u << 10);
uart2_init();
while (1)
{
uart2_puts("Hello Arda\r\n");
for (volatile uint32_t i = 0; i < 2000000; i++);
}
}
