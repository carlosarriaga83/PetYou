

#include "funciones.h"



int BTN1_PIN = 25;
int BTN2_PIN = 26;
int BTN3_PIN = 27;
int BTN1_S;
int BTN2_S;
int BTN3_S;


#include <SoftwareSerial.h>

SoftwareSerial console(1, 2);
//#define con Serial
#define gpsEXT Serial






int gpsBaud = 9600;



  

void setup()
{

//TEMP_STR.reserve(500);




  pinMode(BTN1_PIN, INPUT);
  pinMode(BTN2_PIN, INPUT);
  pinMode(BTN3_PIN, INPUT);
  

  funciones.REG_ON();


  console.begin(57600);
  gpsEXT.begin(9600);
  Serial.begin(57600);
  //GPS.begin(gpsBaud);


  console.begin(57600);
  console.println("Starting....");


  gpsEXT.begin(gpsBaud);
  
  Serial.begin(57600);
  console.begin(57600);



  console.println("Running....");
  
  funciones.RUN_SETUP();

  funciones.REG_OFF();


}




void loop(){



  
  while(true){
    funciones.RUN_CONDITIONS();
  }
 
  
  console.begin(57600);
  console.println("Listening..");
  //delay(500);
  console.read();

  String STRING_IN;

  //SEND();
  
  while (console.available()==0) {             //Wait for user input
    
      //BTN1_S = digitalRead(BTN1_PIN);
      //if (BTN1_S == 0) {SEND();}
      
  }

  
  STRING_IN = console.readString();    

  STRING_IN.replace("\n","");
  STRING_IN.replace("\r","");
  
  const char *cstr = STRING_IN.c_str();
  
  console.print("STRING_IN: ");
  console.println(String(cstr));
  
  if (STRING_IN.length() > 3 ){
    
    
    //funciones.OLED_DISP("COMMANDO: ", String(cstr), 1,true);

    byte RESPUESTA_BYTE = funciones.AT_SET(cstr,"OK","ERROR",5000,2);
    console.print("[BYTE]: ");
    console.println( RESPUESTA_BYTE );

    String RESPUESTA_STR = funciones.GSM_BUFFER();
    //funciones.OLED_DISP("[BUFFER] ",RESPUESTA_STR , 1,true);
    console.print("[BUFFER]: ");
    console.println( RESPUESTA_STR );
    return;
  }
  
  //if (console.available()) {
  
    char x = console.read();

  
    x = STRING_IN.charAt(0);
    
    //funciones.OLED_DISP("GOT: ", String(x), 1,true);
    
    switch (x) {



      case 'L':
        funciones.GPS_COLD();        break;



      case 'Y':
        funciones.GPS_STBY();        break;


      case 'G':
        funciones.GPS_READ(10);        break;
      case 'E':
        funciones.GPS_ENABLE();        break;
      case 'e':
        funciones.GPS_DISABLE();        break;
      case 'p':
        funciones.GPS_PPS();        break;
        

      case 'f':
        funciones.GSM_TCP_SETUP();        break;

      case 'P':
        funciones.GSM_IP();        break;
      case 'z':
        funciones.GSM_SLEEP();        break;

      case 'T':
        funciones.GSM_BAT();        break;



      case '0':
        funciones.GPS_SETUP(0);        break; 
      case '1':
        funciones.GPS_SETUP(1);        break; 
      case '2':
        funciones.REG_OFF();        break;
      case '3':
        funciones.REG_ON();        break;
        

        
      case '5':
         funciones.GPS_BAUD(4800);        break;
        
      case '6':
         funciones.GPS_LOG_STATUS();      break;
      case '7':
        funciones.GPS_LOG_STOP();        break;
      case 'Q':
        funciones.GPS_LOG_STATUS();        break;
       case 'q':
        funciones.GPS_LOG_ERASE();        break;
        
      case '8':
        funciones.GSM_SEND("GPS2","TEST");        break;
      case '9':
        funciones.GPS_BAUD(9600);        break;
        
      case '?':
        funciones.STATUS();        break;
      case '@':
        console.println(funciones.STATUS_GSM());        break;
      case '#':
        console.println(funciones.STATUS_GPS(1000));        break;
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


}








void softreset() // Restarts program from beginning but does not reset the peripherals and registers
{
  console.print("RESET"); console.println("");
  asm volatile ("  jmp 0");
}
