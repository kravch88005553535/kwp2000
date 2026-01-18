#include <stdio.h>

#include "kwp2000.h"
#include "interfaces/usart/usart_stm32f103.h"
#include "peripherals/rcc/rcc_stm32f103.h"
#include "peripherals/dma/dma_stm32f10x.h"
#include "peripherals/gpio/gpio_stm32f103.h"
#include "peripherals/program_timer/program_timer.h"

int main (void)
{
  
  Rcc rcc(Rcc::System_clock_source_pll_clock, Rcc::Hse_frequency_8Mhz);

  Pin led_pin(GPIOC, 13, Pin::mode_out_pullup);

  KWP2000* mp_kwp2000 = new KWP2000(*(new Usart(USART1, Usart::Interface_UART, Usart::WordLength_8bits, Usart::StopBits_1,
                       Usart::ParityControl_disabled, Usart::Baudrate_10400KBaud, rcc.GetPeripheralClock(USART1))), true);
  
  while(1)
  {
    mp_kwp2000->Execute();
  }
}
