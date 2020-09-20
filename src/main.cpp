/*
    Dev: Hudson Miranda Sousa
    30/08/2020
*/

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <LiquidCrystal_I2C.h>
// #include <DHT.h>
#include <DHTesp.h>
#include <Keypad.h>
#include "time.h"
// #include <NTPClient.h>

#define DHTPIN 4
//#define DHTTYPE DHT11 //Define o tipo de sensor DHT
#define switchJoystick 14//35
#define eixoX 33
#define eixoY 32

struct tm timeinfo, data_alojamento, data_hoje;
timer_t hoje, alojamento;

// Char especial
byte arrow[8] = {0x00, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00};
uint8_t ip[8]={B11100, B01000, B11100, B00000, B00110, B00101, B00110, B00100};
uint8_t he[8]={B01100, B00110, B00110, B01110, B11011, B10001, B11011, B01110};
uint8_t te[8]={B00100, B01110, B01010, B01010, B01010, B10101, B10001, B01110};
byte hu[8]={B00100, B01110, B01110, B11111, B11111, B11101, B11101, B01110};
// byte hu[]={0x04,0x0e,0x0e,0x1f,0x1f,0x1d,0x1d,0x0e};

// Define sendor DHT
DHTesp dht;
float t = 0;
float h = 0;
float hic = 0;

// Define wireless connection
const char* ssid = "TP-LINK-2";
const char* password = "mura1357";

// Define NTP
// For UTC +0.00 : 0 * 60 * 60 : 0
// For UTC -3.00 : -3 * 60 * 60 : 10800
int tz = -4,//timezone
    utc = tz * 60 * 60;
const char* ntpServer =  "gps.ntp.br";//"b.st1.ntp.br";
const long  gmtOffset_sec = utc; //0;
const int   daylightOffset_sec = 3600;

// Define matrix keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns

boolean  sub_menu = 0;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

// For ESP32 Microcontroller
byte rowPins[ROWS] = {23, 3, 19, 18}; 
byte colPins[COLS] = {5, 17, 16};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String menulist[] = {"1. Data e Hora    ",
                     "2. Idade Lote     ",
                     "3. Temperatura    ",
                     "4. Umidade        ",
                     "5. Ventilacao     ",
                     "6. Nebulizacao    ",
                     "7. Aquecedor      ",
                     "8. Temporizador   ",
                     "9. Relatorio      "};

char data_aloj[] = {' ',
                    ' ',
                    ' ',
                    ' ',
                    ' ',
                    ' ',
                    ' ',
                    ' '};

// String alojamento_data = "";


int menu_num=0,
    indicator=0,
	cursor,
	dia_alojamento,
	mes_alojamento,
	ano_alojamento;

// size display LCD
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x3F,lcdColumns,lcdRows);

// Define functions
void wait(int i);
void select_menu(int index);
void list_menu();
void wr_menu();
void readJoystick();
void home();
void localTime();
void menu_op1(int &i);

void setup() {
  Serial.begin(115200);
  // dht.begin(); //Inicializa o sensor de temperatura
  dht.setup(DHTPIN,DHTesp::DHT11);
  pinMode(switchJoystick,INPUT_PULLUP); // Button Joystick
  //We create the data to be sent later using lcd.write
  
  // Switch on the backlight
  lcd.createChar(0,arrow);
  lcd.createChar(1,ip);
  lcd.createChar(2,he);
  lcd.createChar(3,te);
  // lcd.createChar(4,hu);
  lcd.load_custom_character(4,hu);

  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    WiFi.reconnect();
    // ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.init();   // initializing the LCD
  lcd.backlight(); // Enable or Turn On the backlight
  lcd.clear();

  // Init and get the time NTP
  configTime(gmtOffset_sec,daylightOffset_sec, ntpServer);
  // printLocalTime(&timeinfo);
  
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  void localTime();



  data_alojamento.tm_year=ano_alojamento-1900;
  data_alojamento.tm_mon=mes_alojamento-1;
  data_alojamento.tm_mday=dia_alojamento;
  alojamento=mktime(&data_alojamento);

  Serial.println("");

  // //disconnect WiFi as it's no longer needed
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
}

