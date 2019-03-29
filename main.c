#include "stm32f4xx.h"

void gpioConfig(void);
void adcConfig(void);
void HSIclockset(void);
void pwmConfig(void);
void movdetect(void);
uint32_t adc2Out(void);
void delay(uint32_t time);

uint32_t value1;    // To read ADC1 
uint32_t value2;		// To read ADC2
uint32_t bright;
uint32_t state=250;

int main()
{
	HSIclockset();
	gpioConfig();
	adcConfig();
	pwmConfig();
	
	while(1)
	{
		ADC1->CR2=0x40000003;
		while((ADC1->SR & (1<<1)) ==0);
		if((ADC1->SR & 0x00000002) !=0)
		{ value1= ADC1->DR;
		}
		ADC1->SR=ADC1->SR & 0x00000000;
		
		if(value1>251)        // check night or day
		{
		movdetect();
		}
		
	}
}
void HSIclockset(void)
{
	RCC->CR &=0xFFFFFFFE;    // Enable the HSI oscillator
	RCC->CR |=0x00000001;
	RCC->CFGR &=0xFFFFFFFC;
	RCC->CFGR |=0x00000000;   // make HSI CLOCK THE SYS CLOCK
}
void adcConfig(void)
{ 
	RCC->APB2ENR |=( 1<<8 | 1<<9);  //ADC1 and ADC2 clock enable
	ADC1->CR1=0x02000000;      // 8bit resolution
	ADC1->CR2=0X00000003;
	ADC1->SMPR2=0x00000001;    //adc sampling 15 cycle  (PA0)
	
	ADC2->CR1=0x02000000;
	ADC2->CR2=0X00000003;
	ADC2->SMPR2 &=0xFFFFFF0F;
	ADC2->SMPR2 |=0x00000010;
	ADC2->SQR3 =0x00000001;     //1st conversion of CH1(PA1)
}
void pwmConfig(void)
{
	RCC->APB2ENR |=(1<<0);  // TIM1 clock Enable
	TIM1->ARR = 0x031F;
	TIM1->CCR1=0x0050;
	TIM1->CR1 &=0xFF0C;
	TIM1->CR1 |=0x0081;
	TIM1->EGR |=(1<<0);
	TIM1->CCMR1 &=0xFF84;
	TIM1->CCMR1 |=0x0068;
	TIM1->CCER= 0x0005;
	TIM1->BDTR |=0x8000;
	TIM1->PSC &=0x0000;
}
void gpioConfig(void)
{ 
	RCC->AHB1ENR |=(1<<0 | 1<<3 | 1<<4);  //clock enable for GPIO A, D and C
	
/* **************PWM PORT initialisation************* */
	
	GPIOE->MODER &=0xFFF3FFFF;     // PE9 pin
	GPIOE->MODER |=0x00080000;
	GPIOE->OTYPER =0x00000000;
	GPIOE->OSPEEDR &=0xFFF3FFFF;
	GPIOE->OSPEEDR |=0x00080000;
	GPIOE->PUPDR &=0x00000000;
	GPIOE->AFR[1] &=0xFFFFFF0F;
	GPIOE->AFR[1] |=0x00000010;  // AF1 for PWM
	
/* **************ADC PORT initialisation************* */	
	GPIOA->MODER |=(15<<0);     //PA0, PA1 as an analog
	GPIOA->OTYPER =0x00000000;
	GPIOA->OSPEEDR |=(1<<0 | 1<<2);
	GPIOA->PUPDR &=0xFFFFFFF0;
	
/* **************PIR PORT initialisation************* */
	GPIOD->MODER &=0xFFFFFFFC;  //PD0 as an input
	GPIOD->OTYPER =0x00000000;   
	GPIOD->OSPEEDR |=(1<<0);    // Speed medium
	GPIOD->PUPDR &=0xFFFFFFFD;  //pullup		
}
uint32_t adc2Out(void)
{
	  ADC2->CR2=0x40000003;
		while((ADC2->SR & (1<<1)) ==0);
		if((ADC2->SR & 0x00000002) !=0)
		{ value2= ADC2->DR;
		}
		ADC2->SR &=0x00000000;
		return(value2);
}
void movdetect(void)
{
	uint32_t cars = adc2Out();
		
			if(cars >=250)    // Cars are detected
			{
				for(bright=state; bright<=799;bright=bright+10)
				{
					TIM1->CCR1 = bright;
					delay(40);             // Brightness will increase gradually
				}
				state=798;
				if(bright>=798)
				{
					delay(5000);     // LED will glow upto 5s
				}
				
			}
		
			else if((GPIOD->IDR & 1<<0) !=0)   // Person detected
			{
				for(bright=state; bright<=799;bright=bright+10)
				{
					TIM1->CCR1 = bright;
					delay(40);
				}
				state=798;
				if(bright>=798)
				{
					delay(5000);
				}
				
			} 
	
			if(value2<250)   								// No movement
			{
				for(bright=state; bright>=250;bright=bright-10)
				{
					TIM1->CCR1 = bright;
					delay(40);
				}
				state=250;
			}
}
void delay(uint32_t time)
{
	for(uint32_t i=0;i<time*4000;i++);   // delay in miliseconds
}