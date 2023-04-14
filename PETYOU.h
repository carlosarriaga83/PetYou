
#include "Arduino.h"
#include <TinyGPS.h>
#include <Wire.h>
#include <VL53L1X.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

//-------------------------------------------------------------------------------------//
// DEFINICIONES
//-------------------------------------------------------------------------------------//




//-------------------------------------------------------------------------------------//
// Protocol IDs
//-------------------------------------------------------------------------------------//

const char * const APN                = "soracom.io";
const char * const TCP_SERVER_URL     = "petyou.hopto.org";
const char * const TCP_SERVER_PORT     = "2345";


const char * const AT                 = "AT";
const char * const OK                 = "OK";
const char * const NOK                 = "NOK";

const char * const ERROR              = "ERROR";
const char * const AT_SLOW_CLOCK_ON   = "AT+CSCLK=2";
const char * const AT_SLOW_CLOCK_OFF  = "AT+CSCLK=0";
const char * const AT_ECHO_ON         = "ATE1";
const char * const AT_ECHO_OFF        = "ATE0";
const char * const AT_FUN_ON          = "AT+CFUN=1";
const char * const AT_FUN_OFF         = "AT+CFUN=0";
const char * const AT_GET_IMEI        = "AT+GSN";
const char * const AT_RESET_DEFAULT   = "ATZ";
const char * const AT_GET_HTTP_STATUS = "AT+SHSTATE?";
const char * const AT_NETLIGHT_ON     = "AT+CNETLIGHT=1";
const char * const AT_NETLIGHT_OFF    = "AT+CNETLIGHT=0";
const char * const AT_REGISTER_ON     = "AT+CNACT=0,2";
const char * const AT_REGISTER_OFF    = "AT+CNACT=0,0";
const char * const AT_GET_SIGNAL      = "AT+CSQ";
const char * const AT_GET_CARRIER     = "AT+COPS?";
const char * const AT_QRY_IP          = "AT+CGCONTRDP";
const char * const AT_HTTP_ON         = "AT+SHCONN";
const char * const AT_HTTP_OFF        = "AT+SHDISC";
const char * const AT_TCP_STATUS      = "AT+CIPSTATUS";
const char * const AT_TCP_SEND        = "AT+CASEND";
const char * const AT_TCP_CLOSE       = "AT+CACLOSE=0";
const char * const AT_IS_APN          = "AT+CGDCONT?";
const char * const AT_POWER_OFF       = "AT+CPOWD=1";
const char * const TCP_GET_PAYLOAD      = "AT+CARECV=0,1024";

//int status;

//-------------------------------------------------------------------------------------//
// PIN DEFINITIONS
//-------------------------------------------------------------------------------------//

// TRANSISTORED OUTPUTS
const uint8_t WIFI_E_PIN       = 12; // WIFI MODULE ENABLE
const uint8_t ONE_E_PIN       = 3; // ON EWIRE SENSORS ENABLE
const uint8_t GPS_E_PIN       = 14; //GPS ENABLE PIN
const uint8_t MODEM_RESET_PIN     = 13; //CONFIGURAR PIN DE RESET DE GSM
const uint8_t I2CBUS_E_PIN      = 23; // OLED ENABLE PIN
const uint8_t IMU_E_PIN       = 15; // ACCELEROMETER ENABLE PIN


// NON TRANSISTORED OUTPUTS
const uint8_t BUZZ_PIN        = 0;
const uint8_t GSM_DTR_PIN       = 18;
const uint8_t NEO_INT_PIN       = 24;
const uint8_t NEO_EXT_PIN       = 20;
const uint8_t LED0_PIN        = 4; //LED4
const uint8_t PWM_LED_PIN       = 4; //LED4


//INPUTS
const uint8_t BATT_CHG_PIN      = 29;
const uint8_t BAT_IN_PIN      = 29; //ANALOG PIN INPUT, BATTERY LEVEL

const uint8_t AN1_PIN         = 30; //
const uint8_t AN2_PIN         = 31; //
const uint8_t CURRENT_PIN       = 30; // CURRENT SENSOR INPUT

