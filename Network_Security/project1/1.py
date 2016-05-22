#!/usr/bin/env python
from pwn import *
ip = "nsproj.bitisle.net"
port = 30008
s = remote(ip, port)
buf = s.recvuntil('username:')
print buf
payload = "demo"
s.sendline(payload)

buf = s.recvuntil('password:')
print buf
payload = "readmyname"
s.sendline(payload)
s.interactive()
