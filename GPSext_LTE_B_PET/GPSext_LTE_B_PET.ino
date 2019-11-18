
#include <Adafruit_NeoPixel.h>
#include "funciones.h"

#include <Adafruit_SSD1306.h>
//#include <HDQ.h>
//HDQ HDQ(24); // Arduino digital pin 24
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

//#include <TinyGPS++.h>
//TinyGPSPlus gps;

#include <TinyGPS.h>
TinyGPS Tgps;

//#include <SoftwareSerial.h>
#include "SIM800.h"

#include "LowPower.h"

//#define PIN 12 //NEOPIXEL
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

//#define PAT "device=GPS3&a=%s*%s&b=%s*%s&c=%s&f=%s"
//#define APN "pda.bell.ca"
//String APN = "internet.fido.ca";
#define APN "TM"
//String APN =  "rogers-core-appl1.apn";
//uint32_t errors = 0;
//String url = "turniot.com/GPS/din.php?";
String url = "gps2.turniot.com/php/din.php?";

CGPRS_SIM800 gprs;

#include <Adafruit_GPS.h>
#define GPSSerial Serial
Adafruit_GPS GPS(&GPSSerial);
#define GPSECHO  false




#include <Wire.h>

#include "RTClib.h"
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

RTC_DS1307 rtc;

float gx = 0;
float gy = 0;

String pausa = "0";

//String punto;
String last_fix;
String bateria;
//String sig;
String qry = "";
String q = "";
String dev = "GPS2";
String payload = "";
String imei = "";
String temp = "";
String tsGPS = "";
String alt_str = "";
String spd_str = "";

int spd_int = 0;
int red = 0;
int green = 0;
int blue = 0;

int sat = 0;
int movido = 0;
int semovio = 0;
int tiempoEST = 0;
int tiempoDIN = 0;
int tiempoGPS = 10;//Initial waiting syncro time
int estadoanterior = 0;
//int delta = 0;
int flag = 0;
int malinicio = 0;


int loopCycles_status = 0;
int loopCycles_ftp = 0;
int loopCycles_led = 10;

int gpson = 0;
bool gpsSTS = 0;
bool gsmon = 0;

#include <SoftwareSerial.h>

SoftwareSerial console(1, 2);
//#define con Serial
#define gpsEXT Serial

#define sim800 Serial1




//SoftwareSerial ss(2,30); // RX,TX CONFIGURAR PIN DE COMUNICACION DE GPS
int gpsonpin = 0; //LED

int GPS_EXT_ON = 14; //HABILITA GPS EXTERNO
int led0 = 2; //DUMMY PIN PORQUE NO ESTA CONECTADO A NADA

int initialUpload = 1;


int reg_e = 30; //Eneable regulator pin
int tilt1 = 18;
//int tilt2 = 25;


//Function extracted from the library http://playground.arduino.cc/Code/Time
//if you just need the function without compiling few kbs and you need to save
//space, this is the way to go

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)

// leap year calulator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )
static  const uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // API starts months from 1, this array starts from 0


bool locusOn = false;
int locusRecords = 0;
int locusRecordsPercent = 0;

struct locus
{

  bool on;
  int percent;
  int records;
  int attempts;
  int cycles;
};


struct config
{

  long targetLat;
  long targetLon;
  float fNum;
  bool first;
  int attempts;

} config;

struct locus LOCUS;

//locus LOCUS;

int loopCycles = 0;
//int gpsBaud = 4800;
int gpsBaud = 9600;

void setup()
{



  LOCUS.on = false;
  LOCUS.percent = 0;
  LOCUS.records = 0;
  LOCUS.attempts = 0;
  LOCUS.cycles = 0;

  Wire.begin();



  pinMode(tilt1, INPUT);
  //pinMode(tilt2, INPUT);
  pinMode(reg_e, OUTPUT); //ENEABLE REGULATOR
  pinMode(gpsonpin, OUTPUT); //ENEABLE GPS_EN
  pinMode(GPS_EXT_ON, OUTPUT); //ENEABLE GPS_EN

  funciones.REG_ON();
  //digitalWrite(reg_e, HIGH);
  //digitalWrite(14,HIGH);
  digitalWrite(gpsonpin, LOW);

  digitalWrite(GPS_EXT_ON, LOW);

  console.begin(57600);
  gpsEXT.begin(9600);
  Serial.begin(57600);
  GPS.begin(gpsBaud);


  console.begin(57600);
  console.println("Starting....");

  
  
  //turngps(gpson);

  pinMode(led0, OUTPUT);

  qry.reserve(250);

  scani2c();



  if (! rtc.begin()) {
    console.println("Couldn't find RTC");
    while (1);
  }

  delay(2500);

  //strip.begin();
  //strip.show(); // Initialize all pixels to 'off'


  gpsEXT.begin(9600);
  delay(200);
  //gpsEXT.println("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");//NMEA MINIMO
  //delay(200);
  //gpsEXT.println("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");//NMEA MINIMO
  //delay(200);
  //gpsEXT.println("$PMTK251,4800*14\r\n");
  //delay(200);


  for (int i = 0; i <= 2; i++) {
    //gpsEXT.begin(gpsBaud);
    //gpsEXT.println("$PMTK251,4800*14\r\n");
    //delay(200);
    //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    delay(200);
    //gpsEXT.println("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
    //delay(200);
    //gpsEXT.println("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
    //delay(200);
    gpsEXT.println("$PMTK185,1*23\r\n");//STOP LOG
    delay(200);
    //gpsEXT.println("$PMTK225,8*23\r\n");//ALLWAYS LOCATE BACKUP
    //delay(200);


    String temp = "PMTK187,1,";
    temp += "1";
    temp = "$" + temp + "*" + checkSum(temp) + "\r\n";
    gpsEXT.println(temp);//EVERY X SEC
    //gpsEXT.println("$PMTK187,1,5*38\r\n");//EVERY 5 SEC
    delay(200);
    funciones.GPS_PPS();

  }

  //gpsEXT.println("$PMTK185,0*22\r\n");//START LOG
  //delay(200);

  //gpsEXT.println("$PMTK185,0*22\r\n");//START LOG
  //delay(200);

  gpsEXT.begin(gpsBaud);
  Serial.begin(57600);

  console.begin(57600);

  console.println("Running....");

  funciones.REG_OFF();
  //digitalWrite(reg_e, LOW);

}


