#include <Arduino.h>
#include "funciones.h"
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

#include "SIM800.h"
#include "LowPower.h"

#include <TinyGPS.h>
TinyGPS TNYgps;


  
SoftwareSerial con(1, 2);
#define GPSe Serial
#define modem Serial1


CGPRS_SIM800 SIMgprs;

//-----------PINES-------------
const uint8_t reg_e_pin = 30;
const uint8_t neo_int_pin = 24;
const uint8_t neo_ext_pin = 21;
const uint8_t led0_pin = 0; //LED0
const uint8_t gps_e_pin = 14; //GPS ENABLE PIN
const uint8_t gps_pps_pin = 29; //GPS PPS

//-----------VARIABLES-------------
//int GPS_baud = 4800;
int GPS_baud = 9600;
bool S_REG;
bool S_GSM;
bool S_GPS;
String C_GPS = "0"; //COMMAND GPS

String R_GPS,R_BAT,R_IP,R_IMEI;

String R_GPS_TS,R_GPS_LAT,R_GPS_LON,R_GPS_LASTFIX,R_GPS_SAT,R_GPS_ALT,R_GPS_SPEED;
String R_GSM_TS,R_GSM_LAT,R_GSM_LON;
String URL = "gps2.turniot.com/php/din.php?";
String DEV_NAME = "GPS2";
String D_OUT = "";
String RAZON = "I";


long int R_RUNTIME;
int T_CONF = 10;
int T_STATIC = 0;
int T_DIN = 0;
int T_GPS = 0;
int T_LAST = 0;
int RETURN_TO_STATIC_COUNT = 0;
int SEND_TRY = 2;// INTENTOS ANTE FALLOS DE CONEXION
int T_FAIL_TRY = 300;// SEG ESPERA ANTE ENTRE INTENTO DE CONEXION
int T_AGE_TOLERANCE = 1000; //MILI SEC TOLERACIA DE AGE
int SPEED_TRIGGER = 1000; // SPEED TRIGGER T_DIN

fclass::fclass(){
  }


