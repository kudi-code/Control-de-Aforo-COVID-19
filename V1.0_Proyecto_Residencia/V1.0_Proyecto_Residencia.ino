//#define TFT_MISO 19  No conectado
//#define TFT_MOSI 23
//#define TFT_SCLK 18
//#define TFT_CS   15  // Chip select control pin
//#define TFT_DC    2  // Data Command control pin
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

//*       T_IRQ           No connection required          
//*       T_DO     19
//*       T_DIN    23
//*       T_CS     5      //set in User_setup.h                
//*       T_CLK    18

//GPIO 22 SLC
//GPIO 21 SDA

//V0R A TX  (TX0)
//V0T A RX  (RX0)

// sd:/mp3/0001Cubrebocas.mp3   (2319 ms)
// sd:/mp3/0002Permitido.mp3    (1710 ms)
// sd:/mp3/0003Tempertura.mp3   (3765 ms)
// sd:/mp3/0004Error.mp3        (2206 ms)           No detectó ni temperatura, ni rostro solo prox

//Únicamente entradas
//GPIO 34
//GPIO 35
//GPIO 36   Rostro
//GPIO 39

//Ultrasonicos
//              Trigger    Echo 
//Entrada         32        33 ultraleft
//Salida          27        14 ultra rigth
//Temperatura     12        13
//_______________________________________________________________________________________________________________________________________
#include "FS.h"                //para Calibración del touch
#include <Ultrasonic.h>        //Ultrasónico
#include <Wire.h>              //Temp
#include <Adafruit_MLX90614.h> //Sensor de Temperatura
#include <SoftwareSerial.h>    //Para el MP3
//#include <DFMiniMp3.h>         //Igual
#include <SPI.h>
#include <TFT_eSPI.h>          // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

#define PIN_ROSTRO 36 //Pin de la detección de rostro (GPIO36 VP)
#define RELAY_PIN 2 //Pin de la puerta (GPIO2)

//int volumen = 80;     //volumen MP3

long i=0;
double sumtemp=0;

int aforomax=0;
long rele_time=0;
long sensado_time=0;

long oldmillis = 0; //Anterior millis()
long TIEMPO_RELE = 5000; //Tiempo de apertura de Puerta
long TIEMPO_SENSADO = 4000; //Tiempo de espera en lo que busca la cámara un rostro

bool DETEC_FACIAL = 0; //Lectura del pin FACE, si detectó o no un rostro
bool PROX_STATUS = 0; //Lectura del sensor de Proximidad
bool ESTADO_RELE = 0;  //Pin de la Puerta
int COMPENSAR_ROSTRO = 0;     //variable que pretende hacer ajuste a la exactitud del sensado

Ultrasonic ultratemp(12, 13);  // (Trig PIN,Echo PIN) del sensor de temp
Ultrasonic ultraleft(32,33);    // Entrada D32 TRIGGER, 33 ECHO
Ultrasonic ultrarigth(27,14);    // salida 

float compensado = 0;
float tempcomp = 0;
//Ultrasónico temperatura

//int personas=0;
//Para la pantalla

//Aforo 
int personas = 0;
int sens1 = 0;
int sens2 = 0;
int totalp =11; 
bool vaf = 0; 
#include "CAforo.h"
#include "Aforolleno.h"
//56,47

//touch
//int TIEMPO_RELE = 5000; //Tiempo de apertura de Puerta
//int TIEMPO_SENSADO = 4000; //Tiempo de espera en lo que busca la cámara un rostro
int aforonum = 0;
bool aforo= 0;   //Configuraciones
int rele = 0;
int sensado=0;

#include "Free_Fonts.h"
#include "Ajustes.h"
#include "ControldeAcceso.h"
#include "AccesoDenegado.h"
#include "Denegadomask.h"
#include "Denegadosigno.h"
#include "Denegadotemp.h"
#include "AccesoPermitido.h"

#define CALIBRATION_FILE "/TouchCalData3"
#define REPEAT_CAL false

