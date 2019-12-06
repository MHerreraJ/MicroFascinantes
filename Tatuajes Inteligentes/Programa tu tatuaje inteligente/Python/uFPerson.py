# /************************************************************
#  * uFPerson.py                      Created on: 30/10/2019  *
#  *                                                          *
#  *      Universidad Nacional Autonoma de Mexico             *
#  *              Instituto de Ingeniería                     *
#  *          Gobierno de la Ciudad de México                 *
#  *                  Microfascinantes                        *
#  *                                                          *
#  *  Desarrollador: Manuel Andres Herrera Juarez.            *
#  *                                                          *
#  ************************************************************/


#Define tipo de objeto Persona
#Manejará los datos de los participantes en el taller
#Codificando o decodificando la información correspondiente
class Person():
    BLOOD_ERROR = -1
    BLOOD_NO = 0
    BLOOD_ON = 1
    BLOOD_OP = 2
    BLOOD_AN = 3
    BLOOD_AP = 4
    BLOOD_BN = 5
    BLOOD_BP = 6
    BLOOD_ABN = 7
    BLOOD_ABP = 8
    BLOOD_TYPES = ['?', 'O-', 'O+', 'A-', 'A+', 'B-', 'B+', 'AB-', 'AB+']

    GENDER_N = 0
    GENDER_F = 16
    GENDER_M = 32
    GENDERS = ['O', 'F', 'M']
    GENDERSB = ['Otro', 'Femenino', 'Masculino']

    def __init__(self):
        self.name = ""
        self.location = ""
        self.allergy = ""
        self.__age = 0
        self.__gender = self.GENDER_N
        self.__bloodType = self.BLOOD_NO

    def getAge(self):
        return '?' if self.__age == 0 else str(self.__age-1)
    
    def getGender(self):
        return self.GENDERS[self.__gender>>4]

    def getGenderBig(self):
        return self.GENDERSB[self.__gender>>4]

    
    def getBloodType(self):
        return self.BLOOD_TYPES[self.__bloodType]

    def setData(self, name, location, allergy, age, gender, bloodType):
        self.name = name
        self.location = location
        self.allergy = allergy
        self.__age = self.__encodeAge(age)
        self.__gender = self.__encodeGender(gender)
        self.__bloodType = self.__encodeBloodType(bloodType)
        if self.__bloodType < 0:
            return False
        return True
    
    def getData(self):
        return [self.name, self.location, self.allergy, self.getAge(), self.getGender(), self.getBloodType]

    def encode(self):
        aName = self.name
        aLoc = self.location
        aAler = self.allergy

        while(len(aName) + len(aLoc) + len(str(self.__age-1)) + len(self.allergy) + 39) > 89:
            aName = aName[:-1]
            aLoc = aLoc[:-1]
            aAler = aAler[:-1]
        
        #return self.__encodeStr(aName) + self.__encodeStr(aLoc) + self.__encodeStr(aAler) + bytes([self.__age + 0x0F, 0x0C, self.__gender + self.__bloodType + 0x0F, 0x0C, ord('\r'), ord('\n')])
        ageEncode = b'. .' if self.__age == 0 else (b'.Tengo ' + str(self.__age-1).encode() + ' años.'.encode())
        return b'Soy ' + self.name.encode() + ageEncode + b'Estoy en ' + self.location.encode() + b'.Alergico a ' + self.allergy.encode() + bytes([0x0D, 0x0A]) #+ b'T' + bytes([0x54, 0x0F, 0x013, 0x61])
    
    def decode(self, dataBytes):
        #print(dataBytes)
        try:
            info = dataBytes.split(b'.')
            if b'Soy ' in info[0]:
                self.name = info[0].decode().replace('Soy', '')
            else:
                return False
            if b'Tengo ' in info[1] and ' años'.encode() in info[1]:
                self.__age = self.__encodeAge(info[1].decode().replace('Tengo ', '').replace(' años', ''))
            else:
                self.__age = 0
            
            if b'Estoy en ' in info[2]:
                self.location = info[2].decode().replace('Estoy en ', '')
            else:
                return False
            
            if b'Alergico a ' in info[3]:
                self.allergy = info[3].decode().replace('Alergico a ', '').replace('\r\n', '')
            else:
                return False

            return True
        except:
            return False


    def __encodeStr(self, txt, eol=1):
        if(eol):
            return txt.encode() + bytes([0x0C])
        return txt.encode()

    def __encodeAge(self, age):
        if type(age) is int:
            return max(0, min(age + 1, 200))
        elif type(age) is str:
            return max(0, 0 if age == '?' else min(int(age) + 1, 200))
        return 0

    def __encodeGender(self, gender):
        if type(gender) is int:
            return min(2, gender)<<4
        elif type(gender) is str:
            gender = gender.upper()
            if gender in self.GENDERS:
                return self.GENDERS.index(gender)<<4
            elif gender in self.GENDERSB:
                return self.GENDERSB.index(gender)<<4

        return self.GENDER_N
    
    def __encodeBloodType(self, bloodType):
        if type(bloodType) is int:
            return max(0, min(bloodType, 8))
        elif type(bloodType) is str:
            bloodType = bloodType.upper()
            if bloodType in self.BLOOD_TYPES:
                return self.BLOOD_TYPES.index(bloodType)
        return self.BLOOD_ERROR
    
    def __str__(self):
        return "Nombre: {}\r\n   Edad: {}\r\n   Ubicación: {}\r\n   Alergias: {}\r\n".format(self.name, self.getAge(), self.location, self.allergy)

    def print(self):
        print("Nombre: ", self.name)
        print("Edad: ", self.getAge())
        print("Sexo: ", self.getGenderBig())
        print("Ubicacion: ", self.location)
        print("Alergias: ", self.allergy)
        print("Tipo de Sangre: ", self.getBloodType())
    