void loop() {

 
  //mode_debug();
  //funciones.REG_OFF();
  //funciones.CORE_SLEEP(1);
  funciones.UPDATE();


  
/*
  
  //console.println(leeGPS(1));

  //console.println(tsGPS);
  //console.println(last_fix);

  if (loopCycles_led <= 0) {
    digitalWrite(gpsonpin, HIGH);
    delay(15);
    digitalWrite(gpsonpin, LOW);
    loopCycles_led = 10;
  }


  if (loopCycles_status <= 0) {

    if (modo_normal() == -1) {
      modo_normal();
    }

    savingMode(6000);

    loopCycles_status = tiempoEST;

  }


  if (loopCycles_ftp <= 0) {

    loopCycles_ftp = 10;


    LocusStatus();

    //console.println("LOCUS.on");
    //console.println(LOCUS.on);
    //console.println("LOCUS.records");
    //console.println(LOCUS.records);

    if (LOCUS.records >= 650) {
      if (uploadLog()) {
        LocusErase();

      } else {
        console.println("##UPLOADING ERROR##");
      }
    }

    //console.println("LOCUS.on");
    //console.println(LOCUS.on);

    if (LOCUS.on == false) {
      LocusStart();
      //console.println("#1");
    }


  }

  //if (sim800.available())
  //console.print(char(sim800.read()));




  dormir(1);

  loopCycles_ftp--;
  loopCycles_status--;
  loopCycles_led--;


*/


}


void mode_debug(){
  console.begin(57600);
  //console.println(".");
  delay(500);
  if (console.available()) {
  
    char x = console.read();
    switch (x) {
      case '1':
        funciones.LED0_ON();        break;
      case '0':
        funciones.LED0_OFF();        break;
      case 'i':
        funciones.NEO_INT_SETUP();        break;
      case 'I':
        funciones.NEO_INT_SET(122,122,122);        break;
      case 'r':
        funciones.REG_OFF();        break;
      case 'R':
        funciones.REG_ON();        break;
      case 'L':
        funciones.GPS_COLD();        break;
      case 'M':
        funciones.GPS_NMEA();        break;
      case 'B':
        funciones.GPS_BAUD();        break;
      case 'o':
        funciones.GPS_OFF();        break;    
      case 'O':
        funciones.GPS_ON();        break;
      case 'Y':
        funciones.GPS_STBY();        break;
      case 'D':
        funciones.GPS_LOG_READ();        break;
      case 'S':
        funciones.GPS_LOG_START();        break;
      case 's':
        funciones.GPS_LOG_STOP();        break;
      case 'Q':
        funciones.GPS_LOG_STATUS();        break;
        LocusStatus();
      case '<':
        funciones.GPS_LOG_ERASE();        break;
      case 'G':
        funciones.GPS_READ(10);        break;
      case 'E':
        funciones.GPS_ENABLE();        break;
      case 'e':
        funciones.GPS_DISABLE();        break;
      case 'p':
        funciones.GPS_PPS();        break;
        

      case 'P':
        funciones.GSM_IP();        break;
      case 'z':
        funciones.GSM_SLEEP();        break;
      case 'Z':
        funciones.GSM_ON();        break;
      case 'T':
        funciones.GSM_BAT();        break;
      case 'K':
        console.println(funciones.GSM_CONNECT());        break;
        

      case '?':
        funciones.STATUS();        break;
      case '@':
        console.println(funciones.STATUS_GSM());        break;
      case '#':
        console.println(funciones.STATUS_GPS(10));        break;
      case '$':
        console.println(funciones.STATUS_REG());        break;
                
      case 'c':
        funciones.CORE_SLEEP(10," ");        break;
      case 'm':
        funciones.CORE_RUNTIME();        break;

      case 'u':
        funciones.UPDATE();        break;
        
      default:
        // statements
        break;
    }

    
    //if (initialUpload == 1){initialUpload=0;x = 'U';}

    //if (x == 'R'){leerLog();}


    
    if (x == 'U') {
      uploadLog();
    }
    if (x == 'N') {
      modo_normal();
      
    }




    if (x == 'g') {
      console.println("BEARER?");
      console.println(qry_AT("AT+SAPBR=2,1", 1000, "OK"));
    }






/*
    if (x == '\n') {
      sim800.println();
    } else {
      sim800.print(x);
    }
*/

  }
}


