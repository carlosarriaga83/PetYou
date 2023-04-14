#include "PETYOU.h"


//-------------------------------------------------------------------------------------//
// PORTS
//-------------------------------------------------------------------------------------//

bool BOARD::BT_BEGIN(Stream &stream) {
  BT_PORT = &stream;
  return true;
}

bool BOARD::MODEM_BEGIN(Stream &stream) {
  MODEM_PORT = &stream;
  return true;
}

bool BOARD::CON_BEGIN(Stream &stream) {
  CON_PORT = &stream;
  return true;
}

//-------------------------------------------------------------------------------------//
// CONSOLE
//-------------------------------------------------------------------------------------//

void BOARD::PRINT( int PREFIX, String TITLE , String TEXT , bool NEWLINE) {



  // TAB WITH .....
  String iTAB = "";
  int ACTUAL_LEN = TITLE.length() + (PREFIX * 8);

  for (int i = ACTUAL_LEN; i <= 40; i++) {
    iTAB = iTAB + ".";
  }

  if (TITLE.length() == 0) {
    iTAB = "";
  }

  // PRINT TO STREAM
  if (PREFIX != 0) {
    for (int i = 0; i < PREFIX; i++) {
      CON_PORT->print("\t");
    }
  }

  CON_PORT->print(TITLE);
  CON_PORT->print(iTAB);
  CON_PORT->print(TEXT);

  if (NEWLINE) {
    CON_PORT->println();
  }


  return;

}



//-------------------------------------------------------------------------------------//
// CORE
//-------------------------------------------------------------------------------------//

int BOARD::CORE_BATT(int PIN) {

  PRINT( LEVEL, "CORE BAT", "." , false);

  pinMode(PIN, INPUT); delay(10);

  float voltage0, voltage1, voltage2, V_PROM = 0;
  int x = 0;
  int RAW_V = 0;

  //if ( TX_OK_ID <= 1 ){ x = 0; } else {x = 0;}

  while (x < 2000) {

    RAW_V = analogRead(PIN);
    V_PROM = ( RAW_V * 4.05 ) / 968; delay(1);
    x++;
  }

  //CORE_BAT = V_PROM * 1000;

  PRINT( 0, F(""), String(V_PROM * 1000) , true); //con.print(RAW_V);;con.print(" ");con.println(V_PROM);

  return int(V_PROM * 1000);


}

void BOARD::LED0_TOGGLE() {

  //return;

  pinMode(LED0_PIN, OUTPUT);
  digitalWrite(LED0_PIN, !digitalRead(LED0_PIN));
  //led_breathe.Update();
  return;
}

void BOARD::LED0_ON() {

  //return;


  pinMode(LED0_PIN, OUTPUT);
  digitalWrite(LED0_PIN, HIGH);

  return;
}

void BOARD::LED0_OFF() {


  pinMode(LED0_PIN, OUTPUT);
  digitalWrite(LED0_PIN, LOW);
  pinMode(LED0_PIN, INPUT);

  return;
}



//-------------------------------------------------------------------------------------//
// I2C DEVICES
//-------------------------------------------------------------------------------------//

void BOARD::I2C_BEGIN() {

  PRINT( LEVEL, "I2C BEGIN", "." , true);

  Wire.begin();
  Wire.setClock(400000); //Increase I2C clock speed to 400kHz

}

void BOARD::I2C_END() {

  PRINT( LEVEL, F("I2C END"), "" , true);

  Wire.end();

}

void BOARD::I2C_SCAN() {


  byte error, address;
  int nDevices;


  PRINT( LEVEL, "I2C SCAN", "." , true);

  pinMode(IMU_E_PIN, OUTPUT); digitalWrite(IMU_E_PIN, HIGH);
  pinMode(I2CBUS_E_PIN, OUTPUT); digitalWrite(I2CBUS_E_PIN, HIGH);

  LEVEL++;
  I2C_BEGIN();


  LEVEL++;

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
      if (address < 16) {}
      //con.print("0x");con.print("0");
      PRINT( LEVEL, "", "\tI2C DEVICE OK: " + String(address), true);//con.print(address, HEX);
      nDevices++;
    }
    else if (error == 4)
    {

      if (address < 16) {}

      PRINT( LEVEL, "", "\tI2C DEVICE ERR: " + String(address), true);//con.print(address, HEX);con.print(address, HEX);

    }
  }

  if (nDevices == 0)
  {
    PRINT( LEVEL, "", "\tNO I2C DEVICES", true);
  }//con.print(address, HEX);}

  delay(100);           // wait 5 seconds for next scan


  I2C_END();

  pinMode(IMU_E_PIN, OUTPUT); digitalWrite(IMU_E_PIN, LOW);
  pinMode(I2CBUS_E_PIN, OUTPUT); digitalWrite(I2CBUS_E_PIN, LOW);

}