void fclass::UPDATE() {
  RAZON.reserve(25);
  con.begin(57600);
  con.println("UPDATE: ");
  //R_RUNTIME = CORE_RUNTIME();
  //con.print("R_RUNTIME: ");con.print(R_RUNTIME);
  //con.print(" T_LAST: ");con.println(T_LAST);

  if (true){
  //if ( (R_RUNTIME - T_LAST) >= T_STATIC){ //ENVIAR

    con.println("============= UPDATE: ======================================================="); 
    //T_LAST = CORE_RUNTIME();

    /*
    S_GPS = STATUS_GPS();
    S_GSM = STATUS_GSM();
    S_REG = STATUS_REG();
    
    if (S_GPS == true){R_GPS = GPS_READ(T_GPS);}else{R_GPS = "-1";}
    if (S_GSM == true){R_BAT = GSM_BAT();R_IP = GSM_IP();R_IMEI = leeAT("AT+GSN");}else {R_BAT = "-1";R_IP = "-1";R_IMEI = "-1";}
    
    
    con.print("S_GPS: ");con.println(S_GPS);
    con.print("S_GSM: ");con.println(S_GSM);
    con.print("S_REG: ");con.println(S_REG);
    con.print("R_GPS: ");con.println(clean(R_GPS));
    con.print("R_BAT: ");con.println(R_BAT);
    con.print("R_IP: ");con.println(clean(R_IP));
    con.print("R_RUNTIME: ");con.println(R_RUNTIME);
    con.print("T_LAST: ");con.println(T_LAST);
    con.print("T_CONF: ");con.println(T_CONF);
    con.print("T_STATIC: ");con.println(T_STATIC);
    con.print("T_DIN: ");con.println(T_DIN);
    con.print("T_GPS: ");con.println(T_GPS);
    con.print("C_GPS: ");con.println(C_GPS);
    */
    
    con.println("=============TO DO=======");

      R_GPS_TS=R_GPS_LAT=R_GPS_LON= R_GPS_LASTFIX= R_GPS_SAT= R_GPS_ALT=R_GPS_SPEED=R_GPS_SPEED="-1";
  
      con.println("ENVIAR:");
      //S_GPS = STATUS_GPS(1);
      S_GSM = STATUS_GSM();

    con.print("S_GPS: ");con.println(S_GPS);
    con.print("S_GSM: ");con.println(S_GSM);
    con.print("C_GPS: ");con.println(C_GPS);
      if (((S_GPS) == 0) && (C_GPS == "1"))
      {
        REG_ON();delay(200);GPS_ON();R_GPS = GPS_READ(T_GPS);
       }else{
              if (((S_GPS) == 1) && (C_GPS == "1")){R_GPS = GPS_READ(T_GPS);}
            }

      
    
      if (S_GSM == false){GSM_ON_FULL();delay(500);R_IP = GSM_IP();R_IMEI = leeAT("AT+GSN");R_BAT = GSM_BAT();}//Enciende modem
      if (S_GSM == true){delay(500);R_IP = GSM_IP();R_IMEI = leeAT("AT+GSN");R_BAT = GSM_BAT();}
  


      
            D_OUT = "DEV=" + DEV_NAME + "&"
            "BAT=" + R_BAT + "&"
            "IMEI=" + R_IMEI + "&"
            "RAZON=" + String(RAZON);


      con.print("R_GPS: ");con.println(R_GPS);
      
      if (R_GPS == "0" || R_GPS == "") { //IF GPS IS NOT VALID
          REG_ON();
          GSM_LOC();
          D_OUT = D_OUT + "&"
          "R_GSM_LAT=" + R_GSM_LAT + "&"
          "R_GSM_LON=" + R_GSM_LON + "&"
          "R_GSM_TS=" + R_GSM_TS;
        } 
        
      if (R_GPS == "1") { //IF GPS IS  VALID
            D_OUT = D_OUT + "&"
            "R_GPS_TS=" + R_GPS_TS + "&"
            "R_GPS_LAT=" + R_GPS_LAT + "&"
            "R_GPS_LON=" + R_GPS_LON + "&"
            "R_GPS_LASTFIX=" + R_GPS_LASTFIX + "&"
            "R_GPS_SAT=" + R_GPS_SAT + "&"
            "R_GPS_ALT=" + R_GPS_ALT +  "&"
            "R_GPS_SPEED=" + R_GPS_SPEED;
          
        }

      
            
      clean(D_OUT);     
      //con.print(URL);
      //con.println(D_OUT);

      for (int i = 1; i <= SEND_TRY; i++){
        if (GSM_SEND(URL,D_OUT) == true){i = SEND_TRY;break;}
        if (GSM_SEND(URL,D_OUT) == true){i = SEND_TRY;}else{con.print("NOK ");con.println(i);GSM_SLEEP();REG_OFF();CORE_SLEEP(T_FAIL_TRY,"F");}
      }
      
      
      if (C_GPS == "0"){REG_OFF();}

      if (R_GPS_SPEED.toInt() >= SPEED_TRIGGER){// SI SE ESTA MOVIENDO
          RETURN_TO_STATIC_COUNT = 0;
          if (T_DIN > 60 ){REG_OFF();GSM_SLEEP();}
          CORE_SLEEP(T_DIN,"D" + String(RETURN_TO_STATIC_COUNT));
          RAZON = "D" + String(RETURN_TO_STATIC_COUNT);
        }
        
      if (R_GPS_SPEED.toInt() < SPEED_TRIGGER){// NO SE ESTA MOVIENDO
        if (RETURN_TO_STATIC_COUNT <= 10){
            RETURN_TO_STATIC_COUNT++;
            if (T_DIN > 60 ){REG_OFF();GSM_SLEEP();}
            CORE_SLEEP(T_DIN,"D_2_E:" + String(RETURN_TO_STATIC_COUNT));
            RAZON = "D_2_E" + String(RETURN_TO_STATIC_COUNT);
            return;
          }

          
          if (T_STATIC > 60 ){REG_OFF();GSM_SLEEP();}
          
          CORE_SLEEP(T_STATIC,"S");
          RAZON = "S";
        
        }
      
          
  }

  con.println("=============END=========");

  
}