void LocusErase() {
  console.println("Erase Log");
  gpsEXT.begin(gpsBaud);
  gpsEXT.println("$PMTK184,1*22\r\n");

}

void LocusStart() {
  gpsEXT.begin(gpsBaud);
  String temp = "PMTK187,1,";
  temp += "7";
  temp = "$" + temp + "*" + checkSum(temp) + "\r\n";
  gpsEXT.println(temp);//EVERY X SEC
  //gpsEXT.println("$PMTK187,1,5*38\r\n");//LOCUS EVERY 5 SEC
  gpsEXT.println("$PMTK185,0*22\r\n");//LOCUS START
}

void LocusStop() {
  gpsEXT.begin(gpsBaud);
  gpsEXT.println("$PMTK185,1*23\r\n");//LOCUS STOP
}

void LocusStatus() {
  GPS.begin(gpsBaud);
  uint32_t updateTime = 4000;

  if (millis() > updateTime)
  {
    updateTime = millis() + 1000;
    if (GPS.LOCUS_ReadStatus()) {
      console.print("\n\nLog #");
      console.print(GPS.LOCUS_serial, DEC);
      if (GPS.LOCUS_type == LOCUS_OVERLAP)
        console.print(", Overlap, ");
      else if (GPS.LOCUS_type == LOCUS_FULLSTOP)
        console.print(", Full Stop, Logging");

      if (GPS.LOCUS_mode & 0x1) console.print(" AlwaysLocate");
      if (GPS.LOCUS_mode & 0x2) console.print(" FixOnly");
      if (GPS.LOCUS_mode & 0x4) console.print(" Normal");
      if (GPS.LOCUS_mode & 0x8) console.print(" Interval");
      if (GPS.LOCUS_mode & 0x10) console.print(" Distance");
      if (GPS.LOCUS_mode & 0x20) console.print(" Speed");

      console.print(", Content "); console.print((int)GPS.LOCUS_config);
      console.print(", Interval "); console.print((int)GPS.LOCUS_interval);
      console.print(" sec, Distance "); console.print((int)GPS.LOCUS_distance);
      console.print(" m, Speed "); console.print((int)GPS.LOCUS_speed);
      console.print(" m/s, Status ");
      if (GPS.LOCUS_status) {
        LOCUS.on = true;
        //locusOn = true;
        console.print("LOGGING, ");
        //console.print(locusOn);
      } else {
        LOCUS.on = false;
        //locusOn = false;
        console.print("OFF, ");
        //console.print(locusOn);
      }
      console.print((int)GPS.LOCUS_records); console.print(" Records, ");
      LOCUS.records = (int)GPS.LOCUS_records;
      console.print((int)GPS.LOCUS_percent); console.print("% Used ");
      //LOCUS.percent = (int)GPS.LOCUS_percent;

    }//if (GPS.LOCUS_ReadStatus())
  }//if (millis() > updateTime)


}


bool uploadLog() {

  if (gsmon == 0) {
    digitalWrite(led0, LOW);
  }
  if (gsmon <= 0) {
    confmodem();
  }

  console.println("################configure_FTP");
  if (configure_FTP()) {

    ////uploadFTP();
    console.println("################uploadFTP");
    if (upload_FTP()) {
      return true;
    }

    return false;
  }



  console.println("##UPLOAD LOG##");
  return false;
}
////////////////////////////////////////////FTP ROUTINES

bool configure_FTP() {



  leeAT("AT+SAPBR=1,1");
  leeAT("AT+SAPBR=2,1");
  leeAT("AT+FTPCID=1");
  leeAT("AT+FTPSERV=\"gps.turniot.com\"");
  leeAT("AT+FTPPORT=21");
  leeAT("AT+FTPUN=\"gps@gps.turniot.com\"");
  //leeAT("AT+FTPUN=\"carlos\"");
  leeAT("AT+FTPPW=\"Arriaga83\"");
  leeAT("AT+FTPPUTNAME=\"LOCUS.txt\"");
  leeAT("AT+FTPPUTPATH=\"/\"");
  //leeAT("AT+FTPPUT=1");


  if (qry_AT("AT+FTPPUT=1", 10000, "FTPPUT") != 0) {
    return true;
  }

  console.println("##CONFIG FTP##");
  return false;



}


bool upload_FTP() {

  int invalidLog = 0;
  int cont = 0;
  String str;

  console.println("GPS to FTP");

  sim800.println("AT+FTPPUT=2,236");

  //delay(1500);

  gpsEXT.begin(gpsBaud);
  gpsEXT.begin(gpsBaud);

  delay(1500);

  gpsEXT.println("$PMTK622,1*29");


  while (gpsEXT.available() > 0) {
    str = gpsEXT.readStringUntil('$');
    //sprintf(str2, "%236s", str);
    str = str + "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";

    //str = str + "@@@@@@@@@@@@@@@@@@@@@@\n";

    //sim800.print(str);

    int pos = str.indexOf("PMTKLOX");

    if (pos >= 0) {

      sim800.print(str);
      invalidLog = 0;
    } else {
      int temp = str.indexOf("GNRMC");
      if (temp >= 0) {
        invalidLog++;
      }
      if (str.indexOf("ERROR") >= 0) {
        invalidLog = 15;
        break;
      }
      if (str.indexOf("PMTK001,622,") >= 0) {
        break;
      }
    }


    if (invalidLog >= 15) {

      console.println("##INVALID/FINISH LOG##");
      break;
    }

    qry_AT(" ", 500, "FTPPUT");
    sim800.println("AT+FTPPUT=2,236");

    cont++;

  }



  qry_AT("AT+FTPPUT=2,0", 2000, "OK");

  gsmon = 0; gprs.httpUninit(); gprs.sleep(true);

  if (invalidLog >= 15) {
    return false;
  }
  return true;



}




