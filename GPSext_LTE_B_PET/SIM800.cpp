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


bool CGPRS_SIM800::init()
{
    //Serial1.begin(57600);
    
    pinMode(SIM800_RESET_PIN, OUTPUT);
    digitalWrite(SIM800_RESET_PIN, HIGH);
    delay(10);
    digitalWrite(SIM800_RESET_PIN, LOW);
    delay(100);
    digitalWrite(SIM800_RESET_PIN, HIGH);
    delay(3000);
    
    if (sendCommand("AT")) {
        sendCommand("AT+IPR=57600");
        //sendCommand("AT+IPR=115200");
        //Serial1.begin(115200);
        sendCommand("ATE0");
        sendCommand("AT+CFUN=1", 10000);
        return true;
    }else{sendCommand("ATE0");}
    return false;
}

bool CGPRS_SIM800::only_init()
{
    //Serial1.begin(57600);
    
    pinMode(SIM800_RESET_PIN, OUTPUT);
    digitalWrite(SIM800_RESET_PIN, HIGH);
    delay(10);
    digitalWrite(SIM800_RESET_PIN, LOW);
    delay(100);
    digitalWrite(SIM800_RESET_PIN, HIGH);
    delay(3000);
    if (sendCommand("AT")) {
        sendCommand("AT+IPR=57600");
        sendCommand("ATE0");
        sendCommand("AT+CFUN=0", 10000);
        return true;
    }else{sendCommand("ATE0");}
    return false;
}

bool CGPRS_SIM800::inisimserial(){Serial1.begin(57600);}
//bool CGPRS_SIM800::inisimserial(){Serial1.begin(115200);}

/*bool CGPRS_SIM800::init()
{
    
    if (sendCommand("AT")) {
        //sendCommand("AT+IPR=9600");
        sendCommand("ATE0");
        //sendCommand("AT+CFUN=1", 10000);
        return true;
    }else{sendCommand("AT+CFUN=1", 10000);sendCommand(0);}
    
    
    
    //Serial1.listen();
    pinMode(SIM800_RESET_PIN, OUTPUT);
    digitalWrite(SIM800_RESET_PIN, LOW);
    delay(10);
    digitalWrite(SIM800_RESET_PIN, HIGH);
    delay(100);
    digitalWrite(SIM800_RESET_PIN, LOW);
    delay(3000);
    if (sendCommand("AT")) {
        //sendCommand("AT+IPR=9600");
        sendCommand("ATE0");
        //sendCommand("AT+CFUN=1", 10000);
        return true;
    }
    return false;
}
*/
byte CGPRS_SIM800::setup(String apn)
{
  //inisimserial();
  bool success = false;
  for (byte n = 0; n < 30; n++) {
    if (sendCommand("AT+CREG?", 2000)) {
        char *p = strstr(buffer, "0,");
        if (p) {
          char mode = *(p + 2);

#if DEBUG
          //console.print("Mode:");
          //console.println(mode);
#endif

          if (mode == '1' || mode == '5') {
            success = true;
            break;
          }
        }
    }
    delay(1000);
  }
  //inisimserial();    
  if (!success)
    return 1;
  
  if (!sendCommand("AT+CGATT?",1000))
    return 2;
    
   
  if (!sendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\""))
    return 3;
    
  delay(1000);
  Serial1.print(F("AT+SAPBR=3,1,\"APN\",\""));
  Serial1.print(apn);
  Serial1.println('\"');
  delay(1000);
  
  if (!sendCommand(0))
    return 4;

  //if (sendCommand("AT+SAPBR=2,1","ERROR","+SAPBR: 1,3",2000)==2){sendCommand("AT+SAPBR=1,1", 10000);sendCommand(0);}
    
  if (sendCommand("AT+SAPBR=2,1","ERROR","+SAPBR: 1,1",2000)==2){sendCommand(0);success = true;}else{sendCommand("AT+SAPBR=1,1", 10000);sendCommand(0);}

  if (sendCommand("AT+SAPBR=2,1","ERROR","+SAPBR: 1,1",2000)==2){sendCommand(0);success = true;}else{sendCommand("AT+SAPBR=1,1", 10000);sendCommand(0);return 5;}

    
  //sendCommand("AT+SAPBR=1,1", 10000);
  //sendCommand("AT+SAPBR=2,1", 10000);

  //sendCommand("AT+CMGF=1");    // sets the SMS mode to text
  //sendCommand("AT+CPMS=\"SM\",\"SM\",\"SM\""); // selects the memory

  if (!success)
    return 6;

  return 0;
}
bool CGPRS_SIM800::getOperatorName()
{
    //SoftwareSerial Serial1(10, 11);
  //Serial1.listen();
  // display operator name
  //inisimserial();  
  if (sendCommand("AT+CBC", "OK\r", "ERROR\r") == 1) {
      char *p = strstr(buffer, ",");
      if (p) {
          p += 1;
          char *s = strchr(p, ',');
          if (s) *s = 0;
          strcpy(buffer, p);
          return true;
      }
  }
  sendCommand(0);
  return false;
}
/*
bool CGPRS_SIM800::checkSMS()
{
  if (sendCommand("AT+CMGR=3", "+CMGR:", "ERROR") == 1) {
    // reads the data of the SMS
    sendCommand(0, 100, "\r\n");
    if (sendCommand(0)) {
      // remove the SMS
      //sendCommand("AT+CMGD=1");
      return true;
    }
  }
  return false; 
}
*/
bool CGPRS_SIM800::getSignalQuality()
{
    //SoftwareSerial Serial1(10, 11);
  //Serial1.listen();
  // display operator name
  if (sendCommand("AT+CSQ", "OK\r", "ERROR\r") == 1) {
      char *p = strstr(buffer, ": ");
      if (p) {
          p += 1;
          char *s = strchr(p, ',');
          if (s) *s = 0;
          strcpy(buffer, p);
          return true;
      }
  }
  return false;
}

