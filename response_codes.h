#ifndef __RESPONSE_CODES_H__
#define __RESPONSE_CODES_H__

enum class NegResponseCodes
{
  NRC_generalReject                                = 0x10, 
  NRC_serviceNotSupported                          = 0x11,
  NRC_subFunctionNotSupported_invalidFormat        = 0x12,
  NRC_busy_RepeatRequest                           = 0x21,
  NRC_conditionsNotCorrect_or_requestSequenceError = 0x22,
  NRC_routineNotComplete                           = 0x23,
  NRC_requestOutOfRange                            = 0x31,
  NRC_securityAccessDenied                         = 0x33,
  NRC_invalidKey                                   = 0x35,
  NRC_exceedNumberOfAttempts                       = 0x36,
  NRC_requiredTimeDelayNotExpired                  = 0x37,
  NRC_downloadNotAccepted                          = 0x40,
  NRC_improperDownloadType                         = 0x41,
  NRC_cannotDownloadToSpecifiedAddress             = 0x42,
  NRC_cannotDownloadNumberOfBytesRequested         = 0x43,
  NRC_uploadNotAccepted                            = 0x50,
  NRC_improperUploadType                           = 0x51,
  NRC_cannotUploadFromSpecifiedAddress             = 0x52,
  NRC_cannotUploadNumberOfBytesRequested           = 0x53,
  NRC_transferSuspended                            = 0x71,
  NRC_transferAborted                              = 0x72,
  NRC_illegalAddressInBlockTransfer                = 0x74,
  NRC_illegalByteCountInBlockTransfer              = 0x75,
  NRC_illegalBlockTransferType                     = 0x76,
  NRC_blockTransferDataChecksumError               = 0x77,
  NRC_requestCorrectlyReceived_ResponsePending     = 0x78,
  NRC_incorrectByteCountDuringBlockTransfer        = 0x79
  //0x80 - 0xFF manufacturerSpecificCodes
};

#endif //__RESPONSE_CODES_H__
