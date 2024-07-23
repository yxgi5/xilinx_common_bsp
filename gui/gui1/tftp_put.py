# coding=utf-8

# 导包
import sys
import struct
import hashlib
from socket import *
import time
from PyQt5 import QtCore, QtGui
from PyQt5.QtWidgets import *


# 主程序
def tftp_md5_put(uploadFileName, server_ip, mydialog):
    # fd = open(uploadFileName, "rb")
    # fcont = fd.read()
    # fileLen=fd.tell()
    # print("file-len:%d"%fileLen)
    # fmd5 = hashlib.md5(fcont)
    # md5string=fmd5.hexdigest().upper()
    # print(md5string)
    # file_len_str=str(fileLen)
    # fd.close()
    # mydialog.textEdit_upgradeFile_out_put.setText(md5string)
    # sendDataFirst = struct.pack('!H%dsb5sb5sb%dsb' % (len(md5string),len(file_len_str)), 2, md5string.encode('gb2312'), 0,
    #                             'octet'.encode('gb2312'), 0, 'tsize'.encode('gb2312'), 0,file_len_str.encode(), 0)
    sendDataFirst = b"clear"
    # s = socket(AF_INET, SOCK_DGRAM)
    s = socket(AF_INET, SOCK_STREAM)
    s.connect((server_ip, 6789))

    f = open(uploadFileName, 'rb')
    # print("reopen file ok")
    print("open file ok")

    s.sendto(sendDataFirst, (server_ip, 6789))  # 第一次发送, 连接tftp服务器

    print("send put")

    # 第一次接收数据
    responseData = s.recvfrom(1024)
    recvData, serverInfo = responseData
    print("recv ack")
    fileNum = 0  # 表示接收文件的序号
    packetOpt = struct.unpack("!H", recvData[:2])  # 操作码
    packetNum = struct.unpack("!H", recvData[2:4])  # 块编号
    # print("start recv11")
    # 解包
    if packetOpt[0] == 5:
        print("tftp服务器发生错误！")
        f.close()
        s.close()
        return
    print("start recv")
    while True:
        # 　从文件中读取512字节数据
        readFileData = f.read(512)

        # 　打包
        sendData = struct.pack('!HH', 3, fileNum) + readFileData

        # 发送数据到tftp服务器
        s.sendto(sendData, serverInfo)  # 第二次发给服务器的随机端口

        # 接受服务器回传数据
        recvData, serverInfo = s.recvfrom(1024)

        # print(recvData)

        # 解包
        packetOpt = struct.unpack("!H", recvData[:2])  # 操作码
        packetNum = struct.unpack("!H", recvData[2:4])  # 块编号

        if packetOpt[0] == 5:
            print("tftp服务器发生错误！")
            exit()

        if len(sendData) < 516 or packetNum[0] != fileNum:
            print("%s文件上传成功！" % uploadFileName)
            f.close()
            _thread.exit_thread()
            break

        fileNum += 1
        if (fileNum % 100) == 0:
            time.sleep(0.005)
    # while True:

