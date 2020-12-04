#include <Arduino.h>

#include <TinyGPS.h>
//TinyGPS TNYgps;



#ifndef funciones_h
#define funciones_h

class fclass
{
  public:
    fclass();
    bool REG_ON();
    bool REG_OFF();


    
    void RUN_SETUP();
    void RUN_CONDITIONS();


    void OLED_DISP(String label, String val, int linea, bool clean);
    void OLED_INI();

    String CURRENT_READ(); 
    void CURRENT_INI();
    
    void NEO_INT_SETUP();
    void NEO_INT_SET(int r, int g, int b);
    
    void LED0_ON();
    void LED0_OFF();
    void LED0_TOGGLE();
    
    void GPS_SETUP(int COMPLETE);
    void GPS_NMEA(int mode);
    bool GPS_BAUD(int baud);
    void GPS_ENABLE();
    void GPS_DISABLE();
    void GPS_ON();
    void GPS_OFF();
    void GPS_STBY();
    void GPS_COLD();
    void GPS_HOT();
    void GPS_PPS();
    bool GPS_READ(int timeout);
    
    void GPS_LOG_START(int freq);
    void GPS_LOG_STOP();
    void GPS_LOG_STATUS();
    void GPS_LOG_READ();
    void GPS_LOG_ERASE();
    void GPS_LOG_UPLOAD();
    

    void IMU_SETUP();
    void IMU_READ();

    String GSM_IP();
    String GSM_TIME();
    String GSM_FUNCTIONAL(int x);
    String GSM_ECHO(int x);
   
    bool GSM_LOC();
    void GSM_SLEEP();
    bool GSM_PWR(int x);
    bool GSM_ACTIVE();


    
    String GSM_BAT();
    String GSM_BUFFER();
    
    
    bool GSM_BEFORE_CHECKLIST();                        //VERIFY THINGS BEFORE SENDING
    bool GSM_SEND(String DEVICE,String RAZON);      // SEND QRY AND GET PAYLOAD PL
    bool GSM_AFTER_CHECKLIST();                           // UPDATE THINGS/PARAM BASED ON PL
    
    
    bool GSM_TCP_SETUP();
    bool GSM_TCP_LOG_SEND();
    String GSM_TCP_SEND(String QRY);
    String GSM_HTTP_SEND(String QRY);
    
    String leeAT(const char* cm);
    //byte AT_SET(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout, unsigned int attempts);
    byte AT_SET(String cmd, const char* expected1, const char* expected2, unsigned int timeout, unsigned int attempts);
    String AT_GET(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout);


    bool RTC_RESET();
    bool RTC_START();
    long int RTC_READ();
    bool RTC_SET(String TS);
    String UNIX_2_TS(long int UNIX_TS);
    int TS_2_UNIX(String TS);

    void I2C_SCAN();
    void I2C_BEGIN();

    void STATUS();
    bool STATUS_GPS(unsigned int to);
    bool STATUS_GSM();
    bool STATUS_REG();



    void CORE_SLEEP(int k,String razon);
    long unsigned CORE_RUNTIME();

    void UPDATE();







    String checkSum(String theseChars);
    String explotar(String data, char separator, int index);
    String printFloat(float val, bool valid, int len, int prec);
    void smartDelay(unsigned long ms);
    String print_date(TinyGPS &TNYgps);
    String print_int(unsigned long val, unsigned long invalid, int len);
    String clean(String s);

    template <class T> int EEPROM_WRITE(int ee, const T& value);
    template <class T> int EEPROM_READ(int ee, T& value);

    
};

extern fclass funciones;

#endif
