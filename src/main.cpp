/*
 * main.cpp
 *
 *  Created on: Aug 30, 2020
 *      Author: hudsonm
 */
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
// #include <NTPClient.h>
#include <Keypad.h>
#include <time.h>
// - - - - - - - - - - - - - - - - - - - - - //
#include <DataHora.h>
#include <Temperatura.h>
#include <Umidade.h>
#include <Nebulizacao.h>
#include <Ventilacao.h>
#include <Navegacao.h>
#include <Temporizador.h>
#include <Aquecedor.h>
#include <Naveg.h>
#include <Menu.h>

#define DHTPIN 4
#define DHTTYPE DHT11 //Define o tipo de sensor DHT
#define switchJoystick 14//35
#define eixoX 33
#define eixoY 32

// Char especial
// byte arrow[] = {0x00, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00};
byte ip[]={B11100, B01000, B11100, B00000, B00110, B00101, B00110, B00100};
byte he[]={B01100, B00110, B00110, B01110, B11011, B10001, B11011, B01110};
byte te[]={B00100, B01110, B01010, B01010, B01010, B10101, B10001, B01110};
byte hu[]={B00100, B01110, B01110, B11111, B11111, B11101, B11101, B01110};
byte re[]={B00000, B01110, B10101, B10111, B10001, B01110, B00000, B00000};
// uint8_t hu[]={0x04,0x0e,0x0e,0x1f,0x1f,0x1d,0x1d,0x0e};

// Define sendor DHT
DHT dht(DHTPIN,DHTTYPE);
float t = 0;
short h = 0;
float hic = 0;

// Define wireless connection
const char* ssid = "TP-LINK-2";
const char* password = "mura1357";

// Define NTP
// For UTC +0.00 : 0 * 60 * 60 : 0
// For UTC -3.00 : -3 * 60 * 60 : 10800
int tz = -3;//timezone
    // utc = tz * 60 * 60;
const char* ntpServer = "pool.ntp.org";//"b.st1.ntp.br";//"gps.ntp.br";
const long  gmtOffset_sec = 0;//utc;
const int   daylightOffset_sec = tz*3600;


// size display LCD
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x3F,lcdColumns,lcdRows);
// LiquidCrystal_I2C lcd(PCF8574A_ADDR_A21_A11_A01);

// Define matrix keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns


char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

// For keypad
byte rowPins[ROWS] = {23, 3, 19, 18};
byte colPins[COLS] = {5, 17, 16};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// String menulist2[] = {"1. Data e Hora      ",
//                      "2. Data Alojamento  ",
//                      "3. Temperatura      ",
//                      "4. Umidade          ",
//                      "5. Ventilacao       ",
//                      "6. Nebulizacao      ",
//                      "7. Aquecedor        ",
//                      "8. Temporizador     ",
//                      "9. Relatorio        "};

//Define Data Alojamento
Menu ListMenu;
DataHora Alojamento, Now;
Temperatura Temperature;
Umidade Humidity;
Nebulizacao Neb;
Ventilacao Ventilation;
Aquecedor Forno;
Temporizador Luz;

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
	  cursor;// navigations


// Define functions
void wait(int i);
void select_menu(int index);
void list_menu();
void wr_menu();
void readJoystick();
void home();
void localTime();
void menu_alojamento(int &i);
void menu_temperatura();
void menu_temperatura_ajuste(short i,short l, short c);
void menu_temperatura_max_min(char inMax[],char inMin[]);
void menu_umidade();
void menu_umidade_desejada(short i);
void menu_umidade_max_mix(short l, short c);
void MenuNebulizacao(short i,short l, short c);

void setup() {
  // lcd.begin();
  lcd.init();   // initializing the LCD
  lcd.backlight(); // Enable or Turn On the backlight
  lcd.clear();

  Luz.InitLCD();

  Serial.begin(115200);
  // dht.begin(); //Inicializa o sensor de temperatura
  dht.begin();
  pinMode(switchJoystick,INPUT_PULLUP); // Button Joystick
  //We create the data to be sent later using lcd.write

  // Switch on the backlight
  // lcd.begin(lcdColumns,lcdRows);
  // lcd.createChar(0,arrow);
  // lcd.createChar(1,ip);
  // lcd.load_custom_character(1,ip);
  lcd.createChar(0,re);
  lcd.createChar(1,ip);
  lcd.createChar(2,he);
  lcd.createChar(3,te);
  lcd.createChar(4,hu);

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

  // Init and get the time NTP
  configTime(gmtOffset_sec,daylightOffset_sec, ntpServer);
  // printLocalTime(&timeinfo);

  if(!getLocalTime(&Now.getData())){
    Serial.println("Failed to obtain time");
    return;
  }
  // localTime();

  // //disconnect WiFi as it's no longer needed
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
}

void loop() {
  // char key = keypad.getKey();
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    // WiFi.
    // ESP.restart();
  }
  home();
  //  menu_alojamento(cursor);
  //  menu_temperatura();
  //  menu_umidade();
  //  MenuNebulizacao(1,2,0);
  // Ventilation.MenuVentilation(lcd);
  // Forno.MenuAquecedor(lcd);
  // Luz.MenuTimer(lcd);
  // Luz.MenuTimer();

  delay(500);
  if(digitalRead(switchJoystick)==0){
      ListMenu.loopMenu(lcd);
  }
}