bool fclass::GSM_LOC() {
  
  con.begin(57600);
  con.println("GSM_LOC");
  SIMgprs.inisimserial();
  String q = "";
  q = F("0,0,0,0");
  GSM_LOCATION loc;
  if (SIMgprs.getLocation(&loc)) {
    q = SIMgprs.buffer;
    q = clean(q);

    //+CIPGSMLOC: 0,-79.367783,43.652210,2017/03/01,01:26:01
    
    R_GSM_LAT = explotar(q, ',', 1); 
    R_GSM_LON = explotar(q, ',', 0); 

    
    String f_gsm = explotar(q, ',', 2); //FECHA GSM
    String ano = explotar(f_gsm, '-', 0); ano.replace("-", "");
    String mes = explotar(f_gsm, '-', 1); mes.replace("-", "");
    String dia = explotar(f_gsm, '-', 2); dia.replace("-", "");

    
    
    //console.print(ano); console.print("/"); console.print(mes); console.print("/"); console.println(dia);


    String t_gsm = explotar(q, ',', 3); //HORA GSM
    String hor = explotar(t_gsm, ':', 0);
    String mi = explotar(t_gsm, ':', 1);
    String se = explotar(t_gsm, ':', 2);

    //console.print(hor); console.print(":"); console.print(mi); console.print(":"); console.println(se);

    R_GSM_TS = ano + "-" + mes + "-" + dia + " " + hor + ":" + mi + ":" + se;
    
    int year;
    byte month, day, hour, minute, second, hundredths;

    //rtc.adjust(DateTime(ano.toInt() - 2000, mes.toInt(), dia.toInt(), hor.toInt(), mi.toInt(), se.toInt()));
    //rtc.adjust(DateTime(year, month, day, hour, minute, second));

    return true;
  } else {
    delay(100);
    return false;
  }
}

bool fclass::GSM_SEND(String url,String qry) {

  con.print("URL: ");con.println(url);
  con.print("D_OUT: ");con.println(qry);
  
  SIMgprs.inisimserial();
  int watchbit = 0;
  
  if (S_GSM == false) {GSM_ON_FULL();}
  SIMgprs.inisimserial();
  if (GSM_CONNECT() == false){con.println("ERR CONN");GSM_SLEEP();return false;} 
  clean(qry);

  //////////////////////////////////////////////////////////ENVIA
            SIMgprs.httpConnect(url, qry);
            //console.print("IS CONECTED?");console.println(gprs.httpState);
            while (SIMgprs.httpIsConnected() == 0) {
              // can do something here while waiting
              for (byte n = 0; n < 25 && !SIMgprs.available(); n++) {
                delay(10);
                //console.println("IS CONECTED?");
              }
            }
            watchbit = 0;
         SIMgprs.inisimserial();
            //console.print("ERROR?");console.println(gprs.httpState);
            if (SIMgprs.httpState == HTTP_ERROR) {
              delay(100);
              
              con.println("ERROR DESC");
              GSM_SLEEP();
              watchbit++;
              if (watchbit >= 3) {
                //softreset();
                return false;
              }
              return false;
            }
            watchbit = 0;
          
            //console.print("READING?");console.println(gprs.httpState);
            SIMgprs.inisimserial();
            SIMgprs.httpRead();
            watchbit = 0;
          
            int ret;
            while ((ret = SIMgprs.httpIsRead()) == 0) {
              // can do something here while waiting
              delay(100);
              SIMgprs.inisimserial();
              watchbit++;
              con.print("READING?"); con.println(watchbit);
              if (watchbit >= 50) {
                //softreset();
                return false;
              }
              SIMgprs.inisimserial();
            }
            watchbit = 0;
            SIMgprs.inisimserial();
            //console.print("READED?");console.println(gprs.httpState);
            if (SIMgprs.httpState == HTTP_ERROR) {
              delay(3000);
              SIMgprs.inisimserial();
              con.println("READED?");
              //return q;
            }
            watchbit = 0;

  //////////////////////////////////////////////////////////RECIBE
  con.println("DONE");//console.println("");
  SIMgprs.inisimserial();
  String payload = SIMgprs.buffer;
  con.print("PAYLOAD:");con.println(payload);
  /////////////////ANALISIS DE DATOS RECIBIDOS
  if ((payload == "0") || (payload == "")) {
    return false;
  }

  String temp = "";

   
    temp = explotar(payload, ',', 0);
    T_STATIC = temp.toInt();
    
    temp = explotar(payload, ',', 1);
    T_DIN = temp.toInt();

    temp = explotar(payload, ',', 2);
    T_GPS = temp.toInt();

    temp = explotar(payload, ',', 3);
    C_GPS = temp.toInt();

    temp = explotar(payload, ',', 4);
    SEND_TRY = temp.toInt();

    temp = explotar(payload, ',', 5);
    T_FAIL_TRY = temp.toInt();

    temp = explotar(payload, ',', 6);
    T_AGE_TOLERANCE = temp.toInt();

    temp = explotar(payload, ',', 7);
    SPEED_TRIGGER = temp.toInt();

    

return true;

}

