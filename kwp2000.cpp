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
  , m_txrx_data{}
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

  printf("Array addres :0x%X\r\n", &m_txrx_data[0]);
  std::fill(std::begin(m_txrx_data), std::end(m_txrx_data), 0xAA);
  
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
        mp_rx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_txrx_data[0]));
        mp_rx_dma_controller->SetMemorySize(STM32_DMA::MemorySize_8bit);
        mp_rx_dma_controller->EnableMemoryIncrement();
        mp_rx_dma_controller->SetMode(STM32_DMA::Mode_Peripheral2Memory);
        mp_rx_dma_controller->SetChannelPriority(STM32_DMA::ChannelPriority_veryhigh);
        mp_rx_dma_controller->SetTransferSize(7);

        mp_tx_dma_controller->SetChannel(DMA1_Channel4);
        mp_tx_dma_controller->SetPeripheralAddress(reinterpret_cast<uint32_t>(&USART1->DR));
        mp_tx_dma_controller->SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
        mp_tx_dma_controller->DisablePeripheralIncrement();
        mp_tx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_txrx_data[0]));
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
        mp_rx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_txrx_data[0]));
        mp_rx_dma_controller->SetMemorySize(STM32_DMA::MemorySize_8bit);
        mp_rx_dma_controller->EnableMemoryIncrement();
        mp_rx_dma_controller->SetMode(STM32_DMA::Mode_Peripheral2Memory);
        mp_rx_dma_controller->SetChannelPriority(STM32_DMA::ChannelPriority_veryhigh);
        mp_rx_dma_controller->SetTransferSize(7);

        mp_tx_dma_controller->SetChannel(DMA1_Channel7);
        mp_tx_dma_controller->SetPeripheralAddress(reinterpret_cast<uint32_t>(&USART2->DR));
        mp_tx_dma_controller->SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
        mp_tx_dma_controller->DisablePeripheralIncrement();
        mp_tx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_txrx_data[0]));
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
        mp_rx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_txrx_data[0]));
        mp_rx_dma_controller->SetMemorySize(STM32_DMA::MemorySize_8bit);
        mp_rx_dma_controller->EnableMemoryIncrement();
        mp_rx_dma_controller->SetMode(STM32_DMA::Mode_Peripheral2Memory);
        mp_rx_dma_controller->SetChannelPriority(STM32_DMA::ChannelPriority_veryhigh);
        mp_rx_dma_controller->SetTransferSize(7);

        mp_tx_dma_controller->SetChannel(DMA1_Channel2);
        mp_tx_dma_controller->SetPeripheralAddress(reinterpret_cast<uint32_t>(&USART3->DR));
        mp_tx_dma_controller->SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
        mp_tx_dma_controller->DisablePeripheralIncrement();
        mp_tx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_txrx_data[0]));
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
        mp_rx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_txrx_data[0]));
        mp_rx_dma_controller->SetMemorySize(STM32_DMA::MemorySize_8bit);
        mp_rx_dma_controller->EnableMemoryIncrement();
        mp_rx_dma_controller->SetMode(STM32_DMA::Mode_Peripheral2Memory);
        mp_rx_dma_controller->SetChannelPriority(STM32_DMA::ChannelPriority_veryhigh);
        mp_rx_dma_controller->SetTransferSize(7);

        mp_tx_dma_controller->SetChannel(DMA2_Channel5);
        mp_tx_dma_controller->SetPeripheralAddress(reinterpret_cast<uint32_t>(&UART4->DR));
        mp_tx_dma_controller->SetPeripheralSize(STM32_DMA::PeripheralSize_8bit);
        mp_tx_dma_controller->DisablePeripheralIncrement();
        mp_tx_dma_controller->SetMemoryAddress(reinterpret_cast<uint32_t>(&m_txrx_data[0]));
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
  switch(m_status)
  {
    case PeripheralInitialized:
    case DmaInitializationFailed:
    {
      PerformInitialization();
    }
    break;
    
    case FullyInitialized:
    {
      //check session timers and execute logic.
    }
    break;
    
    case Kwp2000Idle:
    case PeripheralInitializationFailed:
    case Uninitialized:
    default:
      __ASM("nop");
    break;
  }
}

