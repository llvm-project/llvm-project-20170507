//===-- RegisterContextPOSIXCore_arm.cpp ------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "RegisterContextPOSIXCore_arm.h"

#include "lldb/Target/Thread.h"
#include "lldb/Utility/RegisterValue.h"

using namespace lldb_private;

RegisterContextCorePOSIX_arm::RegisterContextCorePOSIX_arm(
    Thread &thread, RegisterInfoInterface *register_info,
    const DataExtractor &gpregset, llvm::ArrayRef<CoreNote> notes)
    : RegisterContextPOSIX_arm(thread, 0, register_info) {
  m_gpr_buffer.reset(
      new DataBufferHeap(gpregset.GetDataStart(), gpregset.GetByteSize()));
  m_gpr.SetData(m_gpr_buffer);
  m_gpr.SetByteOrder(gpregset.GetByteOrder());
}

RegisterContextCorePOSIX_arm::~RegisterContextCorePOSIX_arm() {}

bool RegisterContextCorePOSIX_arm::ReadGPR() { return true; }

bool RegisterContextCorePOSIX_arm::ReadFPR() { return false; }

bool RegisterContextCorePOSIX_arm::WriteGPR() {
  assert(0);
  return false;
}

bool RegisterContextCorePOSIX_arm::WriteFPR() {
  assert(0);
  return false;
}

bool RegisterContextCorePOSIX_arm::ReadRegister(const RegisterInfo *reg_info,
                                                RegisterValue &value) {
  lldb::offset_t offset = reg_info->byte_offset;
  uint64_t v = m_gpr.GetMaxU64(&offset, reg_info->byte_size);
  if (offset == reg_info->byte_offset + reg_info->byte_size) {
    value = v;
    return true;
  }
  return false;
}

bool RegisterContextCorePOSIX_arm::ReadAllRegisterValues(
    lldb::DataBufferSP &data_sp) {
  return false;
}

bool RegisterContextCorePOSIX_arm::WriteRegister(const RegisterInfo *reg_info,
                                                 const RegisterValue &value) {
  return false;
}

bool RegisterContextCorePOSIX_arm::WriteAllRegisterValues(
    const lldb::DataBufferSP &data_sp) {
  return false;
}

bool RegisterContextCorePOSIX_arm::HardwareSingleStep(bool enable) {
  return false;
}
