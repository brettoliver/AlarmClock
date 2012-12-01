#include <LiquidCrystal.h>
#include <Wire.h>
#include <DHT.h>
#include <DS1307RTC.h>
#include <Time.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>


#define DHTPIN 9
DHT dht(DHTPIN, DHT11);

#define REDLed A3
#define GREENLed A2
#define PosUp 1
#define BuzzerPin 8

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

long StartMillis = 0;
long BlinkMillis = 0;
long DisplayTemp = 0;
#define UpdateTemp 20000
#define UpdateMessage 300000
long MessageMillis = 0;


float CurrentTemp, CurrentHum = -70;

int CurrentHour = -70;
int CurrentMin = -70;
boolean BlinkDot = true;
boolean TempOrHum = true;
boolean Display = true;


/*
byte cel[8] = {            // degrees & C in 1 character
  B01000,
  B10100,
  B01000,
  B00011,
  B00100,
  B00100,
  B00011,
  B00000
};
*/

byte cel[8] = {            // degrees sign
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
  B00000
};


byte omega[8] = {
  B01110,
  B10001,
  B10001, 
  B10001,
  B01010,
  B01010,
  B11011,
  B00000
};


byte delta[8] = {
  B00100,
  B01010,
  B10001, 
  B10001,
  B10001,
  B10001,
  B11111,
  B00000
};

byte pi[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B00000
};

byte lamda[8] = {
  B00100,
  B01010,
  B10001, 
  B10001,
  B10001,
  B10001,
  B10001,
  B00000
};


byte gamma[8] = {
  B11111,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B00000
};

byte ksi[8] = {
  B11111,
  B00000,
  B00000,
  B01110,
  B00000,
  B00000,
  B11111,
  B00000
};

byte misopanw[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000
};
  

byte misokatw[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111
};



byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
char serverName[] = "10.15.158.65";
byte ip[] = { 10, 15, 158, 105 };
byte subnet[] = { 255, 255, 255, 192 };
byte gateway[] = { 10, 15, 158, 99 };

EthernetUDP Udp;
EthernetClient client;

EthernetServer server(80);
String readString;



IPAddress timeServer(10, 15, 158, 65); // time.nist.gov NTP server
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

unsigned long unixtime;


int AlarmHour = 0;
int AlarmMinute = 0;

int RemainingHour = -70;
int RemainingMinute = -70;
int CurrentRemainingHour = -70;
int CurrentRemainingMinute = -70;
int CurrentMonth = -70;
int CurrentYear = -70;
int CurrentDay = -70;
int CurrentWeekday = -70;

boolean RedLedState = true;
boolean LastRedLedState = true;




