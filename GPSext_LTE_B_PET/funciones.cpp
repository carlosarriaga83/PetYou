#include <Arduino.h>
#include "funciones.h"
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <Adafruit_SSD1306.h>
#include <Time.h>

#include "DS1307.h"
DS1307 clock;//define a object of DS1307 class


#include <TimeLib.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance

#include "SIM800.h"
#include "LowPower.h"

#include <TinyGPS.h>
TinyGPS TNYgps;

#include "MPU9250.h"
// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x69);
int status;
  
#include <Wire.h>

#include "RTClib.h"

RTC_DS1307 rtc;

#include <Adafruit_GPS.h>
#define GPSSerial Serial
Adafruit_GPS GPS(&GPSSerial);
#define GPSECHO  false


  
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
const uint8_t analog_in4_pin = 4; //ANALOG PIN INPUT 4

//-----------VARIABLES-------------
//int GPS_baud = 4800;



int GPS_baud = 9600;
//int GPS_baud = 57600;
bool S_REG;
bool S_GSM;
bool S_GPS;
String C_GPS = "0"; //COMMAND GPS
bool K_GPS; //KEEP ACTIVE


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
int FAIL_SEND = 5;// INTENTOS PARA OBTENER IP

long int TS_NOW;

long int T_AGE_TOLERANCE = 900000; //MILI SEC TOLERACIA DE AGE
int SPEED_TRIGGER = 2; // SPEED TRIGGER T_DIN
int COUNTER_DIN = 0;

float VECTOR_OLD, VECTOR_NEW, VECTOR_DELTA;

float  VECTOR_TOLERANCE = 0.4;


float a, ax, ay, az, h, hx, hy, hz;

struct config_t
{
    int LOG;
    
    long int T_STATIC;
    int T_DIN;
    int T_GPS;
    int TRIGGER_SPEED;
    int LOG_FREQ;
    int UPLOAD_FREQ;
    long int LAST;
    long int NEXT;
    long int LAST_LOG_UPLOAD;
    long int NEXT_LOG_UPLOAD;
    int PAUSE;
    
    
} SETTINGS;



//-----------ESTRUCTURAS-------------

struct locus
{

  bool on;
  int percent;
  int records;
  int attempts;
  int cycles;
};

struct locus LOCUS;


  
fclass::fclass(){
  }


void fclass::RUN_SETUP() {

    con.println(" ");
     con.println("------------------------------------------------------------------------------------SETUP");
    con.begin(57600);
    con.println("SETUP: ");





    
    REG_OFF();delay(100);REG_ON();



    
    
       
    RTC_RESET();

    I2C_SCAN();

    
    
    SETTINGS.PAUSE = 0;

    EEPROM_WRITE(1,SETTINGS);

    RAZON = "START"; 

    REG_ON();delay(1000);GPS_BAUD(9600);GPS_SETUP(1); 

    GPS_ON();
    delay(1500);
    GPS_ON();
    delay(1500);

    REG_ON();delay(100);
    //con.println("------------------------------------------------------------------------------------");
  
  }





void fclass::RUN_CONDITIONS() {
  
                                    con.begin(57600);
                                    con.println(" ");
                                    con.println("------------------------------------------------------------------------------------CONDITIONS");
                                  
                                              REG_ON();
                                              
                                              LED0_ON();
                                            
                                              
                                              
                                              EEPROM_READ(1, SETTINGS);
                                            
                                              
                                              long int TS_NOW = RTC_READ();
                                            
                                            
                                              long int DIFF =   SETTINGS.NEXT - TS_NOW;
                                              
                                              con.print("NOW:  ");con.print(UNIX_2_TS(TS_NOW));con.print(" "); con.println(TS_NOW);
                                              con.print("NEXT: ");con.print(UNIX_2_TS(SETTINGS.NEXT)); con.print(" ");con.print(SETTINGS.NEXT); con.print(" "); con.println(DIFF);
                                              
                                              long int DIFF_1 =  SETTINGS.NEXT_LOG_UPLOAD - TS_NOW   ;
                                              con.print("LOG:  ");con.print(UNIX_2_TS(SETTINGS.NEXT_LOG_UPLOAD));con.print(" ");con.print(SETTINGS.NEXT_LOG_UPLOAD); con.print(" "); con.println(DIFF_1);
                                            
                                              
                                    
                                    con.println(" ");
                                    con.println("------------------------------------------------------------------------------------TRIGGERS");
            
                                                //TRIGGER BY TIMEOUT
                                                if (  TS_NOW >= SETTINGS.NEXT  ) {  
                                                  
                                                            RAZON = "TIMEOUT";
                                                            con.print("RAZON:  ");con.println(RAZON);
                                                            
                                                        if (GSM_BEFORE_CHECKLIST() == true ){

                                                            if (  GSM_SEND("GPS2",RAZON) == true ){  
                                                                GSM_AFTER_CHECKLIST();
                                                            }
                                                      }
                                                      
                                                }

                                                //TRIGGER BY VECTOR
                                                IMU_READ();
                                                
                                                if (  VECTOR_DELTA >= VECTOR_TOLERANCE  ) {  
                                                  
                                                            RAZON = "VECTOR";
                                                            con.print("RAZON:  ");con.println(RAZON);
                                                            
                                                        if (GSM_BEFORE_CHECKLIST() == true ){
                                                            

                                                            if (  GSM_SEND("GPS2",RAZON) == true ){  
                                                                GSM_AFTER_CHECKLIST();
                                                            }
                                                      }
                                                      
                                                }
                                  
                                    
                                                //TRIGGER BY    LOG    TIMEOUT
                                                if (  TS_NOW >= SETTINGS.NEXT_LOG_UPLOAD && SETTINGS.LOG == 1  ) {  
                                                      
                                                      GPS_LOG_UPLOAD();
                                                    
                                                }

                                                
                                  
                                  
                                    //SAFETY CHECKS
                                    con.println(" ");
                                    con.println("------------------------------------------------------------------------------------CHECKS");
                                          EEPROM_READ(1, SETTINGS);
                                  
                                          TS_NOW = RTC_READ();
                                          DIFF =   SETTINGS.NEXT - TS_NOW;
                                          
                                          //GPS
                                          if ( DIFF > 30 && COUNTER_DIN  == 0){REG_OFF();LED0_OFF();}
                                  
                                          //PAUSE
                                          if ( DIFF < 15 ){SETTINGS.PAUSE = 0;}
                                          if ( DIFF >= 60 ){SETTINGS.PAUSE = 10;}
                                          EEPROM_WRITE(1, SETTINGS);
                                  
                                          
                                          //MODEM
                                          if (STATUS_GSM() == 1  && DIFF > 60  && COUNTER_DIN  == 0 ){  GSM_PWR(0);     }else{   }
                                  
                                          con.print("COUNTER_DIN: ");con.println(COUNTER_DIN);
                                          
                                    //con.println(" ");
                                    con.println("------------------------------------------------------------------------------------SLEEP");
                                    EEPROM_READ(1, SETTINGS);
                                    CORE_SLEEP(SETTINGS.PAUSE,"Idle");

  

  
}


