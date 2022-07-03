#include <Arduino.h>
//#include <SPI.h>
//#include <SD.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "DHT11.h"
#include "Ds18B20.h"
#include "RelojRTC.h"


// ------Config Pin Tarjeta SD -------

extern const int lectorSD = 5;
String dataMessage;
//File dataFile;

// ---- Funciones para tarjeta SD ------

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}
//Mes ,Día ,Hora ,Minuto ,Temperatura , Humedad, Temp Interior, Temp Colector 1, Temp Colector 2
/*
 dataMessage = String(mes)" ," + String(dia)" ," + String(hora)+ "Hs" " ," + String(minuto) + " ," + String(temp) + "ºC" " ," + String(hum) + "%"  + "\r\n" ;
*/
void grabarDatos(){
  Serial.println("......................GRABANDO...........");
dataMessage = String(mes) + ", " + String(dia) + ", " + String(hora) + "Hs" + ", " + String(minuto) + " '' " + ", " + String(temp,2) + "ºC" + ", " + String(hum,2) + "%" + ", "
             + String(temp_int,2) + "ºC" + ", " +  String(temp_cl1,2) + "ºC" + ", " + String(temp_cl2,2) + "ºC" +"\r\n" ;
  Serial.println(dataMessage);
   File file = SD.open("/datos.txt");
      appendFile(SD, "/datos.txt", dataMessage.c_str());
      readFile(SD, "/datos.txt");
    file.close();

  }
// --- FIN Funciones tarjeta SD ------


void ConfigSD(){
  pinMode(lectorSD, INPUT);
  // -------Inicializar tarjeta SD ---------
  if(!SD.begin(lectorSD)){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }
      uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    File file = SD.open("/datos.txt");
   
    if(!file){
      Serial.println("Creando archivo de datos en la SD");
      writeFile(SD, "/datos.txt", "Mes ,Día ,Hora ,Minuto ,Temperatura ,Humedad ,Temp-Interior , Temp-Cl1 , Temp-Cl2 \r\n");
      } else {
        Serial.println("El archivo ya existe");
        }

         file.close();

  }