void setup()
{
  pinMode(REDLed, OUTPUT);
  pinMode(GREENLed, OUTPUT);
  pinMode(PosUp, INPUT_PULLUP);
  lcd.createChar(1, cel);
  lcd.createChar(2, delta);
  lcd.createChar(3, pi);
  lcd.createChar(4, omega);
  lcd.createChar(5, lamda);
  lcd.createChar(6, gamma);
  lcd.createChar(7, ksi);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("  Magla  Clock");
  delay(500);
  lcd.setCursor(0,1);
  lcd.print("EKKINH");
  lcd.print((char)246);
  lcd.print("H ");
  lcd.write(2);
  lcd.print("IKTYOY");
  Ethernet.begin(mac, ip, dns, gateway, subnet);
  delay(500);
  lcd.setCursor(0,1);
  lcd.print((char)246);
  lcd.print("Y");
  lcd.write(6);
  lcd.print("XPONI");
  lcd.print((char)246);
  lcd.print("MO");
  lcd.print((char)246);
  lcd.print("... ");
  SynchronizeRTCwithNTP();
  Udp.stop();
  delay(500);
  setSyncProvider(RTC.get);  
  lcd.setCursor(0,1);
  lcd.print("EKKINH");
  lcd.print((char)246);
  lcd.print("H WEB... ");
  delay(500);
  Ethernet.begin(mac, ip);
  server.begin();
  pinMode(BuzzerPin, OUTPUT);
  dht.begin();
  // set up the LCD's number of columns and rows:
  lcd.setCursor(0,0);
  // 0123456789012345
  lcd.print("                "); 
  lcd.setCursor(0,1);
  lcd.print("                "); 
  CurrentHum = dht.readHumidity();
  CurrentTemp = dht.readTemperature();
  setSyncProvider(RTC.get);
  CurrentHour = hour();
  CurrentMin = minute();
  lcd.setCursor(0,0);
  if (CurrentHour<10)
    lcd.print("0");
  lcd.print(CurrentHour);
  lcd.setCursor(3,0);
  if (CurrentMin<10)
    lcd.print("0");
  lcd.print(CurrentMin);
  lcd.setCursor(8,0);
  if (CurrentTemp<10)
    lcd.print("0");
  lcd.print(CurrentTemp,0);
// lcd.print((char)223);
  lcd.write(1);
  lcd.print("C");
  lcd.setCursor(13,0);
  if (CurrentHum<10)
    lcd.print("0");
  lcd.print(CurrentHum,0);
  lcd.print("%");
  BlinkDot=true;
  Display=true;
  StartMillis = millis();
  BlinkMillis = millis();
  DisplayTemp = millis();
  MessageMillis = millis();
    
  
  AlarmHour = EEPROM.read(0);
  AlarmMinute = EEPROM.read(1);
  if ((AlarmHour<0) || (AlarmHour>23))
  {
    AlarmHour=0;
    EEPROM.write(0,0);
  }
  if ((AlarmMinute<0) || (AlarmMinute>59))
  {
    AlarmMinute=0;
    EEPROM.write(1,0);
  }
  
  digitalWrite(GREENLed, LOW);
  digitalWrite(REDLed, LOW);
  if (digitalRead(PosUp)==HIGH)
  {
    RedLedState=true;
    LastRedLedState=false;
    digitalWrite(GREENLed, LOW);
    digitalWrite(REDLed, HIGH);
  }
  else
  {
    RedLedState=false;
    LastRedLedState=true;
    digitalWrite(GREENLed, HIGH);
    digitalWrite(REDLed, LOW);
  }
  lcd.setCursor(2,0);
  lcd.print(":");
}