// COMUNICATION PINS
const uint8_t CON_RX_PIN      = 21;
const uint8_t CON_TX_PIN      = 22;
const uint8_t ONE_WIRE_BUS      = 28; // ON EWIRE SENSORS




//-------------------------------------------------------------------------------------//
// ESTRUCTURAS
//-------------------------------------------------------------------------------------//

typedef struct config_t
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
  float VECTOR_TOLERANCE;
  int OLED;
  int BUZZ;
  int E_GPS;

  long int KEEP_SENDING_UNTIL;


};


typedef struct locus
{

  bool on;
  int percent;
  int records;
  int attempts;
  int cycles;
};



//-------------------------------------------------------------------------------------//
// CLASE
//-------------------------------------------------------------------------------------//



class BOARD
{
  public:




    VL53L1X TOF_I2C;

    Stream* MODEM_PORT;
    Stream* GPS_PORT;
    Stream* RF_PORT;
    Stream* BT_PORT;
    Stream* CON_PORT;

    String MODEM_PAYLOAD;
    String TCP_SERVER_IP;

    int LEVEL;
    String TABING = "\t";

    String SEPARATOR = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

    StaticJsonDocument<200> doc;



    

    locus LOCUS;

    config_t SETTINGS;

    bool REG_ON();
    bool REG_OFF();

    String TEMP_READ();

    void RUN_SETUP();
    void RUN_CONDITIONS();

    //---------------------------------CORE------------------------------------------------//
    int CORE_BATT(int PIN);
    byte CORE_SLEEP(int k, String razon);
    long unsigned CORE_RUNTIME();


    //---------------------------------PUERTOS------------------------------------------------//
    bool BT_BEGIN(Stream &stream);
    bool MODEM_BEGIN(Stream &stream);
    bool CON_BEGIN(Stream &stream);

    //---------------------------------OLED------------------------------------------------//
    void OLED_DISP(String label, String val, int linea);
    void OLED_INI();
    void OLED_BAR(int POS, String VAR);
    void OLED_TOP(int POS, String VAR);

    //---------------------------------SENSORS------------------------------------------------//

    void I2C_SCAN();
    void I2C_BEGIN();
    void I2C_END();

    void CURRENT_READ();
    void CURRENT_INI();
    void AN1_READ();
    void AN2_READ();
    byte TOF_SETUP(int E_PIN);
    String TOF_READ(int E_PIN);
    void IMU_SETUP();
    //void IMU_READ();
    byte IMU_TRIGGER();         //CHECK IF IMU IS MOVING
    void IR_SETUP();
    bool IR_CONNECTED();
    void IR_READ();

    //---------------------------------MISC------------------------------------------------//
    void NEO_INT_SETUP();
    void NEO_INT_SET(int r, int g, int b);
    void colorWipe(uint32_t c, uint8_t wait);
    void LED0_ON();
    void LED0_OFF();
    void LED0_TOGGLE();
    void LED_ERR_ON();
    void LED_ERR_OFF();
    void LED_ERR_TOGGLE();
    void BUZZ(int beeps);
    bool RTC_RESET();
    bool RTC_START();
    long int RTC_READ();
    bool RTC_SET(String TS);


    //---------------------------------EXTERNAL GPS------------------------------------------//
    void GPS_SETUP(int COMPLETE);
    void GPS_NMEA(int mode);
    bool GPS_BAUD(int baud);
    void GPS_ENABLE();
    void GPS_DISABLE();
    void GPS_ON(bool PAUSE_ON);
    void GPS_OFF();
    void GPS_STBY();
    void GPS_COLD();
    void GPS_RESET();
    void GPS_HOT();
    void GPS_PPS();
    bool GPS_READ(int timeout);

    void GPS_LOG_START(int freq);
    void GPS_LOG_STOP();
    void GPS_LOG_STATUS();
    void GPS_LOG_READ();
    void GPS_LOG_ERASE();
    void GPS_LOG_UPLOAD();



    //---------------------------------MODEM------------------------------------------//

