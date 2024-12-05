#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from socket import *
from threading import *

import time
import struct

import stopThreading

BUFSIZ = 65507

# class Singleton(object):
#     def __init__(self, cls):
#         self._cls = cls
#         self._instance = {}
#     def __call__(self):
#         if self._cls not in self._instance:
#             self._instance[self._cls] = self._cls()
#         return self._instance[self._cls]

class Singleton(type):
    """
    The Singleton class can be implemented in different ways in Python. Some
    possible methods include: base class, decorator, metaclass. We will use the
    metaclass because it is best suited for this purpose.
    """
    _instances = {}
    def __call__(cls, *args, **kwargs):
        """
        Possible changes to the value of the `__init__` argument do not affect
        the returned instance.
        """
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
        return cls._instances[cls]

class client(metaclass=Singleton):
    
    def __init__(self, server_host='192.168.1.10', server_port=5555):
        self.server_host = server_host
        self.server_port = server_port
        self.target_addr = ("%s"%(self.server_host),  self.server_port)
       
        self.client_thread = None
        self.link=False
        
        self.recieved_even = Event()
        self.recieved_even.clear()    # 设置为False
        self.stop_running = Event() 
        self.stop_running.clear()      # 将running设置为False
        
        try:
            self.client_start()
        except Exception as ret:
            raise Exception(ret)
        
    def client_start(self):
        """
        开启UDP客户端方法
        :return:
        """
        self.udp_socket = socket(AF_INET, SOCK_DGRAM)
        self.client_thread = Thread(target=self.client_concurrency)
        self.client_thread.start()
        self.link = True
            
    def client_concurrency(self):
        """
        用于创建一个线程持续监听UDP通信
        :return:
        """
        """
        #test thread
        while True:
            time.sleep(1)
            print("xxx")  
        """
        while not self.stop_running.is_set():
            # print ('waiting for message...')
            try:
                self.recv_msg, self.recv_addr = self.udp_socket.recvfrom(BUFSIZ)
                # self.recv_msg = self.udp_socket.recv(BUFSIZ)
                # print(self.recv_msg)
            except Exception as ret:
                # print('My exception occurred, value:',ret.value)
                print(ret)
                pass
            else:
                print(self.recv_msg)
                self.recieved_even.set()

        # print('thread stoping\n')
        self.stop_running.set()
        self.udp_socket.close()

        
    def send(self, send_msg):
        """
        功能函数，用于UDP客户端发送消息
        :return: None
        """
        if self.link is False:
            try:
                self.client_start()
            except Exception as ret:
                raise Exception(ret)
        else:
            try:
                self.udp_socket.sendto(send_msg, self.target_addr)
            except Exception as ret:
                print('发送失败\n')
                raise Exception(ret)

    def close(self):
        """
        功能函数，关闭网络连接的方法
        :return:
        """
        # print('call shutdown_socket\n')
        try:
            # self.udp_socket.shutdown(SHUT_RD)
            self.udp_socket.shutdown(SHUT_RDWR)
        except OSError as ret:
            # print(ret)
            pass

        # print('call close_socket\n')
        try:
            self.udp_socket.close()
            Singleton._instances = {}
        except OSError as ret:
            # print(ret)
            pass

        # print('call stop_running\n')
        try:
            self.stop_running.set()
        except Exception as ret:
            # print(ret)
            pass

        # print('call stop_thread\n')
        try:
            stopThreading.stop_thread(self.client_thread)
        except Exception as ret:
            # print(ret)
            pass


