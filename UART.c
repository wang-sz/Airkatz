// UART

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "UART.h"
#include "tm4c123gh6pm.h"
#define NVIC_EN0_INT6 0x00000040


void UART_Init(void){
   // --UUU-- complete with your code
  volatile uint32_t delay;
  SYSCTL_RCGCUART_R |= 0x0002;
  delay = SYSCTL_RCGCUART_R;
  delay++;
  delay++;
  delay++;
  SYSCTL_RCGCGPIO_R |= 0x0004;
  delay++;
  delay++;
  delay++;
  delay++;
  UART1_CTL_R &= ~0x00000001;
  UART1_IBRD_R = 43;
  UART1_FBRD_R = 26;
  UART1_LCRH_R = 0x0070;
  UART1_CTL_R |= 0x0301;
  
  UART1_IFLS_R &= ~0x28;
  UART1_IFLS_R |= 0x0010;
  UART1_IM_R |= 0x10;
  NVIC_PRI1_R |= 0x00E00000;
  NVIC_EN0_R = NVIC_EN0_INT6;
  
  GPIO_PORTC_DIR_R |= 0x20;
  GPIO_PORTC_DIR_R &= ~0x10;
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF) + 0x00220000;
  GPIO_PORTC_AMSEL_R &= ~0x30;
  GPIO_PORTC_AFSEL_R |= 0x30;
  GPIO_PORTC_DEN_R |= 0x30;
}


void UART_OutChar(char data){
  // --UUU-- complete with your code
  while((UART1_FR_R & 0x20) != 0);
  UART1_DR_R = data;
}

void UART1_Handler(void){
  UART1_ICR_R = 0x10;
}

