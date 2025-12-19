#include "kwp2000.h"
#include "pid.h"

Header::Header(const uint8_t a_format, const uint8_t a_target, const uint8_t a_source, const uint8_t a_length)
  : m_format{a_format}
  , m_target{a_target}
  , m_source{a_source}
  , m_length{a_length}
{}
  
KWP2000::KWP2000(Usart& aref_usart)
  : mref_usart{aref_usart}
  , m_tx_data{}
  , m_rx_data{}
  , m_kwp2000_timer{Program_timer::TimerType_one_pulse, 400}
  , m_p1_timer{Program_timer::TimerType_one_pulse}
  , m_p2_timer{Program_timer::TimerType_one_pulse}
  , m_p3_timer{Program_timer::TimerType_one_pulse}
  , m_p4_timer{Program_timer::TimerType_one_pulse}
  , m_status{Status::Uninitialized}
{
}
  
void KWP2000::Execute()
{
  //check if diag session is lost
  //if lost -> retry
  bool init_success{false};
  if(m_kwp2000_timer.Check())
  {
    init_success = PerformFastInitialization();
  }
  
  if(init_success)
  {
    m_status = Status::Initialized;
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