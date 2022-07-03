#include <Arduino.h>
#include "DHT11.h"
#include "DS18B20.h"
#include "TarjetaSD.h"
#include "RelojRTC.h"

#include <WiFi.h>
#include <WebServer.h>

 const char PinGrabacionEnb =9; // Habilita por jumper el registro de datos en la SD
 const char PinConfig = 10; //Habilita por datos el modo configuracion
 const char Cooler = 15;  // Pin de salida para el cooler
  unsigned int contador = 0; // Variable para el tiempo de las acciones
 int tiempoMinutos =0;
 bool bandera = false;


//--------Configuracion de interrupcion -------------

volatile int interruptCounter;

 
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
 
}

// ------ Modo configuración -----------

//pinMode(PinConfig,INPUT); // Entrada para modo configuracion.
void Configuracion(){
  while( digitalRead( PinConfig )== LOW ){
  
  }
}

// ------Configuracion del WiFi------------
static const char *SSID = "Med_TEMP";
static const char *PASS = "12345678";

// -----Configuracion de la IP fija -------
IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,22);
IPAddress subnet(255,255,255,0);

void WiFiAPEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
  // Handling function code
  if (event == SYSTEM_EVENT_AP_START) {
    Serial.println("AP Started");
  }
  else if (event == SYSTEM_EVENT_AP_STACONNECTED) {
    Serial.println("Client connected");
  }
  else if (event == SYSTEM_EVENT_AP_STADISCONNECTED) {
    Serial.println("Client disconnected");
  }
}

WebServer server(80); // Crear el servidor puerto 80