void loop() {
//  char key;// = keypad.getKey();
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    // WiFi.
    // ESP.restart();
  }
//  home();
  menu_op1(cursor);
//  Serial.printf("HOME\n- - - - - - - - - - - - - -\n");
//  delay(500);
//  if (key=='*'){
//    wr_menu();
//  }
}

void wait(int i){
    // Escape timer    
  int esc=0;
  while (analogRead(eixoX)>1000 && analogRead(eixoX)<3000 && analogRead(eixoY)>1000 && analogRead(eixoY)<3000){ // && esc<=15){
    if(esc==i){
      printf("ESC OK");
      return;
    }
    Serial.printf("%d Waiting\n",esc);
    delay(1000);
    esc++;
  }
  //Serial.printf("return HOME");
  //return;
}

void select_menu(int index){
  char save = keypad.getKey();
  Serial.printf("Index selection: %d\n- - - - - - - - - - - - - -\n",index);
  switch (index){
    case 0:
      lcd.clear();
      while (analogRead(eixoX)>1000){
        lcd.setCursor(0,0);
        lcd.print("|   DATA E HORA    |");
        lcd.setCursor(0,2); lcd.print(">");
        lcd.setCursor(1,2); lcd.print("Informe manualmente");
        delay(500);
        // if (analogRead(eixoX)>3000){
        if(save=='#'){
          lcd.clear();
          lcd.setCursor(7,1);
          lcd.print("|SAVE|");
          Serial.printf("|SAVE|");
          delay(5000);
          lcd.clear(); 
        }
      }
      Serial.printf("\n1→ Return to Menu.\n");
      return;
    case 1:
      lcd.clear();
      while (analogRead(eixoX)>1000){
        lcd.setCursor(0,0);
        lcd.print("|    IDADE LOTE    |");
        lcd.setCursor(0,2); lcd.print("Idade:");
        lcd.setCursor(7,2); lcd.print("00");
        lcd.setCursor(7,3); lcd.print("|");
        delay(500);
        // if (analogRead(eixoX)>3000){
        if(save=='#'){
          lcd.clear();
          lcd.setCursor(7,1);
          lcd.print("|SAVE|");
          Serial.printf("|SAVE|");
          delay(5000);
          lcd.clear(); 
        }
      }
      Serial.printf("2→ Return to Menu.");
      return;
    case 2:
      lcd.clear();
      while (analogRead(eixoX)>1000){
        lcd.setCursor(0,0);
        lcd.print("Message Case 3");
      }
      Serial.printf("3→ Return to Menu.");
      return;
    case 3:
      lcd.clear();
      while (analogRead(eixoX)>1000){
        lcd.setCursor(0,0);
        lcd.print("Message Case 4");
      }
      Serial.printf("4→ Return to Menu.");
      return;
    case 4:
      lcd.clear();
      while (analogRead(eixoX)>1000){
        lcd.setCursor(0,0);
        lcd.print("Message Case 5");
      }
      Serial.printf("5→ Return to Menu.");
      return;
    case 5:
     lcd.clear();
      while (analogRead(eixoX)>1000){
        lcd.setCursor(0,0);
        lcd.print("Message Case 6");
      }
      Serial.printf("6→ Return to Menu.");
      return;
    case 6:
      lcd.clear();
      while (analogRead(eixoX)>1000){
        lcd.setCursor(0,0);
        lcd.print("Message Case 7");
      }
      Serial.printf("7→ Return to Menu.");
      return;
    case 7:
      lcd.clear();
      while (analogRead(eixoX)>1000){
        lcd.setCursor(0,0);
        lcd.print("Message Case 8");
      }
      Serial.printf("8→ Return to Menu.");
      return;
    case 8:
      lcd.clear();
      while (analogRead(eixoX)>1000){
        lcd.setCursor(0,0);
        lcd.print("Message Case 9");
      }
      Serial.printf("9→ Return to Menu.");
      return;  
    default:
      break;
  }
  
}

void list_menu(){
	int i=0;
//	if(menu_num<=3) i=0;
	if(menu_num>3 && menu_num<=7) i+=4;
	else if(menu_num>7) i+=8;

	lcd.setCursor(1,0); lcd.print(menulist[i]);
	lcd.setCursor(1,1); lcd.print(menulist[i+1]);
	lcd.setCursor(1,2); lcd.print(menulist[i+2]);
	lcd.setCursor(1,3); lcd.print(menulist[i+3]);
}