byte BOARD::TOF_SETUP(int E_PIN) {

  PRINT( LEVEL, F("TOF SETUP"), " PIN " + String(E_PIN) , true);//PRINTLN(F("TOF SETUP"));

  pinMode(E_PIN, OUTPUT); digitalWrite(E_PIN, HIGH);

  delay(100);

  TOF_I2C.setTimeout(500);

  if (!TOF_I2C.init())
  {
    PRINT( LEVEL, F(""), F("\tTOF ERROR"), true);
    //while (1);
    pinMode(E_PIN, OUTPUT); digitalWrite(IMU_E_PIN, LOW);
    return 0;
  }

  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  TOF_I2C.setDistanceMode(VL53L1X::Long);
  TOF_I2C.setMeasurementTimingBudget(50000);

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  TOF_I2C.startContinuous(50);
  //digitalWrite(ONE_E_PIN, LOW);

  //pinMode(E_PIN, OUTPUT);digitalWrite(IMU_E_PIN, LOW);

  return 1;

}

String BOARD::TOF_READ(int E_PIN) {


  PRINT( LEVEL, F("TOF READ"), " PIN " + String(E_PIN) , true);//PRINTLN(F("TOF SETUP"));

  TOF_SETUP(E_PIN);

  pinMode(E_PIN, OUTPUT); digitalWrite(IMU_E_PIN, HIGH);
  delay(100);

  int average = (TOF_I2C.read() + TOF_I2C.read() + TOF_I2C.read()) / 3;
  String temp = String(average);

  if (TOF_I2C.timeoutOccurred()) {
    digitalWrite(E_PIN, LOW);  //TIMEOUT
    return "0";
  }

  PRINT( LEVEL, "", "\t" + temp, true);

  digitalWrite(E_PIN, LOW);

  return temp;

}



//-------------------------------------------------------------------------------------//
// MODEM
//-------------------------------------------------------------------------------------//

byte BOARD::GSM_ENSURE_PWR( bool MODE) {



  if (MODE == true) {
    PRINT( LEVEL, "GSM_PWR_ON", "" , true);
  }
  if (MODE == false) {
    PRINT( LEVEL, "GSM_PWR_OFF", "" , true);
  }

  byte BYTE_TEMP;

  LEVEL++;

  BYTE_TEMP = GSM_AT();

  if (MODE == true && BYTE_TEMP == 0) {

    pinMode(GSM_DTR_PIN, OUTPUT); digitalWrite(GSM_DTR_PIN, LOW);// LOW = module can quit sleep mode.


    PRINT( LEVEL, "GSM_KEY", TABING + String(MODE) , false);
    
    pinMode(MODEM_RESET_PIN, OUTPUT);
    
    PRINT( 0, "", "H " , false);
    digitalWrite(MODEM_RESET_PIN, HIGH); delay(2000);
    PRINT( 0, "", "L " , false);
    digitalWrite(MODEM_RESET_PIN, LOW); delay(2000);
    PRINT( 0, "", "H " , false);
    digitalWrite(MODEM_RESET_PIN, HIGH);//delay(1100);

    PRINT( 0, "", TABING + "DONE" , true);

    pinMode(MODEM_RESET_PIN, INPUT);



    LEVEL--;
    return 1;

  }





  if (MODE == true && BYTE_TEMP == 1) {
    PRINT( LEVEL, "MODEM ON", OK , true);
    LEVEL--;
    return 1;
  }

  if (MODE == false && BYTE_TEMP == 0) {
    //GSM_NETLIGHT_OFF();
    LEVEL--;
    return 1;
  }

  if (MODE == false && BYTE_TEMP == 1) {

    GSM_HTTP_OFF();
    GSM_TCP_CLOSE();
    GSM_REGISTER_OFF();
    GSM_NETLIGHT_OFF();




    BYTE_TEMP = SEND_CHECK_REPLY(AT_POWER_OFF, "DOWN", ERROR, 10000, 1);




    if (BYTE_TEMP == 1) {
      PRINT( LEVEL, "MODEM OFF", OK , true);
      LEVEL--;
      PRINT( 0, "", "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" , true);
      return 1;
    }

    LEVEL--;
    PRINT( 0, "", "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" , true);
    return 0;
  }
  //GSM_NETLIGHT_OFF();
  LEVEL--;
  return 0;

}