void fclass::GPS_LOG_UPLOAD(){

        con.println("------------------------------------------------------------------------------------UPLOAD LOG");
          long int TS_NOW = RTC_READ();
        bool ERR = false;

        if (  GSM_ACTIVE() == false ) { ERR = true; con.println("XXXX UPLOAD LOG XXXX"); return;}


        
        GPS_LOG_STOP();
        
        if (ERR == false ){
                if (  GSM_TCP_LOG_SEND() == 1 ){  
                  
                    SETTINGS.NEXT_LOG_UPLOAD = TS_NOW + SETTINGS.UPLOAD_FREQ;
                    EEPROM_WRITE(1, SETTINGS);
                    delay(10000);
                    GPS_LOG_ERASE();
                    GPS_LOG_ERASE();
                    
                }

        }

        EEPROM_READ(1, SETTINGS);

        if (SETTINGS.LOG == 1){  REG_ON(); GPS_LOG_START(SETTINGS.LOG_FREQ);   }else{  GPS_LOG_STOP();}
        
        //con.println("+++++++++++++++++++++++++++++++++++++++++++++++++");
  
}


bool fclass::GSM_AFTER_CHECKLIST(){
  
  con.begin(57600);
 con.println(" ");
    con.println("--------------------------AFTER SEND");
                
               
                
                  EEPROM_READ(1, SETTINGS);
                  REG_ON();REG_ON();
                  I2C_SCAN();
                  delay(50);
                  TS_NOW = RTC_READ();

                  

                  //ACTIONS AFTER READING SETTINGS FROM SERVER
                    
                    //LOG
                    if (SETTINGS.LOG == 1){  REG_ON(); GPS_LOG_START(SETTINGS.LOG_FREQ);   }else{  GPS_LOG_STOP(); }

                    //con.print("-----NEXT?: " );
          
                    //DINAMYC
                      //SE EMPIEZA A MOVER Y ESTABA PARADO
                      if ( R_GPS_SPEED.toInt() >= SETTINGS.TRIGGER_SPEED  && COUNTER_DIN == 0) { 
                          COUNTER_DIN = 5; 
                          con.println("#1");
                          
                          SETTINGS.NEXT = TS_NOW + SETTINGS.T_DIN;
                          //RAZON = "SE EMPIEZA A MOVER Y ESTABA PARADO";
                          
                      }
                      //SE SIGUE MOVIENDO
                      if ( R_GPS_SPEED.toInt() >= SETTINGS.TRIGGER_SPEED  && COUNTER_DIN != 0) { 
                          COUNTER_DIN = 5; 
                          con.println("#2");
                          TS_NOW = RTC_READ();
                          SETTINGS.NEXT = TS_NOW + SETTINGS.T_DIN;
                          RAZON = "SE SIGUE MOVIENDO";
                          
                      }
                      //YA NO SE MUEVE, ANTES SI
                      if ( R_GPS_SPEED.toInt() < SETTINGS.TRIGGER_SPEED  && COUNTER_DIN != 0) { 
                        COUNTER_DIN--; 
                        if ( COUNTER_DIN == 0 ){
                            con.println("#3");
                           
                            SETTINGS.NEXT = TS_NOW + SETTINGS.T_STATIC;
                            //RAZON = "YA NO SE MUEVE, ANTES SI";
                            
                          }
                      }
                      //NUNCA SE HA MOVIDO
                      if ( R_GPS_SPEED.toInt() < SETTINGS.TRIGGER_SPEED  && COUNTER_DIN == 0) { 
                            COUNTER_DIN = 0; 
                            con.print("#4 - ");
                          
                            SETTINGS.NEXT = TS_NOW + SETTINGS.T_STATIC;
                            //RAZON = "NUNCA SE HA MOVIDO";
                            con.print(TS_NOW);con.print(" + ");con.print(SETTINGS.T_STATIC);con.print(" = ");con.println(SETTINGS.NEXT);
                      }

                      //VECTOR DELTA
                      if ( VECTOR_DELTA >= VECTOR_TOLERANCE  && COUNTER_DIN == 0) { 
                            COUNTER_DIN = 0; 
                            con.print("#5 - ");
                           
                            SETTINGS.NEXT = TS_NOW + SETTINGS.T_STATIC;
                            //RAZON = "VECTOR DELTA";
                            con.print(TS_NOW);con.print(" + ");con.print(SETTINGS.T_STATIC);con.print(" = ");con.println(SETTINGS.NEXT);
                      }          

                    
                    EEPROM_WRITE(1, SETTINGS);EEPROM_READ(1, SETTINGS);
                    
                    
                    con.begin(57600);con.print("--------------------------NEXT_TS: " ); con.println(UNIX_2_TS(SETTINGS.NEXT));      
                    
                    
                    //MODEM
                    //if (STATUS_GSM() == 1  && SETTINGS.T_STATIC > 60  && COUNTER_DIN  == 0  ){  GSM_PWR(0);     }else{   }

                    //RAZON
                    if (  COUNTER_DIN  == 0){ RAZON = "T_STA";SETTINGS.PAUSE == 10;}else { RAZON + String(COUNTER_DIN); SETTINGS.PAUSE == 0;}
                    EEPROM_WRITE(1, SETTINGS);EEPROM_READ(1, SETTINGS);
                    
                    //PAUSE
                    //if (RAZON != "START" ){SETTINGS.PAUSE = 10;}else{SETTINGS.PAUSE = 0;}
                    //EEPROM_WRITE(1, SETTINGS);
  
  
  }

bool fclass::GSM_BEFORE_CHECKLIST(){
  
  con.begin(57600);
 con.println(" ");
    con.println("--------------------------BEFORE SEND");

    
        bool ERR = false;

        if (  GSM_ACTIVE() == false  ) { ERR = true; con.println("XXXXXXXXXXXX---------- GSM ACTIVE FAIL ");return 0; }
        
        if (ERR == false){


              int GSM_TS_1 = TS_2_UNIX(GSM_TIME());
              int GSM_TS_2 = TS_2_UNIX(GSM_TIME());

              if (GSM_TS_2 - GSM_TS_1 >= 5){con.println("XXXXXXXXXXXX----------GSM_TIME ");return 0;}//LECTURA DE TIEMPO CONSISTENTE
              
              String GSM_TS = GSM_TIME();
              
              
              //con.begin(57600);con.print("GSM_TS: " ); con.println(GSM_TS);

              if (  RTC_SET(GSM_TS ) == false){con.println("XXXXXXXXXXXX----------  RTC_SET ");return 0;}
              
          }

        if (  GSM_ACTIVE() == false  ) { ERR = true; con.println("XXXXXXXXXXXX---------- STATIC2 ");return 0; }

        return 1;
  
  }


void fclass::IMU_SETUP() {
  
    // serial to display data
  //con.begin(57600);


  
  
 
con.print("IMU_SETUP: ");
while(!con) {}
REG_ON();  

  
  // start communication with IMU 
  con.print(".");
  status = IMU.begin();
  con.print(".");

  while(status < 0) {
    //con.println("IMU initialization unsuccessful");
    //con.println("Check IMU wiring or try cycling power");
    //con.print("Status: ");
    //con.println(status);
    con.print(".");
    status = IMU.begin();

  }

  con.print(" OK ");
  
}

