/*
    Dev: Hudson Miranda
    30/08/2020
*/

// Import required libraries

#include <ESP8266WiFi.h>
// - - - - - - - - - - - - - - - 
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
// - - - - - - - - - - - - - - - 
#include <WiFiUdp.h>
// - - - - - - - - - - - - - - - 
#include <NTPClient.h>
// - - - - - - - - - - - - - - - 
#include <LiquidCrystal_I2C.h>
// - - - - - - - - - - - - - - - 
#include <DHT.h>

#define DHTPIN 14 //Pino (D5) de ligacao do DHT11
#define DHTTYPE DHT11 //Define o tipo de sensor DHT

// current temperature & humidity, updated in loop()
float t = 0;
float h = 0;
float hic = 0;

// size display LCD
int lcdColumns = 20;
int lcdRows = 4;

// Char especial
byte ip[8]={B11100, B01000, B11100, B00000, B00110, B00101, B00110, B00100};
byte he[8]={B01100, B00110, B00110, B01110, B11011, B10001, B11011, B01110};
byte te[8]={B00100, B01110, B01010, B01010, B01010, B10101, B10001, B01110};
byte hu[8]={B00100, B01110, B01110, B11111, B11111, B11101, B11101, B01110};


DHT dht(DHTPIN, DHTTYPE); // DHT dht(5, DHT11);

LiquidCrystal_I2C lcd(0x3F, lcdColumns, lcdRows);

// Define object UDP and NTP Client
int timeZone = -3;
//const long offsetInSeconds = -10800;

WiFiUDP udp;
//NTPClient timeClient(udp, "b.st1.ntp.br", offsetInSeconds);
NTPClient timeClient(udp, "b.st1.ntp.br", timeZone*3600, 60000);

int hh;
int mm;
int ss;
String date;

//struct Date{
//    int day;
//    int month;
//    int year;
//    int hours;
//    int minutes;
//    int seconds;
//};

// Replace with your network credentials
const char* ssid = "TP-LINK-2";
const char* password = "mura1357";

const int relay = 2;
const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000; 

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <link rel="stylesheet" 
href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" 
integrity="sha384-
fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" 
crossorigin="anonymous">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .dht-labels{ font-size: 2rem; vertical-align:middle; padding-bottom: 15px; }
    .switch {position: relative; display: inline-block; width: 60px; height: 
34px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; 
background-color: #ccc; border-radius: 32px}
    .slider:before {position: absolute; content: ""; height: 26px; width: 26px; 
left: 4px; bottom: 4px; background-color: #fff; -webkit-transition: .4s; 
transition: .4s; border-radius: 32px}
    input:checked+.slider {background-color: #0070C0}
    input:checked+.slider:before {-webkit-transform: translateX(26px); 
-ms-transform: translateX(26px); transform: translateX(26px)}

    .onoffswitch {
        position: relative; width: 90px;
        -webkit-user-select:none; -moz-user-select:none; -ms-user-select: none;
    }
    .onoffswitch-checkbox {
        position: absolute;
        opacity: 0;
        pointer-events: none;
    }
    .onoffswitch-label {
        display: block; overflow: hidden; cursor: pointer;
        border: 2px solid #999999; border-radius: 20px;
    }
    .onoffswitch-inner {
        display: block; width: 200%; margin-left: -100%;
        transition: margin 0.3s ease-in 0s;
    }
    .onoffswitch-inner:before, .onoffswitch-inner:after {
        display: block; float: left; width: 50%; height: 30px; padding: 0; 
line-height: 30px;
        font-size: 14px; color: white; font-family: Trebuchet, Arial, 
sans-serif; font-weight: bold;
        box-sizing: border-box;
    }
    .onoffswitch-inner:before {
        content: "ON";
        padding-left: 10px;
        background-color: #34A7C1; color: #FFFFFF;
    }
    .onoffswitch-inner:after {
        content: "OFF";
        padding-right: 10px;
        background-color: #EEEEEE; color: #999999;
        text-align: right;
    }
    .onoffswitch-switch {
        display: block; width: 20px; margin: 5px;
        background: #FFFFFF;
        position: absolute; top: 0; bottom: 0;
        right: 56px;
        border: 2px solid #999999; border-radius: 20px;
        transition: all 0.3s ease-in 0s; 
    }
    .onoffswitch-checkbox:checked + .onoffswitch-label .onoffswitch-inner {
        margin-left: 0;
    }
    .onoffswitch-checkbox:checked + .onoffswitch-label .onoffswitch-switch {
        right: 0px; 
    }

  </style>
</head>
<body>
  <div>
    <h2>ESP Web Server</h2>
    %BUTTONPLACEHOLDER% 


  </div>
  <div>
  	<div>
  		
  			<span class="dht-labels" id="heat">%HEAT%</span> 
        <i class="fas fa-temperature-high" style="color:#FD7E14;"></i> 
  		
  	</div>
  	<div>
  		
  			<span class="dht-labels" id="temperature">%TEMPERATURE%</span> 
        <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
  		
  	</div>
  	<div>
  		
  			<span class="dht-labels" id="humidity">%HUMIDITY%</span>
        <i class="fas fa-tint" style="color:#00add6;"></i> 
  		
  	</div>
  </div>
<script>
function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=0", 
true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  xhr.send();
}
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("heat").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/heat", true);
  xhttp.send();
}, 10000 ) ;
</script>
</body>
</html>
)rawliteral";

