AlarmClock
==========

Alarm Clock with Ethernet and Temperature/Humidity Sensor.


- Arduino Uno rev.3
- Ethernet Shield
- DHT11 Temperature/Humidity Sensor
- RTC (Real Time Clock)
- 16x2 LCD HD44780 Display
- Buzzer, Switch, LEDs, 
  and about of 30k of code.

The alarm is programmable via a webpage.
It has just one switch, Alarm On/Off.
If theremaining time before alarm is more than 6 hours it displays some messages, 
else it counts down time left (in hour & minutes). If time left is less than a minute, 
is counting down seconds.

Display:
- First row: 
  Time, Temperature, Humidity
- Second row: 
  (In Alam mode): Some messages or counting down time before alarm. Red LED is on.
  (In Clock mode): Weekday, Day, Month, Year.

Networking features: 
- Will acquire IP address from DHCP server (or if is not available it will try with a static one).
- It synchronizes the RTC with a NTP server everytime is powered on (or every 7 days).
- It has a simple web page server, so you can connect to it and change the alarm time. 
  The alarm time is saved in EEPROM, so it will be remembered on power failures.

Some photos:
- https://picasaweb.google.com/104656736936976952947/Hardware#5815628038286763890

Wiring:
- It's easy. Just connect the LCD to the corresponding pins, the LEDs (with a resistor), the Buzzer etc.
  I'm pretty sure that you can do it without a how to or photos :)



* WARNING! 
  Never give the Clock's IP address to your supervisor. 
  He will login (as it's not password protected) and he will change the alarm time,
  so you have no excuse when you're late at work!