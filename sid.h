#ifndef __SID_H__
#define __SID_H__

#include <stdint.h>

enum class SID: uint8_t
{
  SID_startDiagnosticSession                 = 0x10,
  SID_ecuReset                               = 0x11,
  SID_readFreezeFrameData                    = 0x12,
  SID_readDiagnosticTroubleCodes             = 0x13,
  SID_clearDiagnosticInformation             = 0x14,
  SID_readStatusOfDiagnosticTroubleCodes     = 0x17,
  SID_readDiagnosticTroubleCodesByStatus     = 0x18,
  SID_readEcuIdentification                  = 0x1A,
  SID_stopDiagnosticSession                  = 0x20,
  SID_readDataByLocalIdentifier              = 0x21,
  SID_readDataByCommonIdentifier             = 0x22,
  SID_readMemoryByAddress                    = 0x23,
  SID_setDataRates                           = 0x26,
  SID_securityAccess                         = 0x27,
  SID_DynamicallyDefineLocalIdentifier       = 0x2C,
  SID_writeDataByCommonIdentifier            = 0x2E,
  SID_inputOutputControlByCommonIdentifier   = 0x2F,
  SID_inputOutputControlByLocalIdentifier    = 0x30,
  SID_startRoutineByLocalIdentifier          = 0x31,
  SID_stopRoutineByLocalIdentifier           = 0x32,
  SID_requestRoutineResultsByLocalIdentifier = 0x33,
  SID_requestDownload                        = 0x34,
  SID_requestUpload                          = 0x35,
  SID_transferData                           = 0x36,
  SID_requestTransferExit                    = 0x37,
  SID_startRoutineByAddress                  = 0x38,
  SID_stopRoutineByAddress                   = 0x39,
  SID_requestRoutineResultsByAddress         = 0x3A,
  SID_writeDataByLocalIdentifier             = 0x3B,
  SID_writeMemoryByAddress                   = 0x3D,
  SID_testerPresent                          = 0x3E,
  SID_escCode /* (Not part of Diagnostic Services Specification; KWP 2000 only)*/ = 0x80,
//------------------------------ manufacturer specific
  SID_startCommunication                     = 0x81,
  SID_stopCommunication                      = 0x82,
};

enum class SID_NR: uint8_t
{
  SID_NR_startDiagnosticSession                 = 0x50,
  SID_NR_ecuReset                               = 0x51,
  SID_NR_readFreezeFrameData                    = 0x52,
  SID_NR_readDiagnosticTroubleCodes             = 0x53,
  SID_NR_clearDiagnosticInformation             = 0x54,
  SID_NR_readStatusOfDiagnosticTroubleCodes     = 0x57,
  SID_NR_readDiagnosticTroubleCodesByStatus     = 0x58,
  SID_NR_readEcuIdentification                  = 0x5A,
  SID_NR_stopDiagnosticSession                  = 0x60,
  SID_NR_readDataByLocalIdentifier              = 0x61,
  SID_NR_readDataByCommonIdentifier             = 0x62,
  SID_NR_readMemoryByAddress                    = 0x63,
  SID_NR_setDataRates                           = 0x66,
  SID_NR_securityAccess                         = 0x67,
  SID_NR_DynamicallyDefineLocalIdentifier       = 0x6C,
  SID_NR_writeDataByCommonIdentifier            = 0x6E,
  SID_NR_inputOutputControlByCommonIdentifier   = 0x6F,
  SID_NR_inputOutputControlByLocalIdentifier    = 0x70,
  SID_NR_startRoutineByLocalIdentifier          = 0x71,
  SID_NR_stopRoutineByLocalIdentifier           = 0x72,
  SID_NR_requestRoutineResultsByLocalIdentifier = 0x73,
  SID_NR_requestDownload                        = 0x74,
  SID_NR_requestUpload                          = 0x75,
  SID_NR_transferData                           = 0x76,
  SID_NR_requestTransferExit                    = 0x77,
  SID_NR_startRoutineByAddress                  = 0x78,
  SID_NR_stopRoutineByAddress                   = 0x79,
  SID_NR_requestRoutineResultsByAddress         = 0x7A,
  SID_NR_writeDataByLocalIdentifier             = 0x7B,
  SID_NR_writeMemoryByAddress                   = 0x7D,
  SID_NR_testerPresent                          = 0x7E,
  SID_NR_escCode /* (Not part of Diagnostic Services Specification; KWP 2000 only)*/ = 0xC0,
  SID_NR_startCommunication                     = 0xC1,
  SID_NR_stopCommunication                      = 0xC2
};


#endif //__SID_H__