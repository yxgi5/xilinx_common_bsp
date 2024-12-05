#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from socket import *
import struct

def recv_msg(data,s,addr):
	if data == b'':
		return
	
	recv_lenth=struct.unpack('H', data[0:2])[0]
	print(recv_lenth)
	recv_cmd=data[4]
	print(hex(recv_cmd))
	recv_addr=struct.unpack('I',data[5:9])[0]
	print(hex(recv_addr))
	recv_value=struct.unpack('I',data[9:13])[0]
	print(recv_value)
	
	if recv_cmd == 0x10:
		print("read cmd")
		pass
	elif recv_cmd == 0x11:
		print("write cmd")
		pass
	
	try:
		print("sending ", data)
		s.send(data)
		# s.send(b'Thank you for connecting')
	except Exception as ret:
		print('发送失败\n')
		raise Exception(ret)
	pass

def main():

	host="0.0.0.0"
	port = 5555
	s = socket(AF_INET, SOCK_STREAM)
	s.bind((host,port))
	s.listen(5)
	
	addr = (host,port)
	print(addr)

	buf=1024

	while True:
		#s.settimeout(2)
		conn, addr = s.accept()
		print(f"Accepted connection from {addr[0]}:{addr[1]}")

		while True:
			try:
				data = conn.recv(buf)
			except Exception as ret:
				print(ret)
				#pass
				raise Exception(ret)

			else:
				# if data == None:
				if data == b'':
					# print("data is None")
					break

				else:
					print(data)
					print(addr)
					recv_msg(data,conn,addr)
					# conn.send(b'Thank you for connecting')
					# print("sent done")

		print("Connection to client closed")		
		conn.close()

if __name__ == '__main__':
    main()