bool KWP2000::PerformInitialization()
{
  static InitStep init_step{InitBegin};
  switch(init_step)
  {
    case InitBegin:
    {
      if(m_kwp2000_timer.Check())
      {
        mp_tx_pin->SetMode(Pin::mode_out_pulldown);
        m_kwp2000_timer.SetInterval_ms(24);
        m_kwp2000_timer.Start();
        init_step = OnBus25msLowCondition;
      }
    }
    break;
        
    case OnBus25msLowCondition:
    {
      if(m_kwp2000_timer.Check())
      {
        m_kwp2000_timer.Start();
        mp_tx_pin->SetMode(Pin::mode_out_pullup);
        m_txrx_data[0] = static_cast<uint8_t>(HeaderFromat::PhysicalAddressing); //fmt
        m_txrx_data[1] = static_cast<uint8_t>(FunctionalAddress::Ecu);           //tgt
        m_txrx_data[2] = static_cast<uint8_t>(FunctionalAddress::Tester);        //src
        m_txrx_data[3] = static_cast<uint8_t>(SID_Req::SID_startCommunication);      //SID
        m_txrx_data[4] = static_cast<uint8_t>(0x03);                   //crc
        init_step = OnBus25msHighCondition;
      }
    }
    break;
        
    case OnBus25msHighCondition:
    {
      if(m_kwp2000_timer.Check())
      {
        mp_tx_pin->SetMode(Pin::mode_alternate_function_pushpull);
        MakeRequest();
        init_step = TransmissionInitData;
      }
    }
    break;
    
    case TransmissionInitData:
    {
      if(mp_tx_dma_controller->IsTransferComplete())
      {
        m_p4_timer.SetInterval_ms(p2min);
        m_p4_timer.Start();
        init_step = WaitForInitEnd;
      }
    }
    break;
    
    case WaitForInitEnd:
    {
      if(m_p4_timer.Check())
      {
        std::fill(std::begin(m_txrx_data), std::end(m_txrx_data), 0);
        mref_usart.EnableReciever();
        mref_usart.EnableDmaReciever();
        mp_rx_dma_controller->SetTransferSize(255);
        mp_rx_dma_controller->EnableChannel();
        m_p4_timer.SetInterval_ms(500);
        m_p4_timer.Start();
        init_step = InitEnd;
      }
    }
    break;
    
    case InitEnd:
    {
      if(m_p4_timer.Check())
      {
        if(ParseResponse())
        {
          //configure peripherals etc.
          //start request/respone timers
          
          init_step = InitFinished;
          m_status = FullyInitialized;
        }
        else
        {
          init_step = InitBegin;
          m_status = InitProcessFailed;
          m_kwp2000_timer.SetInterval_ms(3000);  ///!!!!!!!!!!!!!!!!!!!!!
          m_kwp2000_timer.Start();
        }
      }
    }
    break;
    
    case InitFinished:
    {
      return 1;
    }
    break;
        
    default:
    break;
  };
  
  return 0;
}

void KWP2000::SetPackageSize(const uint8_t a_size)
{
}

uint8_t KWP2000::GetPackageSize() const
{
  uint8_t package_size{0};
  if(m_txrx_data[0] & 0x3F)
  {
    package_size = m_txrx_data[0] & 0x3F;
  }
  else
  {
    package_size = m_txrx_data[3];
  }
  return package_size;
}

void KWP2000::MakeRequest()
{
  mp_tx_dma_controller->SetTransferSize(5);
  mp_tx_dma_controller->EnableChannel();
}

void KWP2000::WaitForResponse()
{
  //check when dmar flag stops changing or check for no RXNE flag on usart register
}

bool KWP2000::ParseResponse()
{
  constexpr uint8_t max_package_size{255};
  constexpr uint8_t no_length_byte_package_size{63};
  const uint8_t package_size {GetPackageSize()};
  
  if(package_size == 0)
  {
    printf("RX package size = 0\r\n");
    return 0;
  }
  printf("RX package size = %X\r\n", package_size);
  
  uint8_t last_array_index{0};
  uint8_t sid_index{0};
  if(package_size < no_length_byte_package_size)
  {
    constexpr uint8_t header_size{3};
    last_array_index = package_size + header_size - 1;
    sid_index = 3;
  }
  else
  {
    constexpr uint8_t header_size{4};
    last_array_index = package_size + header_size - 1;
    sid_index = 4;
  }
  printf("last array index = %d\r\n", last_array_index);
  
  const uint8_t crc{CalculateCrc(last_array_index)};
  printf("CRC = %X\r\n", crc);
  
  const auto crc_index{last_array_index + 1};
  
  printf("CRC index = %d\r\n", crc_index);
  if(crc != m_txrx_data[crc_index])
  {
    return 0;
  }
  
  const SID_Rsp sid = static_cast <SID_Rsp>(m_txrx_data[sid_index]);
  
  switch(sid)
  {
    case SID_Rsp::SID_Rsp_startCommunication:
    {
      return 1;
    }
    
    case SID_Rsp::SID_Rsp_negativeResponse:
    {
      return ParseNegativeResponse(/**/);
    }
    deafult:
      return 0;
    break;
  }
  return 0;
}

bool KWP2000::ParseNegativeResponse()
{
  return 1;
}

uint8_t KWP2000::CalculateCrc(const uint8_t a_last_index) const
{
  uint8_t crc{0}; //can be overflown, it is OK!
  for(uint8_t i{0}; i <= a_last_index; ++i)
  {
    crc += m_txrx_data[i];
  }
  return crc;
}