String outputState(int output){
  if(digitalRead(output)){
    return "";
  }
  else {
    return "checked";
  }
}

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
     buttons += "<h3>Relay - GPIO "+(String)relay+"</h3><label 
class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" 
id=\""+(String)relay+"\" " + outputState(2) + "><span 
class=\"slider\"></span></label>";

//    buttons +=/ "<h3>Relay - GPIO " +(String)relay+ "</h3><div 
class=\"onoffswitch\"><input type=\"checkbox\" name=\"onoffswitch\" 
class=\"onoffswitch-checkbox\" id=\""+(String)relay+"\" tabindex=\"0\" 
"+outputState(relay)+"><label class=\"onoffswitch-label\" 
for=\""+(String)relay+"\"><span class=\"onoffswitch-inner\"></span><span 
class=\"onoffswitch-switch\"></span></label></div>";

    return buttons;
  }
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  else if(var == "HEAT"){
    return String(hic);
  }
  
  return String();
}

void printIpAdress(){
  lcd.setCursor(0,0);
  lcd.write(1);
  lcd.setCursor(1,0);
  Serial.println(WiFi.localIP());
  lcd.print(WiFi.localIP());
}

void getDate(){
  String date_time = timeClient.getFormattedDate();
  int index_date = date_time.indexOf("T");
  date = date_time.substring(0, index_date);
  
  //Serial.println(date+"\n");
  //Passa os dados da string para a struct
  //Date date;
  //sscanf(strDate, "%d-%d-%dT%d:%d:%dZ", 
  //                &date.year, 
  //                &date.month, 
  //                &date.day, 
  //                &date.hours, 
  //                &date.minutes,
  //                &date.seconds);
  //return date;
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  dht.begin(); //Inicializa o sensor de temperatura
  lcd.init(); // initialize LCD                 
  lcd.backlight(); // turn on LCD backlight

  lcd.createChar(1,ip);
  lcd.createChar(2,he);
  lcd.createChar(3,te);
  lcd.createChar(4,hu);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  timeClient.begin();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to 
<ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on 
<ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    Serial.print("GPIO: ");
    Serial.print(inputMessage1);
    Serial.print(" - Set to: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });

    // Send a GET request to <ESP_IP>/<data DHT>
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });
  server.on("/heat", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(hic).c_str());
  });

  // Start server
  server.begin();
}

void loop() {
  timeClient.update();

  //Recupera os dados sobre a data e horário
  //Date date = getDate();

  hh = timeClient.getHours();
  mm = timeClient.getMinutes();
  ss = timeClient.getSeconds();
  getDate();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) { 
    previousMillis = currentMillis; // save the last time you updated the DHT 
values
    
    // Reading temperature or humidity takes about 250 milliseconds!
    t = dht.readTemperature(); // Read temperature as Celsius (the default)
    //float t = dht.readTemperature(true);
    h = dht.readHumidity(); // Sensor readings may also be up to 2 seconds 'old' 
(its a very slow sensor)
    
    // Compute heat index in Celsius (isFahreheit = false)
    hic = dht.computeHeatIndex(t, h, false);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    else
    {
      lcd.clear();
      delay(1000);

      // Print ESP Local IP Address
      printIpAdress();

      // Serial.println(hic);
      lcd.setCursor(0,1);
      lcd.write(2); 
      lcd.setCursor(1,1);
      lcd.print((String)hic+"C");
      Serial.println("Heat index: "+(String)hic+"C ");

      // Serial.println(t);
      lcd.setCursor(8,1);
      lcd.write(3);
      lcd.setCursor(9,1);
      String st = String(t,1);
      lcd.print(st+"C");
      Serial.println("Temperature: "+st+"C ");

      // Serial.println(h);
      lcd.setCursor(15,1);
      lcd.write(4);
      lcd.setCursor(16,1);
      String sh = String(h,0);
      lcd.print(sh+"%");
      Serial.println("Humidity: "+sh+"%");

      
      lcd.setCursor(0,3);
      lcd.printf("%02d:%02d",hh,mm);
      lcd.setCursor(6,3);
      lcd.print(date);
      lcd.setCursor(17,3);
      lcd.print("000");
      Serial.printf("%02d:%02d:%02d\n",hh,mm,ss);
      Serial.println(date);
      
      //Exibe os dados no display
      //Serial.printf("%02d/%02d/%d\n%02d:%02d:%02d",
      //    date.day, 
      //    date.month,
      //    date.year, 
      //    date.hours,
      //    date.minutes, 
      //    date.seconds);

      Serial.println("\n");
    }
  }
}
