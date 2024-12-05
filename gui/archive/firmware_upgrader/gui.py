import sys
import os
from turtle import width
from PyQt5 import QtCore, QtGui, QtWidgets, uic
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
# import math
# import struct
# import socket
# import random
import tcp_client
import udp_client
import memory_ops
# import csv
import check_ip_addr
import check_ip_alive
import var_msg_ops
import tcp_updata
import udp_updata


HARD_VER_IDX = 0
SOFT_VER_IDX = 1

class MyDialog(QtWidgets.QMainWindow):
    def __init__(self,parent=None):
        super(MyDialog, self).__init__()

#************************************************        
        self.setWindowIcon(QIcon(get_resource_path('favicon.ico')))
        # uic.loadUi('gui.ui', self)
        uic.loadUi(get_resource_path('gui.ui'), self)
#***********************************************
        self.setupUi()
        self.show() # Show the GUI
    #初始化窗口
    def setupUi(self):
        self.setFixedSize(1050,750)
        self.connection_status = False
        
        self.upgrade_file_button.setDisabled(True)
        self.upgrade_button.setDisabled(True)

        # connection
        self.ipaddr = "192.168.1.10"
        self.ipaddr_lineEdit.setText(self.ipaddr)
        self.ipaddr_disconnect_btn.setDisabled(True)

        # self.timer = QTimer(self)
        self.ipaddr_connect_btn.clicked.connect(self.ipaddr_connect_btn_handler)
        self.ipaddr_disconnect_btn.clicked.connect(self.ipaddr_disconnect_btn_handler)

        protocol_type = {'TCP','UDP'}
        self.comboBox.addItems(protocol_type)
        self.port_set.setText('6789')
        self.comboBox.setDisabled(False)

    def ConnectSignalSlot(self):
        # timer
        # self.timer.timeout.connect(self.check_connection_alive)

        #upgrade file
        self.upgrade_file_button.clicked.connect(self.open_upgrade_file_handler)
        self.upgrade_button.clicked.connect(self.upgrade_file_handler)
        # self.upgrade_file_extract_btn.clicked.connect(self.upgrade_extract_handler)
        # self.upgrade_button_fpga.clicked.connect(self.upgrade_fpga_handler)
        # self.upgrade_button_sw.clicked.connect(self.upgrade_sw_handler)
        pass

    def DisconnectSignalSlot(self):     
        # timer
        # self.timer.timeout.disconnect(self.check_connection_alive)

        #upgrade
        self.upgrade_file_button.clicked.disconnect(self.open_upgrade_file_handler)
        self.upgrade_button.clicked.disconnect(self.upgrade_file_handler)
        # self.upgrade_file_extract_btn.clicked.disconnect(self.upgrade_extract_handler)
        # self.upgrade_button_fpga.clicked.disconnect(self.upgrade_fpga_handler)
        # self.upgrade_button_sw.clicked.disconnect(self.upgrade_sw_handler)
        pass
 
    def closeEvent(self, event):
        reply = QtWidgets.QMessageBox.question(self,
                                               '确认关闭',
                                               "是否要退出程序？",
                                               QtWidgets.QMessageBox.Yes | QtWidgets.QMessageBox.No,
                                               QtWidgets.QMessageBox.No)
        if reply == QtWidgets.QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()
            return

        if(self.connection_status == True):
            try:
                self.m1
                self.vm1
            except Exception as ret:
                print(ret)
            else:
                del self.m1
                del self.vm1
            try:
                self.ut
            except Exception as ret:
                print(ret)
            else:
                self.ut.close()
                del self.ut

    def open_upgrade_file_handler(self):
        print("select file")
        fileName = QFileDialog.getOpenFileName(
            self, 'choose the file', options=QFileDialog.DontUseNativeDialog)
        self.textEdit_upgradeFile.setText(fileName[0])
    
    def RecvSendTreadSinalHandler(self, ls):
        # 使用传回的返回值
        if(ls == ['programme exit']):
            pass   
        elif(ls == ['Verify Operation Successful.\r\n']):
            self.thread_cancle()
        for word in ls:
            if ((word == "#") or (word == ".") or (word == "*")):
                self.textEdit_upgradeFile_out_put.moveCursor(QTextCursor.End)
                self.textEdit_upgradeFile_out_put.insertPlainText(word)
            else :
                self.textEdit_upgradeFile_out_put.append(word)
    
    def upgrade_file_handler(self):
        # self.timer.stop()#传送开始前关闭定时器
        print("upgrade firmeware")
        if self.textEdit_upgradeFile.text()=="":
            QMessageBox.information(self, "Error!", "please select the file")
            return
        print("self.textEdit_upgradeFile.text()")
        if self.comboBox.currentText() == 'UDP':
            self.udpThread = udp_updata.UDP_sent_thread(self.textEdit_upgradeFile.text(),self.ipaddr_lineEdit.text(),self.port_set.text())
            self.udpThread.finishSignal.connect(self.RecvSendTreadSinalHandler)
            self.udpThread.start()
        else:
            self.tcpThread = tcp_updata.TCP_sent_thread(self.textEdit_upgradeFile.text(),self.ipaddr_lineEdit.text(),self.port_set.text())
            self.tcpThread.finishSignal.connect(self.RecvSendTreadSinalHandler)
            self.tcpThread.start()
    def thread_cancle(self):
        if(self.comboBox.currentText() == 'UDP'):
            self.udpThread.stop()
            self.udpThread.quit()
            self.udpThread.wait()
            self.udpThread.deleteLater()
        else:
            self.tcpThread.stop()
            self.tcpThread.quit()
            self.tcpThread.wait()
            self.tcpThread.deleteLater()
    def ipaddr_connect_btn_handler(self):
        status = check_ip_addr.is_valid_ipv4_address(self.ipaddr_lineEdit.text())
        if status is False:
            QMessageBox.information(self,"Error!", "ip address errrrror!") 
        else:
            self.ipaddr = self.ipaddr_lineEdit.text()

            try:
                if self.comboBox.currentText() == 'UDP':
                    self.ut = udp_client.client(server_host=self.ipaddr)
                else:
                    self.ut = tcp_client.client(server_host=self.ipaddr)
            except Exception as ret:
                print(ret)
                QMessageBox.information(self,"Error!", "connection failed!")
                self.ut.close()
                del self.ut
                return
        
            else:
                # self.timer.start(1000)
                self.m1 = memory_ops.memory_ops(self.ut)
                self.vm1 = var_msg_ops.var_msg_ops(self.ut)
                self.connection_status = True
                self.ipaddr_disconnect_btn.setDisabled(False)
                self.ipaddr_connect_btn.setDisabled(True)
                self.comboBox.setDisabled(True)
                    

            if(self.connection_status == True):
                status=self.vm1.read_var(HARD_VER_IDX)
                if status[0]:
                    print("ok, 0x%x"%status[1])
                    self.hardware_ver_label.setText("0x"+("%08x"%status[1]).upper())
                else:
                    QMessageBox.information(self,"Error!", "communication status err") 
                    print("ng")
                
                status=self.vm1.read_var(SOFT_VER_IDX)
                if status[0]:
                    print("ok, 0x%x"%status[1])
                    self.firmware_ver_label.setText("0x"+("%08x"%status[1]).upper())
                else:
                    QMessageBox.information(self,"Error!", "communication status err") 
                    print("ng")
                self.ConnectSignalSlot()
                self.upgrade_file_button.setDisabled(False)
                self.upgrade_button.setDisabled(False)

                pass
            pass
        pass

    def clear_display(self):
        self.textEdit_upgradeFile.setText("")
        self.hardware_ver_label.setText("?")
        self.firmware_ver_label.setText("?")    
        self.upgrade_file_button.setDisabled(True)
        self.upgrade_button.setDisabled(True)

        # print("clear_display")
        pass

    def ipaddr_disconnect_btn_handler(self):
        self.ipaddr_disconnect_btn.setDisabled(True)
        self.ipaddr_connect_btn.setDisabled(False)
        self.comboBox.setDisabled(False)
        # self.timer.stop()
        self.connection_status = False
        del self.m1
        del self.vm1
        self.ut.close()
        del self.ut
        self.DisconnectSignalSlot()
        self.clear_display()
  
    def check_connection_alive(self):
        Status = check_ip_alive.check_ip_alive(self.ipaddr)
        if Status == False:
            self.ipaddr_disconnect_btn_handler()
        else:
            pass

    def is_hex(self, s):
        try:
            int(s, 16)
            return True
        except ValueError:
            return False
            
    def is_int(self, s):
        try:
            int(s, 10)
            return True
        except ValueError:
            return False
        
def main():
    app = QtWidgets.QApplication(sys.argv)
    form = MyDialog()
    sys.exit(app.exec_())

def get_resource_path(relative_path):
    if hasattr(sys, '_MEIPASS'):
        return os.path.join(sys._MEIPASS, relative_path)
    return os.path.join(os.path.abspath("."), relative_path)

if __name__ == '__main__':
    main()
