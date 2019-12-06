# /************************************************************
#  * SerialPortHandler.py             Created on: 30/10/2019  *
#  *                                                          *
#  *      Universidad Nacional Autonoma de Mexico             *
#  *              Instituto de Ingeniería                     *
#  *          Gobierno de la Ciudad de México                 *
#  *                  Microfascinantes                        *
#  *                                                          *
#  *  Desarrollador: Manuel Andres Herrera Juarez.            *
#  *                                                          *
#  ************************************************************/

import serial
from serial.tools import list_ports
import threading, time

#Manejador del Puerto serial
class SerialPortHandler():    
    def __init__(self, baud, portDesc, portVid, readFunInt, notifFunInt, timeout = 0.1):
        self.__serialPort = None
        self.__assertPort = False
        self.__serialDevice = None
        self.__portDesc = portDesc
        self.__portVid = portVid
        self.__readFxn = readFunInt
        self.__notifFunInt = notifFunInt
        self.__baud = baud
        self.__timeout = timeout
        self.__discover = False
        self.__discoverStr = ""
        self.__discoverTimeout = 0.5

        self.__threadRead = threading.Thread(target=self.__readThread, daemon=True)
        self.__threadAssert = threading.Thread(target=self.__assertPortConnection, daemon=True)

        self.__threadRead.start()
        self.__threadAssert.start()

    
    def connect(self):
        portFound = self.__searchPort()
        if portFound != None:
            self.__serialDevice = portFound
            try:
                time.sleep(1)
                self.__serialPort = serial.Serial(portFound, self.__baud, timeout = self.__timeout)
                if(self.__assertPort == False):
                    self.__serialPort.flush()
                    self.__notifFunInt("CON_INIT", portFound)                
                    pass
                return True
            except:                               
                return False

    def write(self, msg):
        if type(msg) is str:
            msg = msg.encode()
        if type(msg) is not bytes or self.__assertPort or self.__serialPort==None:
            return False
        try:
            self.__serialPort.write(msg)
        except:
                self.__assertPort = True
                if(self.__notifFunInt != None):
                    self.__notifFunInt("CON_LOST")
            

    def portReady(self):
        return not (self.__assertPort or self.__serialPort == None)

    def close(self):
        if self.__serialPort != None:
            self.__serialPort.close()
    
    def setDiscoverState(self, state, strCmd="", timeout=0.5):
        self.__discover = state
        self.__discoverStr = strCmd
        self.__timeout = timeout

    def __searchPort(self):   
        for port in list_ports.comports():
            if self.__portDesc in port.description or self.__portDesc in port.manufacturer or self.__portVid == port.vid:
                return port.device
        return None
    
    def __assertPortConnection(self):
        while True:                        
            if self.__assertPort or self.__serialPort == None:
                if(self.connect()):                                    
                    if(self.__assertPort == True and self.__notifFunInt != None):
                        self.__assertPort = False
                        self.__notifFunInt("CON_RESTART", self.__serialDevice)
                    self.__assertPort = False
            elif self.__discover:
                self.write(self.__discoverStr)
                time.sleep(self.__discoverTimeout)
                continue
            time.sleep(0.5)
    
    def __readThread(self):
        while True:
            if self.__assertPort == False and self.__serialPort != None:
                try:
                    if self.__serialPort.isOpen():
                        while self.__serialPort.inWaiting():
                            if(self.__readFxn != None):
                                self.__readFxn(self.__serialPort.readline())
                    else:
                        time.sleep(0.05)
                except:
                    self.__assertPort = True
                    if(self.__notifFunInt != None):
                        self.__notifFunInt("CON_LOST")
            else:
                time.sleep(0.05)
    