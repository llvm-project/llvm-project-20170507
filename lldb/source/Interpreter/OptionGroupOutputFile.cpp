//===-- OptionGroupOutputFile.cpp -------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lldb/Interpreter/OptionGroupOutputFile.h"

// C Includes
// C++ Includes
// Other libraries and framework includes
// Project includes

using namespace lldb;
using namespace lldb_private;

OptionGroupOutputFile::OptionGroupOutputFile() :
    m_file (),
    m_append (false, false)
{
}

OptionGroupOutputFile::~OptionGroupOutputFile ()
{
}

static OptionDefinition
g_option_table[] =
{
{ LLDB_OPT_SET_1 , false, "outfile", 'o', required_argument, NULL, 0, eArgTypePath , "Specify a path for capturing command output."},
{ LLDB_OPT_SET_1 , false, "append-outfile" , 'A', no_argument, NULL, 0, eArgTypeNone , "Append to the the file specified with '--outfile <path>'."},
};

const uint32_t k_num_file_options = sizeof(g_option_table)/sizeof(OptionDefinition);

uint32_t
OptionGroupOutputFile::GetNumDefinitions ()
{
    return k_num_file_options;
}

const OptionDefinition *
OptionGroupOutputFile::GetDefinitions ()
{
    return g_option_table;
}

Error
OptionGroupOutputFile::SetOptionValue (CommandInterpreter &interpreter,
                                         uint32_t option_idx,
                                         const char *option_arg)
{
    Error error;
    char short_option = (char) g_option_table[option_idx].short_option;

    switch (short_option)
    {
        case 'o':
            error = m_file.SetValueFromCString (option_arg);
            break;

        case 'A':
            m_append.SetCurrentValue(true);
            break;

        default:
            error.SetErrorStringWithFormat ("Unrecognized option '%c'.\n", short_option);
            break;
    }

    return error;
}

void
OptionGroupOutputFile::OptionParsingStarting (CommandInterpreter &interpreter)
{
    m_file.Clear();
    m_append.Clear();
}
