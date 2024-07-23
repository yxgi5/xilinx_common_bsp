#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import time
import struct
import udp_client
import memory_ops

def main():
    ip='192.168.1.10'
    ut = udp_client.client(server_host=ip)
    
    m1=memory_ops.memory_ops(ut)
    status=m1.read_reg(0x81000000)
    if status[0]:
        print("ok, 0x%x"%status[1])
    else:
        print("ng")
    
    del m1
    
    value = status[1] + 1;
    
    m2=memory_ops.memory_ops(ut)
    status=m2.write_reg(0x81000000, value)
    if status[0]:
        print("write ok")
    else:
        print("write ng")
    del m2

    m3=memory_ops.memory_ops(ut)
    status=m3.read_reg(0x81000000)
    if status[0]:
        print("ok, 0x%x"%status[1])
    else:
        print("ng")
    del m3

    # cnt = 252
    # cnt = 270
    # cnt = 375 # MTU=1500 maximum size of the Ethernet frame’s payload. Payload better less than 1472 or 1480
    # cnt = 400
    # cnt = 500
    # cnt = 700

    # m4=memory_ops.memory_ops(ut)
    # # If packet fragmentation is allowed, a packet can be up to 65,535 bytes. It would just be split into multiple frames and sent
    # result = m4.read_bulk(0x81000000, cnt)
    # print(type(result))
    # print(result)
    # for index in range(cnt):
    #     print(hex(result[index]))
    # del m4

    # print("send data:")
    # send = []
    # m5=memory_ops.memory_ops(ut)
    # for index in range(cnt):
    #     send.append(result[index]+1)
    #     print(hex(send[index]))
    
    # res=m5.write_bulk(0x81000000, send)
    # print(res)
    # del m5

    # m6=memory_ops.memory_ops(ut)
    # result = m6.read_bulk(0x81000000, cnt)
    # print(type(result))
    # print(result)
    # for index in range(cnt):
    #     print(hex(result[index]))
    # del m6
    
    ut.close()
    print('main end')

if __name__ == '__main__':
    main()
