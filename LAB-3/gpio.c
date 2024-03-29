// gpio.c

#include "tm4c1294ncpdt.h"
#include <stdint.h>

#define GPIO_PORTA 0x0001
#define GPIO_PORTH 0x0080
#define GPIO_PORTJ 0x0100
#define GPIO_PORTK 0x0200
#define GPIO_PORTL 0x0400
#define GPIO_PORTM 0x0800
#define GPIO_PORTN 0x1000
#define GPIO_PORTP 0x2000
#define GPIO_PORTQ 0x4000

#define NVIC 0xE000E000
#define NVIC_OFFSET_EN 0x104
#define NVIC_OFFSET_PRI 0x430

#define GPIO_PORTJ_INTERRUPT_NUMBER 0x00080000
#define GPIO_PORTJ_PRI_LEVEL 0xE0000000

// -------------------------------------------------------------------------------
void GPIO_Init() {
  uint32_t GPIO_PORTS = GPIO_PORTA;
  GPIO_PORTS |= GPIO_PORTH;
  GPIO_PORTS |= GPIO_PORTJ;
  GPIO_PORTS |= GPIO_PORTK;
  GPIO_PORTS |= GPIO_PORTL;
  GPIO_PORTS |= GPIO_PORTM;
  GPIO_PORTS |= GPIO_PORTN;
  GPIO_PORTS |= GPIO_PORTP;
  GPIO_PORTS |= GPIO_PORTQ;

  // 1a. Ativar o clock para a porta setando o bit correspondente no registrador
  // RCGCGPIO
  SYSCTL_RCGCGPIO_R |= GPIO_PORTS;
  // 1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while ((SYSCTL_PRGPIO_R & (GPIO_PORTS)) != (GPIO_PORTS)) {
  };

  // 2. Limpar o AMSEL para desabilitar a anal�gica
  GPIO_PORTA_AHB_AMSEL_R = 0x00;
  GPIO_PORTH_AHB_AMSEL_R = 0x00;
  GPIO_PORTJ_AHB_AMSEL_R = 0x00;
  GPIO_PORTK_AMSEL_R = 0x00;
  GPIO_PORTL_AMSEL_R = 0x00;
  GPIO_PORTM_AMSEL_R = 0x00;
  GPIO_PORTN_AMSEL_R = 0x00;
  GPIO_PORTP_AMSEL_R = 0x00;
  GPIO_PORTQ_AMSEL_R = 0x00;

  // 3. Limpar PCTL para selecionar o GPIO
  GPIO_PORTA_AHB_PCTL_R = 0x00;
  GPIO_PORTH_AHB_PCTL_R = 0x00;
  GPIO_PORTJ_AHB_PCTL_R = 0x00;
  GPIO_PORTK_PCTL_R = 0x00;
  GPIO_PORTL_PCTL_R = 0x00;
  GPIO_PORTM_PCTL_R = 0x00;
  GPIO_PORTN_PCTL_R = 0x00;
  GPIO_PORTP_PCTL_R = 0x00;
  GPIO_PORTQ_PCTL_R = 0x00;

  // 4. DIR para 0 se for entrada, 1 se for saída
  GPIO_PORTA_AHB_DIR_R = 0xF0;
  GPIO_PORTH_AHB_DIR_R = 0x0F;
  GPIO_PORTJ_AHB_DIR_R = 0x00;
  GPIO_PORTK_DIR_R = 0xFF;
  GPIO_PORTL_DIR_R = 0x00;
  GPIO_PORTM_DIR_R = 0xFF;
  GPIO_PORTN_DIR_R = 0x03; // BIT0 | BIT1
  GPIO_PORTP_DIR_R = 0x20;
  GPIO_PORTQ_DIR_R = 0x0F;

  // 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa
  GPIO_PORTA_AHB_AFSEL_R = 0x00;
  GPIO_PORTH_AHB_AFSEL_R = 0x00;
  GPIO_PORTJ_AHB_AFSEL_R = 0x00;
  GPIO_PORTK_AFSEL_R = 0x00;
  GPIO_PORTL_AFSEL_R = 0x00;
  GPIO_PORTM_AFSEL_R = 0x00;
  GPIO_PORTN_AFSEL_R = 0x00;
  GPIO_PORTP_AFSEL_R = 0x00;
  GPIO_PORTQ_AFSEL_R = 0x00;

  // 6. Setar os bits de DEN para habilitar I/O digital
  GPIO_PORTA_AHB_DEN_R = 0xF0;
  GPIO_PORTH_AHB_DEN_R = 0x0F;
  GPIO_PORTJ_AHB_DEN_R = 0x03; // Bit0 e bit1
  GPIO_PORTK_DEN_R = 0xFF;
  GPIO_PORTL_DEN_R = 0x0F;
  GPIO_PORTM_DEN_R = 0xF7;
  GPIO_PORTN_DEN_R = 0x03; // Bit0 e bit1
  GPIO_PORTP_DEN_R = 0x20;
  GPIO_PORTQ_DEN_R = 0x0F;

  // 7. Habilitar resistor de pull-up interno, setar PUR para 1
  GPIO_PORTJ_AHB_PUR_R = 0x03; // Bit0 e bit1
  GPIO_PORTL_PUR_R = 0xF;

  // 8. Definir a rotina para interrupção geral
  GPIO_PORTJ_AHB_IM_R = 0x0;
  GPIO_PORTJ_AHB_IS_R = 0x0;
  GPIO_PORTJ_AHB_IBE_R = 0x0;
  GPIO_PORTJ_AHB_IEV_R = 0x0001;
  GPIO_PORTJ_AHB_ICR_R = 0x0001;
  GPIO_PORTJ_AHB_IM_R = 0x0001;

  NVIC_EN1_R = GPIO_PORTJ_INTERRUPT_NUMBER;
  NVIC_PRI12_R = GPIO_PORTJ_PRI_LEVEL;
}

