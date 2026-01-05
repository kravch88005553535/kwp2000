#include <stdio.h>

#include "kwp2000.h"
#include "pid.h"

Header::Header(const uint8_t a_format, const uint8_t a_target, const uint8_t a_source, const uint8_t a_length)
  : m_format{a_format}
  , m_target{a_target}
  , m_source{a_source}
  , m_length{a_length}
{}
  
KWP2000::KWP2000(Usart& aref_usart, const bool a_dma_usage)
  : mref_usart{aref_usart}
  , m_tx_data{}
  , m_rx_data{}
  , m_kwp2000_timer{Program_timer::TimerType_one_pulse, 400}
  , m_p1_timer{Program_timer::TimerType_one_pulse}
  , m_p2_timer{Program_timer::TimerType_one_pulse}
  , m_p3_timer{Program_timer::TimerType_one_pulse}
  , m_p4_timer{Program_timer::TimerType_one_pulse}
  , m_status{Status::Uninitialized}
  , mp_rx_dma_controller{new STM32_DMA()}
  , mp_tx_dma_controller{new STM32_DMA()}
{
  printf("Initializing KWP2000.\r\n");

  if(a_dma_usage)
  {
    printf("Initializing DMA TX/RX channels.\r\n");

    switch(mref_usart.GetPeripheralAddress())
    {
      case USART1_BASE:
      {
        USART1->CR1 |= USART_CR1_TE;
        USART1->CR1 |= USART_CR1_RE;
        USART1->CR3 |= USART_CR3_DMAT;
        USART1->CR3 |= USART_CR3_DMAR;
        USART1->SR &= ~USART_SR_TC; //проверить нужно ли сбрасывать этот флаг
        
        mp_rx_dma_controller->SetChannel(DMA1_Channel5);
        mp_rx_dma_controller->SetPeripheralAddress(reinterpret_cast<uint32_t>(&USART1->DR));
        mp_rx_dma_controller->SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
        mp_rx_dma_controller->DisablePeripheralIncrement();
        mp_rx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_rx_data[0]));
        mp_rx_dma_controller->SetMemorySize(STM32_DMA::MemorySize_8bit);
        mp_rx_dma_controller->EnableMemoryIncrement();
        mp_rx_dma_controller->SetMode(STM32_DMA::Mode_Peripheral2Memory);
        mp_rx_dma_controller->SetChannelPriority(STM32_DMA::ChannelPriority_veryhigh);
        mp_rx_dma_controller->SetTransferSize(7);

        mp_tx_dma_controller->SetChannel(DMA1_Channel4);
        mp_tx_dma_controller->SetPeripheralAddress(reinterpret_cast<uint32_t>(&USART1->DR));
        mp_tx_dma_controller->SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
        mp_tx_dma_controller->DisablePeripheralIncrement();
        mp_tx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_tx_data[0]));
        mp_tx_dma_controller->SetMemorySize(STM32_DMA::MemorySize_8bit);
        mp_tx_dma_controller->EnableMemoryIncrement();
        mp_tx_dma_controller->SetMode(STM32_DMA::Mode_Memory2Peripheral);
        mp_tx_dma_controller->SetChannelPriority(STM32_DMA::ChannelPriority_high);
        mp_tx_dma_controller->SetTransferSize(10);
        m_status = Status::PeripheralInitialized;
      }
      break;
      
      case USART2_BASE:
      {
        USART2->CR1 |= USART_CR1_TE;
        USART2->CR1 |= USART_CR1_RE;
        USART2->CR3 |= USART_CR3_DMAT;
        USART2->CR3 |= USART_CR3_DMAR;
        USART2->SR &= ~USART_SR_TC; //проверить нужно ли сбрасывать этот флаг
        
        mp_rx_dma_controller->SetChannel(DMA1_Channel6);
        mp_rx_dma_controller->SetPeripheralAddress(reinterpret_cast<uint32_t>(&USART2->DR));
        mp_rx_dma_controller->SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
        mp_rx_dma_controller->DisablePeripheralIncrement();
        mp_rx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_rx_data[0]));
        mp_rx_dma_controller->SetMemorySize(STM32_DMA::MemorySize_8bit);
        mp_rx_dma_controller->EnableMemoryIncrement();
        mp_rx_dma_controller->SetMode(STM32_DMA::Mode_Peripheral2Memory);
        mp_rx_dma_controller->SetChannelPriority(STM32_DMA::ChannelPriority_veryhigh);
        mp_rx_dma_controller->SetTransferSize(7);

        mp_tx_dma_controller->SetChannel(DMA1_Channel7);
        mp_tx_dma_controller->SetPeripheralAddress(reinterpret_cast<uint32_t>(&USART2->DR));
        mp_tx_dma_controller->SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
        mp_tx_dma_controller->DisablePeripheralIncrement();
        mp_tx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_tx_data[0]));
        mp_tx_dma_controller->SetMemorySize(STM32_DMA::MemorySize_8bit);
        mp_tx_dma_controller->EnableMemoryIncrement();
        mp_tx_dma_controller->SetMode(STM32_DMA::Mode_Memory2Peripheral);
        mp_tx_dma_controller->SetChannelPriority(STM32_DMA::ChannelPriority_high);
        mp_tx_dma_controller->SetTransferSize(10);
        m_status = Status::PeripheralInitialized;
      }
      break;
      
      case USART3_BASE:
      {
        USART3->CR1 |= USART_CR1_TE;
        USART3->CR1 |= USART_CR1_RE;
        USART3->CR3 |= USART_CR3_DMAT;
        USART3->CR3 |= USART_CR3_DMAR;
        USART3->SR &= ~USART_SR_TC; //проверить нужно ли сбрасывать этот флаг
        
        mp_rx_dma_controller->SetChannel(DMA1_Channel3);
        mp_rx_dma_controller->SetPeripheralAddress(reinterpret_cast<uint32_t>(&USART3->DR));
        mp_rx_dma_controller->SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
        mp_rx_dma_controller->DisablePeripheralIncrement();
        mp_rx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_rx_data[0]));
        mp_rx_dma_controller->SetMemorySize(STM32_DMA::MemorySize_8bit);
        mp_rx_dma_controller->EnableMemoryIncrement();
        mp_rx_dma_controller->SetMode(STM32_DMA::Mode_Peripheral2Memory);
        mp_rx_dma_controller->SetChannelPriority(STM32_DMA::ChannelPriority_veryhigh);
        mp_rx_dma_controller->SetTransferSize(7);

        mp_tx_dma_controller->SetChannel(DMA1_Channel2);
        mp_tx_dma_controller->SetPeripheralAddress(reinterpret_cast<uint32_t>(&USART3->DR));
        mp_tx_dma_controller->SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
        mp_tx_dma_controller->DisablePeripheralIncrement();
        mp_tx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_tx_data[0]));
        mp_tx_dma_controller->SetMemorySize(STM32_DMA::MemorySize_8bit);
        mp_tx_dma_controller->EnableMemoryIncrement();
        mp_tx_dma_controller->SetMode(STM32_DMA::Mode_Memory2Peripheral);
        mp_tx_dma_controller->SetChannelPriority(STM32_DMA::ChannelPriority_high);
        mp_tx_dma_controller->SetTransferSize(10);
        m_status = Status::PeripheralInitialized;
      }
      break;

      case UART4_BASE:
      {
        UART4->CR1 |= USART_CR1_TE;
        UART4->CR1 |= USART_CR1_RE;
        UART4->CR3 |= USART_CR3_DMAT;
        UART4->CR3 |= USART_CR3_DMAR;
        UART4->SR &= ~USART_SR_TC; //проверить нужно ли сбрасывать этот флаг
        
        mp_rx_dma_controller->SetChannel(DMA2_Channel3);
        mp_rx_dma_controller->SetPeripheralAddress(reinterpret_cast<uint32_t>(&UART4->DR));
        mp_rx_dma_controller->SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
        mp_rx_dma_controller->DisablePeripheralIncrement();
        mp_rx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_rx_data[0]));
        mp_rx_dma_controller->SetMemorySize(STM32_DMA::MemorySize_8bit);
        mp_rx_dma_controller->EnableMemoryIncrement();
        mp_rx_dma_controller->SetMode(STM32_DMA::Mode_Peripheral2Memory);
        mp_rx_dma_controller->SetChannelPriority(STM32_DMA::ChannelPriority_veryhigh);
        mp_rx_dma_controller->SetTransferSize(7);

        mp_tx_dma_controller->SetChannel(DMA2_Channel5);
        mp_tx_dma_controller->SetPeripheralAddress(reinterpret_cast<uint32_t>(&UART4->DR));
        mp_tx_dma_controller->SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
        mp_tx_dma_controller->DisablePeripheralIncrement();
        mp_tx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_tx_data[0]));
        mp_tx_dma_controller->SetMemorySize(STM32_DMA::MemorySize_8bit);
        mp_tx_dma_controller->EnableMemoryIncrement();
        mp_tx_dma_controller->SetMode(STM32_DMA::Mode_Memory2Peripheral);
        mp_tx_dma_controller->SetChannelPriority(STM32_DMA::ChannelPriority_high);
        mp_tx_dma_controller->SetTransferSize(10);
        m_status = Status::PeripheralInitialized;
      }
      break;
      
      case UART5_BASE:
      {
        printf("KWP2000: wrong UART pointer. DMA initialization failed\r\n");
        printf("Switching to manual TX/RX management\r\n");
        m_status = Status::DmaInitializationFailed;
      }
      break;
      
      default:
      {
        printf("KWP2000: wrong UART pointer. KWP2000 initialization failed\r\n");
        m_status = Status::PeripheralInitializationFailed;
      }
      break;
    }
  }
  
  if(m_status == Status::DmaInitializationFailed or !a_dma_usage)
  {
    m_status = Status::PeripheralInitializationFailed;
  }
}
  