int modo_normal() {

  //reloj();
  //strip.setPixelColor(n, red, green, blue);


  //scani2c();
  //delay(300);
  //loop();

  console.print("MAKEUNIX: "); console.println(makeTime(20, 10, 00, 01, 06, 2015));
  //disp("TEMP: ",readOnewire(),1);
  console.print("TEMP: "); console.println(readOnewire());
  //delay(2000);
  //disp("Iniciando ","",1);

  //turngps(gpson);


  //gpson=1;
  //leeGPS(20);

  //loop();

  //if (gsmon <= 0) {confmodem();}

  //if (gpson == 0){digitalWrite(led0,LOW);}

  //disp(leeGSMbat(),"%",2);


  qry = "x=" + dev + "&a=";
  
  qry =  qry + leeGPS(tiempoGPS) + ",";
  //if (last_fix.toInt() <= 1000 & last_fix.toInt() > 0){
  if (gpsSTS == 1) {
    digitalWrite(led0, HIGH);
    qry =  qry + "0,0,0,0" + ",";
  } else {
    digitalWrite(led0, LOW);
    leeAT("AT");
    delay(100);
    leeAT("AT");
    delay(100);

    qry =  qry + leeGSM() + ",";
    //qry =  qry + "0,0,0,0" + ",";
  }

  String bat = leeGSMbat();
  leeAT("AT");
  delay(100);
  leeAT("AT");
  delay(100);



  //qry =  qry + leeGSM() + ",";

  qry =  qry + bat + ",";

  qry =  qry + String(semovio) + ",";

  temp =  leeAT("AT+CMTE?");
  qry =  qry + temp + ",";

  imei =  leeAT("AT+GSN");
  qry =  qry + imei + ",";

  qry =  qry + tsGPS + ",";

  qry =  qry + alt_str + ",";

  qry =  qry + readOnewire() + ",";

  qry = qry + reloj() + ",";

  qry = qry + spd_str + ",";

  qry = qry + last_fix;





  //console.println(altitud);

  qry.replace(" ", "");


  //console.print("QRY>");console.print(qry);console.println("<");
  disp("Enviando ", "", 1);
  console.print(url); console.println(qry);

  //if (corrupt(qry) != 1){
  payload = sendToServer();
  //}


  console.print("Payload->"); console.println(payload);


  int intentos = 0;

  if ((payload == "0") || (payload == "")) {
    return -1;

  }




  qry = "";

  console.println(payload);
  pausa = explotar(payload, ',', 0);
  tiempoEST = pausa.toInt();
  console.print(F("[TiempoEST]"));
  console.println (tiempoEST);


  pausa = explotar(payload, ',', 1);
  tiempoDIN = pausa.toInt();
  console.print(F("[TiempoDIN]"));
  console.println (tiempoDIN);

  pausa = explotar(payload, ',', 2);
  tiempoGPS = pausa.toInt();
  console.print(F("[TiempoGPS]"));
  console.println (tiempoGPS);

  pausa = explotar(payload, ',', 3);
  gpson = pausa.toInt();
  console.print(F("[GPSON]"));
  console.println (gpson);

  pausa = explotar(payload, ',', 4);
  red = pausa.toInt();
  console.print(F("[RED]"));
  console.println (red);

  pausa = explotar(payload, ',', 5);
  green = pausa.toInt();
  console.print(F("[GREEN]"));
  console.println (green);

  pausa = explotar(payload, ',', 6);
  blue = pausa.toInt();
  console.print(F("[BLUE]"));
  console.println (blue);

  //strip.setPixelColor(0, red, green, blue);
  //strip.setPixelColor(1, red, green, blue);
  //strip.setPixelColor(2, red, green, blue);
  //strip.setPixelColor(3, red, green, blue);
  //strip.show();

  //savingMode(tiempoEST);

  delay(100);

  //turngps(gpson);

  //if (movido >= 1){console.print("zzzzD..");dormir(tiempoDIN);movido = 0;}
  //if (movido >= 1){console.print("zzzzD..");console.println(tiempoDIN * 1000);dormir(tiempoDIN * 1000);movido = 0;}
  //console.print("zzzzE..");

  //turngps(gpson);

  ///////////////////////dormir(tiempoEST);
  //delay(tiempoEST);
  return 1;
}

int corrupt(String torev) {

  if (torev.length() <= 10) {
    return 1;
  }

  //pausa = explotar(torev,',',10);

  //if (pausa.length() <= 3){return 1;}


  //if (ocurrences(torev,"E") != 0 ){return 1;}
  //if (ocurrences(torev,"@") >= 2){return 1;}
  //if (ocurrences(torev,"*") > 0){return 1;}

  return 0;//Healty data
}

