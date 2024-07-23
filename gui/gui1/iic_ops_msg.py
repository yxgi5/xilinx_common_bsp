#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import time
import struct
import udp_logic

class iic_ops_msg(): 
    def __init__(self,comm_instance=None):
        if(comm_instance==None):
            raise Exception("No communication instance")
        else:
            self.ut = comm_instance
            
    def read_prepare(self, channel,length, slave_addr,reg_addr,flag):
        msg = []
        # 消息长占位
        msg.insert(0, 0x00)
        msg.insert(1, 0x00)
        
        # 预留段
        msg.insert(2, 0x00)
        msg.insert(3, 0x00)
        
        # 消息代码
        msg.insert(4, 0x20)
            
        msg.insert(5, channel)
        msg.insert(6, slave_addr)
        msg.insert(7, length)
        msg.insert(8, reg_addr>>8)
        msg.insert(9, reg_addr&0xFF)
        msg.insert(10, flag)
        
        # 消息长
        msg_len=len(msg)+1
        msg[0] = (msg_len&0xff)
        msg[1] = ((msg_len>>8)&0xff)
        
        # 校验和
        checsum = ((~(sum(msg) & 0xff))&0xff)
        msg.append(checsum)

        return struct.pack('B'*len(msg), *msg)
        
            
    def wirte_prepare(self, channel, buf, length, slave_addr,reg_adrr,flag):
        msg = []    
        # 消息长占位
        msg.insert(0, 0x00)
        msg.insert(1, 0x00)
        # 预留段
        msg.insert(2, 0x00)
        msg.insert(3, 0x00)
        # 消息代码
        msg.insert(4, 0x21)

        msg.insert(5, channel)
        msg.insert(6, slave_addr)
        msg.insert(7, reg_adrr>>8)
        msg.insert(8, reg_adrr&0xff)
        msg.insert(9, flag)
        msg.insert(10, length)
        for index in range(length):
            msg.append(buf[index])
            
        # 消息长
        msg_len=len(msg)+1
        msg[0] = (msg_len&0xff)
        msg[1] = ((msg_len>>8)&0xff)
        
        # 校验和
        checsum = ((~(sum(msg) & 0xff))&0xff)
        msg.append(checsum)
               
        return struct.pack('B'*len(msg), *msg)
    
        
    def query_prepare(self, iic_channel,iic_addr):
        msg = []     
        # 消息长占位
        msg.insert(0, 0x00)
        msg.insert(1, 0x00)
        
        # 预留段
        msg.insert(2, 0x00)
        msg.insert(3, 0x00)
        
        # 消息代码
        msg.insert(4, 0x22)
        msg.insert(5,iic_channel)
        msg.append(iic_addr&0xff)
        
        # 消息长
        msg_len=len(msg)+1
        msg[0] = (msg_len&0xff)
        msg[1] = ((msg_len>>8)&0xff)
        
        # 校验和
        checsum = ((~(sum(msg) & 0xff))&0xff)
        msg.append(checsum)
       
        return struct.pack('B'*len(msg), *msg)
        
    def recv_msg_prepare(self, msg):
        recv_msg=msg
        value=struct.unpack('B'*len(recv_msg), recv_msg)
        return value
        
    def getdata_prepare(self):
        msg = []
        # 消息长占位
        msg.insert(0, 0x00)
        msg.insert(1, 0x00)
        
        # 预留段
        msg.insert(2, 0x00)
        msg.insert(3, 0x00)
        
        # 消息代码
        msg.insert(4, 0x25)
        
        # 消息长
        msg_len=len(msg)+1
        msg[0] = (msg_len&0xff)
        msg[1] = ((msg_len>>8)&0xff)
        
        # 校验和
        checsum = ((~(sum(msg) & 0xff))&0xff)
        msg.append(checsum)
        
        return struct.pack('B'*len(msg), *msg)
            
    def iic_send_data(self, channel, buf, length, slave_addr,reg_addr,flag):
        # 打包发送消息
        msg=self.wirte_prepare(channel, buf, length, slave_addr,reg_addr,flag)
        self.ut.udp_send(msg)
        status = self.ut.recieved_even.wait(0.5)
        if status is not True:
            raise Exception("Recv timeout")
        self.ut.recieved_even.clear() # must clear

        recv_msg=self.recv_msg_prepare(self.ut.recv_msg)
        # status = recv_msg[5]
        # if status is not 0x00:
        #     raise Exception("Recv status error")
        return recv_msg#[5:9+length]
      
    def iic_recv_data(self, channel,length, slave_addr,reg_addr,flag):
        '''      
        '''
        # 打包发送消息
        msg=self.read_prepare(channel,length, slave_addr,reg_addr,flag)
        self.ut.udp_send(msg)
        status = self.ut.recieved_even.wait(0.5)
        if status is not True:
            raise Exception("Recv timeout")
        self.ut.recieved_even.clear() # must clear

        recv_msg=self.recv_msg_prepare(self.ut.recv_msg)
        # status = recv_msg[5]
        # if status is not 0x00:
        #     raise Exception("Recv status error")  
        return recv_msg#[9:9+length]

    def iic_query(self, iic_channel,iic_addr):
        # 打包发送消息
        msg=self.query_prepare(iic_channel,iic_addr)
        self.ut.udp_send(msg)
        status = self.ut.recieved_even.wait(0.5)
        if status is not True:
            raise Exception("Recv timeout")
        self.ut.recieved_even.clear() # must clear
        recv_msg=self.recv_msg_prepare(self.ut.recv_msg)
        data1 = recv_msg[5:6][0]    
        return data1
