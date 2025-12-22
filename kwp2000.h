#ifndef __KWP_2000_H__
#define __KWP_2000_H__

/*
Keyword Protocol 2000
Data Link Layer
Recommended Practice
*/

#include <cstdint>
#include <array>

#include "sid.h"
#include "interfaces/usart/usart_stm32f103.h"
#include "peripherals/program_timer/program_timer.h"
#include "peripherals/dma/dma_stm32f10x.h"


class Header
{
  public:
  Header(const uint8_t a_format, const uint8_t a_target, const uint8_t source, const uint8_t a_length);
  Header() = delete;
  ~Header();
  uint8_t m_format;
  uint8_t m_target;
  uint8_t m_source;
  uint8_t m_length;
};

enum class Status : uint8_t
{
  Uninitialized,
  Initialized,
  WaitForResponse,
  
};
class KWP2000
{
  enum class HeaderFromat: uint8_t
  {
    NoAddressInformationIsPresent = 0x00,
    ExceptionModeOfAddressing     = 0x40,
    PhysicalAddressing            = 0x80,
    FunctionalAddressing          = 0xC0,
  };
  enum class FunctionalAddress
  {
    Immo   = 0xC0,
    Ecu    = 0x10,
    Tester = 0xF1,
  };
  public:
  KWP2000(Usart& mref_usart, STM32_DMA* ap_dma_controller = nullptr);
  ~KWP2000();
  
  void Execute();
  
  private:
  bool PerformFastInitialization();
  void StartDiagnosicSession();
  void StopDiagnosticSession();
  void SecurityAccess();
  void TesterPresent();
  void EcuReset();
  void ReadEcuIdentification();
  uint8_t CalculateCrc();
  
  /*
  rx tx buffers (as I remember 255 bytes each);
  */
  
  void MakeRequest(const Header a_header, const SID a_sid /*, parameter_bytes*/);
  Usart& mref_usart;
  
  
  //m_communication_speed
  std::array<uint8_t, 255> m_tx_data;
  std::array<uint8_t, 255> m_rx_data;
  static constexpr auto p1min{0};
  static constexpr auto p1max{20};
  
  static constexpr auto p2min{25};
  static constexpr auto p2max{50};
  
  static constexpr auto p3min{100};
  static constexpr auto p3max{5000};
  
  static constexpr auto p4min{0};
  static constexpr auto p4max{20};
  
  STM32_DMA*    mp_rx_dma_controller;
  STM32_DMA*    mp_tx_dma_controller;
  
  Program_timer m_p1_timer;
  Program_timer m_p2_timer;
  Program_timer m_p3_timer;
  Program_timer m_p4_timer;
  Program_timer m_kwp2000_timer;
  
  Status m_status;
};

//class KWP2000OnKLine : public KWP2000
//{
//  ~KWP2000OnKLine();
//  

//  void Perform5BaudInitialization();
//  
//};

//class KWP2000OnCAN : public KWP2000
//{
//  ~KWP2000OnCAN();
//};

#endif //__KWP_2000_H__