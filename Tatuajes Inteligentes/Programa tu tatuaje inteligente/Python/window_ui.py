# -*- coding: utf-8 -*-
# /************************************************************
#  * window_ui.py                     Created on: 30/10/2019  *
#  *                                                          *
#  *      Universidad Nacional Autonoma de Mexico             *
#  *              Instituto de Ingeniería                     *
#  *          Gobierno de la Ciudad de México                 *
#  *                  Microfascinantes                        *
#  *                                                          *
#  *  Desarrollador: Manuel Andres Herrera Juarez.            *
#  *                                                          *
#  ************************************************************/


from PyQt5 import QtCore, QtGui, QtWidgets

#Clase donde se inicializan la interfaz gráfica de usuario
class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")        
        MainWindow.resize(800, 953)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")

        self.imCDMX = QtWidgets.QLabel(self.centralwidget)
        #self.imCDMX.setGeometry(18, 131, 211, 78)
        self.imCDMX.setObjectName("imCDMX")
    

        self.imuF = QtWidgets.QLabel(self.centralwidget)
        #self.imuF.setGeometry(18, 817, 402, 122)
        self.imuF.setObjectName("imuF")

        self.imII = QtWidgets.QLabel(self.centralwidget)
        #self.imII.setGeometry(497, 873, 190, 57)
        self.imII.setObjectName("imII")

        self.imUNAM = QtWidgets.QLabel(self.centralwidget)
        #self.imUNAM.setGeometry(713, 873, 57, 59)
        self.imUNAM.setObjectName("imUNAM")

        self.lbTatInt = QtWidgets.QLabel(self.centralwidget)
        self.lbTatInt.setObjectName("lbTatInt")
        self.lbTatInt.setText("Tatuajes Inteligentes")

        self.lbProgTat = QtWidgets.QLabel(self.centralwidget)
        self.lbProgTat.setObjectName("lbProgTat")
        self.lbProgTat.setText("¡Programa tu tatuaje!")

        self.lbName = QtWidgets.QLabel(self.centralwidget)
        self.lbName.setObjectName("lbName")
        self.lbName.setText("Nombre (o Apodo): ")

        self.tbName = QtWidgets.QLineEdit(self.centralwidget)
        self.tbName.setObjectName("tbName")   
        self.regExLetters = QtCore.QRegExp("[a-zA-Z ]+")
        self.validatorLetters = QtGui.QRegExpValidator(self.regExLetters, self.tbName)
        self.tbName.setValidator(self.validatorLetters)

        self.lbEdad = QtWidgets.QLabel(self.centralwidget)
        self.lbEdad.setObjectName("lbEdad")
        self.lbEdad.setText("Edad (o '?'): ")

        #self.lbGend = QtWidgets.QLabel(self.centralwidget)
        #self.lbGend.setObjectName("lbGend")
        #self.lbGend.setText("Sexo (M,F,O): ")        

        self.tbEdad = QtWidgets.QLineEdit(self.centralwidget)
        self.tbEdad.setObjectName("tbEdad") 
        self.regExEdad = QtCore.QRegExp("([0-9]{1,3})|\?")
        self.validatorEdad = QtGui.QRegExpValidator(self.regExEdad, self.tbEdad)
        self.tbEdad.setValidator(self.validatorEdad)
        
        #self.tbGend = QtWidgets.QLineEdit(self.centralwidget)
        #self.tbGend.setObjectName("tbGend") 
        #self.regExGend = QtCore.QRegExp("[mMfFoO]")
        #self.validatorGend = QtGui.QRegExpValidator(self.regExGend, self.tbGend)
        #self.tbGend.setValidator(self.validatorGend)

        self.lbLoc = QtWidgets.QLabel(self.centralwidget)
        self.lbLoc.setObjectName("lbLoc")
        self.lbLoc.setText("Ubicación: ")

        self.tbLoc = QtWidgets.QLineEdit(self.centralwidget)
        self.tbLoc.setObjectName("tbLoc")   
        self.tbLoc.setValidator(self.validatorLetters)

        #self.lbBlood = QtWidgets.QLabel(self.centralwidget)
        #self.lbBlood.setObjectName("lbBlood")
        #self.lbBlood.setText("Tipo de sangre (A+, B-, O+, ..., ?):")

        #self.tbBlood = QtWidgets.QLineEdit(self.centralwidget)
        #self.tbBlood.setObjectName("tbBlood")  
        #self.regExBlood = QtCore.QRegExp("(([AaBbOo]|([Aa][Bb]))(\+|\-))|\?") 
        #self.validatorBlood = QtGui.QRegExpValidator(self.regExBlood, self.tbBlood)
        #self.tbBlood.setValidator(self.validatorBlood)

        self.lbAll = QtWidgets.QLabel(self.centralwidget)
        self.lbAll.setObjectName("lbAll")
        self.lbAll.setText("Alergico a: ")

        self.tbAll = QtWidgets.QLineEdit(self.centralwidget)
        self.tbAll.setObjectName("tbAll")   
        self.regExAll = QtCore.QRegExp("([a-zA-Z, ]+)|\?|-") 
        self.validatorAll = QtGui.QRegExpValidator(self.regExAll, self.tbAll)
        self.tbAll.setValidator(self.validatorAll)

        self.tbTerminal = QtWidgets.QTextEdit(self.centralwidget)
        self.tbTerminal.setObjectName("lbTerminal")        
        self.tbTerminal.setReadOnly(True)
        self.tbTerminal.setTextColor(QtGui.QColor(255,255,255))
              
        pal = self.tbTerminal.viewport().palette()        
        pal.setColor(self.tbTerminal.viewport().backgroundRole(), QtGui.QColor(5,0,20))
        pal2= QtGui.QPalette()
        pal2.setColor(self.tbTerminal.backgroundRole(), QtGui.QColor(0, 247, 246))
        self.tbTerminal.viewport().setPalette(pal)
        self.tbTerminal.setPalette(pal2)

        self.btWrite = QtWidgets.QLabel(self.centralwidget)
        self.btWrite.setObjectName("btWrite")

        self.lbWrite = QtWidgets.QLabel(self.centralwidget)
        self.lbWrite.setObjectName("lbWrite")
        self.lbWrite.setText("Escribir")

        self.btRead = QtWidgets.QLabel(self.centralwidget)
        self.btRead.setObjectName("btRead")

        self.lbRead = QtWidgets.QLabel(self.centralwidget)
        self.lbRead.setObjectName("lbRead")
        self.lbRead.setText("Leer")

        self.btErase = QtWidgets.QLabel(self.centralwidget)
        self.btErase.setObjectName("btErase")      

        self.lbErase = QtWidgets.QLabel(self.centralwidget)
        self.lbErase.setObjectName("lbErase")
        self.lbErase.setText("Borrar")

        
        MainWindow.setCentralWidget(self.centralwidget)
        MainWindow.setWindowTitle("MicroFascinantes - Tatuajes Inteligentes")
        
        QtCore.QMetaObject.connectSlotsByName(MainWindow)



if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