// void wait(int i){
//    // Escape timer
//  int esc=0;
//  while (analogRead(eixoX)>1000 && analogRead(eixoX)<3000 && analogRead(eixoY)>1000 && analogRead(eixoY)<3000){ // && esc<=15){
//    if(esc==i){
//      printf("ESC OK");
//      return;
//    }
//    Serial.printf("%d Waiting\n",esc);
//    delay(1000);
//    esc++;
//  }
//  //Serial.printf("return HOME");
//  //return;
// }

// void select_menu(int index){
//  char save = keypad.getKey();
//  Serial.printf("Index selection: %d\n- - - - - - - - - - - - - -\n",index);
//  switch (index){
//    case 0:
//      lcd.clear();
//      while (analogRead(eixoX)>1000){
//        lcd.setCursor(0,0);
//        lcd.print("|   DATA E HORA    |");
//        lcd.setCursor(0,2); lcd.print(">");
//        lcd.setCursor(1,2); lcd.print("Informe manualmente");
//        delay(500);
//        // if (analogRead(eixoX)>3000){
//        if(save=='#'){
//          lcd.clear();
//          lcd.setCursor(7,1);
//          lcd.print("|SAVE|");
//          Serial.printf("|SAVE|");
//          delay(5000);
//          lcd.clear();
//        }
//      }
//      Serial.printf("\n1→ Return to Menu.\n");
//      return;
//    case 1:
//      menu_alojamento(cursor);
//      return;
//    case 2:
//      menu_temperatura();
//      return;
//    case 3:
//      lcd.clear();
//      while (analogRead(eixoX)>1000){
//        lcd.setCursor(0,0);
//        lcd.print("Message Case 4");
//      }
//      Serial.printf("4→ Return to Menu.");
//      return;
//    case 4:
//      lcd.clear();
//      while (analogRead(eixoX)>1000){
//        lcd.setCursor(0,0);
//        lcd.print("Message Case 5");
//      }
//      Serial.printf("5→ Return to Menu.");
//      return;
//    case 5:
//     lcd.clear();
//      while (analogRead(eixoX)>1000){
//        lcd.setCursor(0,0);
//        lcd.print("Message Case 6");
//      }
//      Serial.printf("6→ Return to Menu.");
//      return;
//    case 6:
//      lcd.clear();
//      while (analogRead(eixoX)>1000){
//        lcd.setCursor(0,0);
//        lcd.print("Message Case 7");
//      }
//      Serial.printf("7→ Return to Menu.");
//      return;
//    case 7:
//      lcd.clear();
//      while (analogRead(eixoX)>1000){
//        lcd.setCursor(0,0);
//        lcd.print("Message Case 8");
//      }
//      Serial.printf("8→ Return to Menu.");
//      return;
//    case 8:
//      lcd.clear();
//      while (analogRead(eixoX)>1000){
//        lcd.setCursor(0,0);
//        lcd.print("Message Case 9");
//      }
//      Serial.printf("9→ Return to Menu.");
//      return;
//    default:
//      break;
//  }
// }

void home(){
  // Print ESP Local IP Address
  lcd.setCursor(0,0); lcd.write(1);
  lcd.setCursor(1,0);
  if (WiFi.waitForConnectResult() != WL_CONNECTED){
	  lcd.print("No connetion!       ");
	  Serial.println("No connetion!       ");
  }
  else{
	  lcd.print(WiFi.localIP());
	  // Serial.println(WiFi.localIP());
  }
  localTime();

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

  /** Comfort profile */
  // ComfortState cf;
  // TempAndHumidity values = dht.getTempAndHumidity();
  // Reading temperature or humidity takes about 250 milliseconds!
   t = dht.readTemperature(); // Read temperature as Celsius (the default)
  //  t = dht.getTemperature();
  //float t = dht.readTemperature(true);
   h = dht.readHumidity(); // Sensor readings may also be up to 2 seconds 'old'
  //  h = dht.getHumidity();
  // (its a very slow sensor)

  
   hic = dht.computeHeatIndex(t, h, false);
   // Compute heat index in Celsius (isFahreheit = false)
  // hic = dht.computeHeatIndex(values.temperature,values.humidity,false);
  // float cr = dht.getComfortRatio(cf,values.temperature,values.humidity);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else{
    delay(500);
    
    lcd.setCursor(0,1); lcd.write(2);
    lcd.setCursor(1,1); lcd.printf("%sC",(String)hic);

    lcd.setCursor(8,1); lcd.write(3);
    lcd.setCursor(9,1); lcd.printf("%04.1fC",t);

    lcd.setCursor(15,1); lcd.write(4);
    lcd.setCursor(16,1); lcd.printf("%02u%c",h,'%');

    // lcd.setCursor(0,2);
    // String ch = String(cr);
    // lcd.print(ch);
    // Serial.println("Confort: "+ch);
  }
  ArduinoOTA.handle();
}

void localTime(){
  // struct tm timeinfo;

//	if(!getLocalTime(&timeinfo)){
	if(!getLocalTime(&Now.getData())){
		Serial.println("Failed to obtain time");
		return;
	}
	Now.setDataT(Now.getData());
	// Serial.println(&timeinfo);
	// Serial.println(&Now.getData(), "%A, %B %d %Y %H:%M:%S");
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
//	data_hoje.tm_year=timeinfo.tm_year;
//	data_hoje.tm_mon=timeinfo.tm_mon;
//	data_hoje.tm_mday=timeinfo.tm_mday;
//	hoje=mktime(&data_hoje);

	lcd.setCursor(0,3);   lcd.write(0);
	lcd.setCursor(1,3);   lcd.print(&Now.getData(), "%d/%m/%Y");
	lcd.setCursor(12,3);  lcd.print(&Now.getData(), "%R");

	double idade = difftime(Now.getDataT(),Alojamento.getDataT()) / (60 * 60 * 24);;

	// Serial.println(&Alojamento.getData(), "%A, %B %d %Y %H:%M:%S");
	// Serial.println(&Now.getData(), "%A, %B %d %Y %H:%M:%S");
	// Serial.println((int)idade);
	// Serial.println();

	lcd.setCursor(18,3); lcd.printf("%02d",(int)idade);

	// Serial.println("Time variables");
	// char timeHour[3];
	// strftime(timeHour,3, "%H", &timeinfo);
	// Serial.println(timeHour);
	// char timeWeekDay[10];
	// strftime(timeWeekDay,10, "%A", &timeinfo);
	// Serial.println(timeWeekDay);
}

