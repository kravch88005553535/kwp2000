#include <stdio.h>

#include "kwp2000.h"
#include "interfaces/usart/usart_stm32f103.h"
#include "peripherals/rcc/rcc_stm32f103.h"
#include "peripherals/dma/dma_stm32f10x.h"
#include "peripherals/gpio/gpio_stm32f103.h"
#include "peripherals/program_timer/program_timer.h"

int main (void)
{
  uint8_t array[10]{150, 46, 66, 189, 57, 255, 131, 204, 49, 92};
  
  uint8_t array2[10]{};
  
  Rcc rcc(Rcc::System_clock_source_pll_clock, Rcc::Hse_frequency_8Mhz);

  Pin k_line_tx_pin (GPIOA, 9,  Pin::mode_alternate_function_pushpull);
  Pin k_line_rx_pin (GPIOA, 10, Pin::mode_alternate_function_pushpull);
  Pin led_pin(GPIOC, 13, Pin::mode_out_pullup);
    
  KWP2000* mp_kwp2000 = new KWP2000(*(new Usart(USART1, Usart::Interface_UART, Usart::WordLength_8bits, Usart::StopBits_1,
                       Usart::ParityControl_disabled, Usart::Baudrate_10400KBaud, rcc.GetPeripheralClock(USART1))));

  USART1->CR1 |= USART_CR1_TE;
  USART1->CR1 |= USART_CR1_RE;
  USART1->CR3 |= USART_CR3_DMAT;
  USART1->CR3 |= USART_CR3_DMAR;
  USART1->SR &= ~USART_SR_TC; //проверить нужно ли сбрасывать этот флаг
  
//  NVIC_EnableIRQ(DMA1_Channel5_IRQn);
//  NVIC_EnableIRQ(USART1_IRQn);

  Program_timer pt(Program_timer::TimerType_one_pulse, 500);
  
  STM32_DMA dma_controller{};
  
  dma_controller.SetChannel(DMA1_Channel5);
  dma_controller.SetPeripheralAddress(reinterpret_cast<uint32_t>(&USART1->DR));
  dma_controller.SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
  dma_controller.DisablePeripheralIncrement();
  dma_controller.SetMemoryAddress(reinterpret_cast<uint32_t>(&array2[0]));
  dma_controller.SetMemorySize(STM32_DMA::MemorySize_8bit);
  dma_controller.EnableMemoryIncrement();
  dma_controller.SetMode(STM32_DMA::Mode_Peripheral2Memory);
  dma_controller.SetChannelPriority(STM32_DMA::ChannelPriority_veryhigh);
  dma_controller.SetTransferSize(7);
  dma_controller.EnableChannel();

  dma_controller.SetChannel(DMA1_Channel4);
  dma_controller.SetPeripheralAddress(reinterpret_cast<uint32_t>(&USART1->DR));
  dma_controller.SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
  dma_controller.DisablePeripheralIncrement();
  dma_controller.SetMemoryAddress(reinterpret_cast<uint32_t>(&array[0]));
  dma_controller.SetMemorySize(STM32_DMA::MemorySize_8bit);
  dma_controller.EnableMemoryIncrement();
  dma_controller.SetMode(STM32_DMA::Mode_Memory2Peripheral);
  dma_controller.SetChannelPriority(STM32_DMA::ChannelPriority_high);
  dma_controller.SetTransferSize(10);
  while(1)
  {
    static bool a = {1};
    if(a)
    {
      led_pin.Toggle();
      dma_controller.EnableChannel(DMA1_Channel4);
    }
    
    if(pt.Check())
    {
      __ASM("nop");
    }
  }
}
