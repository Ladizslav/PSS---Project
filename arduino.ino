#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* nazev = "Nazev WiFi site";
const char* password = "Heslo k WiFi siti";
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  connectToWiFi(); // Pripojeni k Wi-Fi
  server.on("/", HTTP_GET, handleRoot); // Obsluha korenove cesty
  server.begin(); // Spusteni serveru
}

void loop() {
  server.handleClient(); // Obsluha klientu
}

void connectToWiFi() {
  Serial.print("Pripojovani k WiFi...");
  WiFi.begin(nazev, password); // Pripojeni k Wi-Fi
  while (WiFi.status() != WL_CONNECTED) { // Cekani na pripojeni
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Pripojeno k WiFi");
}

void handleRoot() {
  String message = "Hello there, General zasuvka!"; // Zaslani zpravy klientovi
  server.send(200, "text/plain", message);
  toggleRelay();  // Prepnuti rele
  delay(10000);  // Pauza 10s 
}

void toggleRelay() {
  static bool relayState = false; // Prepnuti stavu rele
  relayState = !relayState;
  digitalWrite(LED_BUILTIN, relayState); // Nastaveni stavu
}


void handleClientRequest() {
  WiFiClient client = server.available();  // Cekani na klienta
  
  if (client)  // Pokud je klient pripojen
  {
    String currentLine = "";  // Ukladani prichozich dat
    while (client.connected())  // Klient stale pripojen
    {
      if (client.available())  // Data k dispozici
      {
        char c = client.read();  // Cteni klienta
        Serial.write(c);  // Zapis dat
        if (c == '\n')  // Konec radku
        {
          // Konec HTTP pozadavku
          if (currentLine.length() == 0) 
          {
            // Odpoved klientovi
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.println("<!DOCTYPE html><html lang=\"en\">");
            client.println("<head>");
            client.println("<meta charset=\"UTF-8\">");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
            client.println("<title>Ovladani zasuvky</title>");
            client.println("<style>");
            client.println("body { font-family: Arial, sans-serif; text-align: center; margin: 20px; }");
            client.println("button { font-size: 16px; padding: 10px 20px; margin: 10px; cursor: pointer; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>Ovladani Zasuvky</h1>");
            client.println("<button onclick=\"toggleSocket()\">Prepnout Zasuvku</button>");
            client.println("<h2>Nastaveni casovani</h2>");
            client.println("<label for=\"timing\">Cas (v sekundach):</label>");
            client.println("<input type=\"number\" id=\"timing\" name=\"timing\" min=\"1\" value=\"5\">");
            client.println("<button onclick=\"setTiming()\">Nastavit Casovani</button>");
            client.println("<script>");
            client.println("function toggleSocket() {");
            client.println("fetch('/toggleSocket')");
            client.println(".then(response => {");
            client.println("if (!response.ok) {");
            client.println("throw new Error('Network response was not ok');");
            client.println("}");
            client.println("return response.text();");
            client.println("})");
            client.println(".then(data => {");
            client.println("alert(data);");
            client.println("})");
            client.println(".catch(error => {");
            client.println("console.error('There has been a problem with your fetch operation:', error);");
            client.println("});");
            client.println("}");
            client.println("function setTiming() {");
            client.println("var timing = document.getElementById(\"timing\").value;");
            client.println("alert(\"Casovani nastaveno na \" + timing + \" sekund!\");");
            client.println("}");
            client.println("</script>");
            client.println("</body>");
            client.println("</html>");
            break;  // Ukonceni smycky pro zpracovani pozadavku
          } else {
            // Jinak vyprazdni promennou pro aktualni radek pro dalsi data od klienta
            currentLine = "";
          }
        } else if (c != '\r') {
          // Pokud to neni konec radku, pripoji prichozi znak k promenne pro aktualni radek
          currentLine += c;
        }
      }
    }
    // Uzavre spojeni s klientem
    client.stop();
  }
}