void menu_alojamento(int &i){

	char	save = keypad.getKey();
	int		cursor[8]={9,10,12,13,15,16,17,18},
			size_cursor = sizeof(cursor)/sizeof(cursor[0])-1;

//	if (i>size_cursor) i=0;

	lcd.setCursor(0,0);  lcd.print("   -|ALOJAMENTO|-   ");
	lcd.setCursor(0,2);  lcd.print("DATA:");
	lcd.setCursor(11,2); lcd.print("/");
	lcd.setCursor(14,2); lcd.print("/");

	for (int var = 0; var <=size_cursor; var++) {
		lcd.setCursor(cursor[var],2); lcd.print(data_aloj[var]);
	}
	// lcd.setCursor(9,2); lcd.print(data_aloj[0]);
	// lcd.setCursor(10,2);lcd.print(data_aloj[1]);
	// lcd.setCursor(12,2);lcd.print(data_aloj[2]);
	// lcd.setCursor(13,2);lcd.print(data_aloj[3]);
	// lcd.setCursor(15,2);lcd.print(data_aloj[4]);
	// lcd.setCursor(16,2);lcd.print(data_aloj[5]);
	// lcd.setCursor(17,2);lcd.print(data_aloj[6]);
	// lcd.setCursor(18,2);lcd.print(data_aloj[7]);

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
					if (i>size_cursor) i=0;
//		        	Serial.printf("Tecla(char): %c, Tecla(int): %d, Data[%d] = %c\n",save,save,i,data_aloj[i]);
				  break;
				case '*':
					lcd.noCursor(); //cursor_off();
					i=0;
					lcd.clear();
				  break;
				case '#':
					String str = data_aloj;
					Alojamento.setDia(str.substring(0,2).toInt());
					Alojamento.setMes(str.substring(2,4).toInt());
					Alojamento.setAno(str.substring(4,8).toInt());

					lcd.clear();
					lcd.setCursor(7,1);
					lcd.print("|SAVE|");
					lcd.noCursor(); //cursor_off();
					Serial.printf("|SAVE|");
					Serial.printf("\nDia: %d, Mes: %d, Ano: %d\n",Alojamento.getDia(), Alojamento.getMes(), Alojamento.getAno());
					// Serial.println("String: \n"+str);
					delay(5000);
					lcd.clear();
					str.~String();
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
			menu_alojamento(i);
		} else {
			menu_alojamento(i);
		}
	}
	if(analogRead(eixoX)>3000){
		if (i<size_cursor) {
			i++;
			menu_alojamento(i);
		} else {
			menu_alojamento(i);
		}
	}
}

void menu_temperatura(){
  lcd.clear();
	lcd.setCursor(0,Temperature.interator); lcd.print(">");//lcd.write(0);
  do{
    int i=0;
    lcd.setCursor(0,0); lcd.print(Temperature.title);
    lcd.setCursor(1,1); lcd.print(Temperature.menu[i]);
    lcd.setCursor(7,1); lcd.print(Temperature.modos[Temperature.modo]);
    lcd.setCursor(1,2); lcd.print(Temperature.menu[i+1]);
    lcd.setCursor(1,3); lcd.print(Temperature.menu[i+2]);
  } while (analogRead(eixoX)>1000 && analogRead(eixoX)<3000 && analogRead(eixoY)>1000 && analogRead(eixoY)<3000 && digitalRead(switchJoystick)!=0);

  int size = sizeof(Temperature.menu)/sizeof(Temperature.menu[0]);
  // Serial.printf("\nInterator: %d\n",Temperature.interator);

  //	readJoystick();

		// Navigation Top
  if(analogRead(eixoY)<1000){
    if(Temperature.interator>1){
      // menu_enum--;
      Temperature.interator--;

      if(Temperature.interator==0) Temperature.interator=3;
      menu_temperatura();
    }
    else{
      Serial.printf("Begin Menu!");
      menu_temperatura();
    }
  }

  // Navigation Bottom
  if(analogRead(eixoY)>3000){
    if(Temperature.interator<size){
      Temperature.interator++;
      // menu_enum++;

      if(Temperature.interator==4) Temperature.interator=1;
      menu_temperatura();
    }
    else{
      Serial.printf("End Menu!");
      menu_temperatura();
    }
  }
  // -=== Alternador modo ===-
  if (Temperature.interator==1){
    int t = Temperature.modos->length();
    // -=== RIGTH ===-
		if(analogRead(eixoX)>3000){
		  Temperature.modo++;
      if(Temperature.modo == 3) Temperature.modo = 0;
      Serial.printf("t: %d",t);
      Serial.printf("modo: %d",Temperature.modo);
		}
    // -=== LEFT ===-
    if(analogRead(eixoX)<1000){
			Temperature.modo--;
      if(Temperature.modo == -1) Temperature.modo = 2;
      Serial.printf("t: %d\n",t);
      Serial.printf("modo: %d\n",Temperature.modo);
		}
  }

  // -=== Selector ===-
  if(digitalRead(switchJoystick)==0){
    Serial.printf("Swtich Intetator: %d\n",Temperature.interator);
    switch (Temperature.interator){
      case 2:
        Temperature.cursor[0] = 8;
        Temperature.cursor[1] = 1;
        menu_temperatura_ajuste(0,1,0);
        break;
      case 3:
        Temperature.cursor[0] = 6;
        Temperature.cursor[1] = 2;
        char Ma[4], Mi[4];
        sprintf(Ma,"%.1f",Temperature.maximo);
        sprintf(Mi,"%.1f",Temperature.minimo);
        menu_temperatura_max_min(Ma, Mi);
        break;
      default:
        break;
    }
  }

		//End read Joystick


	lcd.clear();

}

