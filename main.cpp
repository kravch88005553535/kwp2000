#include <stdio.h>

#include "kwp2000.h"
#include "interfaces/usart/usart_stm32f103.h"
#include "peripherals/rcc/rcc_stm32f103.h"
#include "peripherals/dma/dma_stm32f10x.h"
#include "peripherals/gpio/gpio_stm32f103.h"
#include "peripherals/program_timer/program_timer.h"

int main (void)
{
//  uint8_t array[10]{150, 46, 66, 189, 57, 255, 131, 204, 49, 92};
//  
//  uint8_t array2[10]{};
  
  Rcc rcc(Rcc::System_clock_source_pll_clock, Rcc::Hse_frequency_8Mhz);

  Pin k_line_tx_pin (GPIOA, 9,  Pin::mode_alternate_function_pushpull);
  Pin k_line_rx_pin (GPIOA, 10, Pin::mode_alternate_function_pushpull);
  Pin led_pin(GPIOC, 13, Pin::mode_out_pullup);
    
  KWP2000* mp_kwp2000 = new KWP2000(*(new Usart(USART1, Usart::Interface_UART, Usart::WordLength_8bits, Usart::StopBits_1,
                       Usart::ParityControl_disabled, Usart::Baudrate_10400KBaud, rcc.GetPeripheralClock(USART1))), true);

  while(1)
  {
    mp_kwp2000->Execute();
  }
}