int ocurrences(String str, String c) {


  int y = 0;
  for (byte i = 0; i < str.length(); i++) {
    if (str[i] == c[0]) {
      y++;
    }
  }
  return y;

}


void savingMode(int z) {

  if (z >= 60) {
    gsmon = 0;
    gprs.httpUninit();
    gprs.sleep(true);
    turngps(0);
  }

}

void leerball() {

  int estadoactual = 0;
  if (digitalRead(tilt1) == HIGH ) {
    estadoactual = 1;
  }
  if (digitalRead(tilt1) == LOW ) {
    estadoactual = 0;
  }

  if (estadoactual != estadoanterior) {
    console.println(F("MOVIMIENTO"));
    semovio = 1;
  } else {
    semovio = 0;
  }
  estadoanterior = estadoactual;

  /*
    if (digitalRead(tilt1) == HIGH && flag == 0) {
    flag = 1;
    console.println(F("HIGH"));
    semovio = 0;
    return;
    }

    if (digitalRead(tilt1) == LOW && flag == 1) {
    flag = 0;
    semovio = 1;
    console.println(F("LOW"));
    return;
    }
  */
}


String leeGPS(int timeout) {

  float flat, flon;
  String gx_st, gy_st;

  unsigned long age;

  q = F("0,0,0");
  //gx="0.0";
  //gy="0.0";

  //turngps(gpson);

  


  for (int i = 0; i <= timeout; i++)
  {

    //gpsEXT.listen();
    //delay(10);

    //reloj();

    smartDelay(1000);

    Tgps.f_get_position(&flat, &flon, &age);
    //console.println(flat);
    //console.println(flon);
    gx_st = printFloat(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6); //print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
    gy_st = printFloat(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);

    last_fix = print_int(age, TinyGPS::GPS_INVALID_AGE, 7);
    last_fix = last_fix.toInt();
    tsGPS = print_date(Tgps);


    String sat_str = print_int(Tgps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
    spd_str = printFloat(Tgps.f_speed_kmph(), TinyGPS::GPS_INVALID_F_SPEED, 6, 2);
    alt_str = printFloat(Tgps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 8, 2);


    //sat = gps.satellites.value();
    //q = gx_st + "," + gy_st + "," + last_fix;

    q = gx_st + "," + gy_st + "," + sat_str.toInt();

    //console.print("TS:");console.println(printDateTime(gps.date, gps.time));


    /*
      altitud = printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
      tsGPS = printDateTime(gps.date, gps.time);
      gx = gps.location.lat();
      gy = gps.location.lng();
      sat = gps.satellites.value();
      q = floatToStr(gx,6) + "," + floatToStr(gy,6) + "," + floatToStr(sat,1);
    */
    digitalWrite(led0, !digitalRead(led0));
    console.println(q);
    //if (sat==0){gx=0;gy=0;}
    //return q;
    if (gpson == 0) {
      i = timeout;
      q = F("0,0,0");
    }

    //if (last_fix.toInt() <= 1000 & last_fix.toInt() > 0){i = timeout;} //  <---FUNCIONA OK
    if (gx_st != "1000.000000") {
      i = timeout;
    }
    if (gx_st == "1000.000000") {
      gx_st = "0.000000";
      gy_st = "0.000000";
      q = gx_st + "," + gy_st + "," + last_fix;
    }
  }

  return q;




}

static void smartDelay(unsigned long ms)
{
  //gpsEXT.listen();

  //delay(10);
  unsigned long start = millis();
  do
  {
    while (gpsEXT.available())
      Tgps.encode(gpsEXT.read());
  } while (millis() - start < ms);


}

String leeAT(const char* cm) {
  if (gsmon <= 0) {
    confmodem();
  }
  gprs.AT(cm);
  return clean(gprs.buffer);
}

byte qry_AT(const char* qcmd, unsigned int timeout, const char* expected) {
  //if (gsmon <= 0) {confmodem();}
  return gprs.qryAT(qcmd, timeout, expected);
  //return clean(gprs.buffer);
}

String leeGSM() {
  if (gsmon <= 0) {
    confmodem();
  }
  GSM_LOCATION loc;
  q = F("0,0,0,0");
  if (gprs.getLocation(&loc)) {
    q = gprs.buffer;
    q = clean(q);

    //+CIPGSMLOC: 0,-79.367783,43.652210,2017/03/01,01:26:01
    String f_gsm = explotar(q, ',', 2); //FECHA GSM
    String ano = explotar(f_gsm, '-', 0); ano.replace("-", "");
    String mes = explotar(f_gsm, '-', 1); mes.replace("-", "");
    String dia = explotar(f_gsm, '-', 2); dia.replace("-", "");

    console.print(ano); console.print("/"); console.print(mes); console.print("/"); console.println(dia);


    String t_gsm = explotar(q, ',', 3); //HORA GSM
    String hor = explotar(t_gsm, ':', 0);
    String mi = explotar(t_gsm, ':', 1);
    String se = explotar(t_gsm, ':', 2);

    console.print(hor); console.print(":"); console.print(mi); console.print(":"); console.println(se);


    int year;
    byte month, day, hour, minute, second, hundredths;





    //rtc.adjust(DateTime(ano.toInt() - 2000, mes.toInt(), dia.toInt(), hor.toInt(), mi.toInt(), se.toInt()));
    rtc.adjust(DateTime(year, month, day, hour, minute, second));

    return q;
  } else {
    delay(100);
    return q;
  }
}


String leeGSMbat() {
  if (gsmon <= 0) {
    confmodem();
    delay(1000);
  }
  q = "0";
  if (gprs.getOperatorName()) {
    q = gprs.buffer;
    return q;
  } else {
    return "-1";
  }
}

String sendToServer() {
  q = "0";


  int watchbit = 0;
  if (gsmon <= 0) {
    confmodem();
  }

  clean(qry);

  //////////////////////////////////////////////////////////ENVIA
  gprs.httpConnect(url, qry);
  //console.print("IS CONECTED?");console.println(gprs.httpState);
  while (gprs.httpIsConnected() == 0) {
    // can do something here while waiting
    for (byte n = 0; n < 25 && !gprs.available(); n++) {
      delay(10);
      //console.println("IS CONECTED?");
    }
  }
  watchbit = 0;
  //console.print("ERROR?");console.println(gprs.httpState);
  if (gprs.httpState == HTTP_ERROR) {
    delay(100);
    gsmon = 0;
    console.println("ERROR?");
    watchbit++;
    if (watchbit >= 50) {
      //softreset();
      return "-1";
    }
    return "-1";
  }
  watchbit = 0;

  //console.print("READING?");console.println(gprs.httpState);
  gprs.httpRead();
  watchbit = 0;

  int ret;
  while ((ret = gprs.httpIsRead()) == 0) {
    // can do something here while waiting
    delay(100);
    watchbit++;
    console.print("READING?"); console.println(watchbit);
    if (watchbit >= 50) {
      //softreset();
      return "-1";
    }

  }
  watchbit = 0;

  //console.print("READED?");console.println(gprs.httpState);
  if (gprs.httpState == HTTP_ERROR) {
    delay(3000);
    gsmon = 0;
    console.println("READED?");
    return q;
  }
  watchbit = 0;

  //////////////////////////////////////////////////////////RECIBE
  console.println("DONE");//console.println("");
  q = gprs.buffer;
  return q;



}
void confmodem() {

  //if (gsmon <= 0){gprs.sleep(false);}
  console.println("CONF");
  gsmon = 0;
  gprs.inisimserial();
  for (;;) {

    while (!gprs.init()) {
    }
    console.println("APN");
    //gprs.inisimserial();
    byte ret = gprs.setup(APN);
    console.print("Regreso->"); console.println(ret);
    if (ret == 0)
      break;

    malinicio++;

    console.print("malinicio "); console.println(malinicio);

    if (malinicio >= 2) {
      malinicio = 0;
      savingMode(180);
      dormir(180);
      digitalWrite(gpsonpin, LOW);
      gprs.sleep(true);
      delay(300);
      digitalWrite(gpsonpin, HIGH);
      gprs.sleep(false);

    }
  }


  for (;;) {
    if (gprs.httpInit()) break;
    console.println(gprs.buffer);
    gprs.httpUninit();
    delay(100);
    malinicio++;
    if (malinicio >= 3 ) {
      malinicio = 0;
      disp("MAL:", String(malinicio), 1);
      delay(1000);
      console.print("MAL:");
      console.println(malinicio);
      gprs.sleep(true);
      confmodem();
    }
  }
  gsmon = 1;

}





String explotar(String data, char separator, int index)
{
  int maxIndex = data.length() - 1;
  int j = 0;
  String chunkVal = "";
  data.reserve(120);
  chunkVal.reserve(25);

  for (int i = 0; i <= maxIndex && j <= index; i++)
  {

    chunkVal.concat(data[i]);
    if (data[i] == separator)
    {
      j++;
      if (j > index)
      {

        chunkVal.trim();
        chunkVal.replace(",", "");
        chunkVal.replace(":", "");
        chunkVal.replace("/", "");

        //console.print(F("+"));
        //console.print(chunkVal);
        //console.println(F("+"));
        return chunkVal;
      }
      chunkVal = "";
    }
  }
}



void dormir(int k) {
  console.begin(57600);
  console.print(" "); console.println(String(k));
  digitalWrite(led0, LOW);

  for (int m = 1; m <= k; m++) {
    //String va = m + " / " + k;
    console.print("z: "); console.println(String(k - m));
    disp("idle: ", String(k - m), 1);

    //LowPower.idle(SLEEP_1S, ADC_ON, TIMER2_ON, TIMER1_ON, TIMER0_ON, SPI_ON, USART0_ON, TWI_ON);
    //LowPower.powerSave(SLEEP_1S, ADC_ON, BOD_ON,TIMER2_ON);

    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); ///ESTA ESTA BIEN

    //delay(1000);

    if (movido == 0) {
      leerball(); /////////////////////////////////////////REVISAR, FUNCIONA DEPENDIENDO DEL ESTADO, SIEMPRE h O l
    }
    if (semovio >= 1 && movido == 0) {
      movido = 1;
      return;
    }
    //delay(1000);
    //reloj();

  }
}