byte BOARD::GSM_SETUP() {

  PRINT( LEVEL, "GSM_SETUP", "" , true );

  uint8_t tmp = 1 << JTD; // Disable JTAG SUPER IMPORTANTE PARA QUE EL PIN 18 FUNCIONE COMO OUTPUT HACIA EL DTR DEL MODEM!!!
  MCUCR = tmp; // Disable JTAG
  MCUCR = tmp; // Disable JTAG

  LEVEL++;
  LED0_ON();
  //GSM_ENSURE_PWR(true);


  pinMode(GSM_DTR_PIN, OUTPUT); digitalWrite(GSM_DTR_PIN, HIGH);// LOW = module can quit sleep mode.

  GSM_AT();

  SEND_CHECK_REPLY(AT_RESET_DEFAULT, OK, ERROR, 1500, 1);

  //GSM_FUN_OFF();
  SEND_CHECK_REPLY("ATZ0", OK, ERROR, 1500, 1);
  SEND_CHECK_REPLY("AT&F0", OK, ERROR, 1500, 1);
  SEND_CHECK_REPLY("ATZ", OK, ERROR, 2500, 1);

  SEND_CHECK_REPLY("AT+CBATCHK=1", OK, ERROR, 150, 1); //OVERVOLTAGE PROTECTION OFF
  SEND_CHECK_REPLY("AT+CTZU=0", OK, ERROR, 150, 1); //GET TIME
  SEND_CHECK_REPLY("AT+CGNSPWR=0", OK, ERROR, 150, 1); //GPS OFF

  GSM_SET_APN(APN);
  //GSM_FUN_ON();
  /*
    SEND_CHECK_REPLY(AT_SLOW_CLOCK_OFF, OK, ERROR, 300, 5);
    SEND_CHECK_REPLY("AT+CPSMS=0" , "OK", "ERROR", 150, 1); //POWER SAVING MODE
    SEND_CHECK_REPLY("AT+CPSMSTATUS=1", "OK", "ERROR", 250, 1); //POWER SAVING MODE

    SEND_CHECK_REPLY(AT_SLOW_CLOCK_OFF, OK, ERROR, 300, 5);
    SEND_CHECK_REPLY("AT+CPSMS=0" , "OK", "ERROR", 150, 1); //POWER SAVING MODE
    SEND_CHECK_REPLY("AT+CPSMSTATUS=1", "OK", "ERROR", 250, 1); //POWER SAVING MODE
    //SEND_CHECK_REPLY("AT+CPSMS=1,,,\"01101110\",\"00000001\"", "OK", "ERROR", 150, 1);



    //SEND_CHECK_REPLY("AT+CNMP=38",OK,ERROR,150,1); //+CNMP: ((2-Automatic),(13-GSM Only),(38-LTE Only),(51-GSM And LTE Only))
    SEND_CHECK_REPLY("AT+CNMP=2", OK, ERROR, 150, 1); //+CNMP: ((2-Automatic),(13-GSM Only),(38-LTE Only),(51-GSM And LTE Only))

    //SEND_CHECK_REPLY("AT+CMNB=1",OK,ERROR,150,1); //+CMNB: ((1-Cat-M),(2-NB-IoT),(3-Cat-M And NB-IoT))
    SEND_CHECK_REPLY("AT+CMNB=3", OK, ERROR, 150, 1); //+CMNB: ((1-Cat-M),(2-NB-IoT),(3-Cat-M And NB-IoT))


    SEND_CHECK_REPLY(AT_SLOW_CLOCK_OFF, OK, ERROR, 300, 5);

    SEND_CHECK_REPLY("AT+CREG=2", OK, ERROR, 1000, 1);



  */

  PRINT( LEVEL, "GSM_SETUP", OK , true );
  LEVEL--;


  return 1;

}


//-------------------------------------------------------------------------------------//
// TCP
//-------------------------------------------------------------------------------------//


byte BOARD::GSM_TCP_STATUS() {

  byte BYTE_TEMP;

  PRINT( LEVEL, "GSM_TCP_STATUS", "" , false );

  GSM_AT();

  BYTE_TEMP = SEND_CHECK_REPLY("AT+CASTATE?", "0,2", "0,1", 1000, 1);
  String temp = MODEM_PAYLOAD;
  temp.replace("\r", ""); temp.replace("\n", ""); temp.replace("STATE:", ""); temp.replace("AT+CIPSTATUS", "");

  if ( BYTE_TEMP == 1 ) {
    PRINT( 0, "", OK , true );
    return 1;
  }

  PRINT( 0, "", NOK , true );

  return 0;
}

byte BOARD::GSM_SOLVE_DNS() {

  //SOLVING DNS
  PRINT( LEVEL, "GSM_SOLVE_DNS", "" , true );//con.print(FIXLEN_STRING(TAB_LEN,"SOLVING DNS"));

  GSM_AT();

  String RESP = "";

  String TEMP = "AT+CDNSGIP=\"";
  TEMP = TEMP + TCP_SERVER_URL;
  TEMP = TEMP + "\",1,2000";

  SEND_CHECK_REPLY(TEMP, OK, ERROR, 2000, 1); //+CDNSGIP: 1,"pellu.ddns.net","76.68.120.152"
  SEND_CHECK_REPLY("", "+CDNSGIP:", ERROR, 1000, 5);
  SEND_CHECK_REPLY("", "\n", ERROR, 1000, 5);

  //con.print(" RESP: ");con.println(RESP);
  RESP = CLEAN(MODEM_PAYLOAD);
  RESP.replace("AT", "");
  RESP.replace("+CDNSGIP:", "");
  RESP.replace("\"", "");



  PRINT( LEVEL + 1, "PL",   TABING +  RESP , true );

  TCP_SERVER_IP = EXPLOTAR(RESP, ',', 2);

  TCP_SERVER_IP.replace(" ", "");

  PRINT( LEVEL, "GSM_SOLVE_DNS", TABING + TCP_SERVER_IP , true );



  //VERIFING IP INTEGRITY
  PRINT( LEVEL+1, "IP INTEGRITY", "" , false );//con.print(FIXLEN_STRING(TAB_LEN, "VERIFING IP INTEGRITY"));

  int i, COUNT;

  for (i = 0, COUNT = 0; TCP_SERVER_IP[i]; i++) {
    COUNT += (TCP_SERVER_IP[i] == '.');
  }

  if (TCP_SERVER_IP != "0.0.0.0" && COUNT == 3) {

    PRINT( 0, "", OK , true );
    return 1;
  } else {
    PRINT( 0, "", NOK , true );
    return 0;
  }

  return 1;

}