void fclass::IMU_READ() {
  
  con.print("IMU_READ: ");  
  
  IMU_SETUP();
  
  
  
  // read the sensor
  IMU.readSensor();
  // display the data
  /*
  con.print(IMU.getAccelX_mss(),6);
  con.print("\t");
  con.print(IMU.getAccelY_mss(),6);
  con.print("\t");
  con.print(IMU.getAccelZ_mss(),6);
  con.print("\t");
  con.print(IMU.getGyroX_rads(),6);
  con.print("\t");
  con.print(IMU.getGyroY_rads(),6);
  con.print("\t");
  con.print(IMU.getGyroZ_rads(),6);
  con.print("\t");
  con.print(IMU.getMagX_uT(),6);
  con.print("\t");
  con.print(IMU.getMagY_uT(),6);
  con.print("\t");
  con.print(IMU.getMagZ_uT(),6);
  con.print("\t");
  con.println(IMU.getTemperature_C(),6);
  */
  
    ax = IMU.getAccelX_mss();
    ay = IMU.getAccelY_mss();
    az = IMU.getAccelZ_mss();
    #define abs(x) ((x)>0?(x):-(x))
    VECTOR_NEW = abs(ax) + abs(ay) + abs(az);
    con.print("new: ");
    con.print(VECTOR_NEW,3);
    con.print(" old: ");
    con.print(VECTOR_OLD,3);
    con.print(" delta: ");
    VECTOR_DELTA = abs(VECTOR_NEW - VECTOR_OLD);
    con.print( VECTOR_DELTA );

    
    if (VECTOR_DELTA > VECTOR_TOLERANCE) {digitalWrite(led0_pin, HIGH);}else{digitalWrite(led0_pin, LOW);}
  
    VECTOR_OLD = VECTOR_NEW;
    
    con.println("   ....OK ");
}

String fclass::GSM_TIME(){
  
  
              AT_SET("ATE0","OK","ERROR",1000,1);
              AT_SET("AT+CCLK?","OK","ERROR",2000,1);
              String GSM_TS = GSM_BUFFER();//+CCLK: "20/04/08,23:32:56-16"
              //con.print("LEN_TS: ");con.println(GSM_TS.indexOf('-'));
              GSM_TS.remove(GSM_TS.indexOf('-'),3);
              GSM_TS.replace("+CCLK: ","");GSM_TS.replace("\"",""); GSM_TS.replace("/","-");GSM_TS.replace("OK","");GSM_TS.replace("\r","");GSM_TS.replace("\n","");
              return GSM_TS;
  
  
  }




  bool fclass::GSM_TCP_SETUP(){

        byte BYTE_TEMP;
        
        con.begin(57600);
        con.println(" GSM_TCP_SETUP-> ");

        AT_SET("ATE1","OK","ERROR",1000,1);

        AT_SET("AT+CACLOSE=0","OK","ERROR",1000,1);
                    
        AT_SET("AT+CASSLCFG=0,\"SSL\",0","OK","ERROR",1000,1);

        BYTE_TEMP = AT_SET("AT+CAOPEN=0,0,\"TCP\",\"pellu.ddns.net\",2345","OK","ERROR",5000,1);

        AT_SET("AT+SHSTATE?","OK","ERROR",1000,1);

        AT_SET("ATE0","OK","ERROR",1000,1);
                
        AT_SET("AT+CASEND=0,5",">","ERROR",1000,2);

        BYTE_TEMP = AT_SET("START","5","ERROR",2000,1);

        delay(200);

        
        //TRANSPARENT MODE
        
        if (BYTE_TEMP == 1){
            con.println("1");
              //AT_SET("AT+CACLOSE=0","OK","ERROR",1000,3);
             AT_SET("AT+CASWITCH=0,1","CONNECT","ERROR",2000,1);
             //AT_SET("AT+CIPMODE=1","OK","ERROR",2000,1);
        
            return 1;
          }else{
            con.println("0");
            AT_SET("AT+CACLOSE=0","OK","ERROR",1000,3);
            con.println(" ERROR TCP SETUP ");
            return 0;
          }
    }
  
    
  bool fclass::GSM_TCP_LOG_SEND(){
    
               byte BYTE_TEMP;
               
                    con.begin(57600);
                    con.print(" GSM_TCP_LOG_SEND->");

                    BYTE_TEMP = GSM_ACTIVE();
                    if (BYTE_TEMP == 0){   return 0;   }
                                        
                    BYTE_TEMP = GSM_TCP_SETUP();
                    if (BYTE_TEMP == 0){   return 0;   }

                    AT_SET("ATE0","OK","ERROR",1000,2);
                    AT_SET("ATE0","OK","ERROR",1000,2);


                    //REG_ON();delay(1500);GPS_SETUP(0);
                    //if (GPS_baud == 9600){GPS_BAUD(4800);}
                    //GPS_SETUP(0);

                    con.print(" . ");
                    
                    
                     Serial1.begin(57600);

                    GPSe.begin(GPS_baud);
                    GPSe.begin(GPS_baud);

                    //if (GPS_baud == 9600){GPS_BAUD(4800);}
                    
                    delay(2000);
                    GPSe.flush();
                    
                    GPSe.println("$PMTK622,1*29\r\n");

                    int pos;
                    int BYTES_TO_SEND_INT;
                    String BYTES_TO_SEND_STR;
                    int i, COUNT;
                    String QRY_SEND_0;
                    int END;
                    int TIME_OUT_COUNTER = 0;
                    
                    String str;
                    str.reserve(300);
                    //String data;
                    //data.reserve(250);
                    delay(500);




                    while (GPSe.available() > 0) {
                          
                          str = GPSe.readStringUntil('$');
                          
                          //data = str;
                          
                          //con.println(data);
                          pos = str.indexOf("KLOX"); //PMTKLOX
                                    
 
                                    
                          //con.println(str);


                                
                          if (pos >= 0 ) {  //LOG IS VALID
                          //if (pos >= 0 && COUNT < 190 ) {  //LOG IS VALID
                          
                              for (i=0, COUNT=0; str[i]; i++){COUNT += (str[i] == 'F');}
                                     int LEN_INT = str.length();
                                    //LEN_INT = LEN_INT - 0;
                                    
                                                                 
                              if (COUNT < 290 && LEN_INT > 200 ) {  //LOG IS VALID
                            
                                    
                                    //str = str + "@@@@\n";
                                    TIME_OUT_COUNTER = 0;
    
                   
                                    //const char *DATA = str.c_str();
                                   
                                    str.replace("PMTKLOX",DEV_NAME);
                                    //str.replace("AT+CASEND=0,","");
                                    str.replace("\n","");
                                    str.replace("\r","");  
                                 
                                    


                                    Serial1.print(str);
                                    //con.print(str);
                                    
                                    //AT_SET("AT+CASEND=0,"+ LEN_STR,">","ERROR",1000,1); 
                                    //AT_SET(str,LEN_CHR,"ERROR",1,1);
                                    //Serial1.println("AT+CASEND=0,"+ LEN_STR);
                                    //Serial1.readStringUntil('>');
                                    
                                    //Serial1.println(str);
    
                                    LED0_TOGGLE();
                              }


                                /*


                                    BYTES_TO_SEND_INT = str.length();
                                    BYTES_TO_SEND_STR = String(BYTES_TO_SEND_INT);
                                    const char *BYTES_TO_SEND_CHR = BYTES_TO_SEND_STR.c_str();
                                    
                                    //233 bytes
                                    QRY_SEND_0 = "AT+CASEND=0,"+ BYTES_TO_SEND_STR;
                                    
                                    //const char *QRY_SEND = QRY_SEND_0.c_str();
                                    
                                    AT_SET(QRY_SEND_0,">","ERROR",200,1); 

                                    
                                    //if (BYTE_TEMP != 1){  con.println(" ERROR TCP1"); return 0;   }
        
                                    //233
                                    AT_SET(str,BYTES_TO_SEND_CHR,"ERROR",1,1);
                                    //if (BYTE_TEMP != 1){  con.println(" ERROR TCP2"); return 0;   }

                                */
                                    
                                
                                
                                
                                
                          }else{  //  LINE NOT VALID DATA
                                
                                END = str.indexOf("K001,622");
                                if (END >= 0) {con.print("***END***"); break;}  //LOG END
                          
                                delay(5);
                                con.print(".");
                                TIME_OUT_COUNTER++;
                                if ( TIME_OUT_COUNTER >= 2000) {
                                      con.print("***LOG TIMEOUT***");
                                      AT_SET("AT+CACLOSE=0","OK","ERROR",1000,1);con.println("XXX LOG NOK XXX");GPS_BAUD(9600);
                                      
                                      //REG_ON();delay(1000);GPS_SETUP(1);GPS_BAUD(9600);GPS_SETUP(1);
                                      return 0;
                                  }
                            
                            }
                          
                      }

                      con.println(" LOG  OK*****");

                      delay(1000);
                      Serial1.println("+++");
                      delay(1000);

                      AT_SET("AT+CACLOSE=0","OK","ERROR",1000,1);

                      //REG_ON();delay(1000);GPS_SETUP(1);GPS_BAUD(9600);GPS_SETUP(1);

                       return 1; 
          
    }



