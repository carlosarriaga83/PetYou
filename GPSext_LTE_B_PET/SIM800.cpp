/*************************************************************************
* SIM800 GPRS/HTTP Library
* Distributed under GPL v2.0
* Written by Stanley Huang <stanleyhuangyc@gmail.com>
* For more information, please visit http://arduinodev.com
*************************************************************************/

#include "SIM800.h"
//#include <SoftwareSerial.h>

//static const int RXPin = 3, TXPin = 4;
//SoftwareSerial Serial1(RXPin, TXPin);

//static const int RXPin = 3, TXPin = 4;
//SoftwareSerial Serial1(6, 7); //GSM proto
//SoftwareSerial Serial1(11, 12); //GSM pcb mini
SoftwareSerial con1(1, 2);




void CGPRS_SIM800::GSM_PWRKEY()
{
    //Serial1.begin(57600);
    con1.begin(57600);
    con1.println("GSM_PWRKEY");
    pinMode(SIM800_RESET_PIN, OUTPUT);


    digitalWrite(SIM800_RESET_PIN, LOW);
    delay(1500);

    digitalWrite(SIM800_RESET_PIN, HIGH);
    
    delay(2000);
    
    digitalWrite(SIM800_RESET_PIN, LOW);
    delay(3500);
    
    //pinMode(SIM800_RESET_PIN, INPUT);
    
    return;
}


bool CGPRS_SIM800::inisimserial(){Serial1.begin(57600);}
//bool CGPRS_SIM800::inisimserial(){Serial1.begin(115200);}




void CGPRS_SIM800::AT(const char* ATcmd)
{
  sendCommand(ATcmd);
}

void CGPRS_SIM800::AT_TIMEOUT(const char* ATcmd, int t,  const char* expected)
{
  sendCommand(ATcmd,t,expected);
}

byte CGPRS_SIM800::qryAT(const char* qcmd, unsigned int timeout, const char* expected)
{
  return sendCommand(qcmd,timeout,expected);
}





//byte CGPRS_SIM800::sendCommand(const char* cmd, unsigned int timeout, const char* expected)
byte CGPRS_SIM800::sendCommand(String cmd, unsigned int timeout, const char* expected)
{
  if (cmd) {
    purgeSerial();

#ifdef DEBUG
    console.print('>');
    console.println(cmd);
#endif
    inisimserial();
    Serial1.println(cmd);
  }
  uint32_t t = millis();
  byte n = 0;
  do {
    if (Serial1.available()) {
      char c = Serial1.read();
      if (n >= sizeof(buffer) - 1) {
        // buffer full, discard first half
        n = sizeof(buffer) / 2 - 1;
        memcpy(buffer, buffer + sizeof(buffer) / 2, n);
      }
      buffer[n++] = c;
      buffer[n] = 0;
      if (strstr(buffer, expected ? expected : "OK\r")) {

#ifdef DEBUG
       console.print(F("[1]"));
       console.println(buffer);
       
#endif
       buf = String(buffer);
       return n;
      }
    }
  } while (millis() - t < timeout);

#ifdef DEBUG
   console.print(F("[0]"));
   console.println(buffer);
#endif
  buf = String(buffer);
  return 0;
}
//byte CGPRS_SIM800::sendCommand(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout)
byte CGPRS_SIM800::sendCommand(String cmd, const char* expected1, const char* expected2, unsigned int timeout)
{
  if (cmd) {
    purgeSerial();

#ifdef DEBUG
    console.print('>');
    console.println(cmd);
#endif
    inisimserial();
    Serial1.println(cmd);
  }
  uint32_t t = millis();
  byte n = 0;
  do {
    if (Serial1.available()) {
      char c = Serial1.read();
      if (n >= sizeof(buffer) - 1) {
        // buffer full, discard first half
        n = sizeof(buffer) / 2 - 1;
        memcpy(buffer, buffer + sizeof(buffer) / 2, n);
      }
      buffer[n++] = c;
      buffer[n] = 0;
      if (strstr(buffer, expected1)) {


#ifdef DEBUG
       console.print(F("[1]"));
       console.println(buffer);
#endif
       buf = String(buffer);
       return 1;
      }
      if (strstr(buffer, expected2)) {

#ifdef DEBUG
       console.print(F("[2]"));
       console.println(buffer);
#endif
       buf = String(buffer);
       return 2;
      }
    }
  } while (millis() - t < timeout);

#if DEBUG
   console.print(F("[0]"));
   console.println(buffer);
#endif
  buf = String(buffer);
  return 0;
}


  
byte CGPRS_SIM800::checkbuffer(const char* expected1, const char* expected2, unsigned int timeout)
{
    
    while (Serial1.available()) {
        char c = Serial1.read();
        if (m_bytesRecv >= sizeof(buffer) - 1) {
            // buffer full, discard first half
            m_bytesRecv = sizeof(buffer) / 2 - 1;
            memcpy(buffer, buffer + sizeof(buffer) / 2, m_bytesRecv);
        }
        buffer[m_bytesRecv++] = c;
        buffer[m_bytesRecv] = 0;
        if (strstr(buffer, expected1)) {
            return 1;
        }
        if (expected2 && strstr(buffer, expected2)) {
            return 2;
        }
    }
    return (millis() - m_checkTimer < timeout) ? 0 : 3;
}

void CGPRS_SIM800::purgeSerial()
{
  while (Serial1.available()) Serial1.read();
}
