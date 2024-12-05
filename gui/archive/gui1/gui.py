import sys
import os
from turtle import width
from PyQt5 import QtCore, QtGui, QtWidgets, uic
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
import math
import struct
import socket
import random
# from ui_gui import Ui_Form
import udp_logic
import memory_ops
import gpio_ops
import csv
import check_ip_addr
import check_ping
import xgpio_ops
import xaxis_switch_ops
import var_msg_ops
import tftp_put
import extract_fw
import string
import iic_ops_msg
#**************************


GPIO_OFFSET = 78
PA_EN1 = GPIO_OFFSET + 91
PA_EN2 = GPIO_OFFSET + 90
C0_1 = GPIO_OFFSET + 89
C0_2 = GPIO_OFFSET + 88
C1_1 = GPIO_OFFSET + 87
C1_2 = GPIO_OFFSET + 86
RST_SI_1 = GPIO_OFFSET + 85
RST_SI_2 = GPIO_OFFSET + 84
SI_1_PASSBY = GPIO_OFFSET + 83
SI_2_PASSBY = GPIO_OFFSET + 82


STANDBY = 0
TRANSMIT = 1
LNA_ON = 2
BYPASS =3

MODEM_BASE = 0x98000000

HARD_VER_ADDR = 0x80000000

CH0_FREQ_ADDR = 0x80090000 + 8
CH0_PIXEL_HEIGHT_ADDR = 0x80090000 + 0
CH0_PIXEL_WIDTH_ADDR = 0x80090000 + 4
CH1_FREQ_ADDR = 0x800A0000 + 8
CH1_PIXEL_HEIGHT_ADDR = 0x800A0000 + 0
CH1_PIXEL_WIDTH_ADDR = 0x800A0000 + 4
CH2_FREQ_ADDR = 0x800B0000 + 8
CH2_PIXEL_HEIGHT_ADDR = 0x800B0000 + 0
CH2_PIXEL_WIDTH_ADDR = 0x800B0000 + 4

HARD_VER_IDX = 0
SOFT_VER_IDX = 1
WIDTH_VER_IDX = 2
HEIGHT_VER_IDX = 3
FREQ_VER_IDX = 4

def hex_to_signed(source):
    """Convert a string hex value to a signed hexidecimal value.

    This assumes that source is the proper length, and the sign bit
    is the first bit in the first byte of the correct length.

    hex_to_signed("F") should return -1.
    hex_to_signed("0F") should return 15.
    """
    if not isinstance(source, str):
        raise ValueError("string type required")
    if 0 == len(source):
        raise valueError("string is empty")
    sign_bit_mask = 1 << (len(source)*4-1)
    other_bits_mask = sign_bit_mask - 1
    value = int(source, 16)
    return -(value & sign_bit_mask) | (value & other_bits_mask)

class MyDialog(QtWidgets.QMainWindow):
    def __init__(self,parent=None):
        super(MyDialog, self).__init__()

        # self.setupUi()
#************************************************        
        self.setWindowIcon(QIcon(get_resource_path('favicon.ico')))
        # uic.loadUi('gui.ui', self)
        uic.loadUi(get_resource_path('gui.ui'), self)
