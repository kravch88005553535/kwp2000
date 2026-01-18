#include <stdio.h>

#include "kwp2000.h"
#include "pid.h"

//Header::Header(const uint8_t a_format, const uint8_t a_target, const uint8_t a_source, const uint8_t a_length)
//  : m_format{a_format}
//  , m_target{a_target}
//  , m_source{a_source}
//  , m_length{a_length}
//{}
  
KWP2000::KWP2000(Usart& aref_usart, const bool a_dma_usage)
  : mref_usart{aref_usart}
  , m_tx_data{}
  , m_rx_data{}
  , m_package_size{0}
  , m_kwp2000_timer{Program_timer::TimerType_one_pulse, 400}
  , m_p1_timer{Program_timer::TimerType_one_pulse}
  , m_p2_timer{Program_timer::TimerType_one_pulse}
  , m_p3_timer{Program_timer::TimerType_one_pulse}
  , m_p4_timer{Program_timer::TimerType_one_pulse}
  , m_status{Status::Uninitialized}
  , mp_rx_dma_controller{new STM32_DMA()}
  , mp_tx_dma_controller{new STM32_DMA()}
  , mp_tx_pin{nullptr}
  , mp_rx_pin{nullptr}
  
{
  printf("Initializing KWP2000.\r\n");

  printf("Array addres :0x%X\r\n", &m_tx_data[0]);
  printf("Array addres :0x%X\r\n", &m_rx_data[0]);
  std::fill(std::begin(m_tx_data), std::end(m_tx_data), 0xAA);
  std::fill(std::begin(m_rx_data), std::end(m_rx_data), 0xBB);
  
  if(a_dma_usage)
  {
    printf("Initializing DMA TX/RX channels.\r\n");

    switch(mref_usart.GetPeripheralAddress())
    {
      case USART1_BASE:
      {
        mp_tx_pin = new Pin(GPIOA, 9,  Pin::mode_in_floating);
        mp_rx_pin = new Pin(GPIOA, 10, Pin::mode_in_floating);
        
        USART1->CR1 |= USART_CR1_TE;
        USART1->CR3 |= USART_CR3_DMAT;
        //USART1->CR3 |= USART_CR3_DMAR;
        //USART1->CR1 |= USART_CR1_RE;
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
  else if(m_status != Status::Initialized)
  {
    if(PerformFastInitialization())
    {
      m_status = Status::Initialized;
    }
  }

  if(m_status == Status::Initialized)
  {
    
  }
}

bool KWP2000::PerformFastInitialization()
{
  if(m_status == Status::PeripheralInitialized)
  {
    if(!m_kwp2000_timer.Check())
    {
      return 0;
    }
    mp_tx_pin->SetMode(Pin::mode_out_pulldown);
    m_kwp2000_timer.SetInterval_ms(24);
    m_kwp2000_timer.Start();
    m_status = Status::OnBus25msLowCondition;
    return 0;
  }
  
  if(m_status == Status::OnBus25msLowCondition)
  {
    if(m_kwp2000_timer.Check())
    {
      m_kwp2000_timer.Start();
      mp_tx_pin->SetMode(Pin::mode_out_pullup);
      m_status = Status::OnBus25msHighCondition;
      m_tx_data[0] = static_cast<uint8_t>(HeaderFromat::PhysicalAddressing); //fmt
      m_tx_data[1] = static_cast<uint8_t>(FunctionalAddress::Ecu);           //tgt
      m_tx_data[2] = static_cast<uint8_t>(FunctionalAddress::Tester);        //src
      m_tx_data[3] = static_cast<uint8_t>(SID::SID_startCommunication);      //SID
      m_tx_data[4] = static_cast<uint8_t>(0x03);                   //crc
      return 0;
    }
  }
  
  if(m_status == Status::OnBus25msHighCondition)
  {
    if(m_kwp2000_timer.Check())
    {
      mp_tx_pin->SetMode(Pin::mode_alternate_function_pushpull);
      mp_tx_dma_controller->SetTransferSize(5);
      mp_tx_dma_controller->EnableChannel();
      m_status = Status::TransmissionInitData;
    }
    return 0;
  }
  
  if(m_status == Status::TransmissionInitData)
  {
    if(mp_tx_dma_controller->IsTransferComplete())
    {
      m_p4_timer.SetInterval_ms(p2min);
      m_p4_timer.Start();
      while(!m_p4_timer.Check())
      {
        __ASM("nop");
      }
      
      USART1->CR1 |= USART_CR1_RE;
      USART1->CR3 |= USART_CR3_DMAR;
      mp_rx_dma_controller->SetTransferSize(15);
      mp_rx_dma_controller->EnableChannel();
      
      m_p4_timer.SetInterval_ms(550);
      m_p4_timer.Start();

      while(!m_p4_timer.Check())
      {
        __ASM("nop");
      }
      m_status = Status::WaitForResponse;
    }
    return 0;
  }
  
  if(m_status == Status::WaitForResponse)
  {

    return 0;
  }
  return 0;
}

//void KWP2000::MakeRequest(const Header a_header, const SID a_sid /*, parameter_bytes*/)
//{
//  constexpr uint8_t ecu_address{0x10};
//  
//  
//  static bool is_t_idle_completed{false};
//}

uint8_t KWP2000::CalculateCrc()
{
  uint8_t crc{0};
  for(auto it{m_tx_data.begin()}; it != m_tx_data.end(); ++it)
  {
    crc += *it;
  }
  return crc;
}