byte BOARD::GSM_TCP_CONNECT() {

  byte BYTE_TEMP;

  PRINT( LEVEL, "GSM_TCP_CONNECT", "" , false );//con.println(FIXLEN_STRING(TAB_LEN,"GSM_TCP_CONNECT " + HOST_IP));

  GSM_AT();

  if (TCP_SERVER_IP.length() == 0) {
    PRINT( 0, "",  " NO TCP IP", true );
    return 0;
  }


  SEND_CHECK_REPLY("AT+CNACT?", OK, ERROR, 500, 1);

  //String TEMP = "AT+CIPSTART=\"TCP\",\"petyou.ddns.net\",2345";
  String TEMP = "AT+CAOPEN=0,0,\"TCP\",\"";
  TEMP = TEMP + TCP_SERVER_IP;
  TEMP = TEMP + "\",";
  TEMP = TEMP + TCP_SERVER_PORT;

  BYTE_TEMP = SEND_CHECK_REPLY(TEMP, "0,0", ERROR, 5000, 1);

  BYTE_TEMP = SEND_CHECK_REPLY("", OK, ERROR, 5000, 1);

  if ( BYTE_TEMP == 1 ) {
    PRINT( 0, "", OK , true );  // OK
    return 1;
  }


  PRINT( 0, "", NOK , true );
  return 0;


}

byte BOARD::GSM_TCP_SEND(String QRY) {

  byte BYTE_TEMP;

  PRINT( LEVEL, "GSM_TCP_SEND", "" , true );

  LEVEL++;

  GSM_AT();

  //if ( GSM_CONNECT() == 0 ) { return 0; }

  PRINT( LEVEL, "PREPARING TO SEND", "" , false );//con.print(FIXLEN_STRING(TAB_LEN,"PREPARING TO SEND"));

  String TEMP = "AT+CASEND=0," + String(QRY.length());

  BYTE_TEMP = SEND_CHECK_REPLY(TEMP, ">", ERROR, 3000, 2);


  if ( BYTE_TEMP == 0 ||  BYTE_TEMP == 2 ) {
    PRINT( 0, "", NOK , true );  //ERROR WHEN SENDING
    LEVEL--;
    return 0;
  }

  if ( BYTE_TEMP == 1 ) {
    PRINT( 0, "", OK , true ); //OK
  }

  PRINT( LEVEL, "SENDING QRY", TABING + QRY , false );//con.print(FIXLEN_STRING(TAB_LEN,"SENDING QRY"));

  BYTE_TEMP = SEND_CHECK_REPLY(QRY, "+CADATAIND: 0", ERROR, 8000, 1);
  //BYTE_TEMP = SEND_CHECK_REPLY(QRY, OK, ERROR, 3000, 1);
  //BYTE_TEMP = SEND_CHECK_REPLY(" ", "+CADATAIND: 0", ERROR, 1000, 5);

  if ( BYTE_TEMP == 0  ) {
    PRINT( 0, "", NOK , true );  //ERROR WHEN SENDING
    LEVEL--;
    return 0;
  }

  if ( BYTE_TEMP == 1 ) {
    PRINT( 0, "", OK , true ); //OK
  }




  PRINT( LEVEL, "READING PAYLOAD", "" , true );//con.println(FIXLEN_STRING(TAB_LEN,"READING PAYLOAD"));

  SEND_CHECK_REPLY(TCP_GET_PAYLOAD, OK, ERROR, 3000, 2);

  String PAYLOAD = MODEM_PAYLOAD;
  PAYLOAD.replace("AT+CARECV=0,1024", ""); PAYLOAD.replace("\r", ""); PAYLOAD.replace("\n", ""); PAYLOAD.replace("OK", "");
  PAYLOAD.replace("+CARECV:", "");
  PAYLOAD.replace(TCP_GET_PAYLOAD, "");  // +CARECV: 19,  {"0":"05","1":"11"}


  PAYLOAD = GET_MID_STRING(PAYLOAD, "{", "}");


  PRINT( LEVEL, "TCP PAYLOAD",   TABING + PAYLOAD , true );

  PRINT( LEVEL, "DECONDING JSON", "" , false );



  DeserializationError error = deserializeJson(doc, PAYLOAD);

  if (error) {
    PRINT( 0, "", error.f_str() , true );
  } else {
    PRINT( 0, "",  OK   + TABING +  String(doc.size()) + " Elements" , true );

  }



  for (int i = 0;  i < doc.size(); i++) {
    PRINT( LEVEL + 1, "CODE #" + String(i) , TABING + String(doc[String(i)]) , true );
  }



  LEVEL--;
  return 1;


}