void fclass::CURRENT_INI() {
      emon1.current(analog_in4_pin, 111.1);             // Current: input pin, calibration.
  }
String fclass::CURRENT_READ() {
      
      double Irms = emon1.calcIrms(1480);  // Calculate Irms only
      con.begin(57600);
      con.print("PWR:");con.println(Irms);
      return String(Irms);
  }


      

void fclass::OLED_INI() {
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

void fclass::OLED_DISP(String label, String val, int linea, bool clean) {
  if (S_REG == 1){
    if(clean == true){OLED_INI();display.clearDisplay();}
    if (linea == 1) { display.setCursor(0, 0); }
    if (linea == 2) { display.setCursor(6, 9); }
    display.print(F(" ")); display.print(label); display.print(val); display.display();
    //fondo();
  }else{
      REG_ON();
        if(clean == true){OLED_INI();display.clearDisplay();}
        if (linea == 1) { display.setCursor(0, 0); }
        if (linea == 2) { display.setCursor(6, 9); }
        display.print(F(" ")); display.print(label); display.print(val); display.display();
        //fondo();
      REG_OFF();
  }
  



  }






bool fclass::GSM_ACTIVE() {
        
        
        
        con.print(" ACTIVATING MODEM:  ");
        byte BYTE_TEMP =  1;
        int T = 0;

        GSM_PWR(1);

        AT_SET("ATE0","OK","ERROR",2000,3);
        
        BYTE_TEMP = AT_SET("AT+CNACT=0,2","OK","ERROR",2000,3);
        delay(200);

        int FAIL_SEND_TMP = FAIL_SEND;
               
          while(T < 20){


                AT_SET("AT+CBATCHK=0","OK","ERROR",1000,1); //OVERVOLTAGE PROTECTION OFF
                
               if (AT_SET("AT+CNACT?","OK","ERROR",1000,1) != 1){return 0;};

                      String IP = GSM_BUFFER();
                      IP.replace("AT+CNACT?","");
                      IP.replace("+CNACT: ","");
                      IP.replace("\"","");
                      IP.replace('\n','*');
                      IP.replace('\r',' ');
                      IP.replace("0,0,","");
                      IP.replace("0,2,","");
                      //con.print("[IP]");con.println(IP);
                      
                      IP = explotar(IP,'*',1);
                      IP.replace(" ","");
                      IP.replace("*","");
                      
      

               
                    int i, COUNT;
    
                    for (i=0, COUNT=0; IP[i]; i++){COUNT += (IP[i] == '.');}

                    con.print("[IP] ");con.print(COUNT);con.print(" ");con.println(IP);
                    
               if (IP != "0.0.0.0" && COUNT == 3) {return 1;}else{AT_SET("AT+CNACT=0,2","OK","ERROR",2000,3);}
                
               
               if (BYTE_TEMP == 0) {              
                      //AT_SET("AT+CNACT=0,0","OK","ERROR",1000,1);
                      //if (STATUS_GSM() == 0){return 0;}
                  } 
                  
               delay(200);
               T++;
               //con.print(".");
               if ( T >= 20){con.println(" XXX ERROR ON ACTIVE  "); FAIL_SEND--; return 0;}
               if ( FAIL_SEND_TMP <= 0 ){con.print(" XXX FAIL SEND"); con.println(FAIL_SEND_TMP);  FAIL_SEND_TMP=FAIL_SEND; return 0;}
            }

         con.println("1 ");

         return 1;

}


            
bool fclass::GSM_SEND(String DEVICE,String RAZON) {




  con.begin(57600);
 
    con.println("------------------------------------------------------------------------------------GSM SEND");
    
int TEMP_INT;

byte BYTE_TEMP;
byte T;

  EEPROM_READ(1, SETTINGS);

  R_GPS_TS=R_GPS_LAT=R_GPS_LON= R_GPS_LASTFIX= R_GPS_SAT= R_GPS_ALT=R_GPS_SPEED=R_GPS_SPEED="-1";

         //if (K_GPS == 0){GPS_HOT();}
         
         GPS_READ(SETTINGS.T_GPS); 

         GPS_OFF();
         
        //BYTE_TEMP = GSM_ACTIVE();
        
        //if (BYTE_TEMP == 0){   return 0;   }


         String BAT = GSM_BAT();
         
         
         String QRY;

         QRY +=  "device=";
         QRY += DEVICE; 
         QRY +=  "&bat=";
         QRY += BAT; 
         QRY +=  "&gps=";
         QRY += R_GPS_LAT; 
         QRY += ","; 
         QRY += R_GPS_LON; 
         QRY +=  "&fechagps=";
         QRY += R_GPS_TS; 
         QRY +=  "&alt=";
         QRY += R_GPS_ALT; 
         QRY +=  "&sat=";
         QRY += R_GPS_SAT; 
         QRY +=  "&speed=";
         QRY += R_GPS_SPEED; 
         QRY +=  "&age=";
         QRY += R_GPS_LASTFIX;
         QRY +=  "&razon=";
         QRY += RAZON; 
                  


         con.println(QRY);
         
         //QRY.toCharArray(buff,250);
         
          EEPROM_READ(1,SETTINGS);

          String PAYLOAD;
          //if (  SETTINGS.T_STATIC < 60   ){  PAYLOAD = GSM_TCP_SEND(QRY);    } 
          //if (  SETTINGS.T_STATIC >= 60 ){  PAYLOAD = GSM_HTTP_SEND(QRY); } 
         PAYLOAD = GSM_HTTP_SEND(QRY); 

                 

  String temp = "";
  
    temp = explotar(PAYLOAD, ',', 1);
    SETTINGS.LOG = temp.toInt();  

    temp = explotar(PAYLOAD, ',', 2);
    SETTINGS.T_STATIC = long(temp.toInt());  
    
    temp = explotar(PAYLOAD, ',', 3);
    SETTINGS.T_DIN = temp.toInt();  

    temp = explotar(PAYLOAD, ',', 4);
    SETTINGS.T_GPS = temp.toInt();  
    
    temp = explotar(PAYLOAD, ',', 5);
    SETTINGS.TRIGGER_SPEED = temp.toInt();
    
    temp = explotar(PAYLOAD, ',', 6);
    SETTINGS.LOG_FREQ = temp.toInt();

    temp = explotar(PAYLOAD, ',', 7);
    SETTINGS.UPLOAD_FREQ =  temp.toInt();


    
    
    EEPROM_WRITE(1, SETTINGS);
    

        //con.println(SETTINGS.LOG);
        //con.println(SETTINGS.T_STATIC);
        //con.println(SETTINGS.T_DIN);
        //con.println(SETTINGS.T_GPS);
        //con.println(SETTINGS.TRIGGER_SPEED);
        //con.println(SETTINGS.LOG_FREQ );
        //con.println(SETTINGS.UPLOAD_FREQ);

        //RUN_CONDITIONS() ;
        
        //CORE_SLEEP(SETTINGS.T_STATIC,"T");

return true;



}


String fclass::GSM_HTTP_SEND(String QRY){

String TEMP_STR0;
byte TEMP_BYTE;
String TO_READ;

                    byte BYTE_TEMP;
               
                    con.begin(57600);
                    con.print(" GSM_HTTP_SEND->");

                    BYTE_TEMP = GSM_ACTIVE();
                    if (BYTE_TEMP == 0){   return "0";   }
                                        

                    AT_SET("ATE0","OK","ERROR",1000,2);
          
          
          QRY = "AT+SHREQ=\"/php/gps/plug.php?" + QRY + "\"" + ",1";
          
          AT_SET("ATE1","OK","ERROR",1000,1);
            
         AT_SET("AT+SHCONF=\"url\",\"http://bus.turniot.com:80\"","OK","ERROR",8000,2);

         AT_SET("AT+SHCONF=\"BODYLEN\",64","OK","ERROR",8000,2);

         AT_SET("AT+SHCONF=\"HEADERLEN\",64","OK","ERROR",8000,2);

         AT_SET("AT+SHCONN","OK","ERROR",8000,2);

         AT_SET("AT+SHSTATE?","1","ERROR",8000,2);

         AT_SET("ATE0","OK","ERROR",8000,1);
        
         //const char *QRY0 = QRY.c_str();
         
         //AT_SET(QRY0,"*","ERROR",5000,1);
         AT_SET(QRY,"OK","ERROR",5000,1);AT_SET("","\n","ERROR",1000,1);AT_SET("","\n","ERROR",1000,1);


         String TEMP_STR = GSM_BUFFER();
       
         con.print("--------------- BUFFER LEIDO: <");con.print(TEMP_STR);con.print(">");

         TEMP_STR = explotar(TEMP_STR,',',4);
         TEMP_STR.replace("*","");
         TEMP_STR.replace(" ","");
         //con.println(TEMP_STR);
         
         
         TO_READ = TEMP_STR;
         
         //TEMP_STR = "AT+SHREAD=0," +  TO_READ;
         TO_READ = "AT+SHREAD=0," +  TO_READ;
                 
         //const char *QRY1 = TEMP_STR.c_str();
                 
         //TEMP_BYTE = AT_SET(QRY1,"}","ERROR",4000,2);
         TEMP_BYTE = AT_SET(TO_READ,"}","ERROR",4000,2);

         if (TEMP_BYTE != 1) {
            con.print("ERROR_READING PL:");con.println(TEMP_BYTE);
            AT_SET("AT+SHDISC","OK","ERROR",8000,1);
            GSM_PWR(0);
            //REG_OFF();
            return "0";
         }

                 TEMP_STR0 = GSM_BUFFER();
                 con.println(TEMP_STR0);

         AT_SET("AT+SHDISC","OK","ERROR",8000,1);

          return TEMP_STR0;
  
}

String fclass::GSM_TCP_SEND(String QRY){
  
        byte BYTE_TEMP;
   
        con.begin(57600);
        con.print(" GSM_TCP_SEND->");

        BYTE_TEMP = GSM_ACTIVE();
        if (BYTE_TEMP == 0){   return "0";   }

        AT_SET("ATE0","OK","ERROR",1000,2);


        AT_SET("ATE1","OK","ERROR",1000,1);

        AT_SET("AT+CACLOSE=0","OK","ERROR",1000,1);
                    
        AT_SET("AT+CASSLCFG=0,\"SSL\",0","OK","ERROR",1000,1);

        BYTE_TEMP = AT_SET("AT+CAOPEN=0,0,\"TCP\",\"pellu.ddns.net\",2345","OK","ERROR",5000,1);

        AT_SET("AT+SHSTATE?","OK","ERROR",1000,1);

        AT_SET("ATE0","OK","ERROR",1000,1);
                
        AT_SET("AT+CASEND=0,5",">","ERROR",1000,2);

        BYTE_TEMP = AT_SET("START","5","ERROR",2000,1);
        
        if (BYTE_TEMP == 0){   return "0";   }

        String LEN = String(QRY.length());
        const char *LEN_CHR = LEN.c_str();
        
        AT_SET("AT+CASEND=0,"+ LEN,">","ERROR",1000,2);
        
        BYTE_TEMP = AT_SET(QRY,LEN_CHR,"ERROR",2000,1);

        AT_SET("AT+CACLOSE=0","OK","ERROR",1000,1);

       String PAYLOAD = GSM_BUFFER();

        
       con.print(" >>>>SERVER: "); con.println(PAYLOAD );

      return PAYLOAD;
        
}

String fclass::UNIX_2_TS(long int UNIX_TS){
  
  
  DateTime T = UNIX_TS;

  char ts_string [25] = "";

  sprintf(  ts_string, "%04d-%02d-%02d %02d:%02d:%02d", T.year(), T.month(), T.day(), T.hour(), T.minute(), T.second()  );

  return ts_string;
  
  
  }



void fclass::STATUS() {
  con.begin(57600);
  con.println("STATUS: ");
  con.print("REG:");con.println(S_REG);
  con.print("GPS:");con.println(S_GPS);
  con.print("GSM:");con.println(S_GSM);
}

bool fclass::STATUS_GPS(unsigned int timeout) {
  con.print(" STATUS_GPS: ");
  GPSe.begin(GPS_baud);
  GPSe.read();
    S_GPS = false;
  
  
  uint32_t t = millis();
  byte n = 0;
  do {
    if (GPSe.available() == 0) {
        
        n++;
    
    }
  } while (millis() - t < timeout);

  if (n >= 5){S_GPS=false;}else{S_GPS=true;}
    con.println(S_GPS);
  return S_GPS;
}

bool fclass::STATUS_GSM() {
  con.print("STATUS_GSM: ");
  SIMgprs.inisimserial();
  if (AT_SET("AT","OK","ERROR", 250,3) != 0) {S_GSM = true;}else{S_GSM = false;}
  con.println(S_GSM);
  return S_GSM;
}

bool fclass::STATUS_REG() {
  return S_REG;
}



void fclass::I2C_BEGIN(){
  
  Wire.begin();  
}

void fclass::I2C_SCAN(){

  

  byte error, address;
  int nDevices;

  con.println("Scanning...");

  Wire.begin(); 

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
      con.print("I2C device found at address 0x");
      if (address < 16)
        con.print("0");
      con.print(address, HEX);
      con.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      con.print("Unknow error at address 0x");
      if (address < 16)
        con.print("0");
      con.println(address, HEX);
    }
  }
  if (nDevices == 0)
    con.println("No I2C devices found\n");
  else
    con.println("done\n");

  //delay(5000);           // wait 5 seconds for next scan



  
}