    void GSM_PURGESERIAL();
    byte GSM_SETUP();
    void GSM_PWR_KEY(int RESET_PIN, bool MODE);
    int GSM_SIGNAL(int TIMEOUT);
    String GSM_IP();
    String GSM_TIME();
    String GSM_FUNCTIONAL(int x);
    String GSM_ECHO(int x);
    byte GSM_PL_READ(String TO_READ );
    byte GSM_PING(int MAX_LAG, int MAX_INTENTOS);
    bool GSM_LOC();
    void GSM_SLEEP();
    bool GSM_PWR(int x);
    bool GSM_PSM(int x);
    byte GSM_CONNECT();
    bool GSM_SYNC_CLOCK();
    byte GSM_SOLVE_DNS();
    byte GSM_IS_REG();
    byte GSM_IS_REG_NBIOT();
    byte GSM_REGISTER();
    byte GSM_DISREGISTER();
    byte GSM_IS_ACT();
    byte GSM_ACT();
    byte GSM_IS_ATT();
    byte GSM_ATT();
    byte GSM_DEATT();
    byte GSM_HTTP_CONF();

    byte GSM_NETLIGHT(bool x);
    byte GSM_SET_APN(String APN);
    byte GSM_PWRKEY();
    String GSM_IMEI();
    byte IS_APN();


    byte GSM_GET_IP(int TIMEOUT);
    byte GSM_FUN_RESET();
    byte GSM_AT();
    byte GSM_ENSURE_AT();
    byte GSM_ENSURE_PWR(bool MODE);
    byte GSM_ENSURE_ONLINE();
    byte GSM_ECHO_ON();
    byte GSM_ECHO_OFF();
    byte GSM_NETLIGHT_ON();
    byte GSM_NETLIGHT_OFF();
    byte GSM_REGISTER_OFF();
    byte GSM_REGISTER_ON();
    byte GSM_FUN_ON();
    byte GSM_FUN_OFF();
    byte GSM_HTTP_ON();
    byte GSM_HTTP_OFF();



    byte GSM_TCP_CONNECT();
    byte GSM_TCP_STATUS();
    bool GSM_TCP_LOG_SEND();
    byte GSM_TCP_SEND(String QRY);
    byte GSM_TCP_CLOSE();

    byte GSM_HTTP_SEND(String QRY);
    byte GSM_HTTP_CLOSE();
    byte GSM_HTTP_STATUS();
    byte GSM_HTTP_CONNECT(String ID);



    byte PL_PROCESS(String PAYLOAD_TEMP);
    String GSM_WAIT_INPUT(int TIMEOUT);

    //String GSM_HTTP_SEND(String QRY);

    String GET_QRY(String DEVICE, String RAZON);     // SEND QRY AND GET PAYLOAD PL


    String GSM_BAT();
    String GSM_BUFFER();

    String leeAT(const char* cm);
    //byte AT_SET(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout, unsigned int attempts);
    byte AT_SET(String cmd, const char* expected1, const char* expected2, unsigned int timeout, unsigned int attempts);
    String AT_GET(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout);

    byte SEND_CHECK_REPLY(String cmd, const char* expected1, const char* expected2, unsigned int timeout, unsigned int attempts);

    void WAIT_INPUT();

    //---------------------------------CONSOLE------------------------------------------//

    void PRINT(int PREFIX, String TITLE , String TEXT , bool NEWLINE);
    void PRINTLN(String TEXT, String TEXT2 = "");




    String printFloat(float val, bool valid, int len, int prec);
    void smartDelay(unsigned long ms);
    String print_date(TinyGPS &TNYgps);
    String print_int(unsigned long val, unsigned long invalid, int len);

    //---------------------------------FUNCIONES------------------------------------------//

    String EXPLOTAR(String data, char separator, int index);
    String GET_MID_STRING(String str, String start, String finish);


    String UNIX_2_TS(long int UNIX_TS);
    long int TS_2_UNIX(String TS);
    String CLEAN(String s);
    String checkSum(String theseChars);

    
    
    String HEX_2_STRING(String hex);

    void STATUS();
    bool STATUS_GPS(unsigned int to);
    byte STATUS_GSM(int RETRY);
    bool STATUS_REG();


    void UPDATE();

  private:



};
