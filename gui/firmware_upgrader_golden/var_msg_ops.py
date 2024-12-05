#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import time
import struct
# import udp_client

class var_msg_ops():
    def __init__(self, comm_instance=None):
        if(comm_instance==None):
            raise Exception("No communication instance")
        else:    
            self.ut = comm_instance
            
    def read_var_prepare(self,  index):
        msg = []
        
        # 消息长占位
        msg.insert(0, 0x00)
        msg.insert(1, 0x00)
        
        # 预留段
        msg.insert(2, 0x00)
        msg.insert(3, 0x00)
        
        # 消息代码
        msg.insert(4, 0x40)
        
        # 消息体
        msg.insert(5, index&0xff)
        msg.insert(6, (index>>8)&0xff)
        
        # 消息长
        msg_len=len(msg)+1
        msg[0] = (msg_len&0xff)
        msg[1] = ((msg_len>>8)&0xff)
        
        # 校验和
        checsum = ((~(sum(msg) & 0xff))&0xff)
        msg.append(checsum)

        return struct.pack('B'*len(msg), *msg)    
    
    def recv_msg_prepare(self, msg):      
        # 0x7e 0x00 转义回 0x7e
        
       # recv_msg=msg.replace(b'\x7e\x00', b'\x7e')
        recv_msg=msg
        
        #value=struct.unpack('B'*len(recv_msg), recv_msg)
        #return value
        return recv_msg
    
    def read_var(self, index):
        self.index=index
        self.msg = self.read_var_prepare(index)
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
            
        status = self.ut.recieved_even.wait(0.1)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)#0
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        if(self.index==0)  or (self.index==1) or (self.index==2) or (self.index==4):
            data1=recv_msg[7:11]
            value=struct.unpack('I',data1)[0]
            result.append(value)
        elif(self.index==3):
            read_len = len(recv_msg)-8
            data1=recv_msg[7:7+read_len]
            # print(data1)
            # value=struct.unpack('6b',data1)[0]
            result.append(data1)
        elif(self.index==10) or (self.index==12) :
            if(len(recv_msg)<36):
                result[0] = False
                return result
                
            data1=recv_msg[7:15] # reserved
            # value=struct.unpack('L',data1)[0]
            value=struct.unpack('II',data1)[0]
            result.append(value) # 0
                
            ###################
            data1=recv_msg[15:19] # ipaddr
            # value=struct.unpack('I',data1)[0]
            result.append(data1) #1

            ##################
            data1=recv_msg[19:23] # mask
            # value=struct.unpack('I',data1)[0]
            result.append(data1) #2
                
            #########################
            data1=recv_msg[23:27] # gw
            # value=struct.unpack('I',data1)[0]
            result.append(data1) #3

            #########################
            data1=recv_msg[27:33]  # mac
            value=struct.unpack('BBBBBB', data1)
            result.append(value) #4

        return result
        
    def write_config_prepare(self, buf):
        msg = []
        
        index = 3
        
        # 消息长占位
        msg.insert(0, 0x00)
        msg.insert(1, 0x00)
        
        # 预留段
        msg.insert(2, 0x00)
        msg.insert(3, 0x00)
        
        # 消息代码
        msg.insert(4, 0x40)
        
        # 消息体
        msg.insert(5, index&0xff)
        msg.insert(6, (index>>8)&0xff)

        msg.insert(7, buf[0]&0xff)
        msg.insert(8, (buf[0]>>8)&0xff)
        msg.insert(9, (buf[0]>>16)&0xff)
        msg.insert(10, (buf[0]>>24)&0xff)
        msg.insert(11, (buf[0]>>32)&0xff)
        msg.insert(12, (buf[0]>>40)&0xff)
        msg.insert(13, (buf[0]>>48)&0xff)
        msg.insert(14, (buf[0]>>56)&0xff)

        msg.insert(15, buf[1]&0xff)
        msg.insert(16, (buf[1]>>8)&0xff)
        msg.insert(17, (buf[1]>>16)&0xff)
        msg.insert(18, (buf[1]>>24)&0xff)
        
        msg.insert(19, buf[2]&0xff)
        msg.insert(20, (buf[2]>>8)&0xff)
        msg.insert(21, (buf[2]>>16)&0xff)
        msg.insert(22, (buf[2]>>24)&0xff)
        
        msg.insert(23, buf[3]&0xff)
        msg.insert(24, (buf[3]>>8)&0xff)
        msg.insert(25, (buf[3]>>16)&0xff)
        msg.insert(26, (buf[3]>>24)&0xff)
        
        msg.insert(27, (buf[4]>>40)&0xff)
        msg.insert(28, (buf[4]>>32)&0xff)
        msg.insert(29, (buf[4]>>24)&0xff)
        msg.insert(30, (buf[4]>>16)&0xff)
        msg.insert(31, (buf[4]>>8)&0xff)
        msg.insert(32, (buf[4]&0xff))
        msg.insert(33, 0)
        msg.insert(34, 0)

        # 消息长
        msg_len=len(msg)+1
        msg[0] = (msg_len&0xff)
        msg[1] = ((msg_len>>8)&0xff)
        
        # 校验和
        checsum = ((~(sum(msg) & 0xff))&0xff)
        msg.append(checsum)

        return struct.pack('B'*len(msg), *msg)    
    
    def write_config(self, buf):
        self.msg = self.write_config_prepare(buf)
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
        
        status = self.ut.recieved_even.wait(10)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        data1=recv_msg[7:8]
        value=struct.unpack('B',data1)[0]
        result.append(value)
        return result
        
    def pwm_auto_prepare(self,  pwm_auto_type):
        index = 7
        msg = []
        
        # 消息长占位
        msg.insert(0, 0x00)
        msg.insert(1, 0x00)
        
        # 预留段
        msg.insert(2, 0x00)
        msg.insert(3, 0x00)
        
        # 消息代码
        msg.insert(4, 0x40)
        
        # 消息体
        msg.insert(5, index&0xff)
        msg.insert(6, (index>>8)&0xff)
        
        msg.insert(7, pwm_auto_type&0xff)
        msg.insert(8, (pwm_auto_type>>8)&0xff)
        msg.insert(9, 0)
        msg.insert(10, 0)
        
        # 消息长
        msg_len=len(msg)+1
        msg[0] = (msg_len&0xff)
        msg[1] = ((msg_len>>8)&0xff)
        
        # 校验和
        checsum = ((~(sum(msg) & 0xff))&0xff)
        msg.append(checsum)

        return struct.pack('B'*len(msg), *msg)    
        
    def pwm_auto_set(self,  pwm_auto_type):
        self.msg = self.pwm_auto_prepare(pwm_auto_type)
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
            
        status = self.ut.recieved_even.wait(0.1)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        
        data1=recv_msg[7:8]
        value=struct.unpack('B',data1)[0]
        result.append(value)
        return result
        
        
    def pwm_auto_get(self):
        index = 6
        self.msg = self.read_var_prepare(index)
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
            
        status = self.ut.recieved_even.wait(0.1)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        data1=recv_msg[7:11]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        return result
    
    def rst_set_prepare (self, val):
        index = 7
        msg = []
        
        # 消息长占位
        msg.insert(0, 0x00)
        msg.insert(1, 0x00)
        
        # 预留段
        msg.insert(2, 0x00)
        msg.insert(3, 0x00)
        
        # 消息代码
        msg.insert(4, 0x40)
        
        # 消息体
        msg.insert(5, index&0xff)
        msg.insert(6, (index>>8)&0xff)
        
        msg.insert(7, val&0xff)
        # msg.insert(8, (Kp>>8)&0xff)
        # msg.insert(9,  Ki&0xff)
        # msg.insert(10, (Ki>>8)&0xff)
        
        # 消息长
        msg_len=len(msg)+1
        msg[0] = (msg_len&0xff)
        msg[1] = ((msg_len>>8)&0xff)
        
        # 校验和
        checsum = ((~(sum(msg) & 0xff))&0xff)
        msg.append(checsum)

        return struct.pack('B'*len(msg), *msg)    		
    def rst_set(self, val):
        self.msg = self.rst_set_prepare(val)
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
            
        status = self.ut.recieved_even.wait(0.1)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        
        data1=recv_msg[7:8]
        value=struct.unpack('B',data1)[0]
        result.append(value)
        return result    
        
    def cur_ch_set_prepare(self, ch):
        index = 5
        msg = []
        
        # 消息长占位
        msg.insert(0, 0x00)
        msg.insert(1, 0x00)
        
        # 预留段
        msg.insert(2, 0x00)
        msg.insert(3, 0x00)
        
        # 消息代码
        msg.insert(4, 0x40)
        
        # 消息体
        msg.insert(5, index&0xff)
        msg.insert(6, (index>>8)&0xff)
        
        msg.insert(7, ch&0xff)
        # msg.insert(8, (Kp>>8)&0xff)
        # msg.insert(9,  Ki&0xff)
        # msg.insert(10, (Ki>>8)&0xff)
        
        # 消息长
        msg_len=len(msg)+1
        msg[0] = (msg_len&0xff)
        msg[1] = ((msg_len>>8)&0xff)
        
        # 校验和
        checsum = ((~(sum(msg) & 0xff))&0xff)
        msg.append(checsum)

        return struct.pack('B'*len(msg), *msg)    
        
    def cur_ch_set(self, ch):
        self.msg = self.cur_ch_set_prepare(ch)
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
            
        status = self.ut.recieved_even.wait(0.1)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        
        data1=recv_msg[7:8]
        value=struct.unpack('B',data1)[0]
        result.append(value)
        return result
        
        
    def cur_ch_get_prepare(self):
        index = 4
        msg = []
        
        # 消息长占位
        msg.insert(0, 0x00)
        msg.insert(1, 0x00)
        
        # 预留段
        msg.insert(2, 0x00)
        msg.insert(3, 0x00)
        
        # 消息代码
        msg.insert(4, 0x40)
        
        # 消息体
        msg.insert(5, index&0xff)
        msg.insert(6, (index>>8)&0xff)
        
        # 消息长
        msg_len=len(msg)+1
        msg[0] = (msg_len&0xff)
        msg[1] = ((msg_len>>8)&0xff)
        
        # 校验和
        checsum = ((~(sum(msg) & 0xff))&0xff)
        msg.append(checsum)

        return struct.pack('B'*len(msg), *msg)    
        
        
    def cur_ch_get(self):
        self.msg = self.cur_ch_get_prepare()
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
            
        status = self.ut.recieved_even.wait(0.1)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        
        data1=recv_msg[7:8]
        value=struct.unpack('B',data1)[0]
        result.append(value)
       
        return result
        
        
    def pwm_print_switch_prepare(self,  pwm_print_switch):
        index = 9
        msg = []
        
        # 消息长占位
        msg.insert(0, 0x00)
        msg.insert(1, 0x00)
        
        # 预留段
        msg.insert(2, 0x00)
        msg.insert(3, 0x00)
        
        # 消息代码
        msg.insert(4, 0x40)
        
        # 消息体
        msg.insert(5, index&0xff)
        msg.insert(6, (index>>8)&0xff)
        
        msg.insert(7, pwm_print_switch&0xff)
        msg.insert(8, (pwm_print_switch>>8)&0xff)
        msg.insert(9, 0)
        msg.insert(10, 0)
        
        # 消息长
        msg_len=len(msg)+1
        msg[0] = (msg_len&0xff)
        msg[1] = ((msg_len>>8)&0xff)
        
        # 校验和
        checsum = ((~(sum(msg) & 0xff))&0xff)
        msg.append(checsum)

        return struct.pack('B'*len(msg), *msg)    
        
    def pwm_print_switch_set(self,  pwm_print_switch):
        self.msg = self.pwm_print_switch_prepare(pwm_print_switch)
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
            
        status = self.ut.recieved_even.wait(0.1)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        
        data1=recv_msg[7:8]
        value=struct.unpack('B',data1)[0]
        result.append(value)
        return result
        
        
    def pwm_print_switchs_get(self):
        index = 8
        self.msg = self.read_var_prepare(index)
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
            
        status = self.ut.recieved_even.wait(0.1)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        data1=recv_msg[7:11]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        return result        
        
    def avg_points_prepare(self,  avg_points):
        index = 11
        msg = []
        
        # 消息长占位
        msg.insert(0, 0x00)
        msg.insert(1, 0x00)
        
        # 预留段
        msg.insert(2, 0x00)
        msg.insert(3, 0x00)
        
        # 消息代码
        msg.insert(4, 0x40)
        
        # 消息体
        msg.insert(5, index&0xff)
        msg.insert(6, (index>>8)&0xff)
        
        msg.insert(7, avg_points&0xff)
        msg.insert(8, (avg_points>>8)&0xff)
        msg.insert(9, (avg_points>>16)&0xff)
        msg.insert(10, (avg_points>>24)&0xff)
        
        # 消息长
        msg_len=len(msg)+1
        msg[0] = (msg_len&0xff)
        msg[1] = ((msg_len>>8)&0xff)
        
        # 校验和
        checsum = ((~(sum(msg) & 0xff))&0xff)
        msg.append(checsum)

        return struct.pack('B'*len(msg), *msg)    
        
    def avg_points_set(self,  pwm_print_switch):
        self.msg = self.avg_points_prepare(pwm_print_switch)
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
            
        status = self.ut.recieved_even.wait(0.1)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        
        data1=recv_msg[7:8]
        value=struct.unpack('B',data1)[0]
        result.append(value)
        return result
        
        
    def avg_points_get(self):
        index = 10
        self.msg = self.read_var_prepare(index)
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
            
        status = self.ut.recieved_even.wait(0.1)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        data1=recv_msg[7:11]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        return result        
        
    def win_start_bbclk_cnt_get(self):
        index = 13
        self.msg = self.read_var_prepare(index)
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
            
        status = self.ut.recieved_even.wait(0.1)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        data1=recv_msg[7:11]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        return result    
        
        
    def rx_data_array_get(self):
        index = 14
        self.msg = self.read_var_prepare(index)
        try:
            self.ut.send(self.msg)
        except Exception as ret:
            raise Exception(ret)
            
        status = self.ut.recieved_even.wait(0.1)
        result=[]
        if status is not True:
            raise Exception("Recv timeout")
            result.append(False)
            return result
        self.ut.recieved_even.clear() # must clear
        result.append(True)
        recv_msg= self.recv_msg_prepare(self.ut.recv_msg)
        data1=recv_msg[7:11]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        
        data1=recv_msg[11:15]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        
        data1=recv_msg[15:19]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        
        data1=recv_msg[19:23]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        
        data1=recv_msg[23:27]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        
        data1=recv_msg[27:31]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        
        data1=recv_msg[31:35]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        
        data1=recv_msg[35:39]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        
        data1=recv_msg[39:43]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        
        data1=recv_msg[43:47]
        value=struct.unpack('I',data1)[0]
        result.append(value)
        
        return result
