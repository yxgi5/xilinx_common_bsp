#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# import sys
# import time
# import struct
import udp_client

def main():
    ip='192.168.1.10'
    ut = udp_client.client(server_host=ip)
    ut1 = udp_client.client(server_host=ip)
    if id(ut) == id(ut1):
        print("Singleton works, both variables contain the same instance.")
    else:
        print("Singleton failed, variables contain different instances.")

    # ut.recieved_even.wait()
    
    ut.close()
    print('main end')

if __name__ == '__main__':
    main()