void KWP2000::Execute()
{
  if(m_status == Status::PeripheralInitializationFailed)
  {
    return;
  }

  if(m_status == Status::PeripheralInitialized)
  {
    if(m_kwp2000_timer.Check())
    {
      if(PerformFastInitialization())
      {
        m_status = Status::Initialized;
      }
    }
  }
}

bool KWP2000::PerformFastInitialization()
{
  constexpr uint8_t t_init_l_h_interval{25};
  
  //reconfigure pin
  Program_timer t_init{Program_timer::TimerType_one_pulse, t_init_l_h_interval};
  
  //set pin low
  while(!t_init.Check())
  {
    __ASM("nop");
  }
  //set pin high
  t_init.Start();
  while(!t_init.Check())
  {
    __ASM("nop");
  }
  
  std::fill(std::begin(m_tx_data), std::end(m_tx_data), 0);
  
  m_tx_data[0] = static_cast<uint8_t>(HeaderFromat::PhysicalAddressing); //fmt
  m_tx_data[1] = static_cast<uint8_t>(FunctionalAddress::Ecu);           //tgt
  m_tx_data[2] = static_cast<uint8_t>(FunctionalAddress::Tester);        //src
  m_tx_data[3] = static_cast<uint8_t>(SID::SID_startCommunication);      //SID
  m_tx_data[4] = static_cast<uint8_t>(CalculateCrc());                   //crc
  
  mp_tx_dma_controller->SetTransferSize(m_tx_data.size());
  mp_tx_dma_controller->EnableChannel();
  
  mref_usart.Transmit(m_tx_data.data(), m_tx_data.size());
  
  
  //wait for response
  //return response_ok  
  
  return true;
}

void KWP2000::MakeRequest(const Header a_header, const SID a_sid /*, parameter_bytes*/)
{
  constexpr uint8_t ecu_address{0x10};
  
  
  static bool is_t_idle_completed{false};
  
//  if(m_)
}

uint8_t KWP2000::CalculateCrc()
{
  uint8_t crc{0};
  for(auto it{m_tx_data.begin()}; it != m_tx_data.end(); ++it)
  {
    crc += *it;
  }
  return crc;
}