bool fclass::RTC_RESET() {
  con.begin(57600);
  con.print("RTC_RESET: ");

      clock.begin();
    clock.fillByYMD(2000, 1, 1); //Jan 19,2013
    clock.fillByHMS(00, 00, 00); //15:28 30"
    clock.fillDayOfWeek(SAT);//Saturday
    clock.setTime();//write time to the RTC chip
    

  //rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));
  SETTINGS.NEXT = SETTINGS.NEXT_LOG_UPLOAD = 946684800;
  EEPROM_WRITE(1,SETTINGS);
     //rtc.begin();
}

bool fclass::RTC_START() {

  con.begin(57600);
  con.print("RTC_START: ");
  
  if (! rtc.begin()) {
    con.println("Couldn't find RTC");
    return 0;
    //while (1);
  }


  if (! rtc.isrunning()) {
    con.println("RTC is NOT running!");
    
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2199, 1, 21, 3, 0, 0));

    return 0;
  }

  con.println("1");
  return 1;
}

long int fclass::RTC_READ() {


  
  con.begin(57600);
  con.print("RTC_READ:");
  while(!con) {}
   
   
   REG_ON(); 
   con.print(",");  
   Wire.begin();  
   REG_ON(); 


      //delay(50);
  
    //I2C_BEGIN();
    Wire.begin();  

    //delay(50);

    con.print(",");
      //if (! rtc.isrunning()) {rtc.begin();}
      if (! rtc.isrunning()) {rtc.begin();con.print("(RTC_KICKED)");}

      //con.print(rtc.isrunning());
      
      delay(15);
      
      DateTime now1;
      con.print(".");    
      //clock.getTime();
      con.print(".");  
      now1 = rtc.now();
      con.print(".");    
      unsigned long unixxtime  = now1.unixtime();
      con.print(".");    
      con.println("OK");
  
  return unixxtime;


}


