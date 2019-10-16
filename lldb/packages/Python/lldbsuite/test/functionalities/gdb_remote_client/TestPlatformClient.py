import lldb
import binascii
import os
from lldbsuite.test.lldbtest import *
from lldbsuite.test.decorators import *
from gdbclientutils import *

def hexlify(string):
    return binascii.hexlify(string.encode()).decode()

class TestPlatformClient(GDBRemoteTestBase):

    def test_process_list_with_all_users(self):
        """Test connecting to a remote linux platform"""

        class MyResponder(MockGDBServerResponder):
            def __init__(self):
                MockGDBServerResponder.__init__(self)
                self.currentQsProc = 0
                self.all_users = False

            def qfProcessInfo(self, packet):
                if "all_users:1" in packet:
                    self.all_users = True
                    name = hexlify("/a/test_process")
                    args = "-".join(map(hexlify,
                                        ["/system/bin/sh", "-c", "/data/local/tmp/lldb-server"]))
                    return "pid:10;ppid:1;uid:2;gid:3;euid:4;egid:5;name:" + name + ";args:" + args + ";"
                else:
                    self.all_users = False
                    return "E04"

            def qsProcessInfo(self):
                if self.all_users:
                    if self.currentQsProc == 0:
                        self.currentQsProc = 1
                        name = hexlify("/b/another_test_process")
                        # This intentionally has a badly encoded argument
                        args = "X".join(map(hexlify,
                                            ["/system/bin/ls", "--help"]))
                        return "pid:11;ppid:2;uid:3;gid:4;euid:5;egid:6;name:" + name + ";args:" + args + ";"
                    elif self.currentQsProc == 1:
                        self.currentQsProc = 0
                        return "E04"
                else:
                    return "E04"

        self.server.responder = MyResponder()

        try:
            self.runCmd("platform select remote-linux")
            self.runCmd("platform connect connect://localhost:%d" %
                        self.server.port)
            self.assertTrue(self.dbg.GetSelectedPlatform().IsConnected())
            self.expect("platform process list -x",
                        substrs=["2 matching processes were found", "test_process", "another_test_process"])
            self.expect("platform process list -xv",
                        substrs=[
                            "PID    PARENT USER       GROUP      EFF USER   EFF GROUP  TRIPLE                         ARGUMENTS",
                            "10     1      2          3          4          5                                         /system/bin/sh -c /data/local/tmp/lldb-server",
                            "11     2      3          4          5          6"])
            self.expect("platform process list -xv", substrs=["/system/bin/ls"], matching=False)
            self.expect("platform process list",
                        error=True,
                        substrs=["error: no processes were found on the \"remote-linux\" platform"])
        finally:
            self.dbg.GetSelectedPlatform().DisconnectRemote()
