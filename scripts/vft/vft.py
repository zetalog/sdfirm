# -*- coding: utf-8 -*-

import os
import sys
import serial
import time
import threading
from socket import *
from subprocess import Popen, PIPE, STDOUT
import logging

class VFT(threading.Thread):
    PENDING = -1
    SUCCESS = 0
    TIMEOUT = 1

    def __init__(self,name='vft'):
        super(VFT,self).__init__(name=name)
        #threading.Thread.__init__(self,name=name)
        self.name = name
        self.expect = ''       #cmd expect result
        self.execute_time = 0  #cmd runtime
        self.timeout = 0       #cmd timeout
        self.buff_line = ''    #serial buff

        self.expect_size = 0
        self.expect_len = [0 for i in range(11)]
        self.expect_pos = [0 for i in range(11)]
        self.expect_str = [''for i in range(11)]
        self.expect_res = [0 for i in range(11)]

        self.capture_on = True

        self.reset_expect(self.SUCCESS)
        self.lock = threading.Lock()


    def saveFile(self,filename,data):
        filename = filename + '.txt'
        file = open(filename,'a')    #save log
        file.write(data)
        file.close()

    def is_success(self):
        return self.result == self.SUCCESS
    def is_pending(self):
        return self.result == self.PENDING
    def is_failure(self):
        return ((self.is_pending() == False) and (self.is_success() == False))

    def capture(self,on):
        self.capture_on = on

    def analysis(self):
        time.sleep(0.1)
        self.lock.acquire()
        is_success = False
        expect_res = self.PENDING
        #logging.info('self.expect = '+str(self.expect_str))             #this info is for debug 
        if (self.expect_size):
            for i in range (0, self.expect_size):
                while (len(self.expect_str[i]) and                       #expect_str 的长度不为空
                       len(self.expect_str[i]) > self.expect_len[i] and  #expect_str 的长度大于已经匹配的长度
                       len(self.buff) > self.expect_len[i] + self.expect_pos[i]): #buff的长度大于已经匹配和起始长度之和
                    if (self.buff[self.expect_len[i] + self.expect_pos[i]] ==
                        self.expect_str[i][self.expect_len[i]]):         #如果下一个字符能够匹配
                        self.expect_len[i] = self.expect_len[i]+1        #更新匹配长度
                        if (len(self.expect_str[i]) == self.expect_len[i] and
                            (self.result == self.PENDING or self.result == self.SUCCESS)): #如果expect_str全匹配 并且结果是未决或者成功
                            is_success = True        #找到标志位置位
                            #logging.info("expect string "+str([expect_res])+ " = " + self.expect_str[i])
                            expect_res = self.expect_res[i] #记录返回值
                    else:
                        self.expect_pos[i] = self.expect_pos[i]+1  #没有匹配到指向下一个位置
                        self.expect_len[i] = 0                     #已匹配的长度清零
            pos = len(self.buff)
            found = False
            for i in range (0, self.expect_size):
                if (self.expect_len[i] != 0 and pos > self.expect_pos[i]): #匹配的长度大于0 然后找到匹配起始最小的位置
                    pos = self.expect_pos[i]
                    found = True
            if (pos != len(self.buff) or found == False):  
                self.buff = self.buff[pos:]                         #丢弃无效buffer
                for i in range (0, self.expect_size):
                    self.expect_pos[i] -= pos
            if (is_success):
                self.reset_expect(expect_res)                       #重置结果
        has_success = False
        if (self.is_pending() and (time.time() - self.execute_time) > self.timeout): #未决且超时
            for i in range (0, self.expect_size):
                if (len(self.expect_str[i])):    #如果长度不为空的话 
                    self.reset_expect(self.TIMEOUT) #结果首先设为超时
                if (self.expect_res[i] == self.SUCCESS): #检测return中是否有成功
                    has_success = True
            if (self.result == self.TIMEOUT and has_success == False): # return中没有成功，并且已经超时
                self.reset_expect(self.SUCCESS)
        if (self.is_failure()):
            self.buff = ""
        self.lock.release()

    def reset(self):
        self.result = self.SUCCESS

    def reset_expect(self,result):
        self.result = result
        #logging.info(self.name+":result="+str(result))
        for i in range(0, self.expect_size):
            self.expect_len[i] = 0
            self.expect_pos[i] = 0
            self.expect_str[i] = ''
        self.expect_size = 0

    def execute(self,cmd,timeout,expect):
        try:
            self.lock.acquire()
            self.reset_expect(self.PENDING)
            self.expect_size = len(expect)
            i = 0
            for key in expect.keys():
                self.expect_str[i] = key
                self.expect_res[i] = expect[key]
                i = i+1
            self.timeout = timeout
            self.execute_time = time.time()
            self.lock.release()
            self.write(cmd)
        except Exception as e:
            logging.info(e)

    def sync(self):
        self.analysis()

    def run(self):
        while (self.stopping == False):
            if self.is_open():
                data = self.read()
                file=open('C:\\Python35\\VMIN test\\log.txt','a')
                try:
                    file.write(data)
                except Exception as e:
                    logging.info(e)
                file.close()
                #self.saveFile(self.name,data)

                try:
                    if (self.capture_on):					    
                        self.print_callback(str(data))            #when the test is running, we'd better open the log to know the cpu status 
                except Exception as e:
                    logging.info(e)
                self.lock.acquire()
                self.buff += data
                if (self.buff.find('\n')>-1):
                    list = self.buff.split('\n', 1)
                    self.lock.release()
                    data = list[0]
                else:
                    self.lock.release()

