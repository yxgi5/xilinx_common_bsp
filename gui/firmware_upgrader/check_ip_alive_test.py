#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import check_ip_alive

def main():
    status = check_ip_alive.check_ip_alive("192.168.1.10")
    print(status)
    pass

if __name__ == '__main__':
    main()