void menu_temperatura_ajuste(short i,short l, short c){
  char	key;
  short s=0,
        ts = sizeof(Temperature.configTemperatura)/sizeof(Temperature.configTemperatura[0])-1,
        blink[] = {8,9,13,14,16};

  if(i<=2) s=0;
	else if(i>2 && i<=5) s+=3;
  else if(i>5 && i<=8) s+=6;
	else if(i>8) s+=9;

  lcd.clear();
  lcd.setCursor(0,0); lcd.print("-|CONF TEMPERATURA|-");

  lcd.setCursor(1,1);  lcd.printf("P:%02u",s);
  lcd.setCursor(6,1);  lcd.printf("I:%02.f",Temperature.configTemperatura[s][0]);
  lcd.setCursor(11,1); lcd.printf("T:%04.1fC",Temperature.configTemperatura[s][1]);
  lcd.setCursor(1,2);  lcd.printf("P:%02u",s+1);
  lcd.setCursor(6,2);  lcd.printf("I:%02.f",Temperature.configTemperatura[s+1][0]);
  lcd.setCursor(11,2); lcd.printf("T:%04.1fC",Temperature.configTemperatura[s+1][1]);
  lcd.setCursor(1,3);  lcd.printf("P:%02u",s+2);
  lcd.setCursor(6,3);  lcd.printf("I:%02.f",Temperature.configTemperatura[s+2][0]);
  lcd.setCursor(11,3); lcd.printf("T:%04.1fC",Temperature.configTemperatura[s+2][1]);
  do{
    key = keypad.getKey();
    lcd.setCursor(Temperature.cursor[0], Temperature.cursor[1]); lcd.blink();
    if(key){
      switch (key) {
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
          char aux_day[3],aux_temp[4];
          sprintf(aux_day,"%02.f",Temperature.configTemperatura[i][0]);
          sprintf(aux_temp,"%.1f",Temperature.configTemperatura[i][1]);
          Serial.printf("aux_day (String): %s\n",aux_day);
          Serial.printf("aux_temp (String): %s\n",aux_temp);
          if(c<=1){
            aux_day[c]=key;
            Serial.printf("IF: aux_day (String): %s\n",aux_day);
            Temperature.configTemperatura[i][0] = atof(aux_day);
            Temperature.configTemperatura[i][1] = atof(aux_temp);
            Serial.printf("=> Temperature (float): {%f,%f}\n",Temperature.configTemperatura[i][0],Temperature.configTemperatura[i][1]);
          }
          if(c>=2){
            short a = c-2;
            if(c==4) a = c-1;
            aux_temp[a]=key;
            Serial.printf("IF: aux_temp (String): %s\n",aux_temp);
            Temperature.configTemperatura[i][0] = atof(aux_day);
            Temperature.configTemperatura[i][1] = atof(aux_temp);
            Serial.printf("=> Temperature (float): {%f,%f}\n",Temperature.configTemperatura[i][0],Temperature.configTemperatura[i][1]);
          }
					lcd.print(key);
				  break;
				case '*':
					lcd.noCursor(); //blink_off();
					lcd.clear();
          return;
			}
    }
  } while (analogRead(eixoX)>1000 && analogRead(eixoX)<3000 && analogRead(eixoY)>1000 && analogRead(eixoY)<3000);

  //  -===LEFT===-
  if(analogRead(eixoX)<1000){
    if(Temperature.cursor[0]>blink[0]){
      c--;
      Temperature.cursor[0] = blink[c];
      Serial.printf("l: %u c: %u i: %u ts: %u cursor[%u][%u]\n",l,c,i,ts,Temperature.cursor[0],Temperature.cursor[1]);
      menu_temperatura_ajuste(i,l,c);
    }
    else{
      Serial.printf("l: %u c: %u i: %u ts: %u cursor[%u][%u]\n",l,c,i,ts,Temperature.cursor[0],Temperature.cursor[1]);
      menu_temperatura_ajuste(i,l,c);
    }

  }
  //  -===RIGTH===-
	if(analogRead(eixoX)>3000){
    if(Temperature.cursor[0]<blink[4]){
      c++;
      Temperature.cursor[0] = blink[c];
      Serial.printf("l: %u c: %u i: %u ts: %u cursor[%u][%u]\n",l,c,i,ts,Temperature.cursor[0],Temperature.cursor[1]);
      menu_temperatura_ajuste(i,l,c);
    }
    else
    {
      Serial.printf("l: %u c: %u i: %u ts: %u cursor[%u][%u]\n",l,c,i,ts,Temperature.cursor[0],Temperature.cursor[1]);
      menu_temperatura_ajuste(i,l,c);
    }

  }
  //  -===TOP===-
  if(analogRead(eixoY)<1000){
    if(i>0){
      i--;
      l--;
      if(l==0) l=3;
      Temperature.cursor[1] = l;
      Serial.printf("l: %u c: %u i: %u ts: %u cursor[%u][%u]\n",l,c,i,ts,Temperature.cursor[0],Temperature.cursor[1]);
      menu_temperatura_ajuste(i,l,c);
    }
    else{
      Temperature.cursor[1] = l;
      Serial.printf("l: %u c: %u i: %u ts: %u cursor[%u][%u]\n",l,c,i,ts,Temperature.cursor[0],Temperature.cursor[1]);
      menu_temperatura_ajuste(i,l,c);
    }
  }
  //  -===BOTTOM===-
	if(analogRead(eixoY)>3000){
        if(i<ts){
        i++;
        l++;
        if(l==4) l=1;
        Temperature.cursor[1] = l;
        Serial.printf("l: %u c: %u i: %u ts: %u cursor[%u][%u]\n",l,c,i,ts,Temperature.cursor[0],Temperature.cursor[1]);
        menu_temperatura_ajuste(i,l,c);
        }
        else{
        Temperature.cursor[1] = l;
        Serial.printf("l: %u c: %u i: %u ts: %u cursor[%u][%u]\n",l,c,i,ts,Temperature.cursor[0],Temperature.cursor[1]);
        menu_temperatura_ajuste(i,l,c);
        }


  }

}