void fclass::CORE_SLEEP(int k, String razon) {
  con.begin(57600);
  con.print("CORE_SLEEP: ");con.print(String(razon));con.print(" FOR: ");con.println(String(k));
  //con.println("CORE_SLEEP ");

  

  for (int m = 1; m <= k; m++) {
    
    
    //con.print(" Z.");con.print(String(razon));con.print("."); con.println(String(k - m));
    
    //LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); ///ESTA ESTA BIEN
    LowPower.powerDown(SLEEP_1S,ADC_OFF, BOD_OFF); ///ESTA ESTA BIEN


    
  }
}

long unsigned fclass::CORE_RUNTIME() {
  con.begin(57600);
  //con.print("CORE_RUNTIME: ");
  //con.println(millis());
  long unsigned sec = millis()/1000;
  return sec;
}

bool fclass::GSM_PWR(int x){
  con.begin(57600);
  con.print("---------------------------------GSM_PWR->");
  con.print(x);
  
  byte S_GSM = STATUS_GSM();
  
  if (x == 0 && S_GSM == true){
        con.println("---------------------------------GSM_SLEEP");
        SIMgprs.sleep(true);
        delay(1000);
        return 1;
    }
  if (x == 0 && S_GSM == false){
        return 1;
    }
  if (x == 1 && S_GSM == true){
        return 1;
    }
  if (x == 1 && S_GSM == false){
    
        
        
        byte TEMP = 0;int ATTEMPTS = 0;
         SIMgprs.GSM_PWRKEY();
        
        while (TEMP == 0){
            REG_ON();
            
            TEMP = AT_SET("","Ready","DISCON",1000,3);
            ATTEMPTS++;
            
            //if (ATTEMPTS >= 2){return 0;}
            if (TEMP == 0) {SIMgprs.GSM_PWRKEY();}
          }
          
          //return 1; 

        
        //byte TEMP = 0;
        TEMP = AT_SET("AT","OK","ERROR",1000,10);

       

        if (TEMP == 1 ){return 1;}
        if (TEMP == 2 ){return 1;}
        if (TEMP == 0 ){return 0;}
        
        //AT_SET("AT+CFUN=1","OK","Ready",1000,5);
        
        //AT_SET("AT+CFUN=1","OK","ERROR",1000,10);
        
                  

                  
                         
                  //AT_SET("AT+CFUN=1","Ready","ERROR",10000,1);

                  
         //AT_SET("AT+CFUN=1","SMS","Ready",10000,10);
                  
         
         AT_SET("ATE1","OK","ERROR",1000,1);

    }

  
  return 1;
  }


  