bool CGPRS_SIM800::getLocation(GSM_LOCATION* loc)
{
  //Serial1.listen();
  if (sendCommand("AT+CIPGSMLOC=1,1", 10000)) do {
    char *p;
    loc->q = String(buffer);
    if (!(p = strchr(buffer, ':'))) break;
    if (!(p = strchr(p, ','))) break;
    loc->lon = atof(++p);
    if (!(p = strchr(p, ','))) break;
    loc->lat = atof(++p);
    if (!(p = strchr(p, ','))) break;
    /*
    loc->year = atoi(++p) - 2000;
    if (!(p = strchr(p, '/'))) break;
    loc->month = atoi(++p);
    if (!(p = strchr(p, '/'))) break;
    loc->day = atoi(++p);
    if (!(p = strchr(p, ','))) break;
    loc->hour = atoi(++p);
    if (!(p = strchr(p, ':'))) break;
    loc->minute = atoi(++p);
    if (!(p = strchr(p, ':'))) break;
    loc->second = atoi(++p);
    */
    return true;
  } while(0);
  sendCommand(0);
  return false;
}

void CGPRS_SIM800::AT(const char* ATcmd)
{
  sendCommand(ATcmd);
}

byte CGPRS_SIM800::qryAT(const char* qcmd, unsigned int timeout, const char* expected)
{
  return sendCommand(qcmd,timeout,expected);
}


void CGPRS_SIM800::httpUninit()
{
  sendCommand("AT+CIPCLOSE=1");
  delay(300);
  sendCommand("AT+HTTPTERM");
  delay(300);
  sendCommand(0);

}

bool CGPRS_SIM800::httpInit()
{
  //Serial1.begin(9600);
  if  ( !sendCommand("AT+HTTPINIT", 10000) || !sendCommand("AT+HTTPPARA=\"CID\",1", 5000)) {
    httpState = HTTP_DISABLED;
    return false;
  }
  httpState = HTTP_READY;
  return true;
}
bool CGPRS_SIM800::httpConnect(String url, String args)
{
    //Serial1.listen();
    // Sets url
    Serial1.print(F("AT+HTTPPARA=\"URL\",\""));
    Serial1.print(url);
    if (args) {
        //Serial1.print(F("?"));
        Serial1.print(args);
    }

    Serial1.println('\"');
    delay(200);
    //sendCommand(0);
    if (sendCommand(0))
    {
        // Starts GET action
        Serial1.println(F("AT+HTTPACTION=0"));
        httpState = HTTP_CONNECTING;
        m_bytesRecv = 0;
        m_checkTimer = millis();
    } else {
        httpState = HTTP_ERROR;
    }
    return false;
}
// check if HTTP connection is established
// return 0 for in progress, 1 for success, 2 for error
byte CGPRS_SIM800::httpIsConnected()
{
    byte ret = checkbuffer("0,200", "0,60", 10000);
    if (ret >= 2) {
        httpState = HTTP_ERROR;
        return -1;
    }
    return ret;
}
void CGPRS_SIM800::httpRead()
{
    Serial1.println(F("AT+HTTPREAD"));
    httpState = HTTP_READING;
    m_bytesRecv = 0;
    m_checkTimer = millis();
}
// check if HTTP connection is established
// return 0 for in progress, -1 for error, number of http payload bytes on success
int CGPRS_SIM800::httpIsRead()
{
    byte ret = checkbuffer("+HTTPREAD: ", "ERROR", 10000) == 1;
    if (ret == 1) {
        m_bytesRecv = 0;
        // read the rest data
        sendCommand(0, 100, "\r\n");
        int bytes = atoi(buffer);
        sendCommand(0);
        //sendCommand(0);
        bytes = min(bytes, sizeof(buffer) - 1);
        buffer[bytes] = 0;
        return bytes;
    } else if (ret >= 2) {
        httpState = HTTP_ERROR;
        return -1;
    }
    return 0;
}
byte CGPRS_SIM800::sendCommand(const char* cmd, unsigned int timeout, const char* expected)
//byte CGPRS_SIM800::sendCommand(String cmd, unsigned int timeout, const char* expected)
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
byte CGPRS_SIM800::sendCommand(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout)
//byte CGPRS_SIM800::sendCommand(String cmd, const char* expected1, const char* expected2, unsigned int timeout)
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
