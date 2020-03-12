//===-- TriCoreRegisterBankInfo.h -------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file declares the targeting of the RegisterBankInfo class for TriCore.
/// \todo This should be generated by TableGen.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TRICORE_TRICOREREGISTERBANKINFO_H
#define LLVM_LIB_TARGET_TRICORE_TRICOREREGISTERBANKINFO_H

#include "llvm/CodeGen/GlobalISel/RegisterBankInfo.h"

#define GET_REGBANK_DECLARATIONS
#include "TriCoreGenRegisterBank.inc"

namespace llvm {

class TargetRegisterInfo;

class TriCoreGenRegisterBankInfo : public RegisterBankInfo {
protected:
  enum PartialMappingIdx {
    PMI_None = -1,
    PMI_DataReg = 1,
    PMI_ExtDataReg,
    PMI_AddrReg,
    PMI_ExtAddrReg,
    PMI_CarryReg,
    PMI_FirstDataReg = PMI_DataReg,
    PMI_LastDataReg = PMI_ExtDataReg,
    PMI_FirstAddrReg = PMI_AddrReg,
    PMI_LastAddrReg = PMI_ExtAddrReg,
    PMI_FirstCarryReg = PMI_CarryReg,
    PMI_LastCarryReg = PMI_CarryReg,
    PMI_Min = PMI_FirstDataReg,
  };

  static RegisterBankInfo::PartialMapping PartMappings[];
  static RegisterBankInfo::ValueMapping ValMappings[];
  static PartialMappingIdx BankIDToCopyMapIdx[];

  enum ValueMappingIdx {
    InvalidIdx = 0,
    First3OpsIdx = 1,
    Last3OpsIdx = 10,
    DistanceBetweenRegBanks = 3,
    TruncDataRegBankIdx = 13,
    TruncAddrRegBankIdx = 15,
    CarryBitIdx = 17,
  };

  // Helper functions to check expectations about the maps and indices.
  static bool checkPartialMap(unsigned Idx, unsigned ValStartIdx,
                              unsigned ValLength, const RegisterBank &RB);
  static bool checkValueMapImpl(unsigned Idx, unsigned FirstInBank,
                                unsigned Size, unsigned Offset);
  static bool checkCarryMapImpl(unsigned Idx, unsigned Offset);
  static bool checkPartialMappingIdx(PartialMappingIdx FirstAlias,
                                     PartialMappingIdx LastAlias,
                                     ArrayRef<PartialMappingIdx> Order);

  /// Return the offset for the value mappings array for the given register bank
  /// \p RBIdx and \p Size. The returned offset is relative to \p RBIdx.
  static unsigned getRegBankBaseIdxOffset(unsigned RBIdx, unsigned Size);

  /// Get the pointer to the ValueMapping representing the RegisterBank
  /// at \p RBIdx with a size of \p Size.
  ///
  /// The returned mapping works for instructions with the same kind of operands
  /// for up to 3 operands.
  ///
  /// \pre \p RBIdx != PartialMappingIdx::None
  static const RegisterBankInfo::ValueMapping *
  getValueMapping(PartialMappingIdx RBIdx, unsigned Size);

  /// Get the pointer to the ValueMapping of the operands of a copy instruction
  /// from the \p SrcBankID register bank to the \p DstBankID register bank with
  /// a size of \p Size.
  static const RegisterBankInfo::ValueMapping *
  getCopyMapping(unsigned DstBankID, unsigned SrcBankID, unsigned Size);

  /// Get the pointer to the ValueMapping of the operands of a trunc instruction
  /// on the \p BankID register bank from size \p SrcSize to size \p DstSize.
  static const RegisterBankInfo::ValueMapping *
  getTruncMapping(unsigned BankID, unsigned DstSize, unsigned SrcSize);

  /// Get the pointer to the ValueMapping of the operands of an
  /// add/sub-with-carry instruction at \p RbIdx with a size of \p Size.
  ///
  /// The returned mapping works for add/sub instructions with a carry-in/-out
  /// and same kind of non-carry operands for up to 5 operands.
  static const RegisterBankInfo::ValueMapping *getCarryMapping();

#define GET_TARGET_REGBANK_CLASS
#include "TriCoreGenRegisterBank.inc"
};

/// This class provides the information for the target register banks.
class TriCoreRegisterBankInfo final : public TriCoreGenRegisterBankInfo {

  /// Get an instruction mapping where all the operands map to the same register
  /// bank and have similar size.
  ///
  /// \pre MI.getNumOperands() <= 3
  ///
  /// \return An InstructionMapping with a statically allocated OperandsMapping
  const InstructionMapping &
  getSameKindOfOperandsMapping(const MachineInstr &MI) const;

  /// Get an instruction mapping for one of the instructions with a carry-in
  /// or carry-out. The non-carry operands map to the data regbank and have
  /// similar size.
  ///
  /// \pre hasCarryBit(MI) == true && MI.getNumOperands() <= 5
  ///
  /// \return An InstructionMapping with a statically allocated OperandsMapping
  const InstructionMapping &getCarryInstMapping(const MachineInstr &MI) const;

public:
  explicit TriCoreRegisterBankInfo(const TargetRegisterInfo &TRI);

  const RegisterBank &getRegBankFromRegClass(const TargetRegisterClass &RC,
                                             LLT) const override;

  const InstructionMapping &
  getInstrMapping(const MachineInstr &MI) const override;
};
} // end namespace llvm
#endif // LLVM_LIB_TARGET_TRICORE_TRICOREREGISTERBANKINFO_H
