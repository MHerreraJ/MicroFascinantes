# /************************************************************
#  * PN532Handler.py                  Created on: 30/10/2019  *
#  *                                                          *
#  *      Universidad Nacional Autonoma de Mexico             *
#  *              Instituto de Ingeniería                     *
#  *          Gobierno de la Ciudad de México                 *
#  *                  Microfascinantes                        *
#  *                                                          *
#  *  Desarrollador: Manuel Andres Herrera Juarez.            *
#  *                                                          *
#  ************************************************************/

from SerialPortHandler import SerialPortHandler
from uFPerson import Person
from time import sleep

#Interfaz que se encargará de enviar y recbir comandos
# o información hacia el Arduino con el módulo
# NFC PN532.

class PN532Handler():
    ACTION_NONE = 0
    ACTION_WR_REQUEST = 1
    ACTION_WR_SENT = 2
    ACTION_WR_COMPLETE = 3
    ACTION_RD_REQUEST = 4
    ACTION_RD_SENT = 5

    def __init__(self, updFun):
        self.__updateFxn = updFun
        self.user = Person()
        self.__currentAction = self.ACTION_NONE 

        self.__Arduino = SerialPortHandler(115200, "Arduino", 9025, self.__ArduinoReadInt, self.__ArduinoNotifyInt)
        self.__ArduinoEnabled = False
        self.__ArduinoAssert = False    

    def sendData(self, name, location, allergy, age, gender, bloodType):
        if not self.user.setData(name, location, allergy, age, gender, bloodType):
            return False

        if(self.__ArduinoAssert):
            self.__Arduino.write(b'W' + self.user.encode())
            self.__currentAction = self.ACTION_WR_REQUEST
            self.__updateFxn("NFC_WRITE_REQUEST", "Escribiendo datos.")
            return True
        return False

    def readData(self):
        if(self.__ArduinoAssert):
            self.__Arduino.write(self.__getAckBytes('R', 0))
            self.__currentAction = self.ACTION_RD_REQUEST
            self.__updateFxn("NFC_READ_REQUEST", "Leyendo datos.")
            return True
        return False
    
    def stopAction(self):
        if(self.__ArduinoAssert):
            self.__Arduino.write(self.__getAckBytes('S', 0))

    def isReady(self):
        return self.__Arduino.portReady() and self.__ArduinoAssert
    
    def close(self):
        self.__Arduino.close()

    def __getAckBytes(self, cmd, param, nl=True):
        auxAck = [0x00, 0x00, 0x0C, 0xFD, ord('u'), ord('F'), ord('T'), ord('I'), ord('\r'), ord('\n')]
        if type(cmd) is str:
            cmd = ord(cmd)
        if type(param) is str:
            param = ord(param)

        auxAck[0] = cmd
        auxAck[1] = param
        return bytes(auxAck if nl else auxAck[:8])


    def __ArduinoReadInt(self, byteMsg): 
        if(len(byteMsg) < 8):
            return

        #print("Rx:", byteMsg)

        if b'TIMEOUT!' in byteMsg[:8]:
            return
        elif self.__getAckBytes('I', 'W') in byteMsg:
            self.__Arduino.write(self.__getAckBytes('I', 'W'))
        elif self.__getAckBytes('I', 'R') in byteMsg:
            self.__updateFxn("NFC_INIT","Conexión establecida con el programador NFC.")
            self.__ArduinoAssert = True
            self.__currentAction = self.ACTION_NONE
            self.__Arduino.setDiscoverState(False)
        elif self.__getAckBytes('A', 'W') in byteMsg:
            self.__currentAction = self.ACTION_WR_SENT
        elif self.__getAckBytes('W', 0) in byteMsg:
            self.__currentAction = self.ACTION_WR_COMPLETE
            self.__updateFxn("NFC_WRITE_SUCCESS", "Datos Guardados.")    
        elif self.__getAckBytes('A', 'R') in byteMsg:
            self.__currentAction = self.ACTION_RD_SENT
        elif self.__getAckBytes('R', 0, False) in byteMsg:
            if self.user.decode(byteMsg[8:]):
                self.__updateFxn("NFC_READ_SUCCESS", self.user)
            else:
                self.__updateFxn("NFC_READ_CORRUPT", "No se pudo leer el tatuaje, trama de información corrupta.")
        elif self.__getAckBytes('R', 'E') in byteMsg:
            self.__updateFxn("NFC_READ_CORRUPT", "No se pudo leer el tatuaje, trama de información corrupta.")
        elif self.__getAckBytes('A', 'T') in byteMsg:
            self.__updateFxn("NFC_ACTION_TIMEOUT", "No se pudo realizar la accion solicitada. Intente de nuevo.")
        # elif self.__getAckBytes('W',0) in byteMsg:
        #     if self.__currentAction == 1:
        #         print("Datos Guardados")

    def __ArduinoNotifyInt(self, msg, params=None):
        if(msg == "CON_INIT"):
            self.__updateFxn(msg, "Arduino encontrado en {}".format(params))  
            self.__ArduinoEnabled = True
            self.__ArduinoAssert = False
            self.__Arduino.setDiscoverState(True, self.__getAckBytes('I', 'W'), 0.2)
            #self.__Arduino.write(self.__getAckBytes('I', 'W'))
        elif(msg == "CON_LOST"):
            self.__updateFxn(msg, "Conexion perdida con el programador NFC")
            self.__ArduinoEnabled = False
            self.__Arduino.setDiscoverState(False)
        elif(msg == "CON_RESTART"):
            self.__updateFxn(msg, "Arduino re-encontrado en {}".format(params))            
            self.__ArduinoEnabled = True
            self.__ArduinoAssert = False
            self.__Arduino.setDiscoverState(True, self.__getAckBytes('I', 'W'), 0.2)
        pass