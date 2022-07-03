#include <Arduino.h>

#include <OneWire.h>
#include <DallasTemperature.h>

//------Config para Sensores Ds18B20 -------
#define ONE_WIRE_BUS 15 // Pin del Esp32
#define TEMPERATURE_PRECISION 10
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
/* 
  Direcciones de cada Sensor, obtener con programa lector
*/

 float temp_int = 0;             //Temperatura interior Ds18B20
 float temp_cl1 = 0;             //Temperatura colector 1 Ds18B20
 float temp_cl2 = 0;             //Temperatura colector 2 Ds18B20

  uint8_t Interior[8] = {  0x28, 0x31, 0xE2, 0xA0, 0x0D, 0x00, 0x00, 0x24};
  uint8_t Colector1[8] = {  0x28, 0x45, 0xB5, 0x9F, 0x0D, 0x00, 0x00, 0x76};
  uint8_t Colector2[8] = {  0x28, 0x8D, 0xC9, 0x9F, 0x0D, 0x00, 0x00, 0x04};

/* 
  Funciones para los sensores Ds18B20
    sensors.requestTemperatures(); para inciciar medicion en loop()
*/

// function to print a device address


  void printAddress(DeviceAddress deviceAddress)
  {
    for (uint8_t i = 0; i < 8; i++)
    {
      // zero pad the address if necessary
      if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
    }
  }

// function to print the temperature for a device
  void printTemperature(DeviceAddress deviceAddress)
  {   
    float tempC = sensors.getTempC(deviceAddress);
    if(tempC == DEVICE_DISCONNECTED_C) 
    {
      Serial.println("Error: Could not read temperature data");
      return;
    }
  
    Serial.print("Temp C: ");
    Serial.print(tempC);
    Serial.println();
    /*
    Serial.print(" Temp F: ");
    Serial.print(DallasTemperature::toFahrenheit(tempC));
    */
  }

// function to print a device's resolution
  void printResolution(DeviceAddress deviceAddress)
  {
    Serial.print("Resolution: ");
    Serial.print(sensors.getResolution(deviceAddress));
    Serial.println();
  }
  
  
  void printData(DeviceAddress deviceAddress)
  {
    Serial.print("Device Address: ");
    printAddress(deviceAddress);
    Serial.print(" ");
    printTemperature(deviceAddress);
    Serial.println();
  }



  void configDs18B20(){
     Serial.println("Sensores Ds18B20");
  sensors.begin();
  // locate devices on the bus
  Serial.println("Buscando Sensores...");
  Serial.print("Encontrados: ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
 
  // set the resolution to 9 bit per device
  sensors.setResolution(Interior, TEMPERATURE_PRECISION);
  sensors.setResolution(Colector1, TEMPERATURE_PRECISION);
  sensors.setResolution(Colector2, TEMPERATURE_PRECISION);
 

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(Interior), DEC);
  Serial.println();
    Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(Colector1), DEC);
  Serial.println();
    Serial.print("Device 2 Resolution: ");
  Serial.print(sensors.getResolution(Colector2), DEC);
  Serial.println();

    }

    void MedirTempDs18B20(){
      
 sensors.requestTemperatures();
 temp_int = sensors.getTempC(Interior);
 temp_cl1 = sensors.getTempC(Colector1);
 temp_cl2 = sensors.getTempC(Colector2);
      }