void fclass::STATUS() {
  con.begin(57600);
  con.println("STATUS: ");
  con.print("REG:");con.println(S_REG);
  con.print("GPS:");con.println(S_GPS);
  con.print("GSM:");con.println(S_GSM);
}

bool fclass::STATUS_GPS(int to) {
  pinMode(gps_pps_pin, INPUT);
  unsigned long duration;
  for (int t=0; t<=to; t++){
    duration = pulseInLong(gps_pps_pin, LOW, 2000000);
    if (duration != 0){t=to;}
  }
  //con.begin(57600);
  //con.print("DURATION: ");con.println(duration);
  if (duration <= 0) {S_GPS = false;} else {S_GPS = true;}
  return S_GPS;
}

bool fclass::STATUS_GSM() {
  SIMgprs.inisimserial();
  if (SIMgprs.sendCommand("AT","OK","OK", 1000) != 0) {S_GSM = true;}else{S_GSM = false;}
  return S_GSM;
}

bool fclass::STATUS_REG() {
  return S_REG;
}

void fclass::CORE_SLEEP(int k, String razon) {
  con.begin(57600);
  con.print("CORE_SLEEP ");con.print(String(razon));con.println(String(k));
  int ajuste15 = 0;
  for (int m = 1; m <= k; m++) {
    con.print("SLEEPING.");con.print(String(razon));con.print("."); con.println(String(k - m));
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); ///ESTA ESTA BIEN
    ajuste15++;
    if (ajuste15 >= 15){ajuste15 = 0;m++;}
  }
}

long unsigned fclass::CORE_RUNTIME() {
  con.begin(57600);
  //con.print("CORE_RUNTIME: ");
  //con.println(millis());
  long unsigned sec = millis()/1000;
  return sec;
}

void fclass::GSM_ON(){
  con.begin(57600);
  con.print("GSM_ON:");
  SIMgprs.inisimserial();
  if (STATUS_GSM() == true) {return;}
  SIMgprs.inisimserial();
  SIMgprs.only_init();
  //con.println(SIMgprs.only_init());
  
  return;
  }

void fclass::GSM_ON_FULL(){
  con.begin(57600);
  con.print("GSM_ON_FULL:");
  SIMgprs.inisimserial();
  if (STATUS_GSM() == true) {return;}
  SIMgprs.inisimserial();
  SIMgprs.init();
  //con.println(SIMgprs.init());
  
  return;
  }
  