class PORT(VFT):
    def __init__(self,name="uart"):
        super(PORT,self).__init__(name)
        self.buff = ''

    def open(self, port, callback):
        self.port = port
        self.rate = 230400
        self.stopping = False
        self.print_callback = callback
        try:
            self.serialport = serial.Serial(self.port,self.rate) # error return
        except Exception as e:
            logging.info('Open port error:',e)
            os._exit(0)
        try:
            self.start()
        except Exception as e:
            logging.info('Port thread error:',e)
            os._exit(0)

    def close(self):
        self.stopping = True
        self.serialport.close()

    def write_port(self,data):
        r = self.serialport.write(data.encode('utf-8'))

    def read_port(self,len):
        try:
            data = self.serialport.read(len).decode()
        except TypeError as e:
            data = ''
        except UnicodeDecodeError as e:
            data = ''
        except AttributeError as e:
            data = ''
        return data

    def is_open(self):
        return self.serialport.isOpen

    def write(self, data):
        try:
            self.write_port(data)
        except Exception as e:
            logging.info(e)

    def read(self):
        try:
            len = self.serialport.inWaiting()
        except Exception as e:
            self.stopping = True
            return ''
        if (len):
            data = self.read_port(len)
            return data
        else:
            return ''

class PIPEFrame(VFT):   #PIPE for backup
    def __init__(self,name="pipe"):
        super(PIPEFrame,self).__init__(name)
        self.buff = ''    #template buff
        self.p = None

    def open(self):
        self.stopping = False
        try:
            os.environ["PATH"] = os.environ["PATH"]+';'+r'C:\python35\vft\fwutils'
            self.p = Popen('imcdiag.exe -p COM3',stdout = PIPE, stdin = PIPE, stderr = STDOUT, shell = True)
            #self.start()
        except Exception as e:
            logging.info(e)
            os._exit(0)

    def is_open(self):
        return True

    def close(self):
        self.stopping = True
        self.p.close()

    def read(self):
        data = self.p.stdout.readline().decode()
        return data

    def write(self,cmd):
        self.p.stdin.write(cmd.encode())

class TCPFrame(VFT):    #TCP for diag tool
    def __init__(self,name="tcp"):
        super(TCPFrame,self).__init__(name)
        self.buff = ''    #template buff
        self.client = None
        self.buffsize = 1024

    def open(self,address = '127.0.0.1',port = 6500):
        self.stopping = False
        ADDR = (address,port)
        try:
            self.client = socket(AF_INET,SOCK_STREAM)
        except error as e:
            logging.info('Create socket:',e)
            os._exit(0)
        try:
            self.client.connect(ADDR)
        except gaierror as e:
            logging.info('Address error connecting to server:',e)
            os._exit(0)
        except error as e:
            logging.info('Connection error:',e)
            os._exit(0)
        try:
            self.start()
        except Exception as e:
            logging.info('TCP Client thread error:',e)
            os._exit(0)

    def is_open(self):
        return True

    def close(self):
        self.stopping = True
        self.client.close()

    def read(self):
        try:
            data = self.client.recv(self.buffsize).decode()
            return data
        except Exception as e:
            logging.info(e)

    def write(self,cmd):
        try:
            logging.info(cmd)
            self.client.send(cmd.encode())
            self.client.send('\r\n'.encode())
        except Exception as e:
            logging.info(e)
