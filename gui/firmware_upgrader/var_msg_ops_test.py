#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import sys
import time
import struct
import udp_client
import tcp_client
import var_msg_ops


class A(object):
    def func(self):
        self.ut = udp_client.client()
        self.vm1 = var_msg_ops.var_msg_ops(self.ut)
    
        self.result = self.vm1.read_var(1)
        print(self.result)
        print("0x%08x" % self.result[1])
    
        self.result = self.vm1.cur_ch_get()
        print(self.result)
        print("0x%08x" % self.result[1])

        self.result = self.vm1.cur_ch_set(4)
        print(self.result)
        print("0x%08x" % self.result[1])

        self.result = self.vm1.cur_ch_get()
        print(self.result)
        print("0x%08x" % self.result[1])

        del self.vm1
        self.ut.close()
        del self.ut


        self.ut = tcp_client.client()
        self.vm1 = var_msg_ops.var_msg_ops(self.ut)
    
        self.result = self.vm1.read_var(1)
        print(self.result)
        print("0x%08x" % self.result[1])
    
        self.result = self.vm1.cur_ch_get()
        print(self.result)
        print("0x%08x" % self.result[1])

        self.result = self.vm1.cur_ch_set(4)
        print(self.result)
        print("0x%08x" % self.result[1])

        self.result = self.vm1.cur_ch_get()
        print(self.result)
        print("0x%08x" % self.result[1])

        del self.vm1
        self.ut.close()
        del self.ut


def main():
    ut = udp_client.client()
    time.sleep(0.1)
    vm1 = var_msg_ops.var_msg_ops(ut)
    
    result = vm1.read_var(1)
    print(result)
    print("0x%08x" % result[1])
    
    result = vm1.cur_ch_get()
    print(result)
    print("0x%08x" % result[1])

    result = vm1.cur_ch_set(4)
    print(result)
    print("0x%08x" % result[1])

    result = vm1.cur_ch_get()
    print(result)
    print("0x%08x" % result[1])

    del vm1
    ut.close()
    del ut


    ut = tcp_client.client()
    time.sleep(0.1)
    vm1 = var_msg_ops.var_msg_ops(ut)
    
    result = vm1.read_var(1)
    print(result)
    print("0x%08x" % result[1])
    
    result = vm1.cur_ch_get()
    print(result)
    print("0x%08x" % result[1])

    result = vm1.cur_ch_set(4)
    print(result)
    print("0x%08x" % result[1])

    result = vm1.cur_ch_get()
    print(result)
    print("0x%08x" % result[1])

    del vm1
    ut.close()
    del ut

    a = A()
    a.func()

    print('main end')

if __name__ == '__main__':
    main()