void menu_temperatura_max_min(char inMax[],char inMin[]){
  char	key;
  // Serial.printf("inMax: %s\n",inMax);
  // Serial.printf("inMin: %s\n",inMin);
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("-|AJUSTE MAX E MIN|-");
  lcd.setCursor(1,2); lcd.printf("Max: %c.%cC",inMax[0],inMax[2]);
  lcd.setCursor(1,3); lcd.printf("Min: %c.%cC",inMin[0],inMin[2]);
  do{
    key = keypad.getKey();
    lcd.setCursor(Temperature.cursor[0], Temperature.cursor[1]); lcd.blink();
    // Serial.printf("%f, %f\n", Temperature.max, Temperature.min);
    if (key) {
      Serial.println(key);
			switch (key) {
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
					if (Temperature.cursor[1]==2){
            if (Temperature.cursor[0]==6){
              //(6,2)
              inMax[0]=key;
              Serial.printf("(%d,%d) = %c\n",
                            Temperature.cursor[0],
                            Temperature.cursor[1],
                            inMax[0]);
              Serial.printf("inMax: %s\n",inMax);
              Serial.printf("inMax[0]: %c\ninMax[1]: %c\ninMax[2]: %c\n",inMax[0],inMax[1],inMax[2]);
            }
            else{
              //(8,2)
              inMax[2]=key;
              Serial.printf("(%d,%d) = %c\n",
                            Temperature.cursor[0],
                            Temperature.cursor[1],
                            inMax[2]);
              Serial.printf("inMax: %s\n",inMax);
              Serial.printf("inMax[0]: %c\ninMax[1]: %c\ninMax[2]: %c\n",inMax[0],inMax[1],inMax[2]);
            }
          }
          else{
            if (Temperature.cursor[0]==6){
              //(6,3)
              inMin[0]=key;
              Serial.printf("(%d,%d) = %c\n",
                            Temperature.cursor[0],
                            Temperature.cursor[1],
                            inMin[0]);
              Serial.printf("inMin: %s\n",inMin);
              Serial.printf("inMin[0]: %c\ninMin[1]: %c\ninMin[2]: %c\n",inMin[0],inMin[1],inMin[2]);
            }
            else{
              //(8,3)
              inMin[2]=key;
              Serial.printf("(%d,%d) = %c\n",
                            Temperature.cursor[0],
                            Temperature.cursor[1],
                            inMin[2]);
              Serial.printf("inMin: %s\n",inMin);
              Serial.printf("inMin[0]: %c\ninMin[1]: %c\ninMin[2]: %c\n",inMin[0],inMin[1],inMin[2]);
            }
          }

					lcd.print(key);
				  break;
				case '*':
					lcd.noCursor(); //blink_off();
					lcd.clear();
          return;
				  // break;
				case '#':
          lcd.noCursor(); //blink_off();
					float tma = atof(inMax),
                tmi = atof(inMin);

          Serial.printf("ftma: %f, ftmi: %f\n",tma,tmi);

					Temperature.setMax(tma);
          Temperature.setMin(tmi);

          Serial.printf("TMax: %f, Tmi: %f\n",Temperature.maximo,Temperature.minimo);

					lcd.clear();
					lcd.setCursor(7,1);
					lcd.print("|SAVE|");
					Serial.printf("|SAVE|");
					// Serial.printf("\nDia: %d, Mes: %d, Ano: %d\n",Alojamento.getDia(), Alojamento.getMes(), Alojamento.getAno());
					// Serial.println("String: \n"+str);
					delay(5000);
					lcd.clear();
          return;
				  // break;
			}
    }
  } while (analogRead(eixoX)>1000 && analogRead(eixoX)<3000 && analogRead(eixoY)>1000 && analogRead(eixoY)<3000); //Serial.printf("modo: %d\n",Temperature.modo);

  //  -===LEFT===-
  if(analogRead(eixoX)<1000){
		if (Temperature.cursor[0]==8) {
			Temperature.cursor[0]=6;
			menu_temperatura_max_min(inMax, inMin);
		} else {
			menu_temperatura_max_min(inMax, inMin);
		}
	}
  //  -===RIGTH===-
	if(analogRead(eixoX)>3000){
		if (Temperature.cursor[0]==6) {
			Temperature.cursor[0]=8;
			menu_temperatura_max_min(inMax, inMin);
		} else {
			menu_temperatura_max_min(inMax, inMin);
		}
	}
  //  -===TOP===-
  if(analogRead(eixoY)<1000){
		if (Temperature.cursor[1]==3) {
			Temperature.cursor[1]=2;
			menu_temperatura_max_min(inMax, inMin);
		} else {
			menu_temperatura_max_min(inMax, inMin);
		}
	}
  //  -===BOTTOM===-
	if(analogRead(eixoY)>3000){
		if (Temperature.cursor[1]==2) {
			Temperature.cursor[1]=3;
			menu_temperatura_max_min(inMax, inMin);
		} else {
			menu_temperatura_max_min(inMax, inMin);
		}
	}
}

