/*
 * ESP8266ConfigPage.h
 *
 *  Created on: 9 mar 2023
 *      Author: Pietia2
 */

#ifndef INC_ESP8266CONFIGPAGE_H_
#define INC_ESP8266CONFIGPAGE_H_

/*
 <html><head></head><body>
 <h1><strong>Weather Station</strong></h1>
 <p><strong>Configuration Page</strong></p>
 <p>___________________________________</p>
 <form method="GET">
 Wi-Fi SSID:<br><input type="text" name="SSID">
 <br>
 Wi-Fi Password:<br><input type="password" name="PASSWORD">
 <input type="submit" value="Save"></form>
 <p>___________________________________</p>
 <form method="GET">
 API Key:<br><input type="password" name="APIKEY">
 <input type="submit" value="Save"></form>
 <p>___________________________________</p>
 <form method="GET">
 City:<br><input type="text" name="CITY">
 <br>
 Latitude :<br><input type="text" name="LATITUDE">
 <br>
 Longitude:<br><input type="text" name="LONGITUDE">
 <input type="submit" value="Save"></form>
 <p>___________________________________</p>
 <form method="GET">
 Date and time:<br><input type="datetime-local" name="DATE">
 <input type="submit" value="Save"></form>
 </body></html>
 */

const char ESP8266ConfigPage[] =
        {
                "<html><head></head><body><h1><strong>Weather Station</strong></h1><p><strong>Configuration Page</strong></p><p>___________________________________</p><form method=\"GET\">Wi-Fi SSID:<br><input type=\"text\" name=\"SSID\"><br>Wi-Fi Password:<br><input type=\"password\" name=\"PASSWORD\"><input type=\"submit\" value=\"Save\"></form><p>___________________________________</p><form method=\"GET\">API Key:<br><input type=\"password\" name=\"APIKEY\"><input type=\"submit\" value=\"Save\"></form><p>___________________________________</p><form method=\"GET\">City:<br><input type=\"text\" name=\"CITY\"><br>Latitude :<br><input type=\"text\" name=\"LATITUDE\"><br>Longitude:<br><input type=\"text\" name=\"LONGITUDE\"><input type=\"submit\" value=\"Save\"></form><p>___________________________________</p><form method=\"GET\">Date and time:<br><input type=\"datetime-local\" name=\"DATE\"><input type=\"submit\" value=\"Save\"></form></body></html>" };
#endif /* INC_ESP8266CONFIGPAGE_H_ */