class tftp_md5_put_tread(QtCore.QThread):  # The timer class is derived from the class threading.Thread
    finishSignal = QtCore.pyqtSignal(list)

    def __init__(self, uploadFileName, server_ip, parent=None):
        super(tftp_md5_put_tread, self).__init__(parent)
        self.uploadFileName = uploadFileName
        self.server_ip = server_ip
        self.thread_stop = False

    def run(self):  # Overwrite run() method, put what you want the thread do here
        try:   
            fd = open(self.uploadFileName, "rb")
        except (IOError, OSError) as ret:
            # QMessageBox.warning(None, "File Error", "File Error")
            return
        fcont = fd.read()
        fileLen = fd.tell()
        print("file-len:%d" % fileLen)
        fmd5 = hashlib.md5(fcont)
        fmd5_bytes=bytes.fromhex(fmd5.hexdigest().upper())
        md5string = fmd5.hexdigest().upper()
        print(md5string)
        # fileLen_byts=fileLen.to_bytes(4,'big')
        fileLen_byts=fileLen.to_bytes(4,'little')
        file_len_str = str(fileLen)
        print(file_len_str)
        fd.close()

        # s = socket(AF_INET, SOCK_DGRAM)
        s = socket(AF_INET, SOCK_STREAM)
        s.connect((self.server_ip, 6789))


        self.finishSignal.emit([self.uploadFileName, " md5sum:", md5string, " \n"])  ## 发送信号 告诉主线程
        # sendDataFirst = struct.pack('!H%dsb5sb5sb%dsb' % (len(md5string), len(file_len_str)), 2,
        #                             md5string.encode('gb2312'), 0,
        #                             'octet'.encode('gb2312'), 0, 'tsize'.encode('gb2312'), 0, file_len_str.encode(), 0)
        sendDataFirst = struct.pack('!6s%ds%ds' % (len(fmd5_bytes), len(fileLen_byts)), 'md5sum'.encode(), fmd5_bytes, fileLen_byts)

        s.sendto(sendDataFirst, (self.server_ip, 6789))

        sendDataFirst = b"clear"

        f = open(self.uploadFileName, 'rb')
        print("reopen file ok")

        s.sendto(sendDataFirst, (self.server_ip, 6789))  # 第一次发送, 连接tftp服务器

        print("send put")

        # 第一次接收数据
        responseData = s.recvfrom(1024)
        recvData, serverInfo = responseData
        print("recv ack")

        # print("start recv11")
        # 解包
        if recvData.decode() != "Clear received data\r\n":
            print("服务器发生错误！")
            f.close()
            s.close()
            return
        print("start recv")
        send_len  = 0
        pre_percent = 0
        while not self.thread_stop:
            # 　从文件中读取1024字节数据
            readFileData = f.read(1024)
            read_len = len(readFileData)
            send_len = read_len+send_len
            # print(send_len)
            if pre_percent == 0 and send_len == 1024:
                self.finishSignal.emit(["Transmission Start!!!"]) 
                self.finishSignal.emit(["Transmission progress: 0%"]) 
            # 　打包
            # sendData = struct.pack('!HH', 3, fileNum) + readFileData

            # 发送数据到tftp服务器
            # s.sendto(sendData, serverInfo)  # 第二次发给服务器的随机端口
            s.sendto(readFileData, (self.server_ip, 6789))
            time.sleep(0.00015)
            data = [0,10,20,30,40,50,60,70,80,90,100]
            process_percent = int((send_len /fileLen) *100)
            if pre_percent != process_percent:
                pre_percent = process_percent
                if process_percent in data:
                    self.finishSignal.emit(["Transmission progress: " + str(process_percent) + "%"]) 
            # time.sleep(0.001)
            # self.finishSignal.emit(["#"])

            # 接受服务器回传数据
            # recvData, serverInfo = s.recvfrom(1024)

            # print(recvData)

            # 解包
            # packetOpt = struct.unpack("!H", recvData[:2])  # 操作码
            # packetNum = struct.unpack("!H", recvData[2:4])  # 块编号

            # if packetOpt[0] == 5:
            #     print("tftp服务器发生错误！")
            #     exit()

            if len(readFileData) < 1024:
                print("%s文件上传成功！" % self.uploadFileName)
                f.close()
                self.thread_stop = True
                self.finishSignal.emit(["transfer success !", "wait for flash erase...."])
                # s.close()
                time.sleep(0.1)
                s.sendto(b"update", (self.server_ip, 6789))
                # s.sendto(b"golden", (self.server_ip, 6789))
                break

            # fileNum += 1
            # if (fileNum % 100) == 0:
            #     time.sleep(0.005)
            #     self.finishSignal.emit(["#"])

        # recvData, serverInfo = s.recvfrom(128) #wait last ack
        # '''
        while True:
            recvData, serverInfo = s.recvfrom(128)
            # strlen = struct.unpack("!H", recvData[:2])  # 字符串长度
            # index_end = strlen[0] + 2
            # strmessage = struct.unpack("%ds" % strlen, recvData[2:index_end])  # 字符串长度
            # print( bytes.decode(strmessage[0]))
            # realstr = bytes.decode(strmessage[0])
            realstr = bytes.decode(recvData)
            time.sleep(0.001)
            self.finishSignal.emit([realstr])
            if realstr == "Verify Operation Successful.\r\n":
                s.close()
                break
            elif realstr == "Verify data error at address 0x%lx.\r\n":
                s.close()
                break
    # '''

    def stop(self):
        self.thread_stop = True