void turngps(int on) {



  //if (on != 0){digitalWrite(gpsonpin,HIGH);gpsEXT.begin(115200);delay(1000);gpsEXT.println("$PMTK251,9600*17\r\n");delay(1000);gpsEXT.println("$PMTK702,1,1,9600*14\r\n");delay(1000);gpsEXT.println("$PMTK250,1,3,9600*14\r\n");delay(1000);console.begin(9600);delay(1000);}

  if (on != 0 && gpsSTS == 0) {
    gpsEXT.println("$PMTK101*32\r\n");
    delay(1000);
  }
  if (on == 1) {
    digitalWrite(gpsonpin, LOW);  //AUTO Allwaysloc
    gpsSTS = 1;
    gpsEXT.println("$PMTK225,0*2B\r\n");
  }
  if (on == 2) {
    digitalWrite(gpsonpin, LOW);  //AUTO Allwaysloc
    gpsSTS = 1;
    gpsEXT.println("$PMTK225,8*23\r\n");
  }
  if (on == 3) {
    digitalWrite(gpsonpin, LOW);  //AUTO Allwaysloc
    gpsSTS = 1;
    gpsEXT.println("$PMTK225,8*23\r\n");
    delay(1000);
  }
  if (on == 0) {
    digitalWrite(gpsonpin, LOW);
    gpsSTS = 0;
    delay(10);
    gpsEXT.println("$PMTK161,0*28\r\n");
    delay(1000);
  }

  //if (on == 1){digitalWrite(gpsonpin,HIGH);gpsSTS=1;gpsEXT.println("$PMTK101*32\r\n");delay(1000);gpsEXT.println("$PMTK225,9*22\r\n");}//HOT START
  //if (on == 2){digitalWrite(gpsonpin,HIGH);gpsSTS=1;gpsEXT.println("$PMTK102*31\r\n");delay(1000);gpsEXT.println("$PMTK225,9*22\r\n");}//WARM START
  //if (on == 3){digitalWrite(gpsonpin,HIGH);gpsSTS=1;gpsEXT.println("$PMTK103*30\r\n");delay(1000);gpsEXT.println("$PMTK225,9*22\r\n");}//COLD START
  //if (on == 0){digitalWrite(gpsonpin,LOW);gpsSTS=0;delay(10);gpsEXT.println("$PMTK161,0*28\r\n");delay(1000);}

  /*
    if (on != 0){gpsEXT.println("$PMTK161,1*28\r\n");delay(100);gpsEXT.println("$PMTK220,200*2C\r\n");}
    if (on == 0){
        gpsSTS=0;
        //gpsEXT.begin(9600);
        gpsEXT.read();
        delay(1000);
        if (gpsEXT.available()){
          gpsEXT.println("$PMTK161,0*28\r\n");
        }
      }
  */







}