String fclass::GSM_BAT(){
  con.begin(57600);
  con.print("GSM_BAT: ");
  SIMgprs.inisimserial();
  String q;
    if (SIMgprs.getOperatorName()) {
      q = SIMgprs.buffer;
      //con.println(q);
    } else {
      //con.println( "?");
      q = "-1";
    }
  return q;
  }


bool fclass::GSM_CONNECT(){
  con.begin(57600);
  con.println("GSM_APN");
  SIMgprs.inisimserial();
  GSM_ON_FULL();
  if (GSM_CONECTADO() == true){return 1;}
  byte gsm_con = SIMgprs.setup("TM"); //Regresa 0 si logro conexion
  if (gsm_con != 0){return 0;}

  if (SIMgprs.httpInit()) {return 1;} //logra conexion

  return 0; //conexion fallida

  }

  
void fclass::GSM_SLEEP(){
  con.begin(57600);
  con.println("GSM_SLEEP");
  SIMgprs.inisimserial();
  if (STATUS_GSM() == false) {return;}
  SIMgprs.inisimserial();
  SIMgprs.sleep(true);
  
  return;
  }

String fclass::GSM_IP(){
  con.begin(57600);
  //con.print("IP: ");
  SIMgprs.inisimserial();
  String ip = leeAT("AT+SAPBR=2,1");
  //con.println(ip);
  return ip;
  }

bool fclass::GSM_CONECTADO(){
  con.begin(57600);
  //con.print("IP: ");
  SIMgprs.inisimserial();
  String ip = leeAT("AT+SAPBR=2,1");
  //con.println(ip);
  if (ip.substring(0,2) == "0.") {return false;}
  if (ip.indexOf("0.0.0") > 0){return false;}
  return true;
  }
  


void fclass::GPS_LOG_START(){
  con.begin(57600);
  con.println("GPS_LOG_START ");
  GPSe.begin(GPS_baud);
  GPSe.println("$PMTK185,0*22\r\n");
  return;
  }
void fclass::GPS_LOG_STOP(){
  con.begin(57600);
  con.println("GPS_LOG_STOP ");
  GPSe.begin(GPS_baud);
  GPSe.println("$PMTK185,1*23\r\n");
  return;
  }
void fclass::GPS_LOG_STATUS(){
  con.begin(57600);
  con.println("GPS_LOG_STATUS ");
  GPSe.begin(GPS_baud);
  GPSe.println("$PMTK183*38\r\n");
  return;
  }
void fclass::GPS_LOG_READ(){
  con.begin(57600);
  con.println("GPS_LOG_READ ");
  GPSe.begin(GPS_baud);
  GPSe.println("$PMTK622,1*29\r\n");
  return;
  }
void fclass::GPS_LOG_ERASE(){
  con.begin(57600);
  con.println("GPS_LOG_ERASE ");
  GPSe.begin(GPS_baud);
  GPSe.println("$PMTK184,1*22\r\n");
  return;
  }


void fclass::GPS_NMEA(){
  con.begin(57600);
  con.println("SET_NMEA_OUTPUT ");
  GPSe.begin(GPS_baud);
  GPSe.println("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");//NMEA MINIMO
  return;
  }

     
void fclass::GPS_BAUD(){
  con.begin(57600);
  con.println("SET_NMEA_BAUDRATE ");
  GPSe.begin(9600);
  GPSe.println("$PMTK251,4800*14\r\n");
  return;
  }

void fclass::GPS_ENABLE(){
  con.begin(57600);
  con.println("GPS_ENABLE ");
  pinMode(gps_e_pin, OUTPUT);
  digitalWrite(gps_e_pin, HIGH);
  return;
  }

void fclass::GPS_DISABLE(){
  con.begin(57600);
  con.println("GPS_DISABLE ");
  pinMode(gps_e_pin, OUTPUT);
  digitalWrite(gps_e_pin, LOW);
  return;
  }
  