class tftp_md5_put_tread_g(QtCore.QThread):  # The timer class is derived from the class threading.Thread
    finishSignal = QtCore.pyqtSignal(list)

    def __init__(self, uploadFileName, server_ip, parent=None):
        super(tftp_md5_put_tread_g, self).__init__(parent)
        self.uploadFileName = uploadFileName
        self.server_ip = server_ip
        self.thread_stop = False

    def run(self):  # Overwrite run() method, put what you want the thread do here
        try:   
            fd = open(self.uploadFileName, "rb")
        except (IOError, OSError) as ret:
            # QMessageBox.warning(None, "File Error", "File Error")
            return
        fcont = fd.read()
        fileLen = fd.tell()
        print("file-len:%d" % fileLen)
        fmd5 = hashlib.md5(fcont)
        fmd5_bytes=bytes.fromhex(fmd5.hexdigest().upper())
        md5string = fmd5.hexdigest().upper()
        print(md5string)
        # fileLen_byts=fileLen.to_bytes(4,'big')
        fileLen_byts=fileLen.to_bytes(4,'little')
        file_len_str = str(fileLen)
        print(file_len_str)
        fd.close()

        # s = socket(AF_INET, SOCK_DGRAM)
        s = socket(AF_INET, SOCK_STREAM)
        s.connect((self.server_ip, 6789))


        self.finishSignal.emit([self.uploadFileName, " md5sum:", md5string, " \n"])  ## 发送信号 告诉主线程
        # sendDataFirst = struct.pack('!H%dsb5sb5sb%dsb' % (len(md5string), len(file_len_str)), 2,
        #                             md5string.encode('gb2312'), 0,
        #                             'octet'.encode('gb2312'), 0, 'tsize'.encode('gb2312'), 0, file_len_str.encode(), 0)
        sendDataFirst = struct.pack('!6s%ds%ds' % (len(fmd5_bytes), len(fileLen_byts)), 'md5sum'.encode(), fmd5_bytes, fileLen_byts)

        s.sendto(sendDataFirst, (self.server_ip, 6789))

        sendDataFirst = b"clear"

        f = open(self.uploadFileName, 'rb')
        print("reopen file ok")

        s.sendto(sendDataFirst, (self.server_ip, 6789))  # 第一次发送, 连接tftp服务器

        print("send put")

        # 第一次接收数据
        responseData = s.recvfrom(1024)
        recvData, serverInfo = responseData
        print("recv ack")

        # print("start recv11")
        # 解包
        if recvData.decode() != "Clear received data\r\n":
            print("服务器发生错误！")
            f.close()
            s.close()
            return
        print("start recv")
        send_len  = 0
        pre_percent = 0
        while not self.thread_stop:
            # 　从文件中读取1024字节数据
            readFileData = f.read(1024)
            read_len = len(readFileData)
            send_len = read_len+send_len
            # print(send_len)
            if pre_percent == 0 and send_len == 1024:
                self.finishSignal.emit(["Transmission Start!!!"]) 
                self.finishSignal.emit(["Transmission progress: 0%"]) 
            # 　打包
            # sendData = struct.pack('!HH', 3, fileNum) + readFileData

            # 发送数据到tftp服务器
            # s.sendto(sendData, serverInfo)  # 第二次发给服务器的随机端口
            s.sendto(readFileData, (self.server_ip, 6789))
            time.sleep(0.00015)
            data = [0,10,20,30,40,50,60,70,80,90,100]
            process_percent = int((send_len /fileLen) *100)
            if pre_percent != process_percent:
                pre_percent = process_percent
                if process_percent in data:
                    self.finishSignal.emit(["Transmission progress: " + str(process_percent) + "%"]) 
            # time.sleep(0.001)
            # self.finishSignal.emit(["#"])

            # 接受服务器回传数据
            # recvData, serverInfo = s.recvfrom(1024)

            # print(recvData)

            # 解包
            # packetOpt = struct.unpack("!H", recvData[:2])  # 操作码
            # packetNum = struct.unpack("!H", recvData[2:4])  # 块编号

            # if packetOpt[0] == 5:
            #     print("tftp服务器发生错误！")
            #     exit()

            if len(readFileData) < 1024:
                print("%s文件上传成功！" % self.uploadFileName)
                f.close()
                self.thread_stop = True
                self.finishSignal.emit(["transfer success !", "wait for flash erase...."])
                # s.close()
                time.sleep(0.1)
                # s.sendto(b"update", (self.server_ip, 6789))
                s.sendto(b"golden", (self.server_ip, 6789))
                break

            # fileNum += 1
            # if (fileNum % 100) == 0:
            #     time.sleep(0.005)
            #     self.finishSignal.emit(["#"])

        # recvData, serverInfo = s.recvfrom(128) #wait last ack
        # '''
        while True:
            recvData, serverInfo = s.recvfrom(128)
            # strlen = struct.unpack("!H", recvData[:2])  # 字符串长度
            # index_end = strlen[0] + 2
            # strmessage = struct.unpack("%ds" % strlen, recvData[2:index_end])  # 字符串长度
            # print( bytes.decode(strmessage[0]))
            # realstr = bytes.decode(strmessage[0])
            realstr = bytes.decode(recvData)
            time.sleep(0.001)
            self.finishSignal.emit([realstr])
            if realstr == "Verify Operation Successful.\r\n":
                s.close()
                break
            elif realstr == "Verify data error at address 0x%lx.\r\n":
                s.close()
                break
    # '''

    def stop(self):
        self.thread_stop = True


'''
if __name__ == '__main__':
    main()
'''
