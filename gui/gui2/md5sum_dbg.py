#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import struct
import hashlib
# import binascii

try:
    fd = open("/opt/workdir/figkey/pre_research/intr_system/microblaze_golden_update/update/vitis/output/output.bin", "rb")
except (IOError, OSError) as ret:
    print(ret)
    pass
else:
    fcont = fd.read()
    fileLen = fd.tell()
    print("file-len:%d" % fileLen)
    fmd5 = hashlib.md5(fcont)
    print(type(fmd5))
    # print(binascii.hexlify(fmd5.hexdigest().upper().encode()).decode())
    fmd5_bytes=bytes.fromhex(fmd5.hexdigest().upper())
    print(fmd5_bytes)
    md5string = fmd5.hexdigest().upper()
    print(md5string)
    print('fileLen=%d=%s'%(fileLen,hex(fileLen)))
    #print('len(fileLen)=%d'%len(fileLen))
    file_len_str = str(fileLen)
    print(file_len_str)

    fileLen_byts=fileLen.to_bytes(4,'big')
    print(hex(fileLen))
    print(fileLen_byts)

    print('len(md5string)=%d' % len(md5string))
    print('len(file_len_str)=%d' % len(file_len_str))
    print((len(md5string), len(file_len_str))) 
    print(struct.pack('!H%ds%ds' % (len(md5string), len(file_len_str)),2, md5string.encode('gb2312'), file_len_str.encode()))

    sendDataFirst = struct.pack('!H%dsb5sb5sb%dsb' % (len(md5string), len(file_len_str)), 2,
                                    md5string.encode('gb2312'), 0,
                                    'octet'.encode('gb2312'), 0, 'tsize'.encode('gb2312'), 0, file_len_str.encode(), 0)


    print(sendDataFirst)

    print(struct.pack('!H%ds%ds' % (len(md5string), len(file_len_str)),2, md5string.encode(), file_len_str.encode()))
    
    print(struct.pack('!6s%ds%ds' % (len(fmd5_bytes), len(file_len_str)), 'md5sum'.encode(), fmd5_bytes, file_len_str.encode()))

    print(struct.pack('!6s%ds5s%ds' % (len(fmd5_bytes), len(fileLen_byts)), 'md5sum'.encode(), fmd5_bytes, 'tsize'.encode(), fileLen_byts))


    print(struct.pack('!6s%ds%ds' % (len(fmd5_bytes), len(fileLen_byts)), 'md5sum'.encode(), fmd5_bytes, fileLen_byts))
    print(struct.pack('!6s%ds%ds' % (len(fmd5_bytes), len(fileLen_byts)), 'md5sum'.encode(), fmd5_bytes, fileLen_byts))
    
    fd.close()
