#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
# import socket
from threading import *
import subprocess

# def check_ip_alive(host_ip='192.168.1.10'):
#     # hostname = "google.com"
#     errorStr = 'Unreachable'
#     p = os.popen("ping -c 2 -r " + host_ip)
#     line = p.read()
#     if errorStr in line:
#         status = False
#     else:
#         status = True
      
#     return status

def check_ip_alive(host_ip='192.168.1.10'):
    if subprocess.call(["ping", "-c", "2", host_ip])==0:
        status = True
    else:
        status = False
      
    return status


# def call_ping(ip):
#     if subprocess.call(["ping", "-c", "2", ip])==0:
#         print("{0} is reachable".format(ip))
#     else:
#         print("{0} is unreachable".format(ip))
        
# def is_reachable(q):
#     try:
#         while True:
#             ip = q.get_nowait()#当队列为空，不等待
#             call_ping(ip)
#     except Empty:
#         pass

# recieved_even = Event()

# def is_reachable(q):
#     print(q)
#     global recieved_even
#     if subprocess.call(["ping", "-c", "2", q])==0:
#         recieved_even.set()
#     else:
#         recieved_even.clear()

# def check_ip_alive(host_ip='192.168.1.10'):
#     thr = Thread(target=is_reachable, args=(host_ip,))
#     global recieved_even
#     recieved_even.clear()
#     thr.start()
#     pass
#     # recieved_even.wait()
#     # if recieved_even.is_set() is not None:
#     if recieved_even.is_set() is True:
#         thr.join()
#         del recieved_even
#         return True
#     else:
#         thr.join()
#         del recieved_even
#         return False

