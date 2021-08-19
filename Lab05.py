from Adafruit_IO import Client, RequestError, Feed
from time import sleep
import serial
import time
import sys

ADAFRUIT_IO_KEY = "aio_Kujq59EID41O56aGmBwDmfoBlgkv"
ADAFRUIT_IO_USERNAME = "Fernando19030"
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

envio = 184
nulo = 0        #valor nulo
nulo = str(nulo)#char nulo
temporal1 = 0
temporal2 = 0

puerto = serial.Serial('COM3', 9600)
puerto.timeout = 10 #tiempo de espera para recibir datos
time.sleep(1)
    
print('El puerto ha sido activado con exito \n')
#----------------Temperatura y humedad valor---------------------------------

while True:
    with puerto:   #utilice el COM serial
        #nombre_feed = cliente.feed('llave_de_mi_feed')
        sensor1_feed = aio.feeds('sensor1')
        sensor2_feed = aio.feeds('sensor2')

        #variable = cliente.receive(mifeed_feed.key) ; Esto para leer valor
        sensor1_data = aio.receive(sensor1_feed.key)
        sensor2_data = aio.receive(sensor2_feed.key)
        
        valor = int(sensor1_data.value)
        puerto.write(b's') #mando codigo de caracter
        val = puerto.readline(3).decode('ascii') #recibo 2 bytes
        val = int(val)  #convierto a entero

        if(valor != temporal1 or val != temporal2):
            print(f'temperatura actual: {sensor2_data.value}')
            print(f'humedad actual: {sensor1_data.value} \n')

            if (valor < 10 and valor > -1):
                puerto.write(nulo.encode('ascii')) #envio un cero
                puerto.write(nulo.encode('ascii'))
                
            elif (valor <100 and valor > 9):
                puerto.write(nulo.encode('ascii')) #envio un cero
            
            envio = str(int(sensor2_data.value))
            puerto.write(envio.encode('ascii'))
            print("Escrito en pic: {} \n".format(envio))
            print("Escrito en AdaFruit: ")
            print(val)
            
            #cliente.send_data(mifeed.feed.key, valor a enviar) ; Esto para mandar dato y leer
            aio.send_data(sensor1_feed.key, val)
            print('----------------------------------------------------')
            temporal1 = int(sensor2_data.value)
            temporal2 = int(sensor1_data.value)
            
        