void wr_menu(){
  lcd.clear();
  lcd.setCursor(0,indicator); lcd.print(">");//lcd.write(0);
  do{
    list_menu();
  } while(analogRead(eixoX)>1000 && analogRead(eixoY)>1000 && analogRead(eixoY)<3000 && digitalRead(switchJoystick)!=0);
  // } while(analogRead(eixoX)>1000 && analogRead(eixoX)<3000 && analogRead(eixoY)>1000 && analogRead(eixoY)<3000 && digitalRead(switchJoystick)!=0);

  readJoystick();
  lcd.clear();
}

void readJoystick(){
  int size = sizeof(menulist)/sizeof(menulist[0])-1;
  
  // Navigation Top
  if(analogRead(eixoY)<1000){    
    Serial.printf("\n- - - - - - - - - - - - - -\n||  TOP ||\n----------\n");
    Serial.printf("analogRead eixoY 33: %d\n",analogRead(eixoY));
    Serial.printf("Sizeof Menu: %d\n",size);
    Serial.printf("menu_num: %d\n- - - - - - - - - - - - - -|\n",menu_num);
    //Serial.printf("Menu option: %s\n",menulist[menu_num]);

    if(menu_num>0){
      Serial.printf("menu_num is: %d, and indicator is: %d\n",menu_num, indicator);
      menu_num--;
      indicator--;

      if(indicator==-1) indicator=3;

      Serial.printf("Send Up Menu, menu_num: %d, indicator: %d\n",menu_num,indicator);
      wr_menu();
    }
    else{
      Serial.printf("Begin Menu!");
      wr_menu();
    }
  }
  // Navigation Bottom
  if(analogRead(eixoY)>3000){
    Serial.printf("\n- - - - - - - - - - - - - -\n||BOTTOM||\n----------\n");
    Serial.printf("analogRead eixoY 33: %d\n",analogRead(eixoY));
    Serial.printf("Sizeof Menu: %d\n",size);
    Serial.printf("menu_num: %d\n- - - - - - - - -\n",menu_num);
    //Serial.printf("Menu option: %s\n",menulist[menu_num]);
    
    if(menu_num<size){
      Serial.printf("menu_num is: %d, and indicator is: %d\n",menu_num, indicator);
      menu_num++;
      indicator++;

      if(indicator==4) indicator=0;
      
      Serial.printf("Send Down Menu, menu_num: %d, indicator: %d\n- - - - - - - - - - - - - -\n",menu_num, indicator);
      wr_menu();
    }
    else{
      Serial.printf("End Menu!");
      wr_menu();
    }
  }
  // Escape User
  if(analogRead(eixoX)<1000){
    Serial.printf("\n General Left → Break\n- - - - - - - - - - - - - -\n");
    Serial.printf("analogRead eixoX 32: %d\n",analogRead(eixoX));
    menu_num=0;
    indicator=0;
    return;
  }
  // Select menu option
  // if(analogRead(eixoX)>3000){
  //   Serial.printf("Right → Select option: %d\n- - - - - - - - - - - - - -\n",menu_num);
  //   Serial.printf("analogRead eixoX 32: %d\n",analogRead(eixoX));
  //   select_menu(menu_num);
  // }
  if(digitalRead(switchJoystick)==0){
    Serial.printf("Select option: %d\n- - - - - - - - - - - - - -\n",menu_num);
    Serial.printf("digitalRead switchJoystick 14: %d\n",digitalRead(switchJoystick));
    select_menu(menu_num);
  }
}