String floatToStr(float floatnumber, int decimals) {

  double floatVal = floatnumber;
  char charVal[15];
  String stringVal = "";

  dtostrf(floatnumber, 2, decimals, charVal);  //4 is mininum width, 2 is precision; float value is copied onto charVal
  delay(10);

  return String(charVal);
}

String clean(String s) {

  //clean from GSM localization
  s.replace(F("\r\n"), "");
  s.replace(F("\r"), "");
  s.replace(F("\n"), "");
  s.replace(F("OK"), "");
  s.replace(F("+CIPGSMLOC: 0,"), "");
  s.replace(F("+CIPGSMLOC: 0,"), "");
  s.replace(F("AT+CIPGSMLOC=1,1"), "");
  s.replace("/", "-");
  s.replace("UNDER-VOLTAGE WARNNING", "");
  s.replace("OVER-VOLTAGE POWER DOWN", "");




  //clean GSM TEMP
  s.replace(F("+CMTE: 0,"), "");

  //clean GSM IMEI


  return s;
}

String print_date(TinyGPS &Tgps)
{
  String fecha = "";
  String hora = "";

  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  Tgps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  //if (age == TinyGPS::GPS_INVALID_AGE)
  //console.print("********** ******** ");
  //else
  {
    char sz[12];
    sprintf(sz, "%02d-%02d-%02d ", year, month, day );
    fecha = String(sz);
    fecha = clean(fecha);

    sprintf(sz, "%02d:%02d:%02d ", hour, minute, second);
    hora = String(sz);
    hora = clean(hora);


    // January 21, 2014 at 3am you would call:
    rtc.adjust(DateTime(year, month, day, hour, minute, second));


    //sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ", month, day, year, hour, minute, second);
    //console.print(sz);
  }

  //smartDelay(0);
  return fecha + "," + hora;
}

/*
  String printDateTime(TinyGPSDate &d, TinyGPSTime &t)
  {
                String fecha = "";
                String hora = "";

                if (!d.isValid())
                {
                  //console.print(F("********** "));
                }
                else
                {
                  char sz[12];
                  sprintf(sz, "%02d-%02d-%02d ", d.year(),d.month(), d.day() );
                  fecha = String(sz);
                  fecha = clean(fecha);
                  //console.print(sz);
                }

                if (!t.isValid())
                {
                  //console.print(F("******** "));
                }
                else
                {
                  char sz[22];
                  sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
                  hora = String(sz);
                  hora = clean(hora);
                  //console.print(sz);
                }

        return fecha + "," + hora;

  }
*/