void fclass::GPS_ON(){
  GPS_ENABLE();
  con.begin(57600);
  con.println("GPS_ON ");
  GPSe.begin(GPS_baud);
  GPS_ENABLE();
  delay(100);
  GPSe.println("$PMTK225,0*2B\r\n"); //‘0’=Back to normal mode
  delay(200);
  GPSe.println("$PMTK101*32\r\n"); //HOT START
  delay(200);
  //GPS_PPS();
  //delay(200);
  S_GPS = true;
  return;
  }

void fclass::GPS_PPS(){
  delay(300);
  con.begin(57600);
  con.println("GPS_PPS ");
  GPSe.begin(GPS_baud);
  GPSe.println("$PMTK255,1*2D\r\n"); //Enable PPS
  return;
  }


void fclass::GPS_OFF(){
  con.begin(57600);
  con.println("GPS_OFF PMTK_CMD_STANDBY_MODE");
  GPSe.begin(GPS_baud);
  GPSe.println("$PMTK161,0*28\r\n"); //PMTK_CMD_STANDBY_MODE
  //GPSe.println("$PMTK225,4*2F\r\n"); //‘4’=Perpetual Backup mode
  S_GPS = false;
  return;
  }

bool fclass::GPS_READ(int timeout){


  con.begin(57600);
  con.println("GPS_READING...");

  
  
  float flat, flon;
  String gx_st, gy_st;

  unsigned long age;
  
  String q = F("0,0,0");
  //gx="0.0";
  //gy="0.0";

  //turngps(gpson);

  /*
  for (int i = 0; i <= timeout; i++)
  {
    if (STATUS_GPS(1)){i = timeout;}
    
  }
  */

  if (STATUS_GPS(1) == false){GPS_HOT();}
  
  for (int i = 0; i <= timeout; i++)
  {

  con.begin(57600);
  con.print("GPS_READING:");con.println(i);
    GPSe.begin(GPS_baud);
    
    smartDelay(1000);
    
    TNYgps.f_get_position(&flat, &flon, &age);
    //con.println(flat);
    //con.println(flon);
    gx_st = printFloat(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6); //print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
    gy_st = printFloat(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);

    String last_fix = print_int(age, TinyGPS::GPS_INVALID_AGE, 7);
    //last_fix = last_fix.toInt();
    String tsGPS = print_date(TNYgps);


    String sat_str = print_int(TNYgps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
    String spd_str = printFloat(TNYgps.f_speed_kmph(), TinyGPS::GPS_INVALID_F_SPEED, 6, 2);
    String alt_str = printFloat(TNYgps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 8, 2);


    //q = tsGPS + "," + gx_st + "," + gy_st + "," + last_fix  + "," + sat_str.toInt()  + "," + alt_str   + "," + spd_str;
    if (gx_st == "1000.000000") { gx_st = "0.0"; gy_st = "0.0"; }
    
    R_GPS_TS = tsGPS;
    R_GPS_LAT = gx_st;
    R_GPS_LON = gy_st;
    R_GPS_LASTFIX = last_fix;
    R_GPS_SAT = sat_str.toInt();
    R_GPS_ALT = alt_str;
    R_GPS_SPEED = spd_str;
    


    

    //digitalWrite(led0_pin, !digitalRead(led0_pin));
    

    /*
    if (gpson == 0) {
      i = timeout;
      q = F("0,0,0");
    }
    */

    if (last_fix.toInt() <= T_AGE_TOLERANCE & last_fix.toInt() > 0){i = timeout;return 1;} //  <---FUNCIONA OK


  }
    q.replace("****** ", "0");

  return 0;
  
    con.begin(57600);
    con.print("GPS_LEIDO: ");con.println(q);
    
  //return q;

  
  }




void fclass::GPS_STBY(){
  con.begin(57600);
  con.println("GPS_STAND_BY ");
  GPSe.begin(GPS_baud);
  GPSe.println("$PMTK161,0*28\r\n");
  return;
  }

void fclass::GPS_HOT(){
  con.begin(57600);
  con.println("GPS_HOT_START ");
  GPSe.begin(GPS_baud);
  GPSe.println("$PMTK101*32\r\n"); //HOT START
  delay(200);
  return;
  }
  
void fclass::GPS_COLD(){
  con.begin(57600);
  con.println("GPS_COLD_START ");
  GPSe.begin(GPS_baud);
  GPSe.println("$PMTK103*30\r\n");
  return;
  }
  
void fclass::LED0_ON(){
  con.begin(57600);
  con.println("LED0_ON");
  pinMode(led0_pin, OUTPUT);
  digitalWrite(led0_pin, HIGH);
  return;
  }

void fclass::LED0_OFF(){
  con.begin(57600);
  con.println("LED0_OFF");
  pinMode(led0_pin, OUTPUT);
  digitalWrite(led0_pin, LOW);
  return;
  }
  
void fclass::REG_ON(){
  con.begin(57600);
  con.println("REG_ON");
  pinMode(reg_e_pin, OUTPUT);
  digitalWrite(reg_e_pin, HIGH);
  S_REG = true;
  S_GPS = true;
  delay(50);
  return;
  }

void fclass::REG_OFF(){
  con.begin(57600);
  con.println("REG_OFF");
  pinMode(reg_e_pin, OUTPUT);
  digitalWrite(reg_e_pin, LOW);
  S_REG = false;
  S_GPS = false;
  return ;
  }



void fclass::NEO_INT_SETUP(){
  con.begin(57600);
  con.println("NEO_INT_SETUP");
  Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, neo_int_pin, NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  return;
  }

void fclass::NEO_INT_SET(int r, int g, int b){
  con.begin(57600);
  con.print("NEO_INT:");con.print(r);con.print(",");con.print(g);con.print(",");con.println(b);
  Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, neo_int_pin, NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.setPixelColor(0, r, g, b);
  strip.show();
  return;
  }





//--------------RUTINAS EXTRAS----------------------------------------------





String fclass::print_int(unsigned long val, unsigned long invalid, int len)
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
  con.print(sz);
  return String(sz);
  smartDelay(0);
}

