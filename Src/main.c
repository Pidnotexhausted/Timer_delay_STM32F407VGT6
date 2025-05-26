#include <stdint.h>
#include <stm32f4xx.h>
#include "Delay.h"

void SysClockConfig(void){
	/*
	 * Step followed
	 * 1. Enabel HSE and wait HSE become ready
	 * 2. Set the power enable clock and voltage regulator
	 * 3. Config the Flash prefetch and LATENCY Related Settings
	 * 4. Config the prescaler HCLK, PCKL1, PCKL2
	 * 5. Config the main PLL
	 * 6. Enable PLL and wait for it become ready
	 * 7. Select the clock source and wait for it to be set
	 */

	// select HSE (8MHz in system_stm32f4xx.c and in schematics), expected systemclock is 64Mhz, select PLL (M=25, N=128, 	P=2)


	// 1. Enabel HSE and wait HSE become ready
	RCC->CR |= RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY)){}

	// 2. Set the power enable clock and voltage regulator
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	//because I just want to run at 64MHz, I choose scale 2 mode (<144MHz) to save energy
	PWR->CR &= ~(1 << 14);

	// 3. Config the Flash prefetch and LATENCY Related Settings
	FLASH->ACR = FLASH_ACR_LATENCY_2WS | FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN;

	// 4. Config the prescaler HCLK, PCKL1, PCKL2
	RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1; //SET AHB PRESCALER
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; //SET APB1 PRESCALER
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; //SET APB2 PRESCALER

	//5. Config the main PLL
	RCC->PLLCFGR = (RCC_PLLCFGR_PLLSRC_HSE | (0 << 16) | (64 << 6) | (4 << 0));

	// 6. Enable PLL and wait for it become ready
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY)){}

	// 7. Select the clock source and wait for it to be set
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){}
}

void GPIO_Config(void){
	/*
	 * Step followed
	 * 1. Enable GPIO clock
	 * 2. Set the pin as output
	 * 3. Configure OUTPUT mode
	 */

	// 1. Enable GPIO clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	// 2. Set the pin as output
	GPIOD->MODER &= ~((0x3 << 30) | (0x3 << 28) | (0x3 << 26) | (0x3 << 24)); //CLEAR
	GPIOD->MODER |= ((0x1 << 30) | (0x1 << 28) | (0x1 << 26) | (0x1 << 24)); // SET OUPUT FOR PD12->15

	// 3. Configure OUTPUT mode
	GPIOD->OTYPER = 0; //MODE PUSH-PULL
	GPIOD->OSPEEDR = 0; //MODE LOW SPEED
	GPIOD->PUPDR = 0; //MODE No pull-up, pull-down
}


int main(void)
{
	SysClockConfig();
	GPIO_Config();
	TIM6_Config();

	while(1){
		GPIOD->BSRR |= (1<<12);
		Delay_ms(1000);
		GPIOD->BSRR |= (1<<13);
		Delay_ms(1000);
		GPIOD->BSRR |= (1<<14);
		Delay_ms(1000);
		GPIOD->BSRR |= (1<<15);
		Delay_ms(1000);
		GPIOD->BSRR |= (0b1111 << 12) << 16;
		Delay_ms(1000);
	}
}