String print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  console.print(sz);
  return String(sz);
  smartDelay(0);
}

String printFloat(float val, bool valid, int len, int prec)
{


  if (!valid)
  {
    while (len-- > 1)
      console.print('*');
    console.print(' ');
  }
  else
  {
    //console.println(val, prec);
    return String(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i = flen; i < len; ++i)
      console.print(' ');

  }
  smartDelay(0);

  return "-1";


}


void confdisp() {
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  //delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  display.stopscroll();
  //display.startscrollleft(0x00, 0x09);
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void disp(String q, String w, int linea) {

  confdisp();
  display.clearDisplay();

  if (linea == 1) {
    display.setCursor(0, 0);
  }
  if (linea == 2) {
    display.setCursor(6, 9);
  }
  //display.setCursor(0,0);
  //display.print(r);
  display.print(F(" "));
  display.print(q);
  display.print(w);
  display.display();
  //fondo();


}
void scani2c() {

  byte error, address;
  int nDevices;

  console.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      console.print("I2C device found at address 0x");
      if (address < 16)
        console.print("0");
      console.print(address, HEX);
      console.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      console.print("Unknow error at address 0x");
      if (address < 16)
        console.print("0");
      console.println(address, HEX);
    }
  }
  if (nDevices == 0)
    console.println("No I2C devices found\n");
  else
    console.println("done\n");

  //delay(5000);           // wait 5 seconds for next scan


}

String reloj() {

  DateTime now = rtc.now();

  char buffer1 [25] = "";

  sprintf(buffer1, "%04d/%02d/%02d\n %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  disp(buffer1, "", 1);


  unsigned long unixxtime  = now.unixtime();
  console.print("UNIX: "); console.println(now.unixtime());


  console.print(now.year(), DEC);
  console.print('/');
  console.print(now.month(), DEC);
  console.print('/');
  console.print(now.day(), DEC);
  console.print(" (");
  console.print(daysOfTheWeek[now.dayOfTheWeek()]);
  console.print(") ");
  console.print(now.hour(), DEC);
  console.print(':');
  console.print(now.minute(), DEC);
  console.print(':');
  console.print(now.second(), DEC);
  console.println();
  char buf[11];
  sprintf (buf, "%lu", unixxtime);
  return String(buf);
  /*
    console.print(" since midnight 1/1/1970 = ");
    console.print(now.unixtime());
    console.print("s = ");
    console.print(now.unixtime() / 86400L);
    console.println("d");

    // calculate a date which is 7 days and 30 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));

    console.print(" now + 7d + 30s: ");
    console.print(future.year(), DEC);
    console.print('/');
    console.print(future.month(), DEC);
    console.print('/');
    console.print(future.day(), DEC);
    console.print(' ');
    console.print(future.hour(), DEC);
    console.print(':');
    console.print(future.minute(), DEC);
    console.print(':');
    console.print(future.second(), DEC);
    console.println();

    console.println();
    delay(3000);
  */
  delay(20);

}


String readOnewire()
{

  //return spd_str + "," + last_fix +  ",0";
  return "0,0,0";
}

String floatToStr(float floatnumber) {

  double floatVal = floatnumber;
  char charVal[10];
  String stringVal = "";

  dtostrf(floatnumber, 4, 2, charVal);  //4 is mininum width, 2 is precision; float value is copied onto charVal
  delay(10);

  return String(charVal);
}


long makeTime(int hr, int min, int sec, int dy, int mnth, int yr) {
  // assemble time elements into time_t
  // note year argument is offset from 1970 (see macros in time.h to convert to other formats)
  // previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9

  // year can be given as full four digit year or two digts (2010 or 10 for 2010);
  //it is converted to years since 1970
  if ( yr > 99)
    yr = yr - 1970;
  else
    yr += 30;

  int i;
  uint32_t seconds;

  // seconds from 1970 till 1 jan 00:00:00 of the given year
  seconds = yr * (SECS_PER_DAY * 365);
  for (i = 0; i < yr; i++) {
    if (LEAP_YEAR(i)) {
      seconds +=  SECS_PER_DAY;   // add extra days for leap years
    }
  }

  // add days for this year, months start from 1
  for (i = 1; i < mnth; i++) {
    if ( (i == 2) && LEAP_YEAR(yr)) {
      seconds += SECS_PER_DAY * 29;
    } else {
      seconds += SECS_PER_DAY * monthDays[i - 1]; //monthDay array starts from 0
    }
  }
  seconds += (dy - 1) * SECS_PER_DAY;
  seconds += hr * SECS_PER_HOUR;
  seconds += min * SECS_PER_MIN;
  seconds += sec;
  return (long)seconds;
}

void softreset() // Restarts program from beginning but does not reset the peripherals and registers
{
  console.print("RESET"); console.println("");
  asm volatile ("  jmp 0");
}


// calculate the checksum:
String checkSum(String theseChars) {
  char check = 0;
  // iterate over the string, XOR each byte with the total sum:
  for (int c = 0; c < theseChars.length(); c++) {
    check = char(check ^ theseChars.charAt(c));
  }
  // return the result
  String temp = String(check, HEX);
  temp.toUpperCase();
  return temp;
}
