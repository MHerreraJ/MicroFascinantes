#!/usr/bin/env python3

# /************************************************************
#  * main.py                          Created on: 30/10/2019  *
#  *                                                          *
#  *      Universidad Nacional Autonoma de Mexico             *
#  *              Instituto de Ingeniería                     *
#  *          Gobierno de la Ciudad de México                 *
#  *                  Microfascinantes                        *
#  *                                                          *
#  *  Desarrollador: Manuel Andres Herrera Juarez.            *
#  *                                                          *
#  ************************************************************/

from window_ui import *
from PyQt5.QtGui import QPixmap, QFont
from PN532Handler import PN532Handler
import threading
from time import sleep
import sys
import os

class MThread(QtCore.QThread):
    def __init__(self, fn, parent=None):
        super(MThread, self).__init__(parent)
        self._fn = fn
    
    def run(self):
        self._fn()

class Monitor(QtCore.QObject):
    updateMonitor = QtCore.pyqtSignal()

    def update(self):
        tTerminal = MThread(self.terminal_vector, parent=self)
        tTerminal.daemon = True
        tTerminal.start()
    
    def terminal_vector(self):
        self.updateMonitor.emit()

class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):
    resized = QtCore.pyqtSignal()

    def __init__(self, *args, **kwargs):
        QtWidgets.QMainWindow.__init__(self, *args, **kwargs)
        self.setupUi(self)
        self.setWindowIcon(QtGui.QIcon(sys.argv[0].replace("main.py", "") + "Imagenes/icon.png"))

        self.resized.connect(self.resizeUpdate)
        self.btWrite.mousePressEvent = self.write_Clicked
        self.btRead.mousePressEvent = self.read_Clicked
        self.btErase.mousePressEvent = self.erase_Clicked
        self.terminalStr = ">> Tatuajes Inteligentes uF"
        self.tbTerminal.setText(self.terminalStr)  

        self.monitor = Monitor()
        self.monitor.updateMonitor.connect(self.updateTerminal)
        self.NFCProgrammer = PN532Handler(self.NFCMessage)     
        self.updRegs = False        
        self.usr = None   

    
    
    def NFCMessage(self, action, params=None):        
        if params != None:
            if action == "NFC_READ_SUCCESS":
                self.updRegs = True
                self.usr = params
            self.terminalStr += "\r\n>> {}".format(params)
            self.monitor.update()
        pass

    def updateTerminal(self):
        self.tbTerminal.setText(self.terminalStr)
        if self.updRegs:
            self.tbName.setText(self.usr.name)
            self.tbEdad.setText(self.usr.getAge())
            #self.tbGend.setText(self.usr.getGender())
            self.tbLoc.setText(self.usr.location)
            #self.tbBlood.setText(self.usr.getBloodType())
            self.tbAll.setText(self.usr.allergy)
            self.updRegs = False

    def write_Clicked(self, event):  
        name = self.tbName.text()
        loc = self.tbLoc.text()
        aller = self.tbAll.text()
        age = self.tbEdad.text()
        #gend = self.tbGend.text()
        #blood = self.tbBlood.text()

        blood = '?'
        gend = 'M'
        if age == "":
            age = '?'
        
        if gend == "":
            gend = 'O'
        
        if blood == '':
            blood = '?'
        
        if aller == '':
            aller = '-'
        
        if loc == '':
            loc = 'Pilares'

        if name == '':
            self.terminalStr += "\r\n>> {}".format("Debes ingresar un nombre.")
            self.monitor.update()
            return

        if not self.NFCProgrammer.sendData(name, loc, aller, age, gend, blood):
            self.terminalStr += "\r\n>> {}".format("No se pudo escribir. Checa que tu información este correcta.")
            self.monitor.update()

    def read_Clicked(self, event):
        self.NFCProgrammer.readData()

    def erase_Clicked(self, event):
        self.tbName.setText("")
        self.tbEdad.setText("")
        #self.tbGend.setText("")
        self.tbLoc.setText("")
        #self.tbBlood.setText("")
        self.tbAll.setText("")

        self.terminalStr = ">> Tatuajes Inteligentes uF"
        self.tbTerminal.setText(self.terminalStr)

        self.NFCProgrammer.stopAction()
        
        pass

    def resizeObject(self, objV, ref, x, y, w, h, im=None, fontSize=None, fontLight=QFont.Light):
        x = x*self.scale
        y = y*self.scale
        refX = self.width()
        refY = self.height()
        w = w*self.scale
        h = h*self.scale

        if ref[0] == 'R':
            x = refX - x
        elif ref[0] == 'C':
            x = refX/2 + x
        
        if ref[1] == 'B':
            y = refY - y
        elif ref[1] == 'C':
            y = refY/2 + y

        objV.setGeometry(x, y, w, h)
        if im!=None:
            im = sys.argv[0].replace("main.py", "")+im
            if type(objV) is QtWidgets.QLabel:
                objV.setPixmap(QPixmap(im).scaledToWidth(w))
            elif type(objV) is QtWidgets.QPushButton:
                objV.setIcon(QtGui.QIcon(im))
                objV.setIconSize(QtCore.QSize(w,h))
                pass
        
        if(fontSize != None):
            fontName = 'Montserrat' if os.name != 'nt' else 'Candara Light'
            objV.setFont(QFont(fontName, fontSize*self.scale, fontLight))


    def resizeEvent(self, event):
        self.resized.emit()
        return super(MainWindow, self).resizeEvent(event)
    
    def resizeUpdate(self):
        self.ratioH = self.width()/800
        self.ratioV = self.height()/953

        self.scale = min(self.ratioV, self.ratioH)

        self.resizeObject(self.imCDMX, "LT", 18, 20, 222, 82, "Imagenes/cdmx.png")
        self.resizeObject(self.imuF, "LB", 18, 131, 402, 122, "Imagenes/uF.png")
        self.resizeObject(self.imII, "CB", 35, 85, 228, 68, "Imagenes/iiunam.png")
        self.resizeObject(self.imUNAM, "RB", 110, 100, 92, 97, "Imagenes/unam.png")

        self.resizeObject(self.lbTatInt, "CT", -124, 40,    492, 60, fontSize=30)
        self.resizeObject(self.lbProgTat, "LT", 33, 131,    394, 60, fontSize=26)

        self.resizeObject(self.lbName,  "LT",   42, 201,    250, 40, fontSize=18)
        self.resizeObject(self.tbName,  "LT",   52, 244,    390, 47, fontSize=17, fontLight=QFont.Normal)

        self.resizeObject(self.lbEdad,  "LT",   42, 318,    160, 40, fontSize=18)
        self.resizeObject(self.tbEdad,  "LT",   52, 361,    390, 47, fontSize=17, fontLight=QFont.Normal)

        #self.resizeObject(self.lbGend,  "LT",   255,318,    175, 40, fontSize=18)
        #self.resizeObject(self.tbGend,  "LT",   265,361,    175, 47, fontSize=17, fontLight=QFont.Normal)

        self.resizeObject(self.lbLoc,   "LT",   42, 435,    185, 40, fontSize=18)
        self.resizeObject(self.tbLoc,   "LT",   52, 478,    390, 47, fontSize=17, fontLight=QFont.Normal)

        #self.resizeObject(self.lbBlood, "LT",   42, 552,    415, 40, fontSize=18)
        #self.resizeObject(self.tbBlood, "LT",   52, 595,    390, 47, fontSize=17, fontLight=QFont.Normal)

        self.resizeObject(self.lbAll,   "LT",   42, 552,    185, 40, fontSize=18)
        self.resizeObject(self.tbAll,   "LT",   52, 595,    390, 47, fontSize=17, fontLight=QFont.Normal)
       
     
        xL = 490*self.scale
        yT = 190*self.scale
        xR = self.width() - 40*self.scale

        w = xR - xL
        self.tbTerminal.setGeometry(xL, yT, w, 510*self.scale)
        self.tbTerminal.setFont(QFont('Monospace' if os.name != 'nt' else 'Consolas', 10*(0.05*self.ratioH+self.scale), QFont.Light))

        self.resizeObject(self.btWrite, "LB",   505,230,    45,45, "Imagenes/nfc.png")
        self.resizeObject(self.lbWrite, "LB",   490,170,    80,30, fontSize=15, fontLight=QFont.Normal)

        self.resizeObject(self.btRead, "CB",   200,230,    45,45, "Imagenes/buscar.png")
        self.resizeObject(self.lbRead, "CB",   205,170,    80,30, fontSize=15, fontLight=QFont.Normal)
        
        self.resizeObject(self.btErase, "RB",   105,230,    45,45, "Imagenes/borrar.png")
        self.resizeObject(self.lbErase, "RB",   116,170,    80,30, fontSize=15, fontLight=QFont.Normal)



if __name__ == "__main__":
    app = QtWidgets.QApplication([])
    window = MainWindow()
    window.show()
    app.exec_()