byte BOARD::GSM_TCP_CLOSE() {

  byte BYTE_TEMP;

  PRINT( LEVEL, "GSM_TCP_CLOSE", "" , false );

  GSM_AT();

  BYTE_TEMP = SEND_CHECK_REPLY(AT_TCP_CLOSE, OK, ERROR, 5000, 1);

  if ( BYTE_TEMP == 1 ) {
    PRINT( 0, "", OK , true );
    return 1;
  } else {
    PRINT( 0, "", NOK , true );
    return 0;
  }



}




//-------------------------------------------------------------------------------------//
// HTTP
//-------------------------------------------------------------------------------------//

byte BOARD::GSM_ENSURE_ONLINE() {


  PRINT( LEVEL, "GSM_ENSURE_ONLINE", "" , true);

  LEVEL++;
  /*
    if ( GSM_HTTP_STATUS() == 1 ) {
      PRINT( LEVEL, "GSM_HTTP_STATUS", OK , true);
      LEVEL--;
      return 1;
    }

  */
  GSM_AT();

  GSM_ECHO_ON();

  GSM_NETLIGHT_ON();

  GSM_REGISTER_OFF();



  for (int J = 0; J <= 2; J++) {
    //PRINTLN(F("========================CONNECTING....."),String(J));
    if ( IS_APN()           == 0  ) {
      GSM_SET_APN(APN);
    }
    if ( GSM_SIGNAL(250)     == 99 ) {
      PRINT( LEVEL, "NO SIGNAL" , "" , true);
      return 0;
    }

    //if ( GSM_REGISTER_ON()  == 0  ) {

    GSM_REGISTER_ON();
    //}

    //GSM_HTTP_CONF();

    if ( GSM_GET_IP(30)  == 0 ) {
      LEVEL--;
      PRINT( 0, "" , "NO HAY IP" , true);
      return 0;

    } else {

      PRINT( LEVEL, "GSM_ENSURE_ONLINE" , OK , true);
      LEVEL--;
      return 1;
    }

    //delay(1000);
    /*
      if ( GSM_HTTP_ON()  == 1 ) {
      LEVEL--;
      PRINT( LEVEL, "GSM_ENSURE_ONLINE" , OK , true);
      return 1;
      }


      if ( J >= 2 ) {
      if ( GSM_HTTP_ON()  == 1 ) {
        LEVEL--;
        PRINT( LEVEL, "GSM_ENSURE_ONLINE" , OK , true);
        return 1;
      }
      GSM_HTTP_OFF();
      }
    */

    if ( J == 1 ) {
      GSM_FUN_OFF();
      GSM_FUN_ON();
    }

    if ( J == 2 ) {
      GSM_ENSURE_PWR(0);
      GSM_ENSURE_PWR(1);
    }

  }

  //TX_NOK_ID++;
  LEVEL--;
  PRINT( LEVEL, "GSM_ENSURE_ONLINE" , NOK , true);
  return 0;


}

byte BOARD::GSM_HTTP_ON() {

  byte BYTE_TEMP;

  PRINT( LEVEL, "GSM_HTTP_ON" , "" , false);

  GSM_AT();

  LEVEL++;
  GSM_HTTP_CONF();
  LEVEL--;

  int Z = 2;

  while (Z != 0 ) {
    BYTE_TEMP = SEND_CHECK_REPLY(AT_HTTP_ON, OK, ERROR, 5000, 1);
    Z--;

    if ( BYTE_TEMP == 1 ) {
      PRINT( 0, "" , OK , true);
      return 1;
    }
  }

  //BYTE_TEMP = SEND_CHECK_REPLY(" ", OK, ERROR, 1000, 15);

  if ( BYTE_TEMP == 0 || BYTE_TEMP == 2 ) {
    PRINT( 0, "" , NOK , true);
    return 0;
  }

  PRINT( 0, "" , OK , true);

  return 1;

}

byte BOARD::GSM_HTTP_OFF() {

  byte BYTE_TEMP;

  PRINT( LEVEL, "GSM_HTTP_OFF" , "" , false);

  GSM_AT();


  BYTE_TEMP = SEND_CHECK_REPLY(AT_HTTP_OFF, OK, ERROR, 15000, 1);

  if ( BYTE_TEMP == 0 || BYTE_TEMP == 2 ) {
    //con.print(ID);
    //con.print(" ");
    PRINT( 0, "" , NOK , true);
    return 0;
  }

  PRINT( 0, "" , OK , true);

  return 1;

}

byte BOARD::GSM_HTTP_CONF() {

  GSM_AT();

  SEND_CHECK_REPLY("AT+SHCONF=\"url\",\"http://bus.turniot.com:80\"", OK, ERROR, 150, 1);

  SEND_CHECK_REPLY("AT+SHCONF=\"TIMEOUT\",30", OK, ERROR, 150, 1);

  SEND_CHECK_REPLY("AT+SHCONF=\"BODYLEN\",1024", OK, ERROR, 150, 1);

  SEND_CHECK_REPLY("AT+SHCONF=\"HEADERLEN\",350", OK, ERROR, 150, 1);

  SEND_CHECK_REPLY("AT+SHCONF=\"POLLCNT\",1", OK, ERROR, 150, 1);

  SEND_CHECK_REPLY("AT+SHCONF=\"POLLINTMS\",1000", OK, ERROR, 150, 1);

  return 1;

}