void home(){ 
  lcd.home();
  // Print ESP Local IP Address
  lcd.setCursor(0,0); //lcd.print("IP:");
  lcd.write(1);
  lcd.setCursor(1,0); lcd.print(WiFi.localIP());
  Serial.println(WiFi.localIP());

  // printLocalTime();
  localTime();
  // lcd.setCursor(0,3);   lcd.write(0);
  // lcd.setCursor(1,3);   lcd.print(&timeinfo, "%d/%m/%Y");
  // lcd.setCursor(12,3);  lcd.print(&timeinfo, "%R");
  // lcd.setCursor(18,3);  lcd.print("00");
  
  // lcd.setCursor(0,3);
  // lcd.print("X:");
  // lcd.setCursor(2,3);
  // lcd.print(analogRead(eixoX));

  // lcd.setCursor(7,3);
  // lcd.print("Y:");
  // lcd.setCursor(9,3);
  // lcd.print(analogRead(eixoY));

  // lcd.setCursor(14,3);
  // lcd.print("Z:");
  // lcd.setCursor(16,3);
  // lcd.print(digitalRead(switchJoystick));

  // lcd.setCursor(0,2); lcd.;

  // lcd.setCursor(0,2); lcd.write(0);
  // lcd.setCursor(1,2); lcd.write(1);
  // lcd.setCursor(2,2); lcd.write(2);
  // lcd.setCursor(3,2); lcd.write(3);
  // lcd.setCursor(4,2); lcd.write(4);


  /** Comfort profile */
  // ComfortState cf;
  TempAndHumidity values = dht.getTempAndHumidity();
  // Reading temperature or humidity takes about 250 milliseconds!
  // t = dht.readTemperature(); // Read temperature as Celsius (the default)
  t = dht.getTemperature();
  //float t = dht.readTemperature(true);
  // h = dht.readHumidity(); // Sensor readings may also be up to 2 seconds 'old' 
  h = dht.getHumidity();
  // (its a very slow sensor)
  
  // Compute heat index in Celsius (isFahreheit = false)
  // hic = dht.computeHeatIndex(t, h, false);
  hic = dht.computeHeatIndex(values.temperature,values.humidity,false);
  // float cr = dht.getComfortRatio(cf,values.temperature,values.humidity);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else{
    delay(1000);

    // Serial.println(hic);
    lcd.setCursor(0,1); //lcd.print("H");
    lcd.write(2);
    lcd.setCursor(1,1);
    lcd.print((String)hic+"C");
    Serial.println("Heat index: "+(String)hic+"C ");

    // Serial.println(t);
    lcd.setCursor(8,1); //lcd.print("T");
    lcd.write(3);
    lcd.setCursor(9,1);
    String st = String(t,1);
    lcd.print(st+"C");
    Serial.println("Temperature: "+st+"C ");

    // Serial.println(h);
    lcd.setCursor(15,1);
    // lcd.print("U");
    lcd.write(0);
    String sh = String(h,0);
    lcd.setCursor(19,1); lcd.print(" ");
    lcd.setCursor(16,1); lcd.print(sh+"%");
    Serial.println("Humidity: "+sh+"%");

    // lcd.setCursor(0,2);
    // String ch = String(cr);
    // lcd.print(ch);
    // Serial.println("Confort: "+ch);
    
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

  ArduinoOTA.handle();
}

void localTime(){
  // struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  // Serial.println(&timeinfo);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  // Serial.print("Day of week: ");
  // Serial.println(&timeinfo, "%A");
  // Serial.print("Month: ");
  // Serial.println(&timeinfo, "%B");
  // Serial.print("Day of Month: ");
  // Serial.println(&timeinfo, "%d");
  // Serial.print("Year: ");
  // Serial.println(&timeinfo, "%Y");
  // Serial.print("Hour: ");
  // Serial.println(&timeinfo, "%H");
  // Serial.print("Hour (12 hour format): ");
  // Serial.println(&timeinfo, "%I");
  // Serial.print("Minute: ");
  // Serial.println(&timeinfo, "%M");
  // Serial.print("Second: ");
  // Serial.println(&timeinfo, "%S");

  // Serial.print("Abreviate: ");
  // Serial.println(&timeinfo, "%d-%m-%Y");

  // tm *data_hoje;//=localtime(&hoje);
  data_hoje.tm_year=timeinfo.tm_year;
  data_hoje.tm_mon=timeinfo.tm_mon;
  data_hoje.tm_mday=timeinfo.tm_mday;
  hoje=mktime(&data_hoje);

  lcd.setCursor(0,3);   lcd.write(0);
  lcd.setCursor(1,3);   lcd.print(&timeinfo, "%d/%m/%Y");
  lcd.setCursor(12,3);  lcd.print(&timeinfo, "%R");

  double idade = difftime(hoje,alojamento) / (60 * 60 * 24);;
  
  Serial.println(&data_alojamento, "%A, %B %d %Y %H:%M:%S");
  Serial.println(&data_hoje, "%A, %B %d %Y %H:%M:%S");
  Serial.println((int)idade);
  
  // lcd.setCursor(18,3); lcd.print("0"); 
  lcd.setCursor(18,3); lcd.printf("%02d",(int)idade);
  
  // Serial.println("Time variables");
  // char timeHour[3];
  // strftime(timeHour,3, "%H", &timeinfo);
  // Serial.println(timeHour);
  // char timeWeekDay[10];
  // strftime(timeWeekDay,10, "%A", &timeinfo);
  // Serial.println(timeWeekDay);
  Serial.println();
}

