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
#include "peripherals/gpio/gpio_stm32f103.h"

//class Header
//{
//public:
//  Header(const uint8_t a_format, const uint8_t a_target, const uint8_t source, const uint8_t a_length);
//  Header() = delete;
//  ~Header();
//  uint8_t m_format;
//  uint8_t m_target;
//  uint8_t m_source;
//  uint8_t m_length;
//};

class KWP2000
{
  enum Status : uint8_t
  {
    Uninitialized,
    DmaInitializationFailed,
    PeripheralInitializationFailed,
    PeripheralInitialized,
    InitProcessFailed,
    FullyInitialized,
    Kwp2000Idle,
    ConnectionLost,
  };
  
  enum InitStep
  {
    InitBegin,
    OnBus25msLowCondition,
    OnBus25msHighCondition,
    TransmissionInitData,
    WaitForInitEnd,
    InitEnd,
    InitFinished,
  };
  
  enum UsartState
  {
    TransmittingData,
    WaitingForResponse, 
    Idle,
  };

  enum HeaderFromat: uint8_t
  {
    NoAddressInformationIsPresent = 0x00,
    ExceptionMode_CARB            = 0x40,
    PhysicalAddressing            = 0x81,
    FunctionalAddressing          = 0xC1,
  };
  
  enum TimingSet
  {
    Unknown,
    Normal,
    Extended,
  };
  enum FunctionalAddress
  {
    Immo   = 0xC0,
    Ecu    = 0x10,
    Tester = 0xF1,
  };
  public:
    
  KWP2000(Usart& aref_usart, const bool a_dma_usage);
  ~KWP2000();
  
  void Execute();
  
  private:
  bool PerformInitialization();
  void SetPackageSize(const uint8_t a_size);
  uint8_t GetPackageSize() const;
  uint8_t CalculateCrc(const uint8_t a_last_index) const; 
  
  void MakeRequest();
  void WaitForResponse();
  bool ParseResponse();
  bool ParseNegativeResponse();
  
//  void StartDiagnosicSession();
//  void StopDiagnosticSession();
//  void SecurityAccess();
//  void TesterPresent();
//  void EcuReset();
//  void ReadEcuIdentification();
  
  static constexpr auto p1min{0};
  static constexpr auto p1max{20};
  
  static constexpr auto p2min{25};
  static constexpr auto p2max{50};
  
  static constexpr auto p3min{100};
  static constexpr auto p3max{5000};
  
  static constexpr auto p4min{0};
  static constexpr auto p4max{20};
  
  static constexpr uint8_t stc_keybyte1{0x6B};
  static constexpr uint8_t stc_keybyte2{0x8F};
  
  Usart&        mref_usart;
  STM32_DMA*    mp_rx_dma_controller;
  STM32_DMA*    mp_tx_dma_controller;
  Pin*          mp_tx_pin; 
  Pin*          mp_rx_pin;
  
  std::array<uint8_t, 255> m_txrx_data;
  uint8_t                  m_package_size;
  
  bool m_is_len_info_in_fmt_byte_supported;
  bool m_is_additional_length_byte_supported;
  bool m_is_1_byte_header_supported;
  bool m_is_tgt_src_address_in_header_supported;
  TimingSet m_timing_set;
  
  Program_timer m_p1_timer;
  Program_timer m_p2_timer;
  Program_timer m_p3_timer;
  Program_timer m_p4_timer;
  Program_timer m_kwp2000_timer;
  
  Status m_status;
};

#endif //__KWP_2000_H__