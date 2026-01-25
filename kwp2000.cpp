#include <stdio.h>

#include "kwp2000.h"
#include "pid.h"
#include "response_codes.h"

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
  , m_is_len_info_in_fmt_byte_supported{false}
  , m_is_additional_length_byte_supported{false}
  , m_is_1_byte_header_supported{false}
  , m_is_tgt_src_address_in_header_supported{false}
  , m_timing_set{Unknown}
  
{
  constexpr uint8_t crc_size{1};
  constexpr uint8_t max_data_size{255};
  constexpr uint8_t max_header_size{4};
  m_txrx_data.reserve(max_header_size + max_data_size + crc_size);
  
  std::fill(std::begin(m_txrx_data), std::end(m_txrx_data), 0xAA);
  
  //printf("Initializing KWP2000.\r\n");

  //printf("Array addres :0x%X\r\n", &m_txrx_data[0]);
  
  
  if(a_dma_usage)
  {
    //printf("Initializing DMA TX/RX channels.\r\n");

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
        //printf("KWP2000: wrong UART pointer. DMA initialization failed\r\n");
        //printf("Switching to manual TX/RX management\r\n");
        m_status = Status::DmaInitializationFailed;
      }
      break;
      
      default:
      {
        //printf("KWP2000: wrong UART pointer. KWP2000 initialization failed\r\n");
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
      m_status = OnKwpInit;
    }
//    break;
    case OnKwpInit:
    {
      PerformInitialization();
    }
    break;
    case InitProcessFailed:
    {
      static uint8_t num_of_failed_init_attepmts{0};
      ++num_of_failed_init_attepmts;
      m_status = OnKwpInit;
    }
    break;
    
    case FullyInitialized:
    {
      if(m_p3_timer.Check())
      {
        static bool package_type{1};
        package_type = !package_type;
        if(package_type)
        {
          MakeRequest(SID_Req::SID_Req_readDiagnosticTroubleCodesByStatus);
        }
        else
        {
          MakeRequest(SID_Req::SID_Req_readDataByLocalIdentifier);
        }
      }
    }
    break;
    
    case TransmittingData:
    {
      if(mp_tx_dma_controller->IsTransferComplete())
      {
        mref_usart.ClearTransferCompleteFlag();        
        m_p2_timer.SetInterval_ms(110);
        m_p2_timer.Start();
        
        m_kwp2000_timer.SetInterval_ms(10);
        m_kwp2000_timer.Start();
        
        m_status = WaitingForResponse;
      }
    }
    break;
    
    case WaitingForResponse:
    {
      if(m_kwp2000_timer.Check())
      {
        std::fill(m_txrx_data.begin(), m_txrx_data.end(), 0);
        m_txrx_data.erase(m_txrx_data.begin(), m_txrx_data.end());
        mp_rx_dma_controller->DisableChannel();
        mp_rx_dma_controller->SetTransferSize(240);
        mp_rx_dma_controller->EnableChannel();
      }
      
      if(m_p2_timer.Check())
      {
        __ASM("nop");
          if(!ParseResponse())
          {
            __ASM("nop");
            //increase error counter
            //if error counter is high set status to init
            //else
            m_status = FullyInitialized;
          }
          else
          {
            __ASM("nop");
//          set error counter to 0
            m_status = FullyInitialized;
          }
      }
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
        init_step = OnBus25msHighCondition;
      }
    }
    break;
        
    case OnBus25msHighCondition:
    {
      if(m_kwp2000_timer.Check())
      {
        mp_tx_pin->SetMode(Pin::mode_alternate_function_pushpull);
        MakeRequest(SID_Req::SID_Req_startCommunication);
        init_step = TransmissionInitData;
      }
    }
    break;
    
    case TransmissionInitData:
    {
      if(mp_tx_dma_controller->IsTransferComplete())
      {
        m_p4_timer.SetInterval_ms(15);
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
          init_step = InitFinished;
          m_status  = FullyInitialized;
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
  if(a_size < 63)
  {
    m_txrx_data[0] |= a_size;
  }
  else
  {
    m_txrx_data[0] &= 0x3F;
    m_txrx_data[3] = a_size; //DANGEROUS SHIT!
  }
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

void KWP2000::MakeRequest(const SID_Req a_sid)
{
  std::fill(m_txrx_data.begin(), m_txrx_data.end(), 0);
  m_txrx_data.erase(m_txrx_data.begin(), m_txrx_data.end());
  switch(a_sid)
  {
    case SID_Req::SID_Req_startCommunication:
    {
      m_txrx_data.push_back(static_cast<uint8_t>(HeaderFromat::PhysicalAddressing));
      m_txrx_data.push_back(static_cast<uint8_t>(FunctionalAddress::Ecu));
      m_txrx_data.push_back(static_cast<uint8_t>(FunctionalAddress::Tester));
      m_txrx_data.push_back(static_cast<uint8_t>(SID_Req::SID_Req_startCommunication));
      SetPackageSize(1);
      m_txrx_data.push_back(static_cast<uint8_t>(CalculateCrc(m_txrx_data.size() - 1)));
    }
    break;
    
    case SID_Req::SID_Req_readDiagnosticTroubleCodesByStatus:
    {
      m_txrx_data.push_back(static_cast<uint8_t>(HeaderFromat::PhysicalAddressing));
      m_txrx_data.push_back(static_cast<uint8_t>(FunctionalAddress::Ecu));
      m_txrx_data.push_back(static_cast<uint8_t>(FunctionalAddress::Tester));
      m_txrx_data.push_back(static_cast<uint8_t>(SID_Req::SID_Req_readDiagnosticTroubleCodesByStatus));
      m_txrx_data.push_back(0);
      m_txrx_data.push_back(0);
      m_txrx_data.push_back(0);
      SetPackageSize(4);
      m_txrx_data.push_back(static_cast<uint8_t>(CalculateCrc(m_txrx_data.size() - 1)));
    }
    break;
    
    case SID_Req::SID_Req_readDataByLocalIdentifier:
    {
      static uint8_t pid{0x01};
      m_txrx_data.push_back(static_cast<uint8_t>(HeaderFromat::PhysicalAddressing));
      m_txrx_data.push_back(static_cast<uint8_t>(FunctionalAddress::Ecu));
      m_txrx_data.push_back(static_cast<uint8_t>(FunctionalAddress::Tester));
      m_txrx_data.push_back(static_cast<uint8_t>(SID_Req::SID_Req_readDataByLocalIdentifier));
      m_txrx_data.push_back(pid);
      SetPackageSize(2);
      m_txrx_data.push_back(static_cast<uint8_t>(CalculateCrc(m_txrx_data.size() - 1)));
    }
    break;
    
    case SID_Req::SID_Req_testerPresent:
    {
      m_txrx_data.push_back(static_cast<uint8_t>(HeaderFromat::PhysicalAddressing));
      m_txrx_data.push_back(static_cast<uint8_t>(FunctionalAddress::Ecu));
      m_txrx_data.push_back(static_cast<uint8_t>(FunctionalAddress::Tester));
      m_txrx_data.push_back(static_cast<uint8_t>(SID_Req::SID_Req_testerPresent));
      m_txrx_data.push_back(static_cast<uint8_t>(0x01));
      SetPackageSize(2);
      m_txrx_data.push_back(static_cast<uint8_t>(CalculateCrc(m_txrx_data.size() - 1)));
    }
    break;
    
    default:
    break;
  }
  printf("\r\nRequest: ");
  for(auto it{m_txrx_data.begin()}; it != m_txrx_data.end(); ++it)
  {
    printf("%X ", static_cast<uint8_t>(*it));
  }
  printf("\r\n");
  mp_tx_dma_controller->SetTransferSize(m_txrx_data.size());
  mp_tx_dma_controller->EnableChannel();
  if(m_status != OnKwpInit)
  {
    m_status = TransmittingData;
  }
}

bool KWP2000::ParseResponse()
{
  m_p3_timer.SetInterval_ms(p3min);
  m_p3_timer.Start();
  constexpr uint8_t max_package_size{255};
  constexpr uint8_t no_length_byte_package_size{63};
  const uint8_t package_size{GetPackageSize()};
  
  if(package_size == 0)
  {
    printf("RX package size = 0\r\n");
    printf("%X\r\n", m_txrx_data[0]);
    return 0;
  }
  
  uint16_t last_array_index{0};
  uint8_t sid_index{0};
  if(m_txrx_data[0] != 0x80)
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
  printf("Response: ");
  for(uint16_t i{0}; i <= last_array_index; ++i)
  {
    printf("%X ", static_cast<uint8_t>(m_txrx_data[i])); //DANGEROUS SHIT!
  }
  printf("\r\n");
  
  const uint8_t crc{CalculateCrc(last_array_index)};
  
  const auto crc_index{last_array_index + 1};
  if(crc != m_txrx_data[crc_index])
  {
    printf("CRC = %X (ERROR)\r\n", crc);
    return 1;
  }
  else
  {
    printf("CRC = %X (OK)\r\n", crc);
  }
  
  const SID_Rsp sid = static_cast <SID_Rsp>(m_txrx_data[sid_index]);
  
  switch(sid)
  {
    /*
      A client (tester) which fulfils Keyword Protocol 2000 according to KWP 2000 - Data Link Layer Recommended
      Practice shall support 100% functionality, independent from the key bytes transferred from the server (ECU).
      A server (ECU) transmits is supported functionality concerning timing, length and address with its key bytes. The
      serverТs (ECUТs) which transmit and receive messages are inside of this definition. The header format of a server
      (ECU) response message must be the same as of the client (tester) request message with the exception of the length
      information. For example, based on a client (tester) request message with format, target and source header
      information the server (ECU) must send a response message containing the same header information.
    */
    case SID_Rsp::SID_Rsp_startCommunication:
    {
      if(m_txrx_data[5] == 0x8F)
      {
        printf("-----------------\r\n");
        
        m_is_len_info_in_fmt_byte_supported = m_txrx_data[4] & 0x01;
        printf("length information in format byte supported = %d\r\n", m_is_len_info_in_fmt_byte_supported);
        
        m_is_additional_length_byte_supported = m_txrx_data[4] & 0x02;
        printf("additional length byte supported = %d\r\n", m_is_additional_length_byte_supported);
        
        m_is_1_byte_header_supported = m_txrx_data[4] & 0x04;
        printf("1 byte header supported = %d\r\n", m_is_1_byte_header_supported);
        
        m_is_tgt_src_address_in_header_supported = m_txrx_data[4] & 0x08;
        printf("target/source address in header supported = %d\r\n", m_is_tgt_src_address_in_header_supported);
        
        printf("timing parameter 0 = %d\r\n", (bool)(m_txrx_data[4] & 0x10));
        printf("timing parameter 1 = %d\r\n", (bool)(m_txrx_data[4] & 0x20));
        
        if(((m_txrx_data[4] & 0x10) == 0) and (m_txrx_data[4] & 0x20))
        {
          m_timing_set = Normal;
          printf("Normal timing set\r\n");
        }
        else if((m_txrx_data[4] & 0x10) and ((m_txrx_data[4] & 0x20) == 0))
        {
          m_timing_set = Extended;
          printf("Extended timing set\r\n");
        }
        
        printf("-----------------\r\n");
      }
      return 1;
    }
    
    case SID_Rsp::SID_Rsp_testerPresent:
    {
      //reload timers for tester present or do smth else here
      return 1;
    }
    break;
    
    case SID_Rsp::SID_Rsp_stopCommunication:
    {
      //end communication between tester & ecu
      return 1;
    }
    break;
    
    case SID_Rsp::SID_Rsp_negativeResponse:
    {
      return ParseNegativeResponse(sid_index);
    }
    break; //not necessary here
    
    deafult:
      return 0;
    break;
  }
  return 0;
}

bool KWP2000::ParseNegativeResponse(const uint16_t a_nrc_index)
{
  const NRC response_code{static_cast<uint8_t>(m_txrx_data[a_nrc_index])};
  
  switch(response_code)
  {
    case NRC::NRC_generalReject: 
    {
    }
    break;
    
    case NRC::NRC_serviceNotSupported:
    {
    }
    break;

    case NRC::NRC_subFunctionNotSupported_invalidFormat:
    {
    }
    break;
    
    case NRC::NRC_busy_RepeatRequest:
    {
    }
    break;
    
    case NRC::NRC_conditionsNotCorrect_or_requestSequenceError:
    {
    }
    break;
    
    case NRC::NRC_routineNotComplete:
    {
    }
    break;
    
    case NRC::NRC_requestOutOfRange:
    {
    }
    break;
    
    case NRC::NRC_securityAccessDenied:
    {
    }
    break;
    
    case NRC::NRC_invalidKey:
    {
    }
    break;
    
    case NRC::NRC_exceedNumberOfAttempts:
    {
    }
    break;
    
    case NRC::NRC_requiredTimeDelayNotExpired:
    {
    }
    break;
    
    case NRC::NRC_downloadNotAccepted:
    {
    }
    break;
    
    case NRC::NRC_improperDownloadType:
    {
    }
    break;
    
    case NRC::NRC_cannotDownloadToSpecifiedAddress:
    {
    }
    break;
    
    case NRC::NRC_cannotDownloadNumberOfBytesRequested:
    {
    }
    break;
    
    case NRC::NRC_uploadNotAccepted:
    {
    }
    break;
    
    case NRC::NRC_improperUploadType:
    {
    }
    break;
    
    case NRC::NRC_cannotUploadFromSpecifiedAddress:
    {
    }
    break;
    
    case NRC::NRC_cannotUploadNumberOfBytesRequested:
    {
    }
    break;
    
    case NRC::NRC_transferSuspended:
    {
    }
    break;
    
    case NRC::NRC_transferAborted:
    {
    }
    break;
    
    case NRC::NRC_illegalAddressInBlockTransfer:
    {
    }
    break;
    
    case NRC::NRC_illegalByteCountInBlockTransfer:
    {
    }
    break;
    
    case NRC::NRC_illegalBlockTransferType:
    {
    }
    break;
    
    case NRC::NRC_blockTransferDataChecksumError:
    {
    }
    break;
    
    case NRC::NRC_requestCorrectlyReceived_ResponsePending:
    {
    }
    break;
    
    case NRC::NRC_incorrectByteCountDuringBlockTransfer:
    
    default:
    break;
  }
  return 1;
}

uint8_t KWP2000::CalculateCrc() const
{
  uint8_t crc{0}; //can be overflown, it is OK!
  for(auto it{m_txrx_data.begin()}; it != m_txrx_data.end(); ++it)
  {
    crc += *it;
  }
  return crc;
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