void menu_op1(int &i){
//	lcd.clear(); lcd.home();

	char	save = keypad.getKey();
	int		cursor[8]={9,10,12,13,15,16,17,18},
			size_cursor = sizeof(cursor)/sizeof(cursor[0])-1;

	if (i>size_cursor) {
		i=0;
	}

	lcd.setCursor(0,0); lcd.print("|    ALOJAMENTO    |");
	lcd.setCursor(0,2); lcd.print("Data:");//       /  /    ");
	lcd.setCursor(11,2); lcd.print("/");
	lcd.setCursor(14,2); lcd.print("/");
//	for (int var = 0; var < size_cursor; var++) {
//		lcd.setCursor(9,2); lcd.print(data_aloj[var]);
//	}
	lcd.setCursor(9,2); lcd.print(data_aloj[0]);
	lcd.setCursor(10,2);lcd.print(data_aloj[1]);
	lcd.setCursor(12,2);lcd.print(data_aloj[2]);
	lcd.setCursor(13,2);lcd.print(data_aloj[3]);
	lcd.setCursor(15,2);lcd.print(data_aloj[4]);
	lcd.setCursor(16,2);lcd.print(data_aloj[5]);
	lcd.setCursor(17,2);lcd.print(data_aloj[6]);
	lcd.setCursor(18,2);lcd.print(data_aloj[7]);

//	lcd.setCursor(10,2); lcd.print(data[i]);
	do {
		save = keypad.getKey();
		// Serial.printf("\ns_cursor: %d, i: %d, cursor: %d\n",size_cursor,i,cursor[i]);
		lcd.setCursor(cursor[i],2);lcd.cursor();//blink();


		if (save) {
			Serial.println(save);
			switch (save) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					data_aloj[i]=save;
					lcd.print(save);
					i++;
//		        	Serial.printf("Tecla(char): %c, Tecla(int): %d, Data[%d] = %c\n",save,save,i,data_aloj[i]);
				  break;
				case '*':
					lcd.cursor_off();
					i=0;
					lcd.clear();
				  break;
				case '#':
					String str = data_aloj;
					Serial.println(str);
					dia_alojamento=str.substring(0,2).toInt();
					mes_alojamento=str.substring(2,4).toInt();
					ano_alojamento=str.substring(4,8).toInt();


					lcd.clear();
					lcd.setCursor(7,1);
					lcd.print("|SAVE|");
					Serial.printf("|SAVE|");
					Serial.printf("\nDia: %d, Mes: %d, Ano: %d\n",dia_alojamento,mes_alojamento,ano_alojamento);
					Serial.println("String: \n"+str);
					delay(5000);
					lcd.clear();
				  break;
			}
		}
		// for (int j = 0; j <= size_cursor; j++) {
		// 	Serial.printf("\nData[%d] = %c ",j,data_aloj[j]);
		// 	Serial.printf("Dia: %d\n",dia_alojamento);
		// }
		delay(500);
	} while (analogRead(eixoX)>1000 && analogRead(eixoX)<3000 && save);

	if(analogRead(eixoX)<1000){
		if (i>0) {
			i--;
			menu_op1(i);
		} else {
			menu_op1(i);
		}
	}
	if(analogRead(eixoX)>3000){
		if (i<size_cursor) {
			i++;
			menu_op1(i);
		} else {
			menu_op1(i);
		}
	}
}