byte BOARD::GSM_HTTP_STATUS() {

  byte BYTE_TEMP;

  PRINT( LEVEL, "GSM_HTTP_STATUS", "" , false);

  GSM_AT();

  BYTE_TEMP = SEND_CHECK_REPLY(AT_GET_HTTP_STATUS, "+SHSTATE: 1", "+SHSTATE: 0", 250, 1);

  if ( BYTE_TEMP == 0 || BYTE_TEMP == 2 ) {
    PRINT( LEVEL, "", NOK , true);
    return 0;
  }

  PRINT( LEVEL, "", OK , true);

  return 1;
}

//-------------------------------------------------------------------------------------//
// MODEM FUNCIONES
//-------------------------------------------------------------------------------------//

void BOARD::GSM_PURGESERIAL() {
  while (MODEM_PORT->available()) MODEM_PORT->read();
}

byte BOARD::GSM_AT() {

  //PRINT( LEVEL, "AT", "" , false);

  byte BYTE_TEMP;

  pinMode(GSM_DTR_PIN, OUTPUT); digitalWrite(GSM_DTR_PIN, LOW);// LOW = module can quit sleep mode

  //SEND_CHECK_REPLY(AT_SLOW_CLOCK_OFF, OK, ERROR, 100, 1);

  BYTE_TEMP = SEND_CHECK_REPLY(AT, OK, ERROR, 100, 30);


  //PRINT( 0, "", String(BYTE_TEMP) , true);

  return BYTE_TEMP;
}


byte BOARD::IS_APN() {



  PRINT( LEVEL, "IS_APN", "" , false);

  String S_TEMP = "";

  S_TEMP = SEND_CHECK_REPLY(AT_IS_APN, OK, ERROR, 1500, 1);

  if ( S_TEMP.indexOf(APN) == 0 ) {
    PRINT( 0, "", NOK , true);
    return 0;
  }

  //AT_SET("AT+CNCFG=0,1,\"" + APN + "\"","OK","ERROR",500,1);


  PRINT( 0, "", OK , true);

  return 1;
}

byte BOARD::GSM_SET_APN(String APN) {

  PRINT( LEVEL, "GSM_SET_APN", "" , false );

  GSM_AT();

  SEND_CHECK_REPLY("AT+CGDCONT=1,\"IP\",\"" + APN + "\"", OK, ERROR, 500, 1);

  PRINT( 0, "", OK + TABING +  APN , true);

  return 1;
}

byte BOARD::GSM_GET_IP(int TIMEOUT) {

  byte BYTE_TEMP;

  PRINT( LEVEL, "GSM_GET_IP" , "" , true);

  LEVEL++;

  GSM_AT();
  GSM_ECHO_OFF();
  LEVEL--;

  for ( int j = 0; j < TIMEOUT; j++) {

    //SEND_CHECK_REPLY(AT_GET_CARRIER, OK, ERROR, 1000, 1);

    GSM_AT();

    BYTE_TEMP = SEND_CHECK_REPLY(AT_QRY_IP, "255.255.255.0\"", ERROR, 1000, 1);

    //PRINT( LEVEL, "AT_QRY_IP" , MODEM_PAYLOAD , true);

    if ( BYTE_TEMP != 0 ) {
      GSM_ECHO_ON();
      PRINT( LEVEL, "GSM_GET_IP" , NOK , true);
      return 0;
    }




    String IP = MODEM_PAYLOAD; //+CGCONTRDP: 1,5,"soracom.io","10.194.222.84.255.255.255.0"
    IP.replace("+CGCONTRDP:", "");
    IP.replace("AT", "");
    IP.replace("OK", "");
    IP.replace("\"", "");
    IP.replace("\n", "");
    IP.replace("\r", "");
    IP.replace(".255.255.255.0", "");
    IP.replace("", "");
    IP.replace(" ", "");

    IP = EXPLOTAR(IP, ',', 3);
    IP.replace(" ", "");
    IP.replace("*", "");


    int i, COUNT;

    for (i = 0, COUNT = 0; IP[i]; i++) {
      COUNT += (IP[i] == '.');
    }

    if (IP != "0.0.0.0" && COUNT == 3) {
      PRINT( LEVEL, "GSM_GET_IP"  , OK  + TABING + IP , true);
      GSM_ECHO_ON();
      //OLED_BAR(14, "-");
      //HOST_IP = IP;
      return 1;
    } else {
      //PRINT( 0, "" , NOK , true);
      //OLED_BAR(14, ".");
    }

  }

  GSM_ECHO_ON();
  return 0;

}

String BOARD::GSM_IMEI() {

  PRINT( LEVEL, "GSM_IMEI", "" , false);

  GSM_AT();

  String IMEI;

  SEND_CHECK_REPLY(AT_GET_IMEI, OK, ERROR, 100, 3);

  IMEI = MODEM_PAYLOAD;

  IMEI = CLEAN(IMEI);

  PRINT( 0, "", OK + TABING + IMEI, true);

  return IMEI;
}

