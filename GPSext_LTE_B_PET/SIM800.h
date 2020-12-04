/*************************************************************************
* SIM800 GPRS/HTTP Library
* Distributed under GPL v2.0
* Written by Stanley Huang <stanleyhuangyc@gmail.com>
* For more information, please visit http://arduinodev.com
*************************************************************************/

#include <Arduino.h>


// change this to the pin connect with SIM800 reset pin
#define SIM800_RESET_PIN 13 //CONFIGURAR PIN DE RESET DE GSM



// change this to the serial UART which SIM800 is attached to
//#define Serial1 Serial1

// define DEBUG to one serial UART to enable debug information output
//#define DEBUG Serial
#include <SoftwareSerial.h>

//SoftwareSerial console(1, 2);
//#define DEBUG serial3
typedef enum {
    HTTP_DISABLED = 0,
    HTTP_READY,
    HTTP_CONNECTING,
    HTTP_READING,
    HTTP_ERROR,
} HTTP_STATES;

typedef struct {
  float lat;
  float lon;
  String q;
  uint8_t year; /* year past 2000, e.g. 15 for 2015 */
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} GSM_LOCATION;

class CGPRS_SIM800 {
public:

    void GSM_PWRKEY();
    CGPRS_SIM800():httpState(HTTP_DISABLED) {}
    // initialize the module
    bool init();
    bool only_init();
    // setup network
    byte setup(String apn);
    // get network operator name
    bool getOperatorName();
    bool inisimserial();
    // check for incoming SMS
    //bool checkSMS();
    // get signal quality level (in dB)
    bool getSignalQuality();
    // get GSM location and network time
    bool getLocation(GSM_LOCATION* loc);
    // initialize HTTP connection
    bool httpInit();
    // terminate HTTP connection
    void httpUninit();
    // connect to HTTP server
    bool httpConnect(String url, String args);
    // check if HTTP connection is established
    // return 0 for in progress, 1 for success, 2 for error
    byte httpIsConnected();
    // read data from HTTP connection
    void httpRead();
    // check if HTTP connection is established
    // return 0 for in progress, -1 for error, bytes of http payload on success
    int httpIsRead();
    //general query command

    void AT(const char* ATcmd);
    void AT_TIMEOUT(const char* ATcmd, int t, const char* expected);
    byte qryAT(const char* qcmd, unsigned int timeout, const char* expected);
    // send AT command and check for expected response
    //byte sendCommand(const char* cmd, unsigned int timeout = 2000, const char* expected = 0);
    byte sendCommand(String cmd, unsigned int timeout = 2000, const char* expected = 0);
    // send AT command and check for two possible responses
    //byte sendCommand(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout = 2000);
    byte sendCommand(String cmd, const char* expected1, const char* expected2, unsigned int timeout = 2000);
    // toggle low-power mode
    bool sleep(bool enabled)
    {
      //pinMode(SIM800_RESET_PIN, OUTPUT);
      if(enabled){
           

    pinMode(SIM800_RESET_PIN, OUTPUT);
    digitalWrite(SIM800_RESET_PIN, LOW);
    //delay(500);
    digitalWrite(SIM800_RESET_PIN, HIGH);
    delay(1100);
    digitalWrite(SIM800_RESET_PIN, LOW);
    //delay(3000);


          
        }else{

          sendCommand("AT+CFUN=1");
          delay(3000);

         
    pinMode(SIM800_RESET_PIN, OUTPUT);
    digitalWrite(SIM800_RESET_PIN, HIGH);
    delay(10);
    digitalWrite(SIM800_RESET_PIN, LOW);
    delay(100);
    digitalWrite(SIM800_RESET_PIN, HIGH);
    delay(3000);
            
            //delay(3000);
            
          }
          return true;
      //return sendCommand(enabled ? "AT+CFUN=0" : "AT+CFUN=1");
    }
    // check if there is available serial data
    bool available()
    {
      //return Serial11.read(); 
    }
    //char buffer[256];
    char buffer[250];
    String buf;
    byte httpState;
private:
    byte checkbuffer(const char* expected1, const char* expected2 = 0, unsigned int timeout = 2000);
    void purgeSerial();
    byte m_bytesRecv;
    uint32_t m_checkTimer;
};
