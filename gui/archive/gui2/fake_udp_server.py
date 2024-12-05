#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from socket import *
import struct

def recv_msg(data,s,addr):
	recv_lenth=struct.unpack('H', data[0:2])[0]
	print(recv_lenth)
	recv_cmd=data[4]
	print(hex(recv_cmd))
	recv_addr=struct.unpack('I',data[5:9])[0]
	print(hex(recv_addr))
	recv_value=struct.unpack('I',data[9:13])[0]
	print(recv_value)
	
	if recv_cmd == 0x10:
		
		pass
	elif recv_cmd == 0x11:
		
		pass
	
	try:
		s.sendto(data, addr)
	except Exception as ret:
		print('发送失败\n')
		raise Exception(ret)
	pass

def main():

	host="0.0.0.0"
	port = 5555
	s = socket(AF_INET,SOCK_DGRAM)
	s.bind((host,port))

	addr = (host,port)
	print(addr)

	buf=1024
	#s.settimeout(2)

	try:
		while True:
			#s.settimeout(2)
			data,addr = s.recvfrom(buf)
			if data is None:
				break
			else:
				print(data)
				print(addr)
				recv_msg(data,s,addr)

	except Exception as ret:
		print(ret)
		pass		
				
	#except timeout:
		#print("timeout")

if __name__ == '__main__':
    main()