void menu_umidade(){
  // short size_menu = sizeof(Humidity.menu)/sizeof(Humidity.menu[0]);
  lcd.clear();
	lcd.setCursor(0,Humidity.iterator); lcd.print(">");//lcd.write(0);
  do{
    // int i=0;
    lcd.setCursor(0,0); lcd.print(Humidity.title);
    lcd.setCursor(1,2); lcd.print(Humidity.menu[0]);
    lcd.setCursor(1,3); lcd.print(Humidity.menu[1]);
  } while (analogRead(eixoX)>1000 && analogRead(eixoX)<3000 && analogRead(eixoY)>1000 && analogRead(eixoY)<3000 && digitalRead(switchJoystick)!=0);

  //  -===TOP===-
  if(analogRead(eixoY)<1000){
    if(Humidity.iterator>2){
      Humidity.iterator--;
      if(Humidity.iterator==1) Humidity.iterator=2;
      Serial.printf("TOP IF | iterator: %u\n",Humidity.iterator);
      menu_umidade();
    }
    else{
      Serial.printf("TOP ELSE | iterator: %u\n",Humidity.iterator);
      menu_umidade();
    }
  }
  //  -===BOTTOM===-
	if(analogRead(eixoY)>3000){
    if(Humidity.iterator<4){
      Humidity.iterator++;
      if(Humidity.iterator==4) Humidity.iterator=3;
      Serial.printf("BOTTOM IF | iterator: %u\n",Humidity.iterator);
      menu_umidade();
    }
    else{
      Serial.printf("BOTTOM ELSE | iterator: %u\n",Humidity.iterator);
      menu_umidade();
    }
  }
  // -=== Selector ===-
  if(digitalRead(switchJoystick)==0){
    Serial.printf("SwitchItetator: %d\n",Humidity.iterator);
    switch (Humidity.iterator){
      case 2:
        menu_umidade_desejada(0);
        break;
      case 3:
        menu_umidade_max_mix(6,2);
        break;
      default:
        break;
    }
  }
}

void menu_umidade_desejada(short i){
  char	key;
  short cursors[2] = {16,17};
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("-|UMIDADE DESEJADA|-");
  lcd.setCursor(0,2); lcd.printf("Umid. des.(Ud): %u%c",Humidity.desejada,'%');
  do
  {
    lcd.setCursor(cursors[i],2);
    lcd.blink();
    key = keypad.getKey();
    if(key){
      switch (key){
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
        lcd.print(key);
        char aux[2];
        sprintf(aux,"%d",Humidity.desejada);
        Serial.printf("aux (String): %s\n",aux);
        aux[i] = key;
        Serial.printf("aux (key): %s\n",aux);
        Humidity.desejada = atoi(aux);
        Serial.printf("Humidity desej.: %u\n",Humidity.desejada);
        break;
      case '*':
        lcd.noCursor(); //blink_off();
        lcd.clear();
        return;
      }
    }
  } while (analogRead(eixoX)>1000 && analogRead(eixoX)<3000);

   //  -===LEFT===-
  if(analogRead(eixoX)<1000){
    if(cursors[i]==17){
      i--;
      Serial.printf("i: %u\n",i);
      menu_umidade_desejada(i);
    }
    else{
      Serial.printf("i: %u\n",i);
      menu_umidade_desejada(i);
    }

  }
  //  -===RIGTH===-
	if(analogRead(eixoX)>3000){
    if(cursors[i]==16){
      i++;
      Serial.printf("i: %u\n",i);
      menu_umidade_desejada(i);
    }
    else{
      Serial.printf("i: %u\n",i);
      menu_umidade_desejada(i);
    }
  }
}

