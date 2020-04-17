import time
import threading
import os
import signal
import sys
import atexit
import csv
import subprocess
import re
import numpy as np 
from vft import VFT,PORT,TCPFrame
from matplotlib import pyplot as plt
import logging
import json

def sigterm_handler(sig, frame):
    logging.info("Ctrl+C")
    vmin_hlos.close()

def display(data):
    sys.stdout.write(data)
    
def load_config():
    with open('C:\\Python35\\VMIN test\\config.json') as json_file:
        data = json.load(json_file)
        return data

setting = load_config()

class VMIN():

    # configurables
    bmc_ip = setting["bmc_ip"]

    diag_ip = '127.0.0.1'
    diag_port = 6500

    ipmi_cmd  = 'ipmitool -I lanplus -H '
    ipmi_cmd += bmc_ip

    #rep bmc username and password
    #ipmi_cmd += ' -U admin -P admin '

    #sdp bmc username and password
    ipmi_cmd += ' -U admin -P Password1 '

    voltage_frequency_dtype = np.dtype([('Chip version', 'U10'), ('Pattern', 'U10'), ('Frequency/MHz', 'int'), ('Vmin/mv', 'int'),('Runtime/s','float'),('Wafer ID','U10')])
    voltage_frequency_dtype_rail = np.dtype([('Chip version', 'U10'), ('Pattern', 'U10'), ('rail', 'int'), ('Frequency/MHz', 'int'), ('Vmin/mv', 'int'),('Runtime/s','float'),('Wafer ID','U10')])
    vminarr = np.empty((1,4),voltage_frequency_dtype)


    def __init__(self):
        super(VMIN,self).__init__()
        os.environ["PATH"] = os.environ["PATH"]+';'+r'C:\fwutils'

        logging.basicConfig(level=logging.DEBUG,
        format='%(asctime)s %(filename)s[line:%(lineno)d] %(levelname)s %(message)s',
        datefmt='%a, %d %b %Y %H:%M:%S',
        filename='C:\\Python35\\VMIN test\\myapp.log',
        filemode='w')
        console = logging.StreamHandler()
        console.setLevel(logging.DEBUG)
        formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
        console.setFormatter(formatter)
        logging.getLogger('').addHandler(console)
        self.pattern_name = setting["cpu_pattern_name"]

        self.repeatflag = False
        self.initial_voltage = 1055
        self.tcu_ip=setting["tcu_ip"]
        self.platform = setting["platform"]

    def success(self,msg):
        self.msg = msg
        self.failed = False

    def failure(self,msg):
        self.msg = msg
        self.failed = True

    def fatal(self,msg):
        self.msg = msg
        logging.info(self.msg)
        os._exit(0)

    def run_ipmi(self, cmd, expect):
        run_cmd = 'C:\\fwutils\\'+self.ipmi_cmd + cmd
        logging.info(run_cmd)
        line = os.popen(run_cmd).read()
        if (not expect or line.find(expect) > -1):
            return True
        else:
            return False

    def simple_expect(self, timeout, string, hint):
        self.simple_expect2(timeout, '', {string:0}, hint)

    def power_off(self):
        last_time = time.time()
        while (self.stopping == False):
            result = self.run_ipmi('power status', "Power is off")
            if (result == True):
                logging.info("power off success")
                break
            if ((time.time() - last_time) > 40):
                self.fatal('power off failure!')
            # sleep to increase success ratio
            time.sleep(5)
            self.run_ipmi('power off', "")

    def power_on(self):
        last_time = time.time()
        while (self.stopping == False):
            result = self.run_ipmi('power status', "Power is on")
            if (result == True):
                logging.info("power on success")
                break
            if ((time.time() - last_time) > 40):
                self.fatal('power on failure!')
            self.run_ipmi('power on', "")
            # sleep to increase success ratio
            time.sleep(5)
    
    def controlTCU(self,value):

        #deliver Temp cmd
        cmd = "TC:COMM:OPEN:TCP {0},AUTO\nCONT:SPT {1}\nMEAS:PV?\nTC:COMM:CLOSE\n".format(self.tcu_ip,value).encode("utf-8")
		#SCPI_CLI.exe dir
        process = subprocess.Popen('C:\\TC_SCPI\\lib\\SCPI-CLI.exe', stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        process.stdin.write(cmd)
        process.stdin.flush()
        logging.info (cmd)
        for line in iter(process.stdout.readline, ''):
            sys.stdout.flush()
            if line.decode().find('"')>0:
                #tcu_ip error
                logging.info ("tcu_ip error")
                break
            if line.decode().find('TC_CLI> TC_CLI>')>-1:
                #communication is ok
                logging.info(line)
                break
        process.terminate()
        
		#validate temp have reached the target
        while True:
            command = 'TC:COMM:OPEN:TCP {0},AUTO\nMEAS:PV?\nTC:COMM:CLOSE\n'.format(self.tcu_ip).encode("utf-8")
            process = subprocess.Popen('C:\\TC_SCPI\\lib\\SCPI-CLI.exe', stdin=subprocess.PIPE, stdout=subprocess.PIPE)
            process.stdin.write(command)
            process.stdin.flush()
            currenttemp = ''
            for line in iter(process.stdout.readline, ''):
                sys.stdout.flush()
                line = line.decode('GBK')
                if line.find('TC_CLI> TC_CLI>')>-1:
                    currenttemp = re.sub(r' |\n|\r','',line)[14:]
                    logging.info(currenttemp)
                    break
            process.terminate()
            time.sleep(2)
            if abs(round(float(currenttemp),1)- int(value))<= 0.5:
                break

    def norminal_frequency(self, argument):
        switcher = {
        0: 2400,
        1: 2100,
        2: 1100,
        3: 500}
        return switcher.get(argument, "nothing")
		
    def select_temperature(self, argument):
        switcher = {
        0: '5',
        1: '25',
        2: '75'
		}
        return switcher.get(argument, "nothing")

    def run(self, run_type, per_rail=True):
        for i in range(1,3):
            temperature = self.select_temperature(i)
            self.controlTCU(temperature)
            self.power_off()
            if (run_type == 'calibration'):
                array_column = 25
            else:
                array_column = 4

            if (per_rail == True):
                max = 8
                with open("C:\\Python35\\VMIN test\\result.csv","a") as csvfile: 
                    writer = csv.writer(csvfile)
                    writer.writerow(["Chip version","Pattern","rail","Frequency/MHz","Vmin/mv","Runtime/s","Wafer ID"])
                self.vminarr = np.empty((max,array_column),self.voltage_frequency_dtype_rail)
            else:
                max = 1
                with open("C:\\Python35\\VMIN test\\result.csv","a") as csvfile:
                    writer = csv.writer(csvfile)
                    writer.writerow(["Chip version","Pattern","Frequency/MHz","Vmin/mv","Runtime/s","Wafer ID"])
                self.vminarr = np.empty((max,array_column),self.voltage_frequency_dtype)

            for rail_num in range(0, max):
                loop = 0
                if (run_type == 'calibration'):
                    frequency = 3000   
                else:
                    frequency = 2600
                while ( frequency >= 600 ):   # repeat VF ponits frequencies
                    if ( frequency == 3000 ):
                        voltage = 1050
                    else:
                        if (run_type == 'norminal' and frequency == 1100):
                            voltage = 580
                        elif (run_type == 'norminal' and frequency == 2600):
                            voltage = 900
                        elif (run_type == 'norminal' and frequency == 2400):
                            voltage = self.vminarr[rail_num][loop-1]['Vmin/mv'] - 20  
                        elif (run_type == 'norminal' and frequency == 2100):
                            voltage = self.vminarr[rail_num][loop-1]['Vmin/mv'] - 40                            
                        else:
                            voltage = self.vminarr[rail_num][loop-1]['Vmin/mv'] 

                    if (self.repeatflag == True):
                        voltage = voltage + 30
                        self.repeatflag = False
                    if (voltage > 1055):
                        voltage = 1055
                    self.initial_voltage = voltage

                    self.boot_board()

                    localtime = time.asctime( time.localtime(time.time()) )
                    logging.info(localtime)
                    start = time.time()
                    self.imc.execute('pmbus open\r\n', 10, self.bare_voltage_open)
                    if (self.sync() == False):
                        logging.info('pmbus open failure!')

                    self.imc.execute('pmbus operation all write nominal\r\n', 30, self.bare_voltage_norminal)
                    if (self.sync() == False):
                        logging.info('pmbus operation failure!')

                    self.imc.execute('pmbus uvout all write 250\r\n', 30, self.bare_voltage_uvout)
                    if (self.sync() == False):
                        logging.info('pmbus uvout write failure!')

                    self.imc.execute('pmbus uvout all read\r\n', 30, self.bare_voltage_uvout_read)
                    if (self.sync() == False):
                        logging.info('pmbus uvout read failure!')

                    self.set_voltage_bare(1055)

                    self.set_frequency_bare(rail_num, frequency)

                    while True:
                        voltage -= 5
                        self.set_voltage_bare(voltage)
                        logging.info('f =  ' + str(frequency) + ' v = ' +str(voltage))
                        time.sleep(2)
                        if (self.run_pattern_bare(rail_num, per_rail) == False):
                            logging.info('find the Vmin in frequency :' + str(frequency))
                            break

                    self.power_off()
                    if (self.initial_voltage == (voltage+5) and (voltage+5) != 1055):
                        self.repeatflag = True       
                    logging.info ('repeatflag = '+str(self.repeatflag))
                    if (self.repeatflag != True):
                        self.vminarr[rail_num][loop]['Vmin/mv'] = voltage + 5
                        self.vminarr[rail_num][loop]['Frequency/MHz'] = frequency

                        end = time.time()
                        self.vminarr[rail_num][loop]['Runtime/s'] = end - start
                        logging.info('vmin is ' + str(self.vminarr[rail_num][loop]['Vmin/mv']))
                        logging.info('runtime is ' + str(self.vminarr[rail_num][loop]['Runtime/s']))

                        localtime = time.asctime( time.localtime(time.time()) )
                        logging.info(localtime)

                        if (run_type == 'calibration'):
                            loop = loop + 1
                            frequency = frequency - 100
                            time.sleep(5)
                        elif (run_type == 'norminal'):
                            frequency = self.norminal_frequency(loop)
                            loop = loop + 1
                            time.sleep(5)
                        else:
                            logging.info("invalid run type!!")

                        if (loop == 1):
                            f = open('C:\\Python35\\VMIN test\\log.txt','r')
                            f.seek(0)
                            str_line = f.readline()
                            while str_line:
                                str_line = f.readline()
                                word1 = str_line.find("Chip version:")
                                if (word1 > -1):
                                    value = str_line[-6:-1].strip()
                                    chip_version = value
                                    chip_version.encode('utf-8')
                                    vmin.vminarr['Chip version'] = chip_version
                                word2 = str_line.find("Wafer ID    :")
                                if (word2 > -1):
                                    value = str_line[-9:-1].strip()
                                    wafer_id = value
                                    wafer_id.encode('utf-8')
                                    vmin.vminarr['Wafer ID'] = wafer_id

                        with open("C:\\Python35\\VMIN test\\result.csv","a") as csvfile:
                            writer = csv.writer(csvfile)
                            if (per_rail == True):
                                writer.writerow([vmin.vminarr[rail_num][loop-1]['Chip version'],self.pattern_name, \
                                                 rail_num,vmin.vminarr[rail_num][loop-1]['Frequency/MHz'], \
                                                 vmin.vminarr[rail_num][loop-1]['Vmin/mv'],vmin.vminarr[rail_num][loop-1]['Runtime/s'], \
                                                 vmin.vminarr[rail_num][loop-1]["Wafer ID"]])
                            else:
                                writer.writerow([vmin.vminarr[rail_num][loop-1]['Chip version'],self.pattern_name, \
                                                 vmin.vminarr[rail_num][loop-1]['Frequency/MHz'],vmin.vminarr[rail_num][loop-1]['Vmin/mv'], \
                                                 vmin.vminarr[rail_num][loop-1]['Runtime/s'],vmin.vminarr[rail_num][loop-1]["Wafer ID"]])				 
            os.rename("C:\\Python35\\VMIN test\\result.csv","bare_"+self.pattern_name+'_'+str(temperature)+'_'+str(self.vminarr[0][0]["Wafer ID"])+'_'+str(self.platform)+"_cpuvmin.csv")

class VMIN_BARE_METAL(VMIN):
    #configurables
    imc_com_port=setting["imc_com_port"]

    # fixed knowledges for bare metal
    bare_voltage_open = {"pmbus is already open":0}
    bare_voltage_norminal = {"pmbus margin mode":0}
    bare_voltage_uvout = {"under faultlimit":0}
    bare_voltage_uvout_read = {"250":0}
    bare_voltage_setvout = {"normal voltage":0}
    bare_pattern_run = {"blocked":0}
    bare_pattern_check = {"VF_passed":0}
    bare_frequency_set = {"is set":0}

    def __init__(self):
        super(VMIN_BARE_METAL,self).__init__()

    def open(self):
        self.imc = PORT()
        self.imc.open(self.imc_com_port, display)
        self.stopping = False
        atexit.register(vmin.close)
        signal.signal(signal.SIGINT, sigterm_handler)
        signal.signal(signal.SIGTERM, sigterm_handler)

    def close(self):
        self.stopping = True
        self.imc.close()

    def open_cui(self):
        self.stopping = False
        self.open()

    def sync(self):
        while (self.stopping == False):
                self.imc.sync()
                if (self.imc.is_success()):
                    return True
                elif (self.imc.is_failure()):
                    return False

    def simple_expect2(self, timeout, cmd, expect, hint):
        self.imc.execute(cmd,timeout,expect)
        if (self.sync() == False):
            self.fatal(cmd + "sync failure")
        else:
            logging.info(hint)

    def simple_expect(self, timeout, cmd, string, hint):
        self.simple_expect2(timeout, cmd, {string:0}, hint)

    def boot_expect(self):
        self.simple_expect(300,'\r\n', "pwr", "pwr")

    def boot_board(self):
        self.power_on()
        self.boot_expect()

    #set frequency
    def set_frequency_bare(self,rail,frequency):
        self.imc.execute('setfreq apc [r' + str(rail) + '] ' + str(frequency) +'\r\n', 10,self.bare_frequency_set)
        if (self.sync() == False):
            self.fatal('set frequency ' + str(frequency) + ' failure!')

    #set voltage
    def set_voltage_bare(self,voltage):
        self.imc.execute('pmbus setvout cpu write ' + str(voltage) +'\r\n', 10, self.bare_voltage_setvout)
        if (self.sync() == False):
            logging.info('set voltage ' + str(voltage) +' failure!')

    #run pattern and check the result
    def run_pattern_bare(self, rail_num, per_rail=True):
        if (per_rail == True):
            self.imc.execute('cbench vf [r'+str(rail_num)+']' +' [' + self.pattern_name + '-1]\r\n', 50, self.bare_pattern_run)
        else:
            self.imc.execute('cbench vf [a] [' + self.pattern_name + '-2]\r\n', 40, self.bare_pattern_run)
        if (self.sync() == True):
            logging.info('something is wrong')

        self.imc.execute('vfcheck\r\n', 10, self.bare_pattern_check)
        if (self.sync() == False):
            logging.info('find vmin!!!')
            return False
        return True

class VMIN_CBF_BARE_METAL(VMIN_BARE_METAL):

    #configurables
    imc_com_port = setting["imc_com_port"]

    # fixed knowledges for CBF bare metal
    bare_frequency_set = {"is set":0}

    def norminal_frequency(self, argument):
        switcher = {
        0: 1700,
        1: 1400,
        2: 500,
        }
        return switcher.get(argument, "nothing")

    def __init__(self):
        super(VMIN_CBF_BARE_METAL,self).__init__()
        self.pattern_name = setting["cbf_pattern_name"]
        self.initial_voltage = 870
        self.repeatflag = False
        self.platform = setting["platform"]

    #set frequency
    def set_frequency_bare(self,frequency):
        self.imc.execute('setfreq cbf '+str(frequency) +'\r\n', 10,self.bare_frequency_set)
        if (self.sync() == False):
            self.fatal('set frequency ' + str(frequency) + ' failure!')

    #set voltage
    def set_voltage_bare(self,voltage):
        self.imc.execute('pmbus setvout cbf write ' + str(voltage) +'\r\n', 10, self.bare_voltage_setvout)
        if (self.sync() == False):
            logging.info('set voltage ' + str(voltage) +' failure!')
			
	#run pattern and check the result
    def run_pattern_bare(self, rail_num, per_rail=True):
        self.imc.execute('cbench vf [c0] ['+ self.pattern_name +']\r\n', 45, self.bare_pattern_run)
		
        if (self.sync() == True):
            logging.info('something is wrong')
			
        self.imc.execute('vfcheck\r\n', 10, self.bare_pattern_check)
        if (self.sync() == False):
            logging.info('find vmin!!!')
            return False

        return True

    def run(self, run_type):

        my_file = 'C:\\Python35\\VMIN test\\log.txt'
        if os.path.exists(my_file):
            os.remove(my_file)
        for i in range(0,3):
            temperature = self.select_temperature(i)
            self.controlTCU(temperature)
            self.power_off()

            if (run_type == 'calibration'):
                array_column = 16
            elif (run_type == 'norminal'):
                array_column = 3

            with open("C:\\Python35\\VMIN test\\result.csv","a") as csvfile:
                writer = csv.writer(csvfile)
                writer.writerow(["Chip version","Pattern","Frequency/MHz","Vmin/mv","Runtime/s"])
            self.vminarr = np.empty(array_column,self.voltage_frequency_dtype)

            frequency = 2100
            loop = 0
            while ( frequency >= 600 ):   # repeat VF ponits frequencies
                if ( frequency == 2100 ):
                    voltage = 850
                else:
                    if (run_type == 'norminal' and frequency == 1700):
                        voltage = self.vminarr[loop-1]['Vmin/mv'] - 50
                    elif (run_type == 'norminal' and frequency == 1400):
                        voltage = self.vminarr[loop-1]['Vmin/mv'] - 20
                    else:
                        voltage = self.vminarr[loop-1]['Vmin/mv']

                if (self.repeatflag == True):
                    voltage = voltage + 30
                    self.repeatflag = False
                if (voltage > 870):
                    voltage = 870
                    
                self.initial_voltage = voltage
                self.boot_board()

                localtime = time.asctime( time.localtime(time.time()) )
                start = time.time()
                self.imc.execute('pmbus open\r\n', 10, self.bare_voltage_open)
                if (self.sync() == False):
                    logging.info('pmbus open failure!')

                self.imc.execute('pmbus operation all write nominal\r\n', 30, self.bare_voltage_norminal)
                if (self.sync() == False):
                    logging.info('pmbus operation failure!')

                self.imc.execute('pmbus uvout all write 250\r\n', 30, self.bare_voltage_uvout)
                if (self.sync() == False):
                    logging.info('pmbus uvout write failure!')

                self.imc.execute('pmbus uvout all read\r\n', 30, self.bare_voltage_uvout_read)
                if (self.sync() == False):
                    logging.info('pmbus uvout read failure!')

                self.set_voltage_bare(900)
                self.set_frequency_bare(frequency)

                while True:
                    voltage -= 5
                    self.set_voltage_bare(voltage)
                    if (self.run_pattern_bare(0, False) == False):
                        logging.info('find the Vmin in frequency :' + str(frequency))
                        break

                self.power_off()
                if (self.initial_voltage == (voltage+5) and (voltage+5) != 870):
                    self.repeatflag = True    
                    
                logging.info ('repeatflag = '+str(self.repeatflag))
                
                if (loop == 0):
                    f = open('C:\\Python35\\VMIN test\\log.txt','r')
                    f.seek(0)
                    str_line = f.readline()
                    while str_line:
                        str_line = f.readline()
                        word1 = str_line.find("Chip version:")
                        if (word1 > -1):
                            value = str_line[-6:-1].strip()
                            chip_version = value
                            chip_version.encode('utf-8')
                            vmin.vminarr['Chip version'] = chip_version
                        word2 = str_line.find("Wafer ID    :")
                        if (word2 > -1):
                            value = str_line[-9:-1].strip()
                            wafer_id = value
                            wafer_id.encode('utf-8')
                            vmin.vminarr['Wafer ID'] = wafer_id 

                if (self.repeatflag != True):
                    self.vminarr[loop]['Vmin/mv'] = voltage + 5
                    self.vminarr[loop]['Frequency/MHz'] = frequency

                    end = time.time()
                    self.vminarr[loop]['Runtime/s'] = end - start
                    logging.info('vmin is ' + str(self.vminarr[loop]['Vmin/mv']))
                    logging.info('runtime is ' + str(self.vminarr[loop]['Runtime/s']))

                    localtime = time.asctime( time.localtime(time.time()) )
                    logging.info(localtime)

                    with open("C:\\Python35\\VMIN test\\result.csv","a") as csvfile:
                        writer = csv.writer(csvfile)
                        writer.writerow([vmin.vminarr[loop]['Chip version'],self.pattern_name,vmin.vminarr[loop]['Frequency/MHz'],vmin.vminarr[loop]['Vmin/mv'],vmin.vminarr[loop]['Runtime/s']])

                    if (run_type == 'calibration'):
                        loop = loop + 1
                        frequency = frequency - 100
                        time.sleep(5)
                    elif (run_type == 'norminal'):
                        frequency = self.norminal_frequency(loop)
                        loop = loop + 1
                        time.sleep(5)
                    else:
                        logging.info("invalid run type!!")
  
            plt.figure()
            plt.xlabel("Vmin/mv")
            plt.ylabel("Frequency/MHz")
            plt.title('dcvs curve cbf ')
            plt.plot(vmin.vminarr['Vmin/mv'],vmin.vminarr['Frequency/MHz'],'ob-')
            for vf in zip(vmin.vminarr['Vmin/mv'],vmin.vminarr['Frequency/MHz']):
                plt.annotate("(%s,%s)" % vf, vf, xytext=(-20, 4), textcoords='offset points')
            os.rename("result.csv","bare_"+self.pattern_name+'_'+str(temperature)+'_'+str(self.vminarr[0]["Wafer ID"])+'_'+str(self.platform)+"_cbfvmin.csv")

class VMIN_HLOS(VMIN):

    # configurables
    apc_com_port='COM4'
    imc_com_port='COM4'

    # fixed knowledges for os
    expect_user = {"ubuntu login:":0}
    expect_pass = {"Password:":0}
    expect_shell = {"ubuntu":0}
    expect_root = {"root@ubuntu:/home/ubuntu#":0}
    expect_frequency = {"write error: Invalid argument":2}
    expect_voltage = {"set_voltage success":0}

    expect_error = {  "kernel paging request":2,"Internal error: Oops":2,
                      "Kernel panic":2,"NMI watchdog":2,"Resetting system!":2,
                      "Hardware Error":2,"rcu_sched detected stalls on CPUs/tasks":2,
                      "PC Alignment exception":2,"Errors detected":2} 

    expect_ras   = {  "DCE Error happened":2,"L3 Error happened":2,"DDR Error happened":2,
                      " L2 SEI happened":2,"Kernel Panic":2,
                      "Finished error fatal processing, notifying sys_m!":2} 

    voltage_frequency_dtype = np.dtype([('Pattern', 'U10'),('rail', 'int'), ('Frequency/MHz', 'int'), ('Vmin/mv', 'int'),('Chip version','U10'),('Wafer ID','U10')])


    def __init__(self):
        super(VMIN_HLOS,self).__init__()
        self.errorflag = False
        self.repeatflag = False
        self.initial_voltage = 0
        self.pattern_name = 'qpower'

    def open(self):
        self.apc = PORT('apc')
        self.apc.open(self.apc_com_port, display)
        self.imc = TCPFrame('imc')
        self.imc.open(self.diag_ip, self.diag_port)
        self.stopping = False
        atexit.register(vmin_hlos.close)
        signal.signal(signal.SIGINT, sigterm_handler)
        signal.signal(signal.SIGTERM, sigterm_handler)

    def close(self):
        self.stopping = True
        self.imc.close()
        self.apc.close()

    def open_cui(self):
        self.open()

    def sync(self,ignore_apc=False,ignore_imc=False):
        while (self.stopping == False):
                self.apc.sync()
                self.imc.sync()
                if ((ignore_apc or self.apc.is_success()) and
                    (ignore_imc or self.imc.is_success())):
                    logging.info('apc.is_success =' + str(self.apc.is_success()))
                    logging.info('imc.is_success =' + str(self.apc.is_success()))
                    return True
                elif (self.apc.is_failure() or self.imc.is_failure()):
                    #logging.info('apc.is_failure =' + str(self.apc.is_failure()))
                    #logging.info('imc.is_failure =' + str(self.apc.is_failure()))
                    return False

    def simple_expect2(self, timeout, cmd, expect, hint):
        self.apc.execute(cmd,timeout,expect)
        self.imc.reset()
        if (self.sync() == False):
            self.fatal("sync failure")
        else:
            logging.info(hint)

    def simple_expect3(self, timeout, cmd, expect, hint):
        self.apc.execute(cmd,timeout,expect)
        self.imc.reset()
        if (self.sync() == False):
            return False
        else:
            logging.info(hint)
            return True

    def simple_expect(self, timeout, cmd, string, hint):
        self.simple_expect2(timeout, cmd, {string:0}, hint)

    def boot_expect(self):
        self.simple_expect(100, '', "PBL, Start", "PBL start")
        self.simple_expect(100, '', "PBL, End", "PBL end")
        self.simple_expect(100, '', "SBL1, Start", "SBL start")
        self.simple_expect(100, '', "IMC Execution, Start", "IMC start")
        self.simple_expect(100, '', "SBL1, End", "SBL end")
        self.simple_expect(100, '', "BL31: Built : ", "TZ start")
        self.simple_expect(100, '', "IMC-TZ SYNC POINT REACHED ", "IMC/TZ sync")

    def boot_board(self):
        self.apc.capture(False)
        self.power_on()
        self.capture_info()
        self.boot_expect()
        self.login_linux()
        self.apc.capture(True)

    def login_linux(self):
        self.simple_expect2(1000, '', self.expect_user,'ubuntu login:')
        self.simple_expect2(100, "ubuntu\r\n", self.expect_pass,'password:')
        self.simple_expect2(100, "ubuntu\r\n", self.expect_shell,'$')
        self.simple_expect2(100, "su\r\n", self.expect_pass,'password:')
        self.simple_expect2(100, "ubuntu\r\n", self.expect_root,'root#')

    def expect_diag(self,cmd,timeout,expects):
        for i in range(0,5):
            self.imc.execute(cmd,timeout,expects)
            self.apc.reset()
            if (self.sync() == True):
                logging.info(cmd+str(' success!'))
                return
            else:
                if(i == 4):
                    self.apc.execute('',20,self.expect_error)
                    self.imc.execute('',20,self.expect_ras)
                    result = self.sync(False,False)
                    logging.info("enter diag result = " + str(result))
                    if ( result == False ):
                        logging.info("enter diag if")
                        self.errorflag = True
                        return
                    else:
                        logging.info("enter diag else")
                        self.fatal(cmd+str(' failure!'))

    def set_frequency(self,frequency):
        for i in range(0,10):
            # Linux accepts a frequency of MHz
            self.apc.execute('sh ./perf.sh ' + str(frequency) + '\r\n', 10,self.expect_frequency)
            self.imc.reset()
            if (self.sync() == True):
                logging.info('set_frequency success')
                return 
        self.fatal('set_frequency failure!')

    def set_voltage(self,rail_num, voltage):
        cmd = 'set_voltage '+str(rail_num)+" "+str(voltage)
        self.expect_diag(cmd,1,self.expect_voltage)

    def capture_error(self):
        self.apc.execute('',20,self.expect_error)
        self.imc.execute('',20,self.expect_ras)
        result1 = self.sync(False,False)
        result2 = self.simple_expect3(0.5,'\r\n', self.expect_root, '********apc is alive*********')
        return result1 and result2 

    def reset_all(self):
        self.apc.reset_expect(self.success)  
        self.imc.reset_expect(self.success)
        self.errorflag = False

    def run_pattern(self,rail_num):
        self.simple_expect2(5,'python ./load_pattern.py ' + str(rail_num) + ' > output.txt 2>&1 &\r\n',self.expect_root,'pattern has been loaded!')

    def capture_info(self):
        f = open('C:\\Python35\\VMIN test\\log.txt','r')
        f.seek(0)
        while True:
            str_line = f.readline()
            if (str_line.find("Silicon Version:") > -1):
                chip_version = str_line[-4:-1].strip()
                chip_version.encode('utf-8')
                self.vminarr['Chip version'] = chip_version
            if (str_line.find("Wafer ID") > -1):
                wafer_id= str_line[-11:-1].strip()
                wafer_id.encode('utf-8')
                self.vminarr['Wafer ID'] = wafer_id
                break

    def run(self, run_type, per_rail=True):
        self.power_off()
        if (run_type == 'calibration'):
            array_column = 21
        elif (run_type == 'norminal'):
            array_column = 4
        with open("result_all.csv","a") as csvfile: 
            writer = csv.writer(csvfile)
            writer.writerow(["Chip version","Pattern","Rail","Frequency/MHz","Vmin/mv","Wafer ID"])

        if (per_rail == True):
            max = 8
        else:
            max = 1
        self.vminarr = np.empty(array_column,self.voltage_frequency_dtype)

        for rail_num in range(max):
            frequency = 1900
            loop = 0
            while (frequency >= 600 ):   # repeat VF ponits frequencies
                if (frequency == 1900 ):
                    voltage = 755
                else:
                    if (run_type == 'norminal' and frequency == 1100):
                        voltage = 580
                    else:
                        voltage = self.vminarr[loop-1]['Vmin/mv'] + 30

                if (self.repeatflag == True):
                    voltage = voltage + 30
                    self.repeatflag = False

                self.initial_voltage = voltage
                self.boot_board()

                ####set_frequency####
                self.set_frequency(frequency)
                logging.info('set_frequency ' + str(frequency) + ' finished!')

                ####run_pattern####
                self.run_pattern(rail_num)

                while (True):
                    if (self.stopping):
                        return

                    ####set_voltage####
                    self.set_voltage(rail_num, voltage*1000)

                    if (self.errorflag == True or self.capture_error() == False):
                        logging.info ('enter errorflag and errorflag')
                        self.vminarr[loop]['Vmin/mv'] = voltage + 5
                        self.vminarr[loop]['Frequency/MHz'] = frequency
                        if (self.initial_voltage == voltage):
                            self.repeatflag = True
                        logging.info ('leave errorflag and errorflag')
                        break
                    else:
                        voltage -= 5
                logging.info ('repeatflag = '+str(self.repeatflag))
                if (self.repeatflag != True):
                    logging.info("************* vmin is found! ***********")
                    f=open('C:\\Python35\\VMIN test\\result.txt','a')
                    vmin = self.vminarr[loop]['Vmin/mv']
                    log='rail='+str(rail_num)+',freq='+str(self.vminarr[loop]['Frequency/MHz'])+',vmin='+str(vmin)
                    logging.info(log)
                    logging.info("************* vmin is found! ***********")
                    f.write(log+'\n')
                    f.close()

                    with open("result_all.csv","a") as csvfile: 
                            writer = csv.writer(csvfile)
                            writer.writerow([self.vminarr[loop]['Chip version'],self.pattern_name,rail_num,self.vminarr[loop]['Frequency/MHz'],self.vminarr[loop]['Vmin/mv'],self.vminarr[loop]["Wafer ID"]])

                    if (run_type == 'calibration'):
                        loop = loop + 1
                        frequency = frequency - 100
                    elif (run_type == 'norminal'):
                        frequency = self.norminal_frequency(loop)
                        loop = loop + 1
                        logging.info('frequency = ' + str(frequency))
                    else:
                        logging.info("invalid run type!!")

                self.power_off()
                self.reset_all()
            plt.title(self.pattern_name + ' dcvs curve rail ' + str(rail_num))
            plt.xlabel("Vmin/mv")
            plt.ylabel("Frequency/MHz")
            plt.plot(self.vminarr['Vmin/mv'],self.vminarr['Frequency/MHz'],'ob-')
            for vf in zip(self.vminarr['Vmin/mv'],self.vminarr['Frequency/MHz']):
                plt.annotate("(%s,%s)" % vf, vf, xytext=(-20, 4), textcoords='offset points')
            plt.savefig("os_"+self.pattern_name+'_40_'+str(self.vminarr[0]["Wafer ID"])+"_cpuvmin"+"_rail_"+str(rail_num)+".png")
            plt.figure()
        os.rename("result_all.csv","os_"+self.pattern_name+'_40_'+str(self.vminarr[0]["Wafer ID"])+"_cpuvmin.csv")

class VMIN_CBF_HLOS(VMIN_HLOS):

    # configurables
    apc_com_port='COM9'
    imc_com_port='COM4'

    voltage_frequency_dtype = np.dtype([('Pattern', 'U10'),('rail', 'int'), ('Frequency/MHz', 'int'), ('Vmin/mv', 'int'),('Chip version','U10'),('Wafer ID','U10')])


    def norminal_frequency(self, argument):
        switcher = {
        0: 1700,
        1: 1400,
        2: 500,
        }
        return switcher.get(argument, "nothing")

    def __init__(self):
        super(VMIN_CBF_HLOS,self).__init__()
        self.errorflag = False
        self.repeatflag = False
        self.initial_voltage = 0
        self.expect_frequency = {"set_perf success":0}
        self.pattern_name = 'stressapp'

    def set_frequency(self,frequency):
        for i in range(0,10):
            # CBF accepts a frequency of MHz
            self.imc.execute('set_perf -d 0 ' + str(frequency) + ' ' + str(frequency) + ' ' + str(frequency) + ' ' + '\r\n', 10,self.expect_frequency)
            self.apc.reset()
            if (self.sync() == True):
                logging.info('set_frequency success')
                return 
        self.fatal('set_frequency failure!')


    def capture_error(self):
        self.apc.execute('',15,self.expect_error)
        self.imc.execute('',15,self.expect_ras)
        result1 = self.sync(False,False)
        result2 = self.simple_expect3(0.5,'\r\n', self.expect_root, '********apc is alive*********')
        self.apc.execute('',20,self.expect_error)
        self.imc.execute('',20,self.expect_ras)
        result3 = self.sync(False,False)
        return result1 and result2 and result3

    def run_pattern(self):
        if (self.pattern_name == "qpower"):
            cmd = "/QPower_K64_v2.01 -runtime 30 -threads 0,40 2>&1 &\r\n"
        elif(self.pattern_name == "stressapp"):
            cmd = "/home/ubuntu/stressapptest/src/stressapptest -s 30 -M 1024 -m 40 -C 40 -W 2>&1 &\r\n"
        elif(self.pattern_name == "lmbench"):
            cmd = "/home/ubuntu/stream.o 2>&1 &\r\n"
        elif(self.pattern_name == "kraken"):
            cmd = "sq_max_data_copy_priv_map.elf 2>&1 &\r\n"
        else:
            logging.info("pattern name is wrong!")
        self.simple_expect3(5,cmd,self.expect_root,'pattern has been loaded!')

    def run(self, run_type, per_rail=True):
        self.power_off()
        if (run_type == 'calibration'):
            array_column = 16
        elif (run_type == 'norminal'):
            array_column = 3
        with open("result_all.csv","a") as csvfile: 
            writer = csv.writer(csvfile)
            writer.writerow(["Chip version","Pattern","Frequency/MHz","Vmin/mv","Wafer ID"])


        self.vminarr = np.empty(array_column,self.voltage_frequency_dtype)

        frequency = 2100
        loop = 0
        while (frequency >= 600 ):   # repeat VF ponits frequencies
            if (frequency == 2100 ):
                voltage = 850
            else:
                if (run_type == 'norminal' and frequency == 1400):
                    voltage = 700
                else:
                    voltage = self.vminarr[loop-1]['Vmin/mv'] + 30

            if (self.repeatflag == True):
                voltage = voltage + 30
                self.repeatflag = False

            self.initial_voltage = voltage
            self.boot_board()

            ####set_frequency####
            self.set_frequency(frequency)
            logging.info('set_frequency ' + str(frequency) + ' finished!')

            while (True):
                if (self.stopping):
                    return

                #kill the thread
                self.simple_expect3(5,'killall sq_max_data_copy_priv_map.elf\r\n',self.expect_root,'pattern has been loaded!')

                ####set_voltage####
                self.set_voltage(8, voltage*1000)
                
                ####run_pattern####
                if (self.errorflag == True or self.run_pattern() == False or self.capture_error() == False):
                    self.vminarr[loop]['Vmin/mv'] = voltage + 5
                    self.vminarr[loop]['Frequency/MHz'] = frequency
                    if (self.initial_voltage == voltage):
                        self.repeatflag = True
                    break
                else:
                    voltage -= 5

            if (self.repeatflag != True):
                logging.info("************* vmin is found! ***********")
                f=open('C:\\Python35\\VMIN test\\result.txt','a')
                vmin = self.vminarr[loop]['Vmin/mv']
                log='freq='+str(self.vminarr[loop]['Frequency/MHz'])+',vmin='+str(vmin)
                logging.info(log)
                logging.info("************* vmin is found! ***********")
                f.write(log+'\n')
                f.close()

                with open("result_all.csv","a") as csvfile: 
                        writer = csv.writer(csvfile)
                        writer.writerow([self.vminarr[loop]['Chip version'],self.pattern_name,self.vminarr[loop]['Frequency/MHz'],self.vminarr[loop]['Vmin/mv'],self.vminarr[loop]["Wafer ID"]])

                if (run_type == 'calibration'):
                    loop = loop + 1
                    frequency = frequency - 100
                elif (run_type == 'norminal'):
                    frequency = self.norminal_frequency(loop)
                    loop = loop + 1
                    logging.info('frequency = ' + str(frequency))
                else:
                    logging.info("invalid run type!!")

            self.power_off()
            self.reset_all()
        plt.title(self.pattern_name+' CBF dcvs curve ')
        plt.xlabel("Vmin/mv")
        plt.ylabel("Frequency/MHz")
        plt.plot(self.vminarr['Vmin/mv'],self.vminarr['Frequency/MHz'],'ob-')
        for vf in zip(self.vminarr['Vmin/mv'],self.vminarr['Frequency/MHz']):
            plt.annotate("(%s,%s)" % vf, vf, xytext=(-20, 4), textcoords='offset points')
        plt.savefig("os_"+self.pattern_name+"_40_0_cbfvmin.png")
        #plt.savefig("os_"+self.pattern_name+'_40_'+str(self.vminarr[0]["Wafer ID"])+"_cbfvmin.png")
        os.rename("result_all.csv","os_"+self.pattern_name+'_40_'+str(self.vminarr[0]["Wafer ID"])+"_cbfvmin.csv")

if __name__ =="__main__":
    setting = load_config()
    if (setting["enviroment"] == 'hlos_cpu'):
        vmin_hlos = VMIN_HLOS()
        vmin_hlos.open_cui()
        if (setting["step"] == 'norminal' or setting["step"] == 'calibration'):
            vmin_hlos.run(setting["step"], True)
        else:
            logging.info('argv[2] is wrong')
            sys.exit(0)

    elif (setting["enviroment"] == 'bare_cpu'):
        vmin = VMIN_BARE_METAL()
        vmin.open_cui()

        if (setting["step"] == 'norminal' or setting["step"] == 'calibration'):
            vmin.run(setting["step"], True)

        else:
            logging.info('argv[2] is wrong')
            sys.exit(0)

    elif (setting["enviroment"] == 'bare_cbf'):
        vmin = VMIN_CBF_BARE_METAL()
        vmin.open_cui()

        if (setting["step"] == 'norminal' or setting["step"] == 'calibration'):
            vmin.run(setting["step"])

        else:
            logging.info('argv[2] is wrong')
            sys.exit(0)
    elif (setting["enviroment"] == 'hlos_cbf'):
        vmin_hlos = VMIN_CBF_HLOS()
        vmin_hlos.open_cui()

        if (setting["step"] == 'norminal' or setting["step"] == 'calibration'):
            vmin_hlos.run(setting["step"])

        else:
            logging.info('argv[2] is wrong')
            sys.exit(0)
    else:
        logging.info('you need to pass two arguments')
    #vmin.close()
