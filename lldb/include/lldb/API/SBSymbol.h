//===-- SBSymbol.h ----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_SBSymbol_h_
#define LLDB_SBSymbol_h_

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBAddress.h"
#include "lldb/API/SBInstructionList.h"
#include "lldb/API/SBTarget.h"

namespace lldb {

class SBSymbol
{
public:

    SBSymbol ();

    ~SBSymbol ();

    SBSymbol (const lldb::SBSymbol &rhs);

    const lldb::SBSymbol &
    operator = (const lldb::SBSymbol &rhs);

    bool
    IsValid () const;


    const char *
    GetName() const;

    const char *
    GetMangledName () const;

    lldb::SBInstructionList
    GetInstructions (lldb::SBTarget target);

    SBAddress
    GetStartAddress ();
    
    SBAddress
    GetEndAddress ();
    
    uint32_t
    GetPrologueByteSize ();

    SymbolType
    GetType ();

    bool
    operator == (const lldb::SBSymbol &rhs) const;

    bool
    operator != (const lldb::SBSymbol &rhs) const;

    bool
    GetDescription (lldb::SBStream &description);

protected:

    lldb_private::Symbol *
    get ();

    void
    reset (lldb_private::Symbol *);
    
private:
    friend class SBAddress;
    friend class SBFrame;
    friend class SBModule;
    friend class SBSymbolContext;

    SBSymbol (lldb_private::Symbol *lldb_object_ptr);
    
    void
    SetSymbol (lldb_private::Symbol *lldb_object_ptr);

    lldb_private::Symbol *m_opaque_ptr;
};


} // namespace lldb

#endif // LLDB_SBSymbol_h_