{//class Mp3Notify
//{
//public:
//  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
//  {
//    if (source & DfMp3_PlaySources_Sd) 
//    {
//        Serial.print("SD Card, ");
//    }
//    if (source & DfMp3_PlaySources_Usb) 
//    {
//        Serial.print("USB Disk, ");
//    }
//    if (source & DfMp3_PlaySources_Flash) 
//    {
//        Serial.print("Flash, ");
//    }
//    Serial.println(action);
//  }
//  static void OnError(uint16_t errorCode)
//  {
//    // see DfMp3_Error for code meaning
//    Serial.println();
//    Serial.print("Com Error ");
//    Serial.println(errorCode);
//  }
//  static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
//  {
//    Serial.print("Play finished for #");
//    Serial.println(track);  
//  }
//  static void OnPlaySourceOnline(DfMp3_PlaySources source)
//  {
//    PrintlnSourceAction(source, "online");
//  }
//  static void OnPlaySourceInserted(DfMp3_PlaySources source)
//  {
//    PrintlnSourceAction(source, "inserted");
//  }
//  static void OnPlaySourceRemoved(DfMp3_PlaySources source)
//  {
//    PrintlnSourceAction(source, "removed");
//  }
//};

// instance a DFMiniMp3 object,
// defined with the above notification class and the hardware serial class
//
//DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial);
}
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void pantverd ()
{
  tft.pushImage(0, 0, CAforoWidth, CAforoHeight, CAforo);//imprime pantalla azul
  tft.setTextColor(TFT_WHITE,TFT_BLACK);   //Letras blancas, fondo negro
  tft.fillRect (53, 143, 213, 107,TFT_BLACK);
  tft.fillRect (53, 315, 213, 107 ,TFT_BLACK); 
  tft.setTextSize(2);
  tft.setCursor(113, 227);
  tft.setFreeFont(FSB24);
  if (personas<10)
  {
    tft.print("0");
    tft.print(personas);
  }
  else if (personas>=10)
  {
    tft.print(personas);
  }
  if ((totalp-personas)<10)
  {
    tft.setCursor(113, 395);
    tft.print("0");
    tft.print(totalp-personas);    
  }
  else if ((totalp-personas)>=10)
  {
      tft.setCursor(113, 395);
      tft.print(totalp-personas);
  }  
}

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }
 if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

void setup() {
  //Pantalla
  tft.init();
  tft.setRotation(0);  // 1 horizontal
  touch_calibrate();
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);  
  delay(100);
  pantverd();
  pinMode(PIN_ROSTRO, INPUT); //Rostro
  pinMode(0, INPUT);
  pinMode(4, OUTPUT);
  Wire.begin(21,22);            //Inicia Temp
  //Serial.begin(115200);
  //Serial.println("initializing...");
  //mp3.begin();             //Inicia MP3
  //uint16_t volumen = mp3.getVolume();  //Set volumenNn
//  Serial.print("volume ");
//  Serial.println(volumen);
//  mp3.setVolume(volumen);
//  uint16_t count = mp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
//  Serial.print("files ");
//  Serial.println(count);
//  Serial.println("starting...");
  mlx.begin();              //Inicia librería de sensor de temp
}

{//void waitMilliseconds(uint16_t msWait) //Función para que no se escuchen pausas al reproducir un audio
//{
//  uint32_t start = millis();
//
//  while ((millis() - start) < msWait)
//  {
//    // calling mp3.loop() periodically allows for notifications
//    // to be handled without interrupts
//    mp3.loop();
//    delay(1);
//  }
//}
}
void compensar(int distancia, float temperatura)
{
  //5 cm .83 °C        30 cm  2.5 °C
  i++;
  if (distancia > 40){
    tempcomp=0;
    i--;
   }
  float lineal = (-.0688) * (distancia) + 35.504; //temperatura linealizada
  if (distancia == 19) temperatura = temperatura + 1.015;
  if (distancia == 20) temperatura = temperatura + 1.694;
  if (distancia == 21) temperatura = temperatura + 1.65;
  if (distancia == 22) temperatura = temperatura + 1.08;
  if (distancia == 23) temperatura = temperatura + .534;
  if (distancia == 36) temperatura = temperatura + .35;
  if (distancia>30&&distancia<35)  temperatura = temperatura - .5;
  if (distancia == 37) temperatura = temperatura - .2;
  if (distancia == 38) temperatura = temperatura - .2;
  if (distancia == 39) temperatura = temperatura - .2;
  if (distancia > 4 && distancia < 41) {
    tempcomp = (temperatura + lineal) / 2;
    // tempcomp=tempcomp+((distancia-5)*(.1));
    tempcomp = tempcomp + .5 + ((distancia - 5) * (.1));
    sumtemp=sumtemp+tempcomp;
    tempcomp=sumtemp/i;
  }
}