void loop()
{
  // If it's time to Alarm and it's not Sunday or Saturnday
  if ((AlarmHour==hour()) && (AlarmMinute==minute()) && (RedLedState==true) && (weekday()!=1) && (weekday()!=7))
  {
    while (digitalRead(PosUp)==HIGH)
    {
      lcd.setCursor(0,1);
      lcd.print("     ");
      lcd.write(7);
      lcd.print("Y");
      lcd.write(3);
      lcd.print("NA!     ");
      for (int i=0; i<3; i++)
        Buzzer();
      delay(100);
      lcd.setCursor(0,1);
      lcd.print("                ");
      for (int i=0; i<3; i++)
        Buzzer();
      delay(100);    
    }
    RedLedState=false;
    LastRedLedState=true;
  }
  
  // If switch is at Alarm On state and has changed state  
  if ((digitalRead(PosUp)==HIGH) && (LastRedLedState==false))
  {
    if ((weekday()!=1) && (weekday()!=7))
    {
      digitalWrite(GREENLed, LOW);
      digitalWrite(REDLed, HIGH);
    }
    else
    {
      digitalWrite(GREENLed, HIGH);
      digitalWrite(REDLed, LOW);
    }
    RedLedState=true;
    LastRedLedState=true;
    if (RemainingAlarmHour()<=6)
    {
      lcd.setCursor(0,1);
      lcd.print("A");
      lcd.write(3);
      lcd.print("OMENOYN    :  ");
      CurrentRemainingHour = -70;
      CurrentRemainingMinute = -70;
    }
    Display=true;
  }
  
  // If switch is at Alarm Off state and has changed state
  if ((digitalRead(PosUp)==LOW) && (LastRedLedState==true))
  {
    digitalWrite(GREENLed, HIGH);
    digitalWrite(REDLed, LOW);
    RedLedState=false;
    LastRedLedState=false;
    lcd.setCursor(0,1);
    lcd.print("       /  /     ");
    LastRedLedState=false;
    CurrentYear=-70;
    CurrentMonth=-70;
    CurrentDay=-70;
    CurrentWeekday=-70;
    Display=true;
  }
  
  // If switch has not change status and is at Alarm ON position
  if ((RedLedState==true) && (LastRedLedState==true))
  {
    if ((RemainingAlarmHour()<=6) && (weekday()!=1) && (weekday()!=7))          // If it's not Sunday or Saturnday
    {
      CountDown();                                                              // Countdown time
    }
    else
    {
      DisplaySomething();                                                       // Display message
    }
  }
  if ((RedLedState==false) && (LastRedLedState==false))
  {
    DisplayWeekDay();
  }

  // Every 500ms (0.5 sec) make a blink.
  if (millis()-BlinkMillis > 500)
  {
    if (BlinkDot)
    {
      lcd.setCursor(2,0);
      lcd.print(":");
    }
    else
    {
      lcd.setCursor(2,0);
      lcd.print(" ");
    }
    BlinkDot=!BlinkDot;
    BlinkMillis = millis();
  }
  
  // Update Temperature & Humidity from sensor every X
  if ((millis()-StartMillis) > UpdateTemp)
  {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(t))
    {
      if (CurrentTemp != t)
      {
        lcd.setCursor(8,0);
        if (t<10)
          lcd.print("0");
        lcd.print(t,0);
        CurrentTemp = t;
      }
    }
    if (!isnan(h))
    {
      if (CurrentHum != h)
      {
        lcd.setCursor(13,0);
        if (h<10)
          lcd.print("0");
        lcd.print(h,0);
        CurrentHum = h;
      }
    }
    StartMillis = millis();
  }

  // Display Time
  PrintTime();

  // Check if we have an Ethernet Client and then serve the web page and process the response.
  EthernetClient client = server.available();
  if (client)
  {
    while (client.connected())
    {
      lcd.setCursor(0,1);
      lcd.write(3);
      lcd.print("PO");
      lcd.print((char)246);
      lcd.print("BA");
      lcd.print((char)246);
      lcd.print("H A");
      lcd.write(3);
      lcd.print("O WEB");
      boolean currentLineIsBlank = true;
      if (client.available())
      {
        char c = client.read();
        if (readString.length() < 100)
        {
          readString += c;
        }
        if (c == '\n' && currentLineIsBlank) 
        {
          if (readString.indexOf("?") > -1)
          {
            int Pos_Hour = readString.indexOf("H");
            int Pos_Minute = readString.indexOf("M", Pos_Hour);
            int End = readString.indexOf("H", Pos_Minute);
            if(End < 0)
            {
              End = readString.length() + 1;
            }
            int bufLength = ((Pos_Minute) - (Pos_Hour+2));
            if (bufLength>3)
              bufLength=3;
            char tmpBuf[3];
            readString.substring((Pos_Hour+2), (Pos_Minute-1)).toCharArray(tmpBuf, bufLength);
            AlarmHour=atoi(tmpBuf);
            bufLength = ((End) - (Pos_Minute+2));
            if (bufLength > 3)
              bufLength = 3;
            readString.substring((Pos_Minute+2), (End-1)).toCharArray(tmpBuf, bufLength);
            AlarmMinute=atoi(tmpBuf);
            EEPROM.write(0,AlarmHour);
            EEPROM.write(1,AlarmMinute);
            lcd.setCursor(0,1);
            lcd.print(" E");
            lcd.write(6);
            lcd.print("INAN A");
            lcd.write(5);
            lcd.write(5);
            lcd.print("A");
            lcd.write(6);
            lcd.print("E");
            lcd.print((char)246);
            lcd.print(" ");
            delay(1500);            
            CurrentRemainingHour=-70;
            CurrentRemainingMinute=-70;
            CurrentDay=-70;
            CurrentWeekday=-70;
            CurrentYear=-70;
            Display=true;
          }
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<html>");
          client.println("<font face=\"Tahoma, Arial, Helvetica\" size=\"4\">");
          client.println("<BODY>");
          client.println(":: <b>Magla Alarm Clock</b> ::<BR><BR>");
          client.print("Temperature: <font color=red><b>");
          client.print(CurrentTemp,0);
          client.println("</b></font>&#176;C<BR>");
          client.print("Humidity: <font color=blue>");
          client.print(CurrentHum,0);
          client.println("</font>%<BR><BR>");
          client.print("Current Time: <font color=blue><b>");
          if (hour()<10)
            client.print("0");
          client.print(hour());
          client.print(":");
          if (minute()<10)
            client.print("0");
          client.print(minute());
          client.println("</b></font><BR>");
          client.print("Alarm: <font color=red>");
          if (AlarmHour<10)
            client.print("0");
          client.print(AlarmHour);
          client.print(":");
          if (AlarmMinute<10)
            client.print("0");
          client.print(AlarmMinute);
          client.println("</font><BR>");
          client.print("Remaining time: <font color=blue>");
          int hr=AlarmHour-hour();
          if (hr<0)
            hr+=24;
          int mn=AlarmMinute-minute();
          if (mn<0)
          {
            mn+=60;
            hr-=1;
          }
          if (hr<10)
            client.print("0");
          client.print(hr);
          client.print(":");
          if (mn<10)
            client.print("0");
          client.print(mn);
          client.println("</font><BR>");
          client.println("<BR>");
          client.println("<form method=get>Hour:<input type=text size=4 name=H> Min:<input type=text size=4 name=M>&nbsp;<input name=E type=submit value=Submit></form>");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(10);
    client.stop();
    readString="";
    delay(500);
    LastRedLedState=!LastRedLedState;
  }
  
  // Refresh Messages every X
  if (millis()-MessageMillis>UpdateMessage)
  {
    Display=true;
    MessageMillis=millis();
  }
  
  // Reset Ethernet @ 3:00 & 15:00
  if (((hour()==3) && (minute()==0)) || ((hour()==15) && (minute()==0)))
  {
    resetEthernet();
  }
}





void PrintTime()
{
  if (CurrentHour!=hour())
  {
    lcd.setCursor(0,0);
    if (hour()<10)
      lcd.print("0");
    lcd.print(hour());
    CurrentHour=hour();
  }
  if (CurrentMin!=minute())
  {
    lcd.setCursor(3,0);
    if (minute()<10)
      lcd.print("0");
    lcd.print(minute());
    CurrentMin=minute();
  }
}



// --------------------------------------------------------------------------------------------------------------------------------------------------------
// NTP RELATED

void SynchronizeRTCwithNTP()
{
  Udp.begin(8888);
  setSyncProvider(RTC.get);
  ntpupdate();
  RTC.set(unixtime+(2*60*60)); // set the RTC and the system time to the received value
  setTime(unixtime+(2*60*60));
}

void ntpupdate()
{
  sendNTPpacket(timeServer);
  delay(1000);
  if ( Udp.parsePacket() ) {
  Udp.read(packetBuffer,NTP_PACKET_SIZE); // read the packet into the buffer
  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  const unsigned long seventyYears = 2208988800UL;
  unsigned long epoch = secsSince1900 - seventyYears;
  unixtime = epoch; // Vazoyme sto unixtime thn wra se unix time format
  }
}

unsigned long sendNTPpacket(IPAddress& address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0; // Stratum, or type of clock
  packetBuffer[2] = 6; // Polling Interval
  packetBuffer[3] = 0xEC; // Peer Clock Precision
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------



void resetEthernet() {
  client.stop();
  delay(1000);
  Ethernet.begin(mac, ip, gateway, subnet);
  delay(1000);
}


void Buzzer()
{
  digitalWrite(BuzzerPin, HIGH);
  delay(50);
  digitalWrite(BuzzerPin, LOW);
  delay(50);
}


void CountDown()
{
  RemainingHour=AlarmHour-hour();
  if (RemainingHour<0)
    RemainingHour+=24;
    
  RemainingMinute=AlarmMinute-minute();
  if (RemainingMinute<0)
  {
    RemainingMinute+=60;
    RemainingHour-=1;
    if (RemainingHour<0)
      RemainingHour+=24;
  }
  
  if ((RemainingHour==0) && (RemainingMinute==1))
  {
    lcd.setCursor(11,1);
    int x=60-second();
    if (x<10)
      lcd.print("0");
    lcd.print(x);
    lcd.print("\" ");
    return;
  }
  
  if (CurrentRemainingHour!=RemainingHour)
  {
    lcd.setCursor(11,1);
    if (RemainingHour<10)
      lcd.print("0");
    lcd.print(RemainingHour);
    CurrentRemainingHour=RemainingHour;
  }
  if (CurrentRemainingMinute!=RemainingMinute)
  {
    lcd.setCursor(14,1);
    if (RemainingMinute<10)
      lcd.print("0");
    lcd.print(RemainingMinute);
    CurrentRemainingMinute=RemainingMinute;
  }
}




void DisplayWeekDay()
{
  if (CurrentWeekday!=weekday())
  {
    lcd.setCursor(1,1);
    switch (weekday())
    {
      case 1:
        lcd.print("KYP");
        break;
      case 2:
        lcd.write(2);
        lcd.print("EY");
        break;
      case 3:
        lcd.print("TPI");
        break;
      case 4:
        lcd.print("TET");
        break;
      case 5:
        lcd.write(3);
        lcd.print("EM");
        break;
      case 6:
        lcd.write(3);
        lcd.print("AP");
        break;
      case 7:
        lcd.print((char)246);
        lcd.print("AB");
        break;
    }
    CurrentWeekday=weekday();
  }
  if (CurrentDay!=day())
  {
    lcd.setCursor(5,1);
    if (day()<10)
      lcd.print("0");
    lcd.print(day());
    CurrentDay=day();
  }
  if (CurrentMonth!=month())
  {
    lcd.setCursor(8,1);
    if (month()<10)
      lcd.print("0");
    lcd.print(month());
    CurrentMonth=month();
  }
  if (CurrentYear!=year())
  {
    lcd.setCursor(11,1);
    lcd.print(year());
    CurrentYear=year();
  }
}


void RefreshState()
{
  if (digitalRead(PosUp)==HIGH)
  {
    RedLedState=true;
    LastRedLedState=true;
    lcd.setCursor(0,1);
    lcd.print("A");
    lcd.write(3);
    lcd.print("OMENOYN    :  ");
    CurrentRemainingHour = AlarmHour - 3;
    CurrentRemainingMinute = AlarmMinute - 3;
    Display=true;
  }
  else
  {
    RedLedState=false;
    LastRedLedState=false;
    lcd.setCursor(0,1);
    lcd.print("       /  /     ");
    LastRedLedState=false;
    CurrentYear=0;
    CurrentMonth=0;
    CurrentDay=0;
    CurrentWeekday=0;
    Display=true;
  }
}

void DisplaySomething()
{
  if (Display)
  {
    lcd.setCursor(0,1);
    if ((hour()>=9) && (hour()<13))
    {
      lcd.print(" KA");
      lcd.write(5);
      lcd.print("HMEPA!    ");
    }
    if ((hour()>=13) && (hour()<17))
    {
      lcd.print(" KA");
      lcd.write(5);
      lcd.print("O ME");
      lcd.print((char)246);
      lcd.print("HMEPI! ");
    }
    if ((hour()>=17) && (hour()<20))
    {
      lcd.print(" KA");
      lcd.write(5);
      lcd.print("O A");
      lcd.write(3);
      lcd.print("O");
      lcd.write(6);
      lcd.print("EYMA! ");
    }
    if ((hour()>=20) && (hour()<24))
    {
      lcd.print("   KA");
      lcd.write(5);
      lcd.print("O BPA");
      lcd.write(2);
      lcd.print("EYMA! ");
    }
    if ((hour()>=0) && (hour()<9))
    {
      lcd.print(" KA");
      lcd.write(5);
      lcd.print("HNYXTA!   ");
    }
    Display=false;
  }
}


int RemainingAlarmHour()
{
  int x=AlarmHour-hour();
  if (x<0)
    x+=24;
  return x;
}