void setup() {
   Serial.begin(115200);
 Serial.println("ESP-32 Datalogger de temperatura");
  pinMode(PinGrabacionEnb, INPUT);
  pinMode(Cooler, OUTPUT);
  
 configuracionDHT11(); // Inicializar sensor DHT11
 configDs18B20(); // Inicializar Sensores Ds18B20
 ConfigSD(); // Inicializar tarjeta SD
 ConfigRTC();


 //--------Inicializar las interrupciones -----------

  timer = timerBegin(0, 800, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
 



 
  //------Inicializar el WI-FI -----------
  Serial.println("Configuring access point...");
  WiFi.softAP(SSID, PASS);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  
  // Attach WiFi events to callback function
  WiFi.onEvent(WiFiAPEvent, SYSTEM_EVENT_AP_START);
  WiFi.onEvent(WiFiAPEvent, SYSTEM_EVENT_AP_STACONNECTED);
  WiFi.onEvent(WiFiAPEvent, SYSTEM_EVENT_AP_STADISCONNECTED);

  
    
 
  // -------Inicializar el Servidor---------
  server.begin();
  server.on ("/",RespuestaCliente);
  ObtenerMinutos();
  tiempoMinutos = minuto;
  
 
}

String SendHTML(float tempSensor1,float tempSensor2,float tempSensor3){
  String ptr = "<!DOCTYPE html>";
  ptr +="<html>";
  ptr +="<head>";
  ptr +="<title>ESP32 Medidor de Temperatura</title>";
  ptr +="<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr +="<meta http-equiv='refresh' content='5' >";
  ptr +="<link href='https://fonts.googleapis.com/css?family=Open+Sans:300,400,600' rel='stylesheet'>";
  
  ptr +="<style>";
  ptr +="html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #444444;}";
  ptr +="body{margin-top: 50px;} ";
  ptr +="h1 {margin: 50px auto 30px;} ";
  ptr +=".side-by-side{display: table-cell;vertical-align: middle;position: relative;}";
  ptr +=".text{font-weight: 600;font-size: 19px;width: 200px;}";
  ptr +=".temperature{font-weight: 300;font-size: 50px;padding-right: 15px;}";
  ptr +=".living-room .temperature{color: #3B97D3;}";
  ptr +=".bedroom .temperature{color: #F29C1F;}";
  ptr +=".kitchen .temperature{color: #26B99A;}";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;right: -5px;top: 15px;}";
  ptr +=".data{padding: 10px;}";
  ptr +=".container{display: table;margin: 0 auto;}";
  ptr +=".icon{width:82px}";
  ptr +="</style>";
 
  ptr +="</head>";
  ptr +="<body>";

  ptr += "<h1>Fundacion El Sol Sale Para Todos</h1>";
            //   ptr +=  "<h2>Temperatura Hambiente : "+String(temp)+"</h2>";
             ptr +=  "<p><FONT SIZE=4>Temperatura Hambiente :</font><FONT SIZE=6>"+String(temp)+"&#186</font></p>";
               //ptr +=  "<h2>Humedad Hambiente: " +String(hum)+ "</h2>" ;
                ptr +=  "<p><FONT SIZE=4>Humedad Hambiente: </font><FONT SIZE=6>"+String(hum)+"%</font></p>";
              //  ptr += "<h2>Temperatura Interior: " +String(temp_int)+"</h2>";
               ptr +=  "<p><FONT SIZE=4>Temperatura Interior: </font><FONT SIZE=6>"+String(temp_int)+"&#186</font></p>";
              //  ptr += "<h2>Temperatura Colector 1: " +String(temp_cl1)+"</h2>" ;
                  ptr +=  "<p><FONT SIZE=4>Temperatura Colector 1:</font><FONT SIZE=6>"+String(temp_cl1)+"&#186</font></p>";  
               // ptr += "<h3>Temperatura Colector 2:</h3><h2> " +String(temp_cl2)+"</h2>" ;
                ptr +=  "<p><FONT SIZE=4>Temperatura Colector 2:</font><FONT SIZE=6>"+String(temp_cl2)+"&#186</font></p>";  
                ptr += "<h3>fecha: " + String(mes)+"/" +String(dia) +"</h3>" ;
                 ptr +="<h3>Hora: " + String(hora)+ ":" +String(minuto) +"</h3>";
/*
  ptr += "<h3>Temperatura Hambiente : "+String(temp)+"</h3>";
               ptr +=  "<h3>Humedad Hambiente: " +String(hum)+ "</h3>" ;
                ptr += "<h3>Temperatura Interior: " +String(temp_int)+"</h3>";
                ptr += "<h3>Temperatura Colector 1: " +String(temp_cl1)+"</h3>" ;
                ptr += "<h3>Temperatura Colector 2: " +String(temp_cl2)+"</h3>" ;
                ptr += "<h3>Mes: " + String(mes)+"</h1>" + "<h1>Dia: " +String(dia) +"</h3>" ;
                 ptr +="<h3>Hora: " + String(hora)+ "</h1> " + "<h1> " +String(minuto) +"</h3>";

                 */
  ptr +="</body>";
  ptr +="</html>";
  return ptr;
}



void RespuestaCliente() {
 
server.send(200,"text/html", SendHTML(temp_int, temp_cl1, temp_cl2) );
  
  }

void capturarDatos(bool flag){
  if (flag == true){
      MedTempyHum();
      MedirTempDs18B20();
      ObtenerTiempoyFecha();
    
    }
  
  }


void loop() {
  server.handleClient();
  delay(100);
  
  if (interruptCounter > 0) {
    // If que entra cada 1 segundo por interrupcion !
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    bandera = !bandera;
    
  }
   ObtenerMinutos();
  capturarDatos(bandera);


  
  if(  tiempoMinutos != minuto){
    
    contador ++;
    tiempoMinutos = minuto;
    Serial.println(contador);
    
    }
    //Tiempo entre cada grabacion
    if(contador == 2){
       grabarDatos();
       contador = 0;
      
      }

  if(temp_int > 60){
    digitalWrite(Cooler, HIGH);
    
    }else{
      digitalWrite(Cooler, LOW);
      }

  


 
}
