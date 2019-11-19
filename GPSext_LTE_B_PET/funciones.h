#include <Arduino.h>

#include <TinyGPS.h>
//TinyGPS TNYgps;



#ifndef funciones_h
#define funciones_h

class fclass
{
  public:
    fclass();
    void REG_ON();
    void REG_OFF();
    
    void NEO_INT_SETUP();
    void NEO_INT_SET(int r, int g, int b);
    
    void LED0_ON();
    void LED0_OFF();
    
    void GPS_NMEA();
    void GPS_BAUD();
    void GPS_ENABLE();
    void GPS_DISABLE();
    void GPS_ON();
    void GPS_OFF();
    void GPS_STBY();
    void GPS_COLD();
    void GPS_HOT();
    void GPS_PPS();
    bool GPS_READ(int timeout);
    
    void GPS_LOG_START();
    void GPS_LOG_STOP();
    void GPS_LOG_STATUS();
    void GPS_LOG_READ();
    void GPS_LOG_ERASE();

    String GSM_IP();
    bool GSM_CONECTADO();
    bool GSM_LOC();
    void GSM_SLEEP();
    void GSM_ON();
    void GSM_ON_FULL();
    String GSM_BAT();
    bool GSM_CONNECT();
    bool GSM_SEND(String url,String qry);

    bool RTC_START();
    String RTC_READ();

    void I2C_SCAN();
    void I2C_BEGIN();

    void STATUS();
    bool STATUS_GPS(int to);
    bool STATUS_GSM();
    bool STATUS_REG();

    

    void CORE_SLEEP(int k,String razon);
    long unsigned CORE_RUNTIME();

    void UPDATE();







    
    String explotar(String data, char separator, int index);
    String printFloat(float val, bool valid, int len, int prec);
    void smartDelay(unsigned long ms);
    String print_date(TinyGPS &TNYgps);
    String print_int(unsigned long val, unsigned long invalid, int len);
    String clean(String s);

    String leeAT(const char* cm);
    
};

extern fclass funciones;

#endif
