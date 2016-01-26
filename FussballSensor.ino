#include <Bridge.h>
#include <YunClient.h>
#include <SPI.h>

const char *server = "172.30.0.206";
char page[] = "/zihack4/api/sensor/notify";

YunClient client;
char buffer[64];

const int ledPin = 13;      // led connected to digital pin 13
const int knockSensor = A0; // the piezo is connected to analog pin 0

int sensorReading = 0;      // variable to store the value read from the sensor pin
int ledState = LOW;         // variable used to store the last LED status, to toggle the light

/*Send HTTP POST request to the Azure Mobile Service data API */
void send_request()
{
  readSensor();
  
  Serial.println("connecting");
  int res = client.connect(server, 80);
  Serial.println(res);

  if (res) {
    Serial.print("sending ");
    Serial.println(sensorReading);

    // POST URI
    sprintf(buffer, "POST %s HTTP/1.1", page);
    client.println(buffer);

    // Host header
    sprintf(buffer, "Host: %s", server);
    client.println(buffer);

    // JSON content type
    client.println("Content-Type: application/json");

    // POST body
    sprintf(buffer, "{TableId: 1, Intensity: %d}", sensorReading);

    // Content length
    client.print("Content-Length: ");
    client.println(strlen(buffer));

    // End of headers
    client.println();

    // Request body
    client.println(buffer);
   } 
   else 
   {
      Serial.println("connection failed");
    }
}

 /* Wait for a response */
void wait_response()
{
  while (!client.available()) {
    if (!client.connected()) {
      return;
    }
  }
}

 /* Read the response and output to the serial monitor */
void read_response()
{
  bool print = true;

  while (client.available()) {
    char c = client.read();
    // Print only until the first carriage return
    if (c == '\n')
      print = false;
      if (print)
        Serial.print(c);
  }
  Serial.println(F(" "));
}

 /* Terminate the connection*/
void end_request()
{
  client.stop();
}

void readSensor()
{
    sensorReading = analogRead(knockSensor);
    
    char strSensorVale[5];
    sprintf(strSensorVale, "%d", sensorReading);
    
    Serial.print(F("Sensor value: "));
    Serial.println(strSensorVale);
}

 /* Arduino Yun Setup */
void setup()
{
  delay(5000);
    Serial.begin(9600);
    Serial.println("Starting Bridge");
    Bridge.begin();
}

 /* Arduino Yun Loop */
void loop()
{
  Serial.println(F(" "));
  send_request();
  wait_response();
  read_response();
//  send_request();
  delay(3000);
}