// Liga transistor dos LEDs
void LedEnable() {
  uint32_t portReading;
  portReading = GPIO_PORTP_DATA_R;

  // Seta bit 5 do Port P
  GPIO_PORTP_DATA_R = portReading | 0x20;
}

// Coloca o valor nos LEDs da PAT
void LedOutput(uint32_t leds) {
  uint32_t ledsPortA = leds & 0xF0;
  uint32_t ledsPortQ = leds & 0x0F;

  // Lê portas e aplica máscara para escrita
  uint32_t portAMasked = GPIO_PORTA_AHB_DATA_R & 0x0F;
  uint32_t portQMasked = GPIO_PORTQ_DATA_R & 0xF0;

  GPIO_PORTA_AHB_DATA_R = portAMasked | ledsPortA;
  GPIO_PORTQ_DATA_R = portQMasked | ledsPortQ;
}

void ClearInterrupt() { GPIO_PORTJ_AHB_ICR_R = 0x0001; }

uint32_t PortJ_Input(void) { return GPIO_PORTJ_AHB_DATA_R; }

void PortN_Output(uint32_t valor) {
  uint32_t temp;
  // vamos zerar somente os bits menos significativos
  // para uma escrita amig�vel nos bits 0 e 1
  temp = GPIO_PORTN_DATA_R & 0xFC;
  // agora vamos fazer o OR com o valor recebido na fun��o
  temp = temp | valor;
  GPIO_PORTN_DATA_R = temp;
}

void PortH_Output(uint32_t valor) {
  uint32_t temp;
  // vamos zerar somente os bits menos significativos
  // para uma escrita amig?vel nos bits 0 e 1
  temp = GPIO_PORTH_AHB_DATA_R & 0xF0;
  // agora vamos fazer o OR com o valor recebido na fun??o
  temp = temp | valor;
  GPIO_PORTH_AHB_DATA_R = temp;
}