void loop() {
  if (ultratemp.Ranging(CM) < 40 && vaf==0) PROX_STATUS = 1; //Set a 1 prox estatus si hay alguen frente al sensor
 // if (digitalRead(0)==0) PROX_STATUS = 1; //Set a 1 prox estatus si hay alguen frente al sensor    
  if (PROX_STATUS == 0&&aforo==0) //AFORO
  {  
 //Inicia TOOOOODO LO DE AFORO
    sens2=ultrarigth.Ranging(CM);//sensor de salida   
    delay(50);
    sens1=ultraleft.Ranging(CM);//sensor de entrada   
    delay(50);  
    //if(digitalRead(0)==0) sens2=23; 
    tft.setCursor(113, 227);
    if (sens1<50&&sens2>50&&personas<totalp) //sensor uno detecta sensor 2 no (ENTRADA)
    {
        personas++;
        tft.fillRect (53, 143, 213, 107,TFT_BLACK);
        tft.fillRect (53, 315, 213, 107 ,TFT_BLACK);
          if (personas<10)
          {
            tft.print("0");
            tft.print(personas);
          }
          else if (personas>=10)
          {
            tft.print(personas);
          }
        tft.setCursor(113, 395);
          if ((totalp-personas)<10)
          {
            tft.print("0");
            tft.print(totalp-personas);    
          }
          else if ((totalp-personas)>=10)
          {
            tft.print(totalp-personas);
          }  
        delay(1000);        
   }
    if (sens2<50&&sens1>50&&vaf==0) //detacta sensor2 y sensor1 no (SALIDA) limpia
    {
      //if (sens2<50) digitalRead(0)==1;
      if((personas>0))
      {
        personas--;
        tft.fillRect (53, 143, 213, 107,TFT_BLACK);
        tft.fillRect (53, 315, 213, 107 ,TFT_BLACK); 
        if (personas<10)
          {
            tft.print("0");
            tft.print(personas);
          }
          else if (personas>=10)
          {
            tft.print(personas);
          }
        tft.setCursor(113, 395);
          if ((totalp-personas)<10)
          {
            tft.print("0");
            tft.print(totalp-personas);    
          }
          else if ((totalp-personas)>=10)
          {
            tft.print(totalp-personas);
          } 
        //Serial.println("Salida limpia:");
        //Serial.println(sens2);    
        delay(1000);
      }
      else if(personas<=0)
      {
        personas=0;
      }
    }
    if (sens2<50&&sens1>50&&vaf==1) //detacta sensor2 y sensor1 no (SALIDA) desde pantalla roja
    {
        personas--;
        //tft.setSwapBytes(true);
        tft.pushImage(0, 0, CAforoWidth, CAforoHeight, CAforo);//imprime pantalla VERDE
        tft.setCursor(113, 227);
        //tft.setTextColor(TFT_WHITE,TFT_BLACK);
        tft.fillRect (53, 143, 213, 107,TFT_BLACK);
        tft.fillRect (53, 315, 213, 107 ,TFT_BLACK); 
        if (personas<10)
          {
            tft.print("0");
            tft.print(personas);
          }
          else if (personas>=10)
          {
            tft.print(personas);
          }
        tft.setCursor(113, 395);
        if ((totalp-personas)<10)
        {
            tft.print("0");
            tft.print(totalp-personas);    
          }
          else if ((totalp-personas)>=10)
          {
            tft.print(totalp-personas);
        } 
        vaf=0;
    }
    if (personas==totalp) //AFORO LLENO
    {
      if (vaf==0)
      {
          tft.setSwapBytes(true);
          tft.pushImage(0, 0, AforollenoWidth, AforollenoHeight, Aforolleno); //pantalla roja 
      }
          vaf=1;
    }
 }
  else if (PROX_STATUS == 1 && aforo==0) {   //TOOOOOOODO LO DE CHEQUEO
    tft.setCursor(113, 227);
    tft.setTextSize(1);
    //tft.setFreeFont(FSB24);
    //Pantalla
    tft.pushImage(0, 0, ControldeAccesoWidth, ControldeAccesoHeight, ControldeAcceso);//imprime espere
    if (oldmillis == 0) oldmillis = millis(); //Guarda el tiempo actual
    DETEC_FACIAL = digitalRead(PIN_ROSTRO);   //Lee pin rostro
    compensar(ultratemp.Ranging(CM), mlx.readObjectTempC());  //tempcomp vafriable lista
    //Lee Temp
    //if (DETEC_FACIAL == 1) COMPENSAR_ROSTRO++; //Compensa lectura erónea
    if (DETEC_FACIAL==1 && millis() - oldmillis > TIEMPO_SENSADO && PROX_STATUS == 1) { //Detectó un rostro, SIN cubreboca
      tft.pushImage(0 ,0, AccesoDenegadoWidth, AccesoDenegadoHeight, AccesoDenegado);//imprime acceso denegado "limpio"  
      tft.pushImage(81 ,117, DenegadomaskWidth, DenegadomaskHeight, Denegadomask);//imprime error de cubrebocas
      tft.setCursor(87, 358);
      tft.println(tempcomp);  
     // mp3.playMp3FolderTrack(1);  //sd:mp3/0001Cubrebocas.mp3
     // waitMilliseconds(2319);
     delay(2000); //OJO
      oldmillis = 0;
      PROX_STATUS =0;
      pantverd();
      i=0;
      sumtemp=0;
    }
    else if (DETEC_FACIAL==0 && millis() - oldmillis > TIEMPO_SENSADO && PROX_STATUS == 1) //DETECTA ROSTRO CON CUBREBOCAS
    {
        if (tempcomp < 37.2 && tempcomp>35.5) { //Aquí se acaba el tiempo de sensado SENSA TEMPERATURA
            //Cubrebocas si, persona si, temperatura si
            tft.pushImage(56 ,48, AccesoPermitidoWidth, AccesoPermitidoHeight, AccesoPermitido);//imprime acceso denegado "limpio"  
            tft.setCursor(87, 358);
            tft.println(tempcomp);  
            ESTADO_RELE = 1;
           // mp3.playMp3FolderTrack(2);  // sd:/mp3/0002Permitido.mp3
           // waitMilliseconds(1710);
           delay(1500); //OJO
            oldmillis = 0;
            PROX_STATUS =0;
            pantverd();
            i=0;
            sumtemp=0;
        }
        else if (tempcomp > 37.2&& PROX_STATUS == 1) { //Si temp alta      
        tft.pushImage(0 ,0, AccesoDenegadoWidth, AccesoDenegadoHeight, AccesoDenegado);//imprime acceso denegado "limpio"  
        tft.pushImage(81 ,117, DenegadotempWidth, DenegadotempHeight, Denegadotemp);//imprime error de Temperatura
        tft.setCursor(87, 358);
        tft.println(tempcomp);  
        //mp3.playMp3FolderTrack(3);  //sd:mp3/0003Temperatura.mp3
        //waitMilliseconds(3765); 
        delay(1500); //OJO
        oldmillis = 0;
        PROX_STATUS =0;
        pantverd();
        i=0;
        sumtemp=0;
        //72,319
        } 
    }
    if (millis() - oldmillis > TIEMPO_SENSADO && tempcomp<35 && PROX_STATUS == 1) {
      //Tiempo agotado,prox on, temp muy anormal
      tft.pushImage(0 ,0, AccesoDenegadoWidth, AccesoDenegadoHeight, AccesoDenegado);//imprime acceso denegado "limpio"  
      tft.pushImage(81 ,117, DenegadosignoWidth, DenegadosignoHeight, Denegadosigno);//imprime error general
      tft.setCursor(87, 358);
      tft.println(tempcomp);  
      //mp3.playMp3FolderTrack(4);  // sd:/mp3/0004Error.mp3
      //waitMilliseconds(2206);
      delay(1500); //OJO
      oldmillis = 0;
      PROX_STATUS =0;
      pantverd();
      i=0;
      sumtemp=0;
    }
    if(millis() - oldmillis < TIEMPO_SENSADO){
       digitalWrite(4,1);                     
    }
    if(millis() - oldmillis > TIEMPO_SENSADO){
       digitalWrite(4,0);
       oldmillis = 0;
       PROX_STATUS =0;
       i=0;
       pantverd();  
       sumtemp=0;                 
    }   
  }
   uint16_t x, y;
   if (tft.getTouch(&x, &y)) //TODO LO DEL TOUCH
  {
    if(x>257&&x<320&&y>410&&y<480&&aforo==0){
      aforomax=totalp;
      rele_time=(TIEMPO_RELE/1000);
      sensado_time=(TIEMPO_SENSADO/1000);
      tft.pushImage(0, 0, AjustesWidth, AjustesHeight, Ajustes);//imprime pantalla azul
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
      tft.setTextSize(1);
      tft.setFreeFont(FSB18);
      tft.setCursor(68,121); 
      tft.println(aforomax);
      tft.setCursor(68,225);
      tft.println(rele_time);
      tft.setCursor(68,335);
      tft.println(sensado_time);
      delay(500);
      aforo=1;
    }
    if(aforo==1){
      if(x>234&&x<271&&y>88&&y<129){ //Sumar Aforo
        aforomax++;
        tft.fillRect (12, 89, 218, 40,TFT_BLACK);
        tft.setCursor(68,121); 
        tft.println(aforomax);
        delay(500);
      }
      if(x>272&&x<310&&y>88&&y<129){ //Restar Aforo
        if (aforomax>1)
        {
        aforomax--;
        } 
        else if (aforomax<=1) aforomax=1;     
        tft.setCursor(68,121);
        tft.fillRect (12, 89, 218, 40,TFT_BLACK); 
        tft.println(aforomax);
        delay(500);
        }        
      if(x>234&&x<271&&y>189&&y<231){ //Sumar Relay
        rele_time++;
         tft.setCursor(68,225);
         tft.fillRect (12, 188, 218, 40,TFT_BLACK);
        tft.println(rele_time);
         delay(500);
      }
      if(x>272&&x<310&&y>189&&y<231){ //Restar relay
        if (rele_time>1)
        {
        rele_time--;
        }
        else if(rele_time<=1)
        {
          rele_time=1;
        }
         tft.setCursor(68,225);
         tft.fillRect (12, 188, 218, 40,TFT_BLACK);
        tft.println(rele_time);
         delay(500);
        }          
      if(x>234&&x<271&&y>302&&y<346){ //Sumar Sensado
        sensado_time++;
        tft.setCursor(68,335);
        tft.fillRect (12, 305, 218, 40,TFT_BLACK);
        tft.println(sensado_time);
        delay(500);
      }
      if(x>272&&x<310&&y>302&&y<346){ //Restar sensado
        if (sensado_time>1)
        {
        sensado_time--;
        }
         else if(sensado_time <= 1)
        {
          sensado_time=1;
        }
        tft.setCursor(68,335);
        tft.fillRect (12, 305, 218, 40,TFT_BLACK);
        tft.println(sensado_time);
        delay(500); 
        }        
       if(x>21&&x<140&&y>396&&y<429){ //Aceptar
        TIEMPO_SENSADO=sensado_time*1000;
        TIEMPO_RELE=rele_time*1000;
        totalp=aforomax;
        pantverd();
        aforo=0; 
        delay(100);     
       }
       if(x>180&&x<301&&y>396&&y<429){ //Cancelar
        pantverd();
        aforo=0;
        delay(100);
       }
    }
  }
}
