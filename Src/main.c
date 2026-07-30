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

static char uart2_getc(void)
{
while (!(USART2_SR & SR_RXNE)) { }
return (char)USART2_DR;
} /*This is the receiving byte code which is needed to write a tiny command shell */

static int streq(const char *a, const char *b)
{
while (*a && (*a == *b)) { a++; b++; }
return *a == *b;
} /* This is a minimal string comparer */

int main(void) {

	RCC_AHB1ENR |= (1u << 0);
	GPIOA_MODER &= ~(3u << 10);
	GPIOA_MODER |= (1u << 10);
	uart2_init();
	uart2_puts("\r\nF411 shell ready. Commands: led on | led off | status\r\n> ");

	char buf[32];
	uint32_t len = 0;

	while (1)
	{
		char c = uart2_getc();

		if (c == '\r' || c == '\n') /* if enter is pressed */
		{
			buf[len] = '\0';
			uart2_puts("r\n");

			if (streq(buf, "led on"))
			{
				GPIOA_ODR |= (1u << 5);
				uart2_puts("LED is ON\r\n");
			}
			else if (streq(buf, "led off"))
			{
			GPIOA_ODR &= ~(1u << 5);
			uart2_puts("LED is OFF\r\n");
			}
			else if (streq(buf, "status"))
			{
			uart2_puts((GPIOA_ODR & (1u << 5)) ? "LED: ON\r\n"
			: "LED: OFF\r\n");
			}
			else if (len > 0)
			{
			uart2_puts("unknown command\r\n");
			}
			len = 0;
			uart2_puts("> ");
		}

		else if (c == 0x7F || c == '\b') /* when pressing backspace */
		{
		if (len > 0) { len--; uart2_puts("\b \b"); }
		}
		else if (len < sizeof(buf) - 1)
		{
		buf[len++] = c;
		uart2_putc(c); /* echo so you see typing */
		}
	}
}
