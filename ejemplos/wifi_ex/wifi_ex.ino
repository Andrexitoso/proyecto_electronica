#include <WiFi.h>
// Replace with your network credentials
const char* ssid = "AndroidAndrews";
const char* password = "ratatouille";
WiFiClient client;
WiFiServer server(80);

void setup() 
 {
   // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid,password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();
} 

void loop() 
{
// put your main code here, to run repeatedly:
client = server.available();
if(client) 
  {
   Serial.println("Cliente disponible");
   while(client.connected()){
   int num =5;
   client.write(num);
   Serial.println("El número es: "+ String(num));
   delay(10000);
   if(!client.connected()) 

     break;
    }
  } 
}