byte BOARD::GSM_FUN_ON() {

  byte BYTE_TEMP;

  PRINT( LEVEL, "GSM_FUN_ON" , "" , true);

  GSM_AT();

  BYTE_TEMP = SEND_CHECK_REPLY("AT+CREG=2", OK, ERROR, 1000, 1);

  GSM_SET_APN(APN);

  SEND_CHECK_REPLY(AT_SLOW_CLOCK_OFF, OK, ERROR, 300, 5);

  PRINT( LEVEL + 1, "WAITING NB-IOT" , "" , false);

  BYTE_TEMP = SEND_CHECK_REPLY(AT_FUN_ON, "Ready", ERROR, 4000, 1);





  BYTE_TEMP = SEND_CHECK_REPLY( "", ": 5", ERROR, 1000, 15);
  SEND_CHECK_REPLY("", "\n", "ERROR", 1000, 4);

  if ( BYTE_TEMP != 1 ) {
    PRINT( 0, "" , NOK , true);
    return 0;
  }


  PRINT( 0, "" , OK , true);

  LEVEL++;

  //GSM_HTTP_CONF();
  LEVEL--;
  return 1;
}

byte BOARD::GSM_FUN_OFF() {

  byte BYTE_TEMP;

  PRINT( LEVEL, "GSM_FUN_OFF" , "" , false);

  GSM_AT();

  BYTE_TEMP = SEND_CHECK_REPLY(AT_FUN_OFF, OK, ERROR, 5000, 1);

  if ( BYTE_TEMP == 0 ) {
    PRINT( 0, "" , NOK , true);
    return 0;
  }

  PRINT( 0, "" , OK , true);

  LEVEL++;
  //GSM_SET_APN(APN);
  //GSM_HTTP_CONF();
  LEVEL--;

  return 1;
}

int BOARD::GSM_SIGNAL(int TIMEOUT) {

  String BER;
  byte BYTE_TEMP;
  String SENAL;


  for (int k = 1; k <= TIMEOUT; k++ ) {

    GSM_AT();

    PRINT( LEVEL, "GSM_SIGNAL" , "" , false);

    BYTE_TEMP = SEND_CHECK_REPLY(AT_GET_SIGNAL, OK, ERROR, 1000, 1); //  CHECKAR SENAL

    SENAL = MODEM_PAYLOAD;

    SENAL.replace(":", ""); SENAL.replace("+CSQ", ""); SENAL.replace("OK", ""); SENAL.replace("\n", ""); SENAL.replace("\r", ""); SENAL.replace("+", ""); SENAL.replace("AT", "");

    if ( SENAL.toInt() != 99 && SENAL.toInt() != 0 ) {
      PRINT( 0, "" , OK + TABING + String(k) + String('/') + String(TIMEOUT) + TABING + SENAL , true);
    } else {
      PRINT( 0, "" , NOK + TABING + String(k) + String('/') + String(TIMEOUT) + TABING + SENAL , true);
    }


    SENAL = EXPLOTAR(SENAL, ',', 0);

    SENAL.replace(" ", "");

    //OLED_TOP(18, SENAL);

    //BER = explotar(SENAL,',',1);

    if ( SENAL.toInt() != 99 && SENAL.toInt() != 0 ) {
      //OLED_BAR(11, "-");
      return SENAL.toInt();
    }

    delay(500);

    //LED_ERR_TOGGLE();

    delay(500);

    //LED_ERR_TOGGLE();

  }

  //OLED_BAR(11, "      ");
  //OLED_BAR(18, "  ");
  return 0;

}


byte BOARD::GSM_REGISTER_OFF() {

  byte BYTE_TEMP;

  GSM_AT();

  PRINT( LEVEL, "GSM_REGISTER_OFF", "" , false);

  BYTE_TEMP = SEND_CHECK_REPLY(AT_REGISTER_OFF, "0,DEACTIVE", ERROR, 5000, 1);

  if ( BYTE_TEMP == 1 ) {
    PRINT( 0, "", OK , true);
    return 1;
  }

  PRINT( 0, "", NOK , true);

  return 0;
}

byte BOARD::GSM_REGISTER_ON() {

  byte BYTE_TEMP;

  GSM_AT();

  PRINT( LEVEL, "GSM_REGISTER_ON", "" , false);

  int Z = 2;
  while (Z != 0) {
    BYTE_TEMP = SEND_CHECK_REPLY(AT_REGISTER_ON, "0,ACTIVE", "0,DEACTIVE", 5000, 2);
    Z--;
    if ( BYTE_TEMP == 1 ) {
      PRINT( 0, "", OK , true);
      return 1;
    }

  }



  PRINT( 0, "", NOK , true);

  return 0;
}


byte BOARD::GSM_NETLIGHT_ON() {
  PRINT( LEVEL, "GSM_NETLIGHT_ON", "" , false);
  GSM_AT();
  SEND_CHECK_REPLY(AT_NETLIGHT_ON , OK, ERROR, 150, 3); //NETLIGHT LED
  PRINT( 0, "", OK , true);
  return 1;
}

