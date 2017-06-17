

#include <ESP8266WiFi.h>

IPAddress ip(192, 168, 1, 101); // this 3 lines for a fix IP-address
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

// Host
const char* host = "dweet.io";
ADC_MODE(ADC_VCC);

void setup() {
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network
    WiFi.mode(WIFI_STA);
    //WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
    WiFi.config(ip, gateway, subnet);  // (DNS not required)
    WiFi.begin("Bbox-4683E2", "EA72CBAF49");

    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);

}



void loop() {

  uint32_t getVcc = ESP.getVcc();
  Serial.print("Battery voltage is ");
  Serial.println(getVcc);

  Serial.print("Connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;}

  // This will send the request to the server
  client.print(String("GET /dweet/for/etimouesp1?battery=") + String(getVcc) + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

  delay(10);

  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");


  delay(1800000);
  
}