void menu_umidade_max_mix(short l, short c){
  char	key;
  short cursors[2] = {l,c};
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("-|UMID.  MAX E MIN|-");
  lcd.setCursor(1,2); lcd.printf("Max: %02u%c",Humidity.maxima,'%');
  lcd.setCursor(1,3); lcd.printf("Min: %02u%c",Humidity.minima,'%');
  do{
    key = keypad.getKey();
    lcd.setCursor(cursors[0], cursors[1]); lcd.blink();
    if (key) {
			switch (key) {
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
          char aux[3];
					if (c==2){  
            sprintf(aux,"%02d",Humidity.maxima);
            Serial.printf("aux (String): %s\n",aux);
            aux[(l-6)] = key;
            Serial.printf("aux (key): %s\n",aux);
            Humidity.maxima = atoi(aux);
            Serial.printf("Humidity max.: %u\n",Humidity.maxima);
          }
          else{
            sprintf(aux,"%02d",Humidity.minima);
            Serial.printf("aux (String): %s\n",aux);
            aux[(l-6)] = key;
            Serial.printf("aux (key): %s\n",aux);
            Humidity.minima = atoi(aux);
            Serial.printf("Humidity min.: %u\n",Humidity.minima);
          }
					lcd.print(key);
				  break;
				case '*':
					lcd.noCursor(); //blink_off();
					lcd.clear();
          return;
				  // break;
			}
    }
  } while (analogRead(eixoX)>1000 && analogRead(eixoX)<3000 && analogRead(eixoY)>1000 && analogRead(eixoY)<3000); //Serial.printf("modo: %d\n",Temperature.modo);

  //  -===LEFT===-
  if(analogRead(eixoX)<1000){
		if (cursors[0]==7) {
			l=6;
			menu_umidade_max_mix(l,c);
		} else menu_umidade_max_mix(l,c);
	}
  //  -===RIGTH===-
	if(analogRead(eixoX)>3000){
		if (cursors[0]==6) {
			l=7;
			menu_umidade_max_mix(l,c);
		} else menu_umidade_max_mix(l,c);
	}
  //  -===TOP===-
  if(analogRead(eixoY)<1000){
		if (cursors[1]==3) {
			c=2;
			menu_umidade_max_mix(l,c);
		} else menu_umidade_max_mix(l,c);
	}
  //  -===BOTTOM===-
	if(analogRead(eixoY)>3000){
		if (cursors[1]==2) {
			c=3;
			menu_umidade_max_mix(l,c);
		} else menu_umidade_max_mix(l,c);
	}
}