#***********************************************
        self.setupUi()
        self.show() # Show the GUI
    #初始化窗口
    def setupUi(self):
        self.connection_status = False
        # ll_mmemory_access
        self.hex_ep2=QRegExp("^[0-9A-Fa-f]{1,2}$")
        self.onlyHex2 = QRegExpValidator(self.hex_ep2)
        self.hex_ep4=QRegExp("^[0-9A-Fa-f]{1,4}$")
        self.onlyHex4 = QRegExpValidator(self.hex_ep4)
        self.hex_ep8=QRegExp("^[0-9A-Fa-f]{1,8}$")
        self.onlyHex8 = QRegExpValidator(self.hex_ep8)
        self.hex_ep12=QRegExp("^[0-9A-Fa-f]{1,12}$")
        self.onlyHex12 = QRegExpValidator(self.hex_ep12)
        
        self.ll_mm_addr_lineEdit.setValidator(self.onlyHex8)
        self.ll_mm_value_lineEdit.setValidator(self.onlyHex8)

        self.ll_mm_read_btn.setDisabled(True)
        self.ll_mm_write_btn.setDisabled(True)

        # ll_gpio_access
        self.ll_gpio_dir_combobox.addItem('IN')
        self.ll_gpio_dir_combobox.addItem('OUT')
        self.ll_gpio_dir= self.ll_gpio_dir_combobox.currentIndex()
        
        for index in range(174):
            self.ll_gpio_globalno_combobox.addItem('%d' % index)
        self.ll_gpio_globalno_combobox.setCurrentIndex(0+78)
        self.ll_gpio_globalno = self.ll_gpio_globalno_combobox.currentIndex()
        
        self.gpio_ep=QRegExp("^[0-1]{1,1}$")
        self.onlyOnOff = QRegExpValidator(self.gpio_ep)
        self.ll_gpio_lineEdit.setValidator(self.onlyOnOff)
        
        self.ll_gpio_input_value=0

        self.ll_spi_send_btn.setDisabled(True)

        self.ll_iic_global_ch = 0
        self.ll_iic_input_values = []
        self.ll_reg_addr = 0
        self.iic_16bit_read.setDisabled(True)
        self.iic_16bit_write.setDisabled(True)
        self.iic_8bit_read.setDisabled(True)
        self.iic_8bit_write.setDisabled(True)
        self.ll_gpio_read_btn.setDisabled(True)
        self.ll_gpio_write_btn.setDisabled(True)

        # ch tab
        self.freq_label.setText("?")
        self.pixel_label.setText("?")

        ## Reset radioButton status
        self.q_radioButton_ch0.setAutoExclusive(False)
        self.q_radioButton_ch0.setChecked(False)
        self.q_radioButton_ch0.setAutoExclusive(True)
        self.q_radioButton_ch0.setDisabled(True)
        self.q_radioButton_ch1.setAutoExclusive(False)
        self.q_radioButton_ch1.setChecked(False)
        self.q_radioButton_ch1.setAutoExclusive(True)
        self.q_radioButton_ch1.setDisabled(True)
        self.q_radioButton_ch2.setAutoExclusive(False)
        self.q_radioButton_ch2.setChecked(False)
        self.q_radioButton_ch2.setAutoExclusive(True)
        self.q_radioButton_ch2.setDisabled(True)
        self.q_radioButton_ch3.setAutoExclusive(False)
        self.q_radioButton_ch3.setChecked(False)
        self.q_radioButton_ch3.setAutoExclusive(True)
        self.q_radioButton_ch3.setDisabled(True)
        # self.q_radioButton_ch4.setAutoExclusive(False)
        # self.q_radioButton_ch4.setChecked(False)
        # self.q_radioButton_ch4.setAutoExclusive(True)
        # self.q_radioButton_ch4.setDisabled(True)

        self.upgrade_file_button.setDisabled(True)
        self.upgrade_button.setDisabled(True)
        self.upgrade_button_golden.setDisabled(True)

        self.pushButton_rst_all.setDisabled(True)
        # settings
        self.settings_mac_lineEdit.setValidator(self.onlyHex12)

        # connection
        self.ipaddr = "192.168.1.10"
        self.ipaddr_lineEdit.setText(self.ipaddr)
        self.ipaddr_disconnect_btn.setDisabled(True)

        self.timer = QTimer(self)
        # self.channel_timer = QTimer(self)

        self.ipaddr_connect_btn.clicked.connect(self.ipaddr_connect_btn_handler)
        self.ipaddr_disconnect_btn.clicked.connect(self.ipaddr_disconnect_btn_handler)

    def ConnectSignalSlot(self):
        # ll_mmemory_access
        self.ll_mm_read_btn.clicked.connect(self.mm_readOnCmd)
        self.ll_mm_write_btn.clicked.connect(self.mm_writeOnCmd)
        
        # ll_gpio_access
        self.ll_gpio_read_btn.clicked.connect(self.ll_gpio_read_btn_handler)
        self.ll_gpio_write_btn.clicked.connect(self.ll_gpio_write_btn_handler)
        self.ll_gpio_dir_combobox.currentIndexChanged.connect(self.ll_gpio_dir_combobox_handler)
        self.ll_gpio_globalno_combobox.currentIndexChanged.connect(self.ll_gpio_globalno_combobox_handler)
        self.ll_gpio_lineEdit.editingFinished.connect(self.ll_gpio_lineEdit_handler)
        
        # # ll_spi_access
        # self.connect(self.ll_spi_send_lineEdit, SIGNAL('editingFinished()'), self.ll_spi_check_spi_send)
        # self.connect(self.ll_spi_send_btn, SIGNAL('clicked()'), self.ll_spi_send_btn_handler)
        # self.connect(self.ll_spi_ch_combobox, SIGNAL('currentIndexChanged(int)'), self.ll_spi_ch_combobox_handler)
        # self.connect(self.ll_spi_cs_combobox, SIGNAL('currentIndexChanged(int)'), self.ll_spi_cs_combobox_handler)
        
        # # ll_iic_access
        # self.connect(self.ll_iic_addr_lineEdit, SIGNAL('editingFinished()'), self.ll_iic_check_addr)
        # self.connect(self.ll_iic_send_lineEdit, SIGNAL('editingFinished()'), self.ll_iic_check_send)
        # self.connect(self.ll_iic_ch_combobox, SIGNAL('currentIndexChanged(int)'), self.ll_iic_ch_combobox_handler)
        # self.connect(self._filter, SIGNAL('focusOut()'), self.ll_iic_check_addr)
        # self.connect(self.ll_iic_write_btn, SIGNAL('clicked()'), self.ll_iic_write_btn_handler)
        # self.connect(self.ll_iic_read_btn, SIGNAL('clicked()'), self.ll_iic_read_btn_handler)



        self.iic_8bit_write.clicked.connect(self.iic_8bit_write_handler)
        self.iic_8bit_read.clicked.connect(self.iic_8bit_read_handler)
        self.iic_16bit_write.clicked.connect(self.iic_16bit_write_handler)
        self.iic_16bit_read.clicked.connect(self.iic_16bit_read_handler)
    
        # ch sel tab
        # self.q_radioButton_ch0.toggled.connect(lambda:self.ch0_select_handler(self.q_radioButton_ch0))
        # self.q_radioButton_ch1.toggled.connect(lambda:self.ch1_select_handler(self.q_radioButton_ch1))
        # self.q_radioButton_ch2.toggled.connect(lambda:self.ch2_select_handler(self.q_radioButton_ch2))
        # self.q_radioButton_ch3.toggled.connect(lambda:self.ch3_select_handler(self.q_radioButton_ch3))
        # self.q_radioButton_ch4.toggled.connect(lambda:self.ch4_select_handler(self.q_radioButton_ch4))
        self.q_radioButton_ch0.toggled.connect(self.ch0_select_handler)
        self.q_radioButton_ch1.toggled.connect(self.ch1_select_handler)
        self.q_radioButton_ch2.toggled.connect(self.ch2_select_handler)
        self.q_radioButton_ch3.toggled.connect(self.ch3_select_handler)
        # self.q_radioButton_ch4.toggled.connect(self.ch4_select_handler)

        self.pushButton_rst_all.clicked.connect(self.pushButton_rst_all_handler)
        
        # timer
        self.timer.timeout.connect(self.check_connection_alive)
        # self.channel_timer.timeout.connect(self.channel_change)

        # settings tab
        self.read_settings_btn_1.clicked.connect(self.read_settings_btn_1_handler)
        self.read_settings_btn_2.clicked.connect(self.read_settings_btn_2_handler)
        self.save_settings_btn_1.clicked.connect(self.save_settings_btn_1_handler)

        #upgrade file
        self.upgrade_file_button.clicked.connect(self.open_upgrade_file_handler)
        self.upgrade_button.clicked.connect(self.upgrade_file_handler)
        self.upgrade_button_golden.clicked.connect(self.upgrade_golden_handler)
        # self.upgrade_file_extract_btn.clicked.connect(self.upgrade_extract_handler)
        # self.upgrade_button_fpga.clicked.connect(self.upgrade_fpga_handler)
        # self.upgrade_button_sw.clicked.connect(self.upgrade_sw_handler)
        
        pass

    def DisconnectSignalSlot(self):     
        # ll_mmemory_access
        self.ll_mm_read_btn.clicked.disconnect(self.mm_readOnCmd)
        self.ll_mm_write_btn.clicked.disconnect(self.mm_writeOnCmd)
        
        # ll_gpio_access
        self.ll_gpio_read_btn.clicked.disconnect(self.ll_gpio_read_btn_handler)
        self.ll_gpio_write_btn.clicked.disconnect(self.ll_gpio_write_btn_handler)
        self.ll_gpio_dir_combobox.currentIndexChanged.disconnect(self.ll_gpio_dir_combobox_handler)
        self.ll_gpio_globalno_combobox.currentIndexChanged.disconnect(self.ll_gpio_globalno_combobox_handler)
        self.ll_gpio_lineEdit.editingFinished.disconnect(self.ll_gpio_lineEdit_handler)
        
        # # ll_spi_access
        # self.disconnect(self.ll_spi_send_lineEdit, SIGNAL('editingFinished()'), self.ll_spi_check_spi_send)
        # self.disconnect(self.ll_spi_send_btn, SIGNAL('clicked()'), self.ll_spi_send_btn_handler)
        # self.disconnect(self.ll_spi_ch_combobox, SIGNAL('currentIndexChanged(int)'), self.ll_spi_ch_combobox_handler)
        # self.disconnect(self.ll_spi_cs_combobox, SIGNAL('currentIndexChanged(int)'), self.ll_spi_cs_combobox_handler)
        
        # # ll_iic_access
        # self.disconnect(self.ll_iic_addr_lineEdit, SIGNAL('editingFinished()'), self.ll_iic_check_addr)
        # self.disconnect(self.ll_iic_send_lineEdit, SIGNAL('editingFinished()'), self.ll_iic_check_send)
        # self.disconnect(self.ll_iic_ch_combobox, SIGNAL('currentIndexChanged(int)'), self.ll_iic_ch_combobox_handler)
        # self.disconnect(self._filter, SIGNAL('focusOut()'), self.ll_iic_check_addr)
        # self.disconnect(self.ll_iic_write_btn, SIGNAL('clicked()'), self.ll_iic_write_btn_handler)
        # self.disconnect(self.ll_iic_read_btn, SIGNAL('clicked()'), self.ll_iic_read_btn_handler) 

        self.iic_8bit_write.clicked.disconnect(self.iic_8bit_write_handler)
        self.iic_8bit_read.clicked.disconnect(self.iic_8bit_read_handler)
        self.iic_16bit_write.clicked.disconnect(self.iic_16bit_write_handler)
        self.iic_16bit_read.clicked.disconnect(self.iic_16bit_read_handler)

        # ch sel tab
        self.q_radioButton_ch0.toggled.disconnect(self.ch0_select_handler)
        self.q_radioButton_ch1.toggled.disconnect(self.ch1_select_handler)
        self.q_radioButton_ch2.toggled.disconnect(self.ch2_select_handler)
        self.q_radioButton_ch3.toggled.disconnect(self.ch3_select_handler)
        # self.q_radioButton_ch4.toggled.disconnect(self.ch4_select_handler)

        self.pushButton_rst_all.clicked.disconnect(self.pushButton_rst_all_handler)

        # settings tab
        self.read_settings_btn_1.clicked.disconnect(self.read_settings_btn_1_handler)
        self.read_settings_btn_2.clicked.disconnect(self.read_settings_btn_2_handler)
        self.save_settings_btn_1.clicked.disconnect(self.save_settings_btn_1_handler)

        #upgrade
        self.upgrade_file_button.clicked.disconnect(self.open_upgrade_file_handler)
        self.upgrade_button.clicked.disconnect(self.upgrade_file_handler)
        self.upgrade_button_golden.clicked.disconnect(self.upgrade_golden_handler)
        # self.upgrade_file_extract_btn.clicked.disconnect(self.upgrade_extract_handler)
        # self.upgrade_button_fpga.clicked.disconnect(self.upgrade_fpga_handler)
        # self.upgrade_button_sw.clicked.disconnect(self.upgrade_sw_handler)
        
        pass

    def mm_readOnCmd(self):
        print("opps!")
        
        if self.ll_mm_addr_lineEdit.text() == "" :
            QMessageBox.information(self,"Error!", "address Unacceptable!")  
            return
            
        addr =  str(self.ll_mm_addr_lineEdit.text()).upper()
        print('0x%08x'%int(addr, 16))
        
        status=self.m1.read_reg(int(addr,16))
        if status[0]:
            print("ok, 0x%x"%status[1])
        else:
            print("ng")
            QMessageBox.information(self,"Error!", "read failed! 请检查网络或端口号") 
            return
        
        self.ll_mm_value_lineEdit.setText(("%08x"%status[1]).upper())

    def mm_writeOnCmd(self):
        
        if self.ll_mm_addr_lineEdit.text() == "" :
            QMessageBox.information(self,"Error!", "address Unacceptable!")  
            return
            
        if self.ll_mm_value_lineEdit.text() == "" :
            QMessageBox.information(self,"Error!", "value Unacceptable!")  
            return
        
        addr = str(self.ll_mm_addr_lineEdit.text()).upper()
        value = str(self.ll_mm_value_lineEdit.text()).upper()
        
        status=self.m1.write_reg(int(addr,16), int(value,16))
        if status[0]:
            print("write ok")
        else:
            print("write ng")
            QMessageBox.information(self,"Error!", "write failed! 请检查网络或端口号")  
            return

    def ll_gpio_lineEdit_handler(self):
        self.ll_gpio_input_value=int(self.ll_gpio_lineEdit.text())
		
    def ll_gpio_dir_combobox_handler(self, index):
        self.ll_gpio_dir = index
    
    def ll_gpio_globalno_combobox_handler(self, index):
        self.ll_gpio_globalno = index

    def ll_gpio_read_btn_handler(self):
        if self.ll_gpio_dir == 0:
            self.g1.gpio_direction_input(self.ll_gpio_globalno)
            result = self.g1.gpio_read_input(self.ll_gpio_globalno)
        else:
            result = self.g1.gpio_get_output_status(self.ll_gpio_globalno)
        
        disp ='%d'% (result)
        self.ll_gpio_lineEdit.setText(disp)

    def ll_gpio_write_btn_handler(self):
        if self.ll_gpio_dir == 0:
            self.g1.gpio_direction_input(self.ll_gpio_globalno)
            self.input_value=1
        else:
            self.g1.gpio_direction_output(self.ll_gpio_globalno, self.ll_gpio_input_value)

    def iic_ch_check(self):
        ch = self.lineEdit.text()
        if(ch.isdigit() == False):
          QMessageBox.information(self,"Error!", "please check ch input!!!") 
          return False
        else:
          self.ll_iic_global_ch = int(ch,16)
          return True
    def is_hexadecimal(self,strstr):
        if strstr.startswith("0x") or strstr.startswith("0X"):
             strstr = strstr[2:]
        if len(strstr) == 0 or not strstr.isalnum():
             return False
        return all(c in string.hexdigits for c in strstr)

    def iic_device_addr_check(self,addr):
        if(self.is_hexadecimal(addr) == True):
            iic_addr = int(addr,16)
            if(iic_addr <= 0) or (iic_addr >= 255):
               QMessageBox.information(self,"Error!", "device addr 1-255!!!") 
               return False 
            else:
               self.ll_iic_addr =  iic_addr
               return True
        else:
            QMessageBox.information(self,"Error!", "please check device addr input!!!") 
            return False
    def iic_reg_addr_check(self,flag,strstr):
        if(self.is_hexadecimal(strstr) == True):
            reg_addr = int(strstr,16)
            if(flag == 1):   #8bit reg address
                if(reg_addr < 0) or (reg_addr >= 256):
                    QMessageBox.information(self,"Error!", "reg addr 0-255!!!") 
                    return False
                else:
                    self.ll_reg_addr = reg_addr
                    return True
            else:           #16bit reg address
                if(reg_addr < 0) or (reg_addr >= 65536):
                    QMessageBox.information(self,"Error!", "reg addr 0-65536!!!") 
                    return False
                else:
                    self.ll_reg_addr = reg_addr
                    return True
        else:
            QMessageBox.information(self,"Error!", "input address error!!!") 
            return False
    
    def iic_input_value_check(self,strstr):
        list_in=strstr.upper().split(',')
        if(len(list_in) == 0):
            QMessageBox.information(self,"Error!", "ll iic input is empty!") 
            return False
        
        self.ll_iic_input_values = []
        if(self.is_hex(list_in[0])==False):
            QMessageBox.information(self,"Error!", "ll iic input data errrrror: !") 
            return False
        elif((len(list_in) > 1) or (int(list_in[0],16) > 255)):           #只接受一个数据输入 并且小于256
            QMessageBox.information(self,"Error!", "ll iic input data errrrror: data overload!") 
            return False
        elif (list_in[0]==''):
            QMessageBox.information(self,"Error!", "ll iic input data errrrror: empty!") 
            return False
        
        else:
            self.ll_iic_input_values.append(int(list_in[0], 16))
            return True

    def iic_8bit_write_handler(self):
        if(self.iic_ch_check() == True):
            if(self.iic_device_addr_check(self.lineEdit_18.text()) == True):
                if(self.iic_reg_addr_check(1,self.lineEdit_19.text()) == True):
                    if(self.iic_input_value_check(self.lineEdit_20.text()) == True):
                        try:
                           result = self.i1.iic_send_data(self.ll_iic_global_ch, self.ll_iic_input_values, len(self.ll_iic_input_values), self.ll_iic_addr,self.ll_reg_addr,1)
                           if(result[5] is not 0x00):
                              QMessageBox.information(self,"提示!", "Register write failed!")  
                           else:
                              QMessageBox.information(self,"提示!", "Register write success!")
                        except Exception as ret:
                            QMessageBox.information(self,"Error!", "Exception %s" % ret) 

    def iic_8bit_read_handler(self):
        if(self.iic_ch_check() == True):
            if(self.iic_device_addr_check(self.lineEdit_15.text()) == True):
                if(self.iic_reg_addr_check(1,self.lineEdit_16.text()) == True):
                    if(self.iic_input_value_check(self.lineEdit_17.text()) == True):
                        try:
                           result = self.i1.iic_recv_data(self.ll_iic_global_ch, self.ll_iic_input_values[0], self.ll_iic_addr,self.ll_reg_addr,1)  
                           if(result[5] is not 0x00):
                              QMessageBox.information(self,"提示!", "Register read failed!")  
                           else:
                                disp =''
                                self.textEdit.append('reg_addr:'+str(self.lineEdit_16.text())+' R ') 
                                self.textEdit.moveCursor(QTextCursor.End)   
                                if len(result)!=0:
                                    for  index in range(self.ll_iic_input_values[0]):
                                        disp=' '+'0x%02x'% (result[6+index])
                                        self.textEdit.insertPlainText(disp)
                                QMessageBox.information(self,"提示!", "Register read success!")
                        except Exception as ret:
                            QMessageBox.information(self,"Error!", "Exception %s" % ret) 

    def iic_16bit_write_handler(self):
        if(self.iic_ch_check() == True):
            if(self.iic_device_addr_check(self.lineEdit_5.text()) == True):
                if(self.iic_reg_addr_check(1,self.lineEdit_6.text()) == True):
                    if(self.iic_input_value_check(self.lineEdit_7.text()) == True):
                        try:
                           result = self.i1.iic_send_data(self.ll_iic_global_ch, self.ll_iic_input_values, len(self.ll_iic_input_values), self.ll_iic_addr,self.ll_reg_addr,0)
                           if(result[5] is not 0x00):
                              QMessageBox.information(self,"提示!", "Register write failed!")  
                           else:
                              QMessageBox.information(self,"提示!", "Register write success!")
                        except Exception as ret:
                            QMessageBox.information(self,"Error!", "Exception %s" % ret) 
        
    def iic_16bit_read_handler(self):
        if(self.iic_ch_check() == True):
            if(self.iic_device_addr_check(self.lineEdit_2.text()) == True):
                if(self.iic_reg_addr_check(1,self.lineEdit_3.text()) == True):
                    if(self.iic_input_value_check(self.lineEdit_4.text()) == True):
                        try:
                           result = self.i1.iic_recv_data(self.ll_iic_global_ch, self.ll_iic_input_values[0], self.ll_iic_addr,self.ll_reg_addr,0)  
                           if(result[5] is not 0x00):
                              QMessageBox.information(self,"提示!", "Register read failed!")  
                           else:
                                disp =''
                                self.textEdit.append('reg_addr:'+str(self.lineEdit_3.text())+' R ') 
                                self.textEdit.moveCursor(QTextCursor.End)   
                                if len(result)!=0:
                                    for  index in range(self.ll_iic_input_values[0]):
                                        disp=' '+'0x%02x'% (result[6+index])
                                        self.textEdit.insertPlainText(disp)
                                QMessageBox.information(self,"提示!", "Register read success!")
                        except Exception as ret:
                            QMessageBox.information(self,"Error!", "Exception %s" % ret)
    # def ch0_select_handler(self, radioButton):
    #     if(radioButton.isChecked()):
    #         print(radioButton.text())
    #         pass
    #     pass

    # def ch1_select_handler(self, radioButton):
    #     if(radioButton.isChecked()):
    #         print(radioButton.text())
    #         pass
    #     pass

    # def ch2_select_handler(self, radioButton):
    #     if(radioButton.isChecked()):
    #         print(radioButton.text())
    #         pass
    #     pass

    # def ch3_select_handler(self, radioButton):
    #     if(radioButton.isChecked()):
    #         print(radioButton.text())
    #         pass
    #     pass

    # def ch4_select_handler(self, radioButton):
    #     if(radioButton.isChecked()):
    #         print(radioButton.text())
    #         pass
    #     pass

    def ch0_select_handler(self):
        radioButton = self.sender()
        if(radioButton.isChecked()):
            print(radioButton.text())
            self.vm1.cur_ch_set(0)
            pass
        pass
    def ch1_select_handler(self):
        radioButton = self.sender()
        if(radioButton.isChecked()):
            print(radioButton.text())
            self.vm1.cur_ch_set(1)
            pass
        pass
    def ch2_select_handler(self):
        radioButton = self.sender()
        if(radioButton.isChecked()):
            print(radioButton.text())
            self.vm1.cur_ch_set(2)
            pass
        pass
    def ch3_select_handler(self):
        radioButton = self.sender()
        if(radioButton.isChecked()):
            print(radioButton.text())
            self.vm1.cur_ch_set(3)
            pass
        pass
    def ch4_select_handler(self):
        radioButton = self.sender()
        if(radioButton.isChecked()):
            print(radioButton.text())
            self.vm1.cur_ch_set(4)
            pass
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

        try:
            self.m1
        except Exception as ret:
            print(ret)
        else:
            del self.m1
            
        try:
            self.ut
        except Exception as ret:
            print(ret)
        else:
            self.ut.udp_close()

    def open_upgrade_file_handler(self):
        print("select file")
        fileName = QFileDialog.getOpenFileName(
            self, 'choose the file', options=QFileDialog.DontUseNativeDialog)
        self.textEdit_upgradeFile.setText(fileName[0])

    def RecvTftpTreadSinal(self, ls):
        # 使用传回的返回值
        # if(ls == ['programme exit']):
        #     self.channel_timer.start(1000)#传送结束后重新开启定时器                        
        for word in ls:
            if ((word == "#") or (word == ".") or (word == "*")):
                self.textEdit_upgradeFile_out_put.moveCursor(QTextCursor.End)
                self.textEdit_upgradeFile_out_put.insertPlainText(word)
               # print(".")
            else :
                self.textEdit_upgradeFile_out_put.append(word)

    # def RecvTftpTreadSinal1(self, ls):
    #     # 使用传回的返回值
    #     # if(ls == ['programme exit']):
    #     #     self.channel_timer.start(1000)#传送结束后重新开启定时器                        
    #     for word in ls:
    #         if ((word == "#") or (word == ".") or (word == "*")):
    #             self.textEdit_upgradeFile_out_put.moveCursor(QTextCursor.End)
    #             self.textEdit_upgradeFile_out_put.insertPlainText(word)
    #            # print(".")
    #         else :
    #             self.textEdit_upgradeFile_out_put.append(word)

    def upgrade_file_handler(self):
        # self.channel_timer.stop()#传送开始前关闭定时器
        print("upgrade firmeware")
        if self.textEdit_upgradeFile.text()=="":
            QMessageBox.information(self, "Error!", "please select the file")
            return
        print("self.textEdit_upgradeFile.text()")
        try:
            # flash_add_cmd = memory_ops.pravite_cmd.upgrade_region_cmd.value
            # flash_addr= memory_ops.flash_upgrade_addr.all.value
            # self.m1.write_pravite_value(flash_add_cmd, flash_addr)
            self.tftpThread = tftp_put.tftp_md5_put_tread(self.textEdit_upgradeFile.text(),self.ipaddr_lineEdit.text())
            self.tftpThread.finishSignal.connect(self.RecvTftpTreadSinal)
            self.tftpThread.start()
        except Exception as ret:
            QMessageBox.information(self, "Error!", "Please check the file format")
            return
        # self.channel_timer.start()  # 传送开始前关闭定时器

    def upgrade_golden_handler(self):
        # self.channel_timer.stop()#传送开始前关闭定时器
        print("golden firmeware")
        if self.textEdit_upgradeFile.text()=="":
            QMessageBox.information(self, "Error!", "please select the file")
            return
        print("self.textEdit_upgradeFile.text()")
        try:
            # flash_add_cmd = memory_ops.pravite_cmd.upgrade_region_cmd.value
            # flash_addr= memory_ops.flash_upgrade_addr.all.value
            # self.m1.write_pravite_value(flash_add_cmd, flash_addr)
            self.tftpThread1 = tftp_put.tftp_md5_put_tread_g(self.textEdit_upgradeFile.text(),self.ipaddr_lineEdit.text())
            self.tftpThread1.finishSignal.connect(self.RecvTftpTreadSinal)
            self.tftpThread1.start()
        except Exception as ret:
            QMessageBox.information(self, "Error!", "Please check the file format")
            return
        # self.channel_timer.start()  # 传送开始前关闭定时器


    def upgrade_fpga_handler(self):
        print("upgrade pl firmware")
        if self.textEdit_upgradeFile.text() == "":
            QMessageBox.information(self, "Error!", "please select the file")
            return
        print("self.textEdit_upgradeFile.text()")

        str_len=len(self.textEdit_upgradeFile.text())
        find_idx=self.textEdit_upgradeFile.text().find('.pl',0,str_len)
        if(find_idx==-1):
            QMessageBox.information(self, "Error!", "please select the right file")
            return
        fd = open(self.textEdit_upgradeFile.text(), "rb")
        fd.seek(-1,2)# 从文件尾部 向前移动1个字节
        realfileLen = 1+fd.tell()-0xdb80 ##0x2db80 是bif 文件中配置的固定偏移量
        fd.close()
        print("pl-file-real-size:"+"%d bytes"%realfileLen)
        cmd = memory_ops.pravite_cmd.upgrade_pl_size.value
        self.m1.write_pravite_value(cmd,realfileLen)
        flash_add_cmd = memory_ops.pravite_cmd.upgrade_region_cmd.value
        flash_addr = memory_ops.flash_upgrade_addr.fpga.value
        self.m1.write_pravite_value(flash_add_cmd, flash_addr)
        self.tftpThread = tftp_put.tftp_md5_put_tread(self.textEdit_upgradeFile.text(), self.ipaddr_lineEdit.text())
        self.tftpThread.finishSignal.connect(self.RecvTftpTreadSinal)
        self.tftpThread.start()
    def upgrade_sw_handler(self):
        print("upgrade pl firmware")
        if self.textEdit_upgradeFile.text() == "":
            QMessageBox.information(self, "Error!", "please select the file")
            return
        print("self.textEdit_upgradeFile.text()")

        str_len=len(self.textEdit_upgradeFile.text())
        find_idx=self.textEdit_upgradeFile.text().find('.sw',0,str_len)
        if(find_idx==-1):
            QMessageBox.information(self, "Error!", "please select the right file")
            return
        fd = open(self.textEdit_upgradeFile.text(), "rb")
        fd.seek(-1,2)# 从文件尾部 向前移动1个字节
        realfileLen = 1+fd.tell()-0xdb80 ##0x2db80 是bif 文件中配置的固定偏移量
        fd.close()
        print("sw-file-real-size:" + "%d bytes" % realfileLen)
        cmd = memory_ops.pravite_cmd.upgrade_sw_size.value
        self.m1.write_pravite_value(cmd,realfileLen)
        flash_add_cmd = memory_ops.pravite_cmd.upgrade_region_cmd.value
        flash_addr = memory_ops.flash_upgrade_addr.sw.value
        self.m1.write_pravite_value(flash_add_cmd, flash_addr)
        self.tftpThread = tftp_put.tftp_md5_put_tread(self.textEdit_upgradeFile.text(), self.ipaddr_lineEdit.text())
        self.tftpThread.finishSignal.connect(self.RecvTftpTreadSinal)
        self.tftpThread.start()

    def upgrade_extract_handler(self):
        print("upgrade firmeware")
        if self.textEdit_upgradeFile.text() == "":
            QMessageBox.information(self, "Error!", "please select the file")
            return
        print("extract file")
        status =extract_fw.extract_xilinx_file(self.textEdit_upgradeFile.text(), 1)
        if(status[0]==True):
            self.textEdit_upgradeFile_out_put.append("creat "+self.textEdit_upgradeFile.text()+".pl"+
                                                     " %d bytes"%status[1])
        if (status[2] == True):
            self.textEdit_upgradeFile_out_put.append(
                "creat " + self.textEdit_upgradeFile.text() + ".sw" + " %d bytes" % status[3])


    def pushButton_rst_all_handler(self):
        try:
            self.vm1.rst_set(1)
        except Exception as ret:
            print(ret)
        pass

    def read_settings_btn_1_handler(self):
        status=self.vm1.read_var(10)
        # self.refresh_all(status)
        # ipaddr
        value = socket.inet_ntoa(status[2])
        self.settings_ipaddr_lineEdit.setText(value)
        # mask
        value = socket.inet_ntoa(status[3])
        self.settings_netmask_lineEdit.setText(value)
        # gw
        value = socket.inet_ntoa(status[4])
        self.settings_gateway_lineEdit.setText(value)
        # mac
        self.settings_mac_lineEdit.setText(('%02x%02x%02x%02x%02x%02x' %(status[5][0], status[5][1], status[5][2], status[5][3], status[5][4], status[5][5])).upper())
        
        
    def read_settings_btn_2_handler(self):
        status=self.vm1.read_var(12)
        # self.refresh_all(status)
        # ipaddr
        value = socket.inet_ntoa(status[2])
        self.settings_ipaddr_lineEdit.setText(value)
        # mask
        value = socket.inet_ntoa(status[3])
        self.settings_netmask_lineEdit.setText(value)
        # gw
        value = socket.inet_ntoa(status[4])
        self.settings_gateway_lineEdit.setText(value)
        # mac
        self.settings_mac_lineEdit.setText(('%02x%02x%02x%02x%02x%02x' %(status[5][0], status[5][1], status[5][2], status[5][3], status[5][4], status[5][5])).upper())
        

    def save_settings_btn_1_handler(self):
        status = check_ip_addr.is_valid_ipv4_address(self.settings_ipaddr_lineEdit.text())
        if status is False:
            QMessageBox.information(self,"Error!", "ip address errrrror!") 
            return

        status = check_ip_addr.is_valid_ipv4_address(self.settings_netmask_lineEdit.text())
        if status is False:
            QMessageBox.information(self,"Error!", "mask address errrrror!") 
            return

        status = check_ip_addr.is_valid_ipv4_address(self.settings_gateway_lineEdit.text())
        if status is False:
            QMessageBox.information(self,"Error!", "gw address errrrror!") 
            return

        if self.settings_mac_lineEdit.text() == "" :
            QMessageBox.information(self,"Error!", "value Unacceptable!")  
            return

        buf=[]
        buf.append(0)
        buf.append(struct.unpack('I', socket.inet_aton(self.settings_ipaddr_lineEdit.text()))[0])  # ipaddr
        buf.append(struct.unpack('I', socket.inet_aton(self.settings_netmask_lineEdit.text()))[0])  # mask
        buf.append(struct.unpack('I', socket.inet_aton(self.settings_gateway_lineEdit.text()))[0])  # gw
        buf.append(int(self.settings_mac_lineEdit.text(), 16))  # mac

        ret = self.vm1.write_config(buf)
        if(ret[0] == False):
            QMessageBox.information(self,"Error!", "network failed while write eeprom!")  
            
        if(ret[1]!=0):
            QMessageBox.information(self,"Error!", "write eeprom failed!")  
        else:
            QMessageBox.information(self,"Error!", "write eeprom success!")  
        pass

    def ipaddr_connect_btn_handler(self):
        status = check_ip_addr.is_valid_ipv4_address(self.ipaddr_lineEdit.text())
        if status is False:
            QMessageBox.information(self,"Error!", "ip address errrrror!") 
        else:
            self.ipaddr = self.ipaddr_lineEdit.text()
            try:
                self.check_connection(self.ipaddr)
                try:
                    self.ut = udp_logic.udp_logic(server_host=self.ipaddr)
                    #self.ut = udp_logic.udp_logic(server_host='192.168.1.10')
                    self.m1 = memory_ops.memory_ops(self.ut)
                    self.m1.read_reg(HARD_VER_ADDR)
                except Exception as ret:
                    print(ret)
                    QMessageBox.information(self,"Error!", "connection failed!")
                    self.ut.udp_close()
                    del self.ut
                    return
                #self.timer.start(1000)
                # self.channel_timer.start(1000)
                try:
                    self.g1 = gpio_ops.gpio_ops(self.ut)
                    self.xg1 = xgpio_ops.xgpio_ops(self.ut)
                    self.xsw1 = xaxis_switch_ops.xaxis_switch_ops(self.ut)
                    self.vm1 = var_msg_ops.var_msg_ops(self.ut)
                    self.i1 = iic_ops_msg.iic_ops_msg(self.ut)
                    self.connection_status = True
                    self.ipaddr_disconnect_btn.setDisabled(False)
                    self.ipaddr_connect_btn.setDisabled(True)
                except Exception as ret:
                    print(ret)
                    QMessageBox.information(self,"Error!", "connection failed!")
                    self.ut.udp_close()
                    del self.ut
                
            except Exception as ret:
                QMessageBox.information(self,"Error!", "target address not alive!") 

            if(self.connection_status == True):
                # if(self.i1.iic_query(self.ll_iic_ch) != 0x00):
                #     self.ll_iic_status_label.setText('Busy')
                # else:
                #     self.ll_iic_status_label.setText('Ready')

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
                # HSY
                # status=self.vm1.read_var(FREQ_VER_IDX)
                # if status[0]:
                #     print("ok, 0x%x"%status[1])
                #     self.freq_label.setText(("%d"%status[1]).upper())
                # else:
                #     QMessageBox.information(self,"Error!", "communication status err") 
                #     print("ng")  

                # width = self.vm1.read_var(WIDTH_VER_IDX)
                # height = self.vm1.read_var(HEIGHT_VER_IDX)
                # if(height[0] and width[0]):
                #     self.pixel_label.setText(("%d"%(width[1]*2)).upper() + "x" +("%d"%height[1]).upper())
                # else:
                #     QMessageBox.information(self,"Error!", "communication status err") 

                # status=self.vm1.cur_ch_get()
                # if status[0]:
                #     print("ok, 0x%x"%status[1])
                #     if(status[1]==0):
                #         self.q_radioButton_ch0.setChecked(True)
                #     elif(status[1]==1):
                #         self.q_radioButton_ch1.setChecked(True)
                #     elif(status[1]==2):
                #         self.q_radioButton_ch2.setChecked(True)
                #     elif(status[1]==3):
                #         self.q_radioButton_ch3.setChecked(True)
                #     # elif(status[1]==4):
                #     #     self.q_radioButton_ch4.setChecked(True)
                #     else:
                #         QMessageBox.information(self,"Error!", "ch return err") 
                # else:
                #     QMessageBox.information(self,"Error!", "communication status err") 
                #     print("ng")


                self.pushButton_rst_all.setDisabled(False)
                # status=self.vm1.read_var(10)
                # # ipaddr
                # value = socket.inet_ntoa(status[2])
                # self.settings_ipaddr_lineEdit.setText(value)
                # # mask
                # value = socket.inet_ntoa(status[3])
                # self.settings_netmask_lineEdit.setText(value)
                # # gw
                # value = socket.inet_ntoa(status[4])
                # self.settings_gateway_lineEdit.setText(value)
                # # mac
                # self.settings_mac_lineEdit.setText(('%02x%02x%02x%02x%02x%02x' %(status[5][0], status[5][1], status[5][2], status[5][3], status[5][4], status[5][5])).upper())
                
                self.ConnectSignalSlot()

                self.q_radioButton_ch0.setDisabled(False)
                self.q_radioButton_ch1.setDisabled(False)
                self.q_radioButton_ch2.setDisabled(False)
                self.q_radioButton_ch3.setDisabled(False)
                # self.q_radioButton_ch4.setDisabled(False)

                self.ll_mm_read_btn.setDisabled(False)
                self.ll_mm_write_btn.setDisabled(False)

                #iic
                self.iic_16bit_read.setDisabled(False)
                self.iic_16bit_write.setDisabled(False)
                self.iic_8bit_read.setDisabled(False)
                self.iic_8bit_write.setDisabled(False)

                self.upgrade_file_button.setDisabled(False)
                self.upgrade_button.setDisabled(False)
                self.upgrade_button_golden.setDisabled(False)

                pass
            pass
        pass

    def clear_display(self):
        self.textEdit_upgradeFile.setText("")
        self.ll_mm_addr_lineEdit.setText("")
        self.ll_mm_value_lineEdit.setText("")
        self.ll_spi_send_lineEdit.setText("")
        self.ll_spi_recv_lineEdit.setText("")
        self.lineEdit_2.setText("")
        self.lineEdit_3.setText("")
        self.lineEdit_4.setText("")

        self.lineEdit_5.setText("")
        self.lineEdit_6.setText("")
        self.lineEdit_7.setText("")

        self.lineEdit_15.setText("")
        self.lineEdit_16.setText("")
        self.lineEdit_17.setText("")

        self.lineEdit_18.setText("")
        self.lineEdit_19.setText("")
        self.lineEdit_20.setText("")

        self.lineEdit.setText("")
        self.textEdit.setText("")

        self.ll_gpio_lineEdit.setText("")
        
        # self.ipaddr_lineEdit_2.setText("")
        # self.ipaddr_lineEdit_3.setText("")
        # self.ipaddr_lineEdit_4.setText("")
        # self.ipaddr_lineEdit_5.setText("")
       
              
        self.ll_spi_ch_combobox.setCurrentIndex(0)
        self.ll_spi_cs_combobox.setCurrentIndex(0)
        self.ll_gpio_globalno_combobox.setCurrentIndex(0+78)
        self.ll_gpio_dir_combobox.setCurrentIndex(0)

        self.hardware_ver_label.setText("?")
        self.firmware_ver_label.setText("?")

        ## Reset radioButton status
        self.q_radioButton_ch0.setAutoExclusive(False)
        self.q_radioButton_ch0.setChecked(False)
        self.q_radioButton_ch0.setAutoExclusive(True)
        self.q_radioButton_ch0.setDisabled(True)
        self.q_radioButton_ch1.setAutoExclusive(False)
        self.q_radioButton_ch1.setChecked(False)
        self.q_radioButton_ch1.setAutoExclusive(True)
        self.q_radioButton_ch1.setDisabled(True)
        self.q_radioButton_ch2.setAutoExclusive(False)
        self.q_radioButton_ch2.setChecked(False)
        self.q_radioButton_ch2.setAutoExclusive(True)
        self.q_radioButton_ch2.setDisabled(True)
        self.q_radioButton_ch3.setAutoExclusive(False)
        self.q_radioButton_ch3.setChecked(False)
        self.q_radioButton_ch3.setAutoExclusive(True)
        self.q_radioButton_ch3.setDisabled(True)
        # self.q_radioButton_ch4.setAutoExclusive(False)
        # self.q_radioButton_ch4.setChecked(False)
        # self.q_radioButton_ch4.setAutoExclusive(True)
        # self.q_radioButton_ch4.setDisabled(True)

        self.ll_mm_read_btn.setDisabled(True)
        self.ll_mm_write_btn.setDisabled(True)

        self.iic_16bit_read.setDisabled(True)
        self.iic_16bit_write.setDisabled(True)
        self.iic_8bit_read.setDisabled(True)
        self.iic_8bit_write.setDisabled(True)

        self.upgrade_file_button.setDisabled(True)
        self.upgrade_button.setDisabled(True)
        self.upgrade_button_golden.setDisabled(True)

        #HSY
        self.freq_label.setText("?")
        self.pixel_label.setText("?")

        pass


    def ipaddr_disconnect_btn_handler(self):
        self.DisconnectSignalSlot()
        self.ipaddr_disconnect_btn.setDisabled(True)
        self.ipaddr_connect_btn.setDisabled(False)
        self.connection_status = False
        # self.channel_timer.stop()
       
        '''
        self.native_loopback_on_radioButton.setChecked(False)
        self.modem_connect_radioButton.setChecked(False)
        self.native_test_pattern_radioButton.setChecked(False)
        self.stream_loopback_on_radioButton.setChecked(False)
        '''
                
        self.q_selecta= STANDBY
        self.q_selectb= STANDBY
        
        del self.m1
        del self.g1
        del self.xg1
        del self.xsw1
        
        self.ut.udp_close()
        del self.ut
        
        self.clear_display()

    def check_connection(self, ip_addr):
        status = check_ping.check_ping(ip_addr)
        if(status is True):
            print("1")
        else:
            print("2")
    
    def check_connection_alive(self):
        self.check_connection(self.ipaddr)

    # HSY
    # def check_channel_connnect(self):
    #     # status = check_ping.check_ping(ip_addr)
    #     # if(status is True):
    #     # if (self.connection_status == True):
    #     try:
    #         status = self.vm1.read_var(FREQ_VER_IDX)
    #         if status[0]:
    #             print("ok, 0x%x" % status[1])
    #             self.freq_label.setText(("%d" % status[1]).upper())
    #         else:
    #             QMessageBox.information(self, "Error!", "communication status err")
    #             print("ng")

    #         width = self.vm1.read_var(WIDTH_VER_IDX)
    #         height = self.vm1.read_var(HEIGHT_VER_IDX)
    #         if (height[0] and width[0]):
    #             self.pixel_label.setText(("%d"%(2*width[1])).upper() + "x" +("%d"%height[1]).upper())
    #         else:
    #             QMessageBox.information(self, "Error!", "communication status err")
    #             #  print("HH")
    #     except Exception as ret:
    #         print("check_channel %s" % ret)
    #     else :
    #        print("check_channel ok")
    # def channel_change(self):
    #     self.check_channel_connnect()
        


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