String fclass::GSM_BAT(){
  con.begin(57600);
  con.print("GSM_BAT? ");
  SIMgprs.inisimserial();

  byte x = AT_SET("AT+CBC","OK","ERROR",1000,2);
  //con.print(x);
  //con.print(" ");
  String temp = GSM_BUFFER(); 
  temp = explotar( temp, ',',1);
  con.print("BAT:");
  con.println(temp);


   return temp;


}




  
void fclass::GSM_SLEEP(){
  con.begin(57600);
  con.println("--------------------------- GSM_SLEEP");
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


  

void fclass::GPS_SETUP(int COMPLETE){
  
  con.begin(57600);
  con.print("GPS_SETUP: ");
  
  
  GPSe.begin(GPS_baud);


    
    GPSe.read();
    GPSe.println("$PMTK185,1*23\r\n");//STOP LOG
    delay(100);
    
    GPSe.read();
    GPSe.println("$PMTK220,1000*1F\r\n");// SET FREQ to 1 Hz
    delay(100);
    
    GPSe.read();
    if ( COMPLETE == 1){GPSe.println("$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");con.println("1");}
    if ( COMPLETE == 0){GPSe.println("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");con.println("0");}
    if ( COMPLETE == 3){GPSe.println("$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");con.println("3");}
    delay(100);
    
    //gpsEXT.println("$PMTK225,8*23\r\n");//ALLWAYS LOCATE BACKUP
    //delay(200);




  
}

void fclass::GPS_LOG_START(int freq){
  K_GPS = 1;
  con.begin(57600);
  con.println("GPS_LOG_START ");
  GPSe.begin(GPS_baud);
  GPSe.read();

  GPSe.println("$PMTK185,0*22\r\n");


      //GPSe.begin(GPS_baud);
      
      String temp = "PMTK187,1,";
      temp += String(freq);
      temp = "$" + temp + "*" + checkSum(temp) + "\r\n";
      GPSe.read();
      GPSe.println(temp);//EVERY X SEC
      
      //gpsEXT.println("$PMTK187,1,5*38\r\n");//LOCUS EVERY 5 SEC
      GPSe.read();
      GPSe.println("$PMTK185,0*22\r\n");//LOCUS START
   return; 
  }

  
void fclass::GPS_LOG_STOP(){
  K_GPS = 0;
  con.begin(57600);
  con.println("GPS_LOG_STOP ");
  GPSe.begin(GPS_baud);
    GPSe.read();
    GPSe.println("$PMTK185,1*23\r\n");//STOP LOG
    delay(200);
  
  return;
  }

  
void fclass::GPS_LOG_STATUS(){
  if (S_REG == false){REG_ON();delay(500);}
  con.begin(57600);
  con.println("GPS_LOG_STATUS ");
  GPSe.begin(GPS_baud);
  GPSe.read();

  GPSe.println("$PMTK183*38\r\n");

    LOCUS.on = false;
    LOCUS.percent = 0;
    LOCUS.records = 0;
    LOCUS.attempts = 0;
    LOCUS.cycles = 0;


          GPS.begin(GPS_baud);
          GPS.read();
          uint32_t updateTime = 4000;
        
          if (millis() > updateTime)
          {
            updateTime = millis() + 1000;
            if (GPS.LOCUS_ReadStatus()) {
              con.print("\n\nLog #");
              con.print(GPS.LOCUS_serial, DEC);
              if (GPS.LOCUS_type == LOCUS_OVERLAP)
                con.print(", Overlap, ");
              else if (GPS.LOCUS_type == LOCUS_FULLSTOP)
                con.print(", Full Stop, Logging");
        
              if (GPS.LOCUS_mode & 0x1) con.print(" AlwaysLocate");
              if (GPS.LOCUS_mode & 0x2) con.print(" FixOnly");
              if (GPS.LOCUS_mode & 0x4) con.print(" Normal");
              if (GPS.LOCUS_mode & 0x8) con.print(" Interval");
              if (GPS.LOCUS_mode & 0x10) con.print(" Distance");
              if (GPS.LOCUS_mode & 0x20) con.print(" Speed");
        
              con.print(", Content "); con.print((int)GPS.LOCUS_config);
              con.print(", Interval "); con.print((int)GPS.LOCUS_interval);
              con.print(" sec, Distance "); con.print((int)GPS.LOCUS_distance);
              con.print(" m, Speed "); con.print((int)GPS.LOCUS_speed);
              con.print(" m/s, Status ");
              if (GPS.LOCUS_status) {
                LOCUS.on = true;
                //locusOn = true;
                con.print("LOGGING, ");
                //con.print(locusOn);
              } else {
                LOCUS.on = false;
                //locusOn = false;
                con.print("OFF, ");
                //con.print(locusOn);
              }
              con.print((int)GPS.LOCUS_records); con.print(" Records, ");
              LOCUS.records = (int)GPS.LOCUS_records;
              con.print((int)GPS.LOCUS_percent); con.print("% Used ");
              //LOCUS.percent = (int)GPS.LOCUS_percent;
        
            }//if (GPS.LOCUS_ReadStatus())
          }//if (millis() > updateTime)


  return;

  
  }

  
void fclass::GPS_LOG_READ(){
  
  con.begin(57600);
  
  
  con.println("GPS_LOG_READ ");
  GPSe.begin(GPS_baud);
  GPSe.read();
  GPSe.println("$PMTK622,1*29\r\n");
 
  return;
  }
void fclass::GPS_LOG_ERASE(){
  con.begin(57600);
  con.println("GPS_LOG_ERASE ");
  GPSe.begin(GPS_baud);
  GPSe.read();
  delay(300);
  GPSe.println("$PMTK184,1*22\r\n");
  return;
  }



void fclass::GPS_NMEA(int mode){
  con.begin(57600);
  con.print(" SET_NMEA_OUTPUT: ");
  GPSe.begin(GPS_baud);
  delay(1500);
  GPSe.flush();
  if (  mode == 1  ) { GPSe.println("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");}//NMEA MINIMO
  if (  mode == 0  ) { GPSe.println("$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");}//NMEA MINIMO
  con.println(mode);
  return;
  }

     
bool fclass::GPS_BAUD(int baud){
  con.begin(57600);
  con.print("SET_GPS_BAUD: ");
  
   
    if (baud == 57600){
 
          GPSe.begin(9600);
          GPSe.begin(9600);
          delay(1500);
          GPSe.flush();
          GPSe.println("$PMTK251,57600*2C\r\n");
          delay(1500);
          GPSe.read();
          GPSe.println("$PMTK251,57600*2C\r\n");


      }
   
    if (baud == 4800){
 
          GPSe.begin(9600);
          GPSe.begin(9600);
          delay(1500);
          GPSe.flush();
          GPSe.println("$PMTK251,4800*14\r\n");
          delay(1500);
          GPSe.read();
          GPSe.println("$PMTK251,4800*14\r\n");


          
          GPSe.begin(4800);  
          GPSe.begin(4800);   
          delay(1500);
          GPSe.flush();
          GPSe.println("$PMTK251,4800*14\r\n");
          delay(1500);
          GPSe.read();
          GPSe.println("$PMTK251,4800*14\r\n");
      }
  
    if (baud == 9600){
          
          GPSe.begin(4800);
          GPSe.begin(4800);  
          delay(1500);
          GPSe.flush();
          GPSe.println("$PMTK251,9600*17\r\n");
          delay(1500);
          GPSe.read();
          GPSe.println("$PMTK251,9600*17\r\n");
          
         
          GPSe.begin(9600);
          GPSe.begin(9600);
          delay(1500);
          GPSe.flush();
          GPSe.println("$PMTK251,9600*17\r\n");
          delay(1500);
          GPSe.read();
          GPSe.println("$PMTK251,9600*17\r\n");          
      }
      
      GPS_baud = baud;

     con.println(GPS_baud);
      con.begin(57600);
      
  
  //GPSe.println("$PMTK251,4800*14\r\n");
  //GPSe.println("$PMTK251,9600*17\r\n");

  
  return 1;
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
  //GPS_ENABLE();
  REG_ON();
  delay(500);
  con.begin(57600);
  con.println("GPS_ON ");
  GPSe.begin(GPS_baud);
  //GPS_ENABLE();
  delay(100);
  GPSe.read();
  GPSe.println("$PMTK225,0*2B\r\n"); //‘0’=Back to normal mode
  delay(100);
  GPSe.read();
  GPSe.println("$PMTK101*32\r\n"); //HOT START
  delay(100);
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
  con.println(" GPS_OFF: PMTK_CMD_STANDBY_MODE");
  GPSe.begin(GPS_baud);
  GPSe.read();
  GPSe.println("$PMTK161,0*28\r\n"); //PMTK_CMD_STANDBY_MODE
  //GPSe.println("$PMTK225,4*2F\r\n"); //‘4’=Perpetual Backup mode
  S_GPS = false;
  return;
  }

bool fclass::GPS_READ(int timeout){


  REG_ON();

  delay(1000);

  con.begin(57600);
  con.println("GPS_READING...");

  //GPS_BAUD(GPS_baud);
  
  float flat, flon;
  String gx_st, gy_st;

  unsigned long age;
  
  String q = F("0,0,0");

  GPSe.begin(GPS_baud);

  //GPS_ON();
  
  for (int i = 0; i <= timeout; i++)
  {

          con.begin(57600);
          con.print(" #");con.println(i);
            //GPSe.begin(GPS_baud);
            
            LED0_TOGGLE();
            
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
            R_GPS_LASTFIX = last_fix.toInt();
            R_GPS_SAT = sat_str.toInt();
            R_GPS_ALT = alt_str;
            R_GPS_SPEED = spd_str.toInt();

            R_GPS_LASTFIX.replace(" ",""); 

            con.print(" "); con.print(R_GPS_LASTFIX); con.print(" "); con.print(T_AGE_TOLERANCE); 
        
            if (last_fix.toInt() <= T_AGE_TOLERANCE & last_fix.toInt() > 0){i = timeout;con.println("");con.println("--------------------GPS_LEIDO");LED0_ON();return 1;} //  <---FUNCIONA OK
            


  }
    q.replace("****** ", "0");

    LED0_ON();

  return 0;
  
    con.begin(57600);
    con.println("GPS_LEIDO: ");con.println(q);
    
  //return q;

  
  }




bool fclass::RTC_SET(String TS){
  

    //TS = 2017-03-01,01:26:01
    REG_ON();
 
    con.begin(57600);
    con.print("RTC_SET (");con.print(TS);con.print(") :");

    if (TS == ""){con.print("TS=NULL");return false;}

    
    int i, COUNT;
    
    for (i=0, COUNT=0; TS[i]; i++){COUNT += (TS[i] == '-');}
    if ( COUNT != 2){con.print(",");return false;}    
    con.print(".");
    for (i=0, COUNT=0; TS[i]; i++){COUNT += (TS[i] == ':');}
    if ( COUNT != 2){con.print(",");return false;}
    con.print(".");

    
    String TS_DATE = explotar(TS, ',', 0); //FECHA
    String ano = explotar(TS_DATE, '-', 0); ano.replace("-", "");
    String mes = explotar(TS_DATE, '-', 1); mes.replace("-", "");
    String dia = explotar(TS_DATE, '-', 2); dia.replace("-", "");

    
    
    //con.print(ano); con.print("/"); con.print(mes); con.print("/"); con.println(dia);


    String TS_TIME = explotar(TS, ',', 3); //HORA GSM
    String hor = explotar(TS_TIME, ':', 0);
    String mi = explotar(TS_TIME, ':', 1);
    String se = explotar(TS_TIME, ':', 2);

    //con.print(hor); con.print(":"); con.print(mi); con.print(":"); con.println(se);

    R_GSM_TS = ano + "-" + mes + "-" + dia + "," + hor + ":" + mi + ":" + se;
    
    
    int year;
    byte month, day, hour, minute, second, hundredths;

    //rtc.adjust(DateTime(ano.toInt() - 2000, mes.toInt(), dia.toInt(), hor.toInt(), mi.toInt(), se.toInt()));

    rtc.adjust(DateTime(ano.toInt() , mes.toInt(), dia.toInt(), hor.toInt(), mi.toInt(), se.toInt()));
    //rtc.adjust(DateTime(year, month, day, hour, minute, second));
    
    con.println(R_GSM_TS);
    delay(20);

      if (! rtc.isrunning()) { rtc.begin();con.print("(RTC_KICKED_0)"); }
    
    return 1;
  }

int fclass::TS_2_UNIX(String TS){

  int Year, Month, Day, Hour, Minute, Second ;

  //TS = 2017-03-01,01:26:01
  

  tmElements_t tm;
  const char *TS_CHR = TS.c_str();
  sscanf(TS_CHR, "%d-%d-%d,%d:%d:%d", &Year, &Month, &Day, &Hour, &Minute, &Second);
  return makeTime(tm);

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
  GPSe.flush();
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

void fclass::LED0_TOGGLE(){
    pinMode(led0_pin, OUTPUT);
    digitalWrite(led0_pin, !digitalRead(led0_pin));
    return;
}

  
bool fclass::REG_ON(){
  con.begin(57600);
    con.print(" ( ");

  con.print("REG_ON...");
  pinMode(reg_e_pin, OUTPUT);
  digitalWrite(reg_e_pin, HIGH);
  S_REG = true;
  S_GPS = true;
  
  con.print("OK  ");


  con.print(" ) ");
  delay(10);
  
  return 1;
  }

bool fclass::REG_OFF(){
  con.begin(57600);
  con.print(" ( ");
  con.print("REG_OFF...");
  if (K_GPS == false) {
      con.print("1 ");
      pinMode(reg_e_pin, OUTPUT);
      digitalWrite(reg_e_pin, LOW);
      S_REG = false;
      S_GPS = false;
  }else{
        con.print("KEPT ON  ");
    }
  con.print("OK  ");
  con.print(" ) ");
  return 1;
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


String fclass::GSM_FUNCTIONAL(int x){
  con.begin(57600);
  con.print("GSM_FUNCTIONAL: ");
  con.println(x);
  SIMgprs.inisimserial();
  String resp;
  if (x ==1){resp = AT_SET("AT+CFUN=1","OK","OK",10000,3);}
  if (x ==0){resp = AT_SET("AT+CFUN=0","OK","OK",10000,3);}
  //con.println(resp);
  return resp;
  }


String fclass::GSM_ECHO(int x){
  con.begin(57600);
  con.print("GSM_ECHO: ");
  con.println(x);
  SIMgprs.inisimserial();
  String resp;
  if (x ==1){resp = AT_GET("ATE1","OK","OK",1000);}
  if (x ==0){resp = AT_GET("ATE0","OK","OK",1000);}
  //con.println(resp);
  return resp;
  }

String fclass::GSM_BUFFER(){
  //con.begin(57600);
  //con.print("GSM_BUFFER: ");
  
  return SIMgprs.buffer;
  
  String x = SIMgprs.buffer;
  //x.replace("\n","");
  //x.replace("\r","");
  //x.replace("\n\r","");
  x.replace("\r\n","*");
 
  return x;


  }

//--------------RUTINAS EXTRAS--------------------------------------------------------------------------------------------------------------------------------------





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
  con.print(' ');
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
  //con.print("********** ******** ");
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
    //con.print(sz);
  }

  //smartDelay(0);
  return fecha + "," + hora;
}

String fclass::printFloat(float val, bool valid, int len, int prec)
{

  con.print(' ');
  if (!valid)
  {
    while (len-- > 1)
      
      con.print(val, prec);con.print('*');
      con.print(' ');
  }
  else
  {
    con.print(val, prec);
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
  
  //GPSe.begin(GPS_baud);
  GPSe.read();
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

byte fclass::AT_SET(String cmd, const char* expected1, const char* expected2, unsigned int timeout, unsigned int attempts) {
//byte fclass::AT_SET(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout, unsigned int attempts) {
  SIMgprs.inisimserial();
  byte resp = 0;
  for (int i = 0; i < attempts; i++) {
       resp = SIMgprs.sendCommand(cmd, expected1,expected2,timeout);
      //con.println(SIMgprs.buffer);
      String buf = SIMgprs.buffer;
        buf.replace(F("\r\n"), "");
        buf.replace(F("\r"), "");
        buf.replace(F("\n"), "");
        buf.replace(F("OK"), "");
        
      if (resp != 0){return resp;}
  }

  return resp;
}

String fclass::AT_GET(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout) {
  SIMgprs.inisimserial();
  byte resp = SIMgprs.sendCommand(cmd, expected1,expected2,timeout);
  String buf = SIMgprs.buffer;
  buf.replace(F("\r\n"), "");
  buf.replace(F("\r"), "");
  buf.replace(F("\n"), "");
  buf.replace(F("OK"), "");
  return buf;
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

        //con.print(F("+"));
        //con.print(chunkVal);
        //con.println(F("+"));
        return chunkVal;
      }
      chunkVal = "";
    }
  }
}

// calculate the checksum:
String fclass::checkSum(String theseChars) {
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

template <class T> int fclass::EEPROM_WRITE(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int fclass::EEPROM_READ(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}
fclass funciones = fclass();