String fclass::print_date(TinyGPS &gps)
{
  String fecha = "";
  String hora = "";

  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  TNYgps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
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
    //rtc.adjust(DateTime(year, month, day, hour, minute, second));


    //sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ", month, day, year, hour, minute, second);
    //console.print(sz);
  }

  //smartDelay(0);
  return fecha + "," + hora;
}

String fclass::printFloat(float val, bool valid, int len, int prec)
{


  if (!valid)
  {
    while (len-- > 1)
      con.print('*');
    con.print(' ');
  }
  else
  {
    //console.println(val, prec);
    return String(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i = flen; i < len; ++i)
      con.print(' ');

  }
  smartDelay(0);

  return "-1";


}


void fclass::smartDelay(unsigned long ms)
{
  
  GPSe.begin(GPS_baud);
  //delay(10);
  unsigned long start = millis();
  do
  {
    while (GPSe.available())
      
      TNYgps.encode(GPSe.read());
      
  } while (millis() - start < ms);

  

}

String fclass::clean(String s) 
{

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
  s.replace("AT+SAPBR=2,1+SAPBR: 1,3,", "");
  s.replace("AT+SAPBR=2,1", "");
  s.replace("+SAPBR: 1,3,", "");
  s.replace(" ", "");
  s.replace("+SAPBR:1,1,", "");




  //clean GSM TEMP
  s.replace(F("+CMTE: 0,"), "");

  //clean GSM IMEI


  return s;
}

String fclass::leeAT(const char* cm) {
  SIMgprs.inisimserial();
  SIMgprs.AT(cm);
  return clean(SIMgprs.buffer);
}



String fclass::explotar(String data, char separator, int index)
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



fclass funciones = fclass();