byte BOARD::GSM_NETLIGHT_OFF() {
  PRINT( LEVEL, "GSM_NETLIGHT_OFF", "" , false);
  GSM_AT();
  SEND_CHECK_REPLY(AT_NETLIGHT_OFF , OK, ERROR, 150, 3); //NETLIGHT LED
  PRINT( 0, "", OK , true);
  return 1;
}

byte BOARD::GSM_ECHO_ON() {
  byte BYTE_TEMP;
  PRINT( LEVEL, "GSM_ECHO_ON", "" , false);
  GSM_AT();
  BYTE_TEMP = SEND_CHECK_REPLY(AT_ECHO_ON, OK, ERROR, 100, 3);
  PRINT( 0, "", OK , true);
  return BYTE_TEMP;
}

byte BOARD::GSM_ECHO_OFF() {
  byte BYTE_TEMP;
  PRINT( LEVEL, "GSM_ECHO_OFF", "" , false);
  GSM_AT();
  BYTE_TEMP = SEND_CHECK_REPLY(AT_ECHO_OFF, OK, ERROR, 100, 3);
  PRINT( 0, "", OK , true);
  return BYTE_TEMP;
}

//-------------------------------------------------------------------------------------//
// SUB FUNCIONES
//-------------------------------------------------------------------------------------//

byte BOARD::SEND_CHECK_REPLY(String cmd, const char* expected1, const char* expected2, unsigned int timeout, unsigned int attempts) {
  //byte CGPRS_SIM800::sendCommand(String cmd, const char* expected1, const char* expected2, unsigned int timeout)

  char buffer[550];

  for (int i = 0; i < attempts; i++) {
    if (cmd) {

      GSM_PURGESERIAL();
      /*
        #ifdef DEBUG
        PORT.print('>');
        PORT.println(cmd);
        #endif
      */
      //Serial1.begin(38400);
      MODEM_PORT->println(cmd);
    }

    uint32_t t = millis();
    byte n = 0;
    do {

      if (MODEM_PORT->available()) {
        //PORT.print("IN");
        char c = MODEM_PORT->read();

        if (n >= sizeof(buffer) - 1) {
          //PORT.print(F("[Adjust buffer]"));
          // buffer full, discard first half
          n = sizeof(buffer) / 2 - 1;
          memcpy(buffer, buffer + sizeof(buffer) / 2, n);
        }
        buffer[n++] = c;
        buffer[n] = 0;

        MODEM_PAYLOAD = String(buffer);


        /////////////EXPECTED FIRST ANSWER
        if (strstr(buffer, expected1)) {

          /*
            #ifdef DEBUG
                  PORT.print(F("[1]"));
                  PORT.println(buffer);
            #endif
          */
          //buf = String(buffer);
          //funciones.OLED_DISP(buf, "" , 5);
          //MODEM_PAYLOAD = String(buffer);
          return 1;
        }

        /////////////EXPECTED SECOND ANSWER
        if (strstr(buffer, expected2)) {
          /*
            #ifdef DEBUG
                  PORT.print(F("[2]"));
                  PORT.println(buffer);
            #endif
          */
          //buf = String(buffer);
          //funciones.OLED_DISP(buf, "" , 5);
          //MODEM_PAYLOAD = String(buffer);
          return 2;
        }
      }

    } while (millis() - t < timeout);
    /*
      #if DEBUG
      PORT.print(F("[0]"));
      PORT.println(buffer);
      #endif
    */
    //buf = String(buffer);
    //funciones.OLED_DISP(buf, "" , 5);
    //return 0;
  }
  MODEM_PAYLOAD = String(buffer);
  return 0;
}

String BOARD::CLEAN(String s) {

  //clean from GSM localization
  s.replace(F("\r\n"), "");
  s.replace(F("\r"), "");
  s.replace(F("\n"), "");
  s.replace(F("OK"), "");
  s.replace(F("+CIPGSMLOC: 0,"), "");
  s.replace(F("+CIPGSMLOC: 0,"), "");
  s.replace(F("AT+CIPGSMLOC=1,1"), "");
  s.replace("/", "-");
  s.replace(F("UNDER-VOLTAGE WARNNING"), "");
  s.replace(F("OVER-VOLTAGE POWER DOWN"), "");
  s.replace(F("AT+SAPBR=2,1+SAPBR: 1,3,"), "");
  s.replace(F("AT+SAPBR=2,1"), "");
  s.replace(F("+SAPBR: 1,3,"), "");
  s.replace(" ", "");
  s.replace(F("+SAPBR:1,1,"), "");


  //clean GSM TEMP
  s.replace(F("+CMTE: 0,"), "");

  //clean GSM IMEI
  s.replace(F("AT+GSN"), "");

  return s;
}

String BOARD::EXPLOTAR(String data, char separator, int index) {
  int maxIndex = data.length() - 1;
  int j = 0;
  String chunkVal = "";
  data.reserve(200);
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

String BOARD::GET_MID_STRING(String str, String start, String finish) {
  int locStart = str.indexOf(start);
  if (locStart == -1) return "";
  locStart += start.length();
  int locFinish = str.indexOf(finish, locStart);
  if (locFinish == -1) return "";
  return start + str.substring(locStart, locFinish) + finish;
}
