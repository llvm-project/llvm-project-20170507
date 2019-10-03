"""
Test lldb Python API for file handles.
"""

from __future__ import print_function

import os
import io
import re
import sys
from contextlib import contextmanager

import lldb
from lldbsuite.test import  lldbtest
from lldbsuite.test.decorators import add_test_categories, no_debug_info_test


def readStrippedLines(f):
    def i():
        for line in f:
            line = line.strip()
            if line:
                yield line
    return list(i())


class FileHandleTestCase(lldbtest.TestBase):

    mydir = lldbtest.Base.compute_mydir(__file__)

    # The way this class interacts with the debugger is different
    # than normal.   Most of these test cases will mess with the
    # debugger I/O streams, so we want a fresh debugger for each
    # test so those mutations don't interfere with each other.
    #
    # Also, the way normal tests evaluate debugger commands is
    # by using a SBCommandInterpreter directly, which captures
    # the output in a result object.   For many of tests tests
    # we want the debugger to write the  output directly to
    # its I/O streams like it would have done interactively.
    #
    # For this reason we also define handleCmd() here, even though
    # it is similar to runCmd().

    def setUp(self):
        super(FileHandleTestCase, self).setUp()
        self.debugger = lldb.SBDebugger.Create()
        self.out_filename = self.getBuildArtifact('output')
        self.in_filename = self.getBuildArtifact('input')

    def tearDown(self):
        lldb.SBDebugger.Destroy(self.debugger)
        super(FileHandleTestCase, self).tearDown()
        for name in (self.out_filename, self.in_filename):
            if os.path.exists(name):
                os.unlink(name)

    # Similar to runCmd(), but this uses the per-test debugger, and it
    # supports, letting the debugger just print the results instead
    # of collecting them.
    def handleCmd(self, cmd, check=True, collect_result=True):
        assert not check or collect_result
        ret = lldb.SBCommandReturnObject()
        if collect_result:
            interpreter = self.debugger.GetCommandInterpreter()
            interpreter.HandleCommand(cmd, ret)
        else:
            self.debugger.HandleCommand(cmd)
        if collect_result and check:
            self.assertTrue(ret.Succeeded())
        return ret.GetOutput()


    @add_test_categories(['pyapi'])
    @no_debug_info_test
    def test_legacy_file_out_script(self):
        with open(self.out_filename, 'w') as f:
            self.debugger.SetOutputFileHandle(f, False)
            # scripts print to output even if you capture the results
            # I'm not sure I love that behavior, but that's the way
            # it's been for a long time.  That's why this test works
            # even with collect_result=True.
            self.handleCmd('script 1+1')
            self.debugger.GetOutputFileHandle().write('FOO\n')
        lldb.SBDebugger.Destroy(self.debugger)
        with open(self.out_filename, 'r') as f:
            self.assertEqual(readStrippedLines(f), ['2', 'FOO'])


    @add_test_categories(['pyapi'])
    @no_debug_info_test
    def test_legacy_file_out(self):
        with open(self.out_filename, 'w') as f:
            self.debugger.SetOutputFileHandle(f, False)
            self.handleCmd('p/x 3735928559', collect_result=False, check=False)
        lldb.SBDebugger.Destroy(self.debugger)
        with open(self.out_filename, 'r') as f:
            self.assertIn('deadbeef', f.read())


    @add_test_categories(['pyapi'])
    @no_debug_info_test
    def test_legacy_file_err(self):
        with open(self.out_filename, 'w') as f:
            self.debugger.SetErrorFileHandle(f, False)
            self.handleCmd('lol', check=False, collect_result=False)
        lldb.SBDebugger.Destroy(self.debugger)
        with open(self.out_filename, 'r') as f:
            self.assertIn("is not a valid command", f.read())


    @add_test_categories(['pyapi'])
    @no_debug_info_test
    def test_sbfile_type_errors(self):
        sbf = lldb.SBFile()
        self.assertRaises(TypeError, sbf.Write, None)
        self.assertRaises(TypeError, sbf.Read, None)
        self.assertRaises(TypeError, sbf.Read, b'this bytes is not mutable')
        self.assertRaises(TypeError, sbf.Write, u"ham sandwich")
        self.assertRaises(TypeError, sbf.Read, u"ham sandwich")


    @add_test_categories(['pyapi'])
    @no_debug_info_test
    def test_sbfile_write(self):
        with open(self.out_filename, 'w') as f:
            sbf = lldb.SBFile(f.fileno(), "w", False)
            self.assertTrue(sbf.IsValid())
            e, n = sbf.Write(b'FOO\nBAR')
            self.assertTrue(e.Success())
            self.assertEqual(n, 7)
            sbf.Close()
            self.assertFalse(sbf.IsValid())
        with open(self.out_filename, 'r') as f:
            self.assertEqual(readStrippedLines(f), ['FOO', 'BAR'])


    @add_test_categories(['pyapi'])
    @no_debug_info_test
    def test_sbfile_read(self):
        with open(self.out_filename, 'w') as f:
            f.write('FOO')
        with open(self.out_filename, 'r') as f:
            sbf = lldb.SBFile(f.fileno(), "r", False)
            self.assertTrue(sbf.IsValid())
            buffer = bytearray(100)
            e, n = sbf.Read(buffer)
            self.assertTrue(e.Success())
            self.assertEqual(buffer[:n], b'FOO')