void MenuNebulizacao(short page,short l, short c){
  char	key;
  lcd.clear();
  lcd.setCursor(0,0); lcd.print(Neb.title);
  lcd.setCursor(0,1); lcd.printf("Pag.%u/4",page);
  lcd.setCursor(10,1); lcd.print("Nb1 |");lcd.setCursor(16,1); lcd.print("Nb2");
  
  if(page==1){
    lcd.setCursor(0,2); lcd.printf("Temp.L - %04.1fC|%04.1fC",Neb.temp[0][0],Neb.temp[0][1]);
    lcd.setCursor(0,3); lcd.printf("Temp.D - %04.1fC|%04.1fC",Neb.temp[1][0],Neb.temp[1][1]);
  }
  else if(page==2){
    lcd.setCursor(0,2); lcd.printf("Umid.L -  %02u%c | %02u%c",Neb.umid[0][0],'%',Neb.umid[0][1],'%');
    lcd.setCursor(0,3); lcd.printf("Umid.D -  %02u%c | %02u%c",Neb.umid[1][0],'%',Neb.umid[1][1],'%');
  }
  else if(page==3){
    lcd.setCursor(0,2); lcd.printf("Time.L - %04us|%04us",Neb.time[0][0],Neb.time[0][1]);
    lcd.setCursor(0,3); lcd.printf("Time.D - %04us|%04us",Neb.time[1][0],Neb.time[1][1]);
  }
  else if(page==4){
    lcd.setCursor(0,2); lcd.printf("Grupo On - %02u |  %02u",Neb.g_on[0],Neb.g_on[1]);
  }

  do{
    key = keypad.getKey();
    switch (page){
      case 1:
        lcd.setCursor(Neb.cursorTemp[c],l); lcd.blink();
        break;
      case 2:
        lcd.setCursor(Neb.cursorUmid[c],l); lcd.blink();
        break;
      case 3:
        lcd.setCursor(Neb.cursorTime[c],l); lcd.blink();
        break;
      case 4:
        lcd.setCursor(Neb.cursorGon[c],l); lcd.blink();
        break;
      
      default:
        break;
    }

    if (key) {
			switch (key) {
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
          switch (page){
            char aux[8];
            short a;
            case 1:
              if(l==2){
                if(c>=0 && c<=2){
                  if(c==2) a=c+1; else a=c;
                  sprintf(aux,"%04.1f",Neb.temp[0][0]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[a]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.temp[0][0] = atof(aux);
                  Serial.printf("=> NebTemp[0][0] (float): {%f}\n",Neb.temp[0][0]);
                }else if(c>=3 && c<=5){
                  if(c==5) a=c-2; else a=c-3;
                  sprintf(aux,"%04.1f",Neb.temp[0][1]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[a]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.temp[0][1] = atof(aux);
                  Serial.printf("=> NebTemp[0][1] (float): {%f}\n",Neb.temp[0][1]);
                }
              }else if (l==3){
                if(c>=0 && c<=2){
                  if(c==2) a=c+1; else a=c;
                  sprintf(aux,"%04.1f",Neb.temp[1][0]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[a]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.temp[1][0] = atof(aux);
                  Serial.printf("=> NebTemp[1][0] (float): {%f}\n",Neb.temp[1][0]);
                }else if(c>=3 && c<=5){
                  if(c==5) a=c-2; else a=c-3;
                  sprintf(aux,"%04.1f",Neb.temp[1][1]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[a]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.temp[1][1] = atof(aux);
                  Serial.printf("=> NebTemp[1][1] (float): {%f}\n",Neb.temp[1][1]);
                }
              }
              break;
            case 2:
              if(l==2){
                if(c==0 || c==1){
                  sprintf(aux,"%02u",Neb.umid[0][0]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[c]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.umid[0][0] = atoi(aux);
                  Serial.printf("=> NebUmid[0][0] (short): {%u}\n",Neb.umid[0][0]);
                }else if(c==2 || c==3){
                  a=c-2;
                  sprintf(aux,"%02u",Neb.umid[0][1]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[a]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.umid[0][1] = atoi(aux);
                  Serial.printf("=> NebUmid[0][1] (short): {%u}\n",Neb.umid[0][1]);
                }
              }else if (l==3){
                if(c>=0 && c<=1){
                  sprintf(aux,"%02u",Neb.umid[1][0]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[c]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.umid[1][0] = atoi(aux);
                  Serial.printf("=> NebUmid[1][0] (short): {%u}\n",Neb.umid[1][0]);
                }else if(c>=2 && c<=3){
                  a=c-2;
                  sprintf(aux,"%02u",Neb.umid[1][1]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[a]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.umid[1][1] = atoi(aux);
                  Serial.printf("=> NebUmid[1][1] (short): {%u}\n",Neb.umid[1][1]);
                }
              }
              break;
            case 3:
              if(l==2){
                if(c>=0 && c<=3){
                  sprintf(aux,"%04u",Neb.time[0][0]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[c]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.time[0][0] = atoi(aux);
                  Serial.printf("=> NebTime[0][0] (short): {%u}\n",Neb.time[0][0]);
                }else if(c>=4 && c<=7){
                  a=c-4;
                  sprintf(aux,"%04u",Neb.time[0][1]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[a]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.time[0][1] = atoi(aux);
                  Serial.printf("=> NebTime[0][1] (short): {%u}\n",Neb.time[0][1]);
                }
              }else if (l==3){
                if(c>=0 && c<=3){
                  sprintf(aux,"%04u",Neb.time[1][0]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[c]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.time[1][0] = atoi(aux);
                  Serial.printf("=> NebTime[1][0] (short): {%u}\n",Neb.time[1][0]);
                }else if(c>=4 && c<=7){
                  a=c-4;
                  sprintf(aux,"%04u",Neb.time[1][1]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[a]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.time[1][1] = atoi(aux);
                  Serial.printf("=> NebTime[1][1] (short): {%u}\n",Neb.time[1][1]);
                }
              }
              break;
            case 4:
              if(c==0 || c==1){
                  sprintf(aux,"%02u",Neb.g_on[0]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[c]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.g_on[0] = atoi(aux);
                  Serial.printf("=> NebG_on[0] (short): {%u}\n",Neb.g_on[0]);
                }else if(c==2 || c==3){
                  a=c-2;
                  sprintf(aux,"%02u",Neb.g_on[1]);
                  Serial.printf("aux (String): %s\n",aux);
                  aux[a]=key;
                  Serial.printf("aux (%ux%u): %s\n",c,l,aux);
                  Neb.g_on[1] = atoi(aux);
                  Serial.printf("=> NebG_on[1] (short): {%u}\n",Neb.g_on[1]);
                }
              break;
            
            default:
              break;
          }
					lcd.print(key);
				  break;
				case '*':
					lcd.noCursor(); //blink_off();
					lcd.clear();
          return;
				  // break;
			}
    }
     
  } while (analogRead(eixoX)>1000 && 
           analogRead(eixoX)<3000 && 
           analogRead(eixoY)>1000 && 
           analogRead(eixoY)<3000 && 
           digitalRead(switchJoystick)!=0);

  
  //  -===LEFT===-
  if(analogRead(eixoX)<1000){
    switch (page){
    case 1:
      if(Neb.cursorTemp[c]>9){
        c--;
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      else{
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      break;
    case 2:
      if(Neb.cursorUmid[c]>10){
        c--;
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      else{
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      break;
    case 3:
      if(Neb.cursorTime[c]>9){
        c--;
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      else{
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      break;
    case 4:
      if(Neb.cursorGon[c]>11){
        c--;
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      else{
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      break;
    
    default:
      break;
    }
  }
  //  -===RIGTH===-
	if(analogRead(eixoX)>3000){
    switch (page){
    case 1:
      if(Neb.cursorTemp[c]<18){
        c++;
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      else{
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      break;
    case 2:
      if(Neb.cursorUmid[c]<17){
        c++;
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      else{
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      break;
    case 3:
      if(Neb.cursorTime[c]<18){
        c++;
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      else{
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      break;
    case 4:
      if(Neb.cursorGon[c]<18){
        c++;
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      else{
        Serial.printf("l: %u c: %u page: %u \n",l,c,page);
        MenuNebulizacao(page,l,c);
      }
      break;
    
    default:
      break;
    }    
  }
  //  -===TOP===-
  if(analogRead(eixoY)<1000){
    if(page>=1){
      l--;
      if(l==1) {l=3;page--;}
      if(page==0) {l=2;page=1;}
      // Temperature.cursor[1] = l;
      Serial.printf("l: %u c: %u page: %u \n",l,c,page);
      MenuNebulizacao(page,l,c);
    }
    else{
      // Temperature.cursor[1] = l;
      Serial.printf("l: %u c: %u page: %u \n",l,c,page);
      MenuNebulizacao(page,l,c);
    }
  }
  //  -===BOTTOM===-
	if(analogRead(eixoY)>3000){
    if(page<4){
      l++;
      if(l==4){l=2;page++;}
      // Temperature.cursor[1] = l;
      Serial.printf("l: %u c: %u page: %u \n",l,c,page);
      MenuNebulizacao(page,l,c);
    }
    else{
      // Temperature.cursor[1] = l;
      Serial.printf("l: %u c: %u page: %u \n",l,c,page);
      MenuNebulizacao(page,l,c);
    }
  }
}