#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

const char* ssid     = "dekip_network";      // SSID lokalne mreze
const char* password = "0611975itstreet";   // lozinka
String APIKEY = "04ca3eafe646af3f057ec37e63026bc0"; // API kljuc
String CityID = "792892"; //ID mesta za koji nam trebaju ocitavanja

WiFiClient client;
char servername[]="api.openweathermap.org";  //udaljeni server na koji se povezujemo
String result;
int  counter = 60;
String weatherDescription ="";
String weatherLocation = "";
String Country;
String Temperature;
String Humidity;
String Pressure;
String Wind;
void setup() {
  int cursorPosition=0;
  Serial.begin(115200);
  Serial.println("OLED test!");
  // Initialize display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // I2C adresa je i u ovom slučaju ista kao i kod drugih OLED ekrana
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
  display.clearDisplay();
  display.print("Hello TTGO"); //pozdravljamo TTGO
  display.display();
  delay(1000);
  display.clearDisplay();
  ///////pocinjemo povezivanje na lokalnu mrezu
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //dok se ne povezemo idu tackice
    display.setCursor(cursorPosition,2);
    display.print(".");
    display.display();
    cursorPosition=cursorPosition+10;
  }
  Serial.println("Connected");
  display.clearDisplay();
  display.print("Connected!"); //povezani smo
  display.display();
  delay(1000);

}

void loop() {
    if(counter == 60) //prikupljanje podataka svakih deset minuta
    {
      counter = 0;
      displayGettingData(); 
      delay(1000);
      getWeatherData();
    }else
    {
      counter++;
      displayWeather(weatherLocation,weatherDescription);//lokacija za gde prikazujemo podatke
      delay(5000);
      displayConditions(Temperature,Humidity,Pressure,Wind); // podaci o vremenu
      delay(5000);
    }
}

void getWeatherData() //funkcija za slanje i primanje GET zahteva
{
  if (client.connect(servername, 80)) {  //proveravamo konekciju
    client.println("GET /data/2.5/weather?id="+CityID+"&units=metric&APPID="+APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed"); //ako iste nema ide eror poruka
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1); //cekamo podatke
  while (client.connected() || client.available()) { //sve dok podataka ima
    char c = client.read();
      result = result+c;
    }

  client.stop();
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);

char jsonArray [result.length()+1];
result.toCharArray(jsonArray,sizeof(jsonArray));
jsonArray[result.length() + 1] = '\0';
// pristupamo root-u
StaticJsonBuffer<1024> json_buf;
JsonObject &root = json_buf.parseObject(jsonArray);
if (!root.success())
{
  Serial.println("parseObject() failed");
}
//prikupljamo podatke koji nam trebaju. ovde se mogu dodati ostali zahtevi
String location = root["name"];
String country = root["sys"]["country"];
float temperature = root["main"]["temp"];
float humidity = root["main"]["humidity"];
String weather = root["weather"]["main"];
String description = root["weather"]["description"];
float pressure = root["main"]["pressure"];
float wind = root["wind"]["speed"];

weatherDescription = description;
weatherLocation = location;
Country = country;
Temperature = temperature;
Humidity = humidity;
Pressure = pressure;
Wind = wind;

}

void displayWeather(String location,String description) //podaci o lokaciji
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print((char*)location.c_str());
  display.print(", ");
  display.print((char*)Country.c_str());
  display.setCursor(0,20);
  display.print((char*)description.c_str());
  display.display();
}

void displayConditions(String Temperature,String Humidity, String Pressure, String Wind)// podaci koje prikazujemo
{
  //prikaz temperature
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("T: "); 
 display.print((char*)Temperature.c_str());
 display.print("C ");
 display.display();
 delay(2000);
 
 //prikaz vlaznosti vazduha
 display.clearDisplay();
 display.setCursor(0,0);
 display.print("H: "); 
 display.print((char*)Humidity.c_str());
 display.print("%");
 display.display();
 delay(2000);

 //prikaz vazdusnog pritiska
 display.clearDisplay();
 display.setCursor(0,0);
 display.print("P: ");
 display.print((char*)Pressure.c_str());
 display.print("hPa");
 display.display();
 delay(2000);
 
  //prikaz brzine vetra
 display.clearDisplay();
 display.setCursor(0,0);
 display.print("W: ");
 display.print((char*)Wind.c_str());
 display.print("m/s");
 display.display();
 delay(2000);
 

}

void displayGettingData()
{
  display.clearDisplay();
  display.print("Getting data");
}
