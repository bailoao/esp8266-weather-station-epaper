/**
GPL V3.0
Copyright (c) 2017 by Hui Lu

*/

#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <JsonListener.h>
#include "Wire.h"
#include "TimeClient.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         
#include "heweather.h"
#include <EEPROM.h>
#include <SPI.h>
#include "EPD_drive.h"
#include "EPD_drive_gpio.h"
#include "bitmaps.h"
#include "lang.h"
#include "FS.h"
ADC_MODE(ADC_VCC);
#define debug 1; ///< 调试模式，打开串口输出
/***************************
  全局设置
 **************************/
bool showtime=0;            ///< 是否显示时间，不支持局刷的屏不要开
const int sleeptime=3;    ///< 更新天气的间隔 单位为分钟,间隔是下面更新时间的间隔的整数倍如更新时间间隔为30分钟，更新天气间隔须为30分钟的倍数，不能是31，43分钟这样
int timeupdateinterval=60;  ///< 更新时间的间隔，单位为秒（显示时间时不可更改，只显示天气可改为60*30）
const float UTC_OFFSET = 0; ///< 时区，忽略
byte end_time=23;           ///< 停止更新天气的时间 23：00
byte start_time=7;          ///< 开始更新天气的时间 7：00
const char* server="www.duckweather.tk"; ///< 提供天气信息的服务器
const char* client_name="news"; ///< 消息用户名设置，news为显示新闻，可以填写自定义用户名，通过duckweather.tk/client.php更新消息
//modify language in lang.h

 /***************************
  **************************/
String city;
String lastUpdate = "--";
bool shouldsave=false;
bool updating=false; ///是否在更新天气数据
TimeClient timeClient(UTC_OFFSET,server);
WaveShare_EPD EPD = WaveShare_EPD();
heweatherclient heweather(server,lang);

Ticker avoidstuck;///防止连接服务器过程中卡死

void pico_i2c_writereg_asm(uint32_t a, uint32_t b)
{
  //调整时钟，没卵用，最低也有65MHz
  //pico_i2c_writereg(103,4,1,0x88);  
  //pico_i2c_writereg(103,4,2,0xf1); //underclock
  asm volatile (".global pico_i2c_writereg_asm\n.align 4\npico_i2c_writereg_asm:\n_s32i.n  a3, a2, 0\n_memw\n_l32i.n a3, a2, 0\nbbci  a3, 25, .term_pico_writereg\n.reloop_pico_writereg:\n_memw\n_l32i.n a3, a2, 0\nbbsi  a3, 25, .reloop_pico_writereg\n.term_pico_writereg:\n_ret.n");
}
#define pico_i2c_writereg( reg, hostid, par, val ) pico_i2c_writereg_asm( (hostid<<2) + 0x60000a00 + 0x300, (reg | (par<<8) | (val<<16) | 0x01000000 ) )

void saveConfigCallback () {
   shouldsave=true;
}
//void ICACHE_RAM_ATTR clear_wifi();
void setup() { 
  #ifdef debug
  Serial.begin(115200);  
  Serial.println(ESP.getResetInfoPtr()->reason);
  Serial.printf("Serial begins at %dms\n",millis()); 
  #endif
  read_time_from_rtc_mem();  
  check_rtc_mem();  
 if(ESP.getResetInfoPtr()->reason==5)
 {
  #ifdef debug
  Serial.println("Reset after deep sleep");
  #endif
  if (read_config()==126)
  {
    #ifdef debug
    Serial.println("Readconfig=126");
    #endif
    EPD.deepsleep(); ESP.deepSleep(60 * 60 * 1000000);
    }
 }
  pinMode(0,INPUT_PULLUP);
  pinMode(CS,OUTPUT);
  pinMode(DC,OUTPUT);
  pinMode(RST,OUTPUT);
  pinMode(BUSY,INPUT);
  pinMode(CLK,OUTPUT);
  pinMode(DIN,OUTPUT);
  //attachInterrupt(digitalPinToInterrupt(0),clear_wifi, FALLING);


  
  EEPROM.begin(20);  
  //SPI.beginTransaction(SPISettings(20000000L, MSBFIRST, SPI_MODE0));//20000000L
  //SPI.begin();
  
  
  #ifdef debug
  Serial.printf("EPD init finish at %dms\n",millis());
  #endif
 
  update_time();//display time
  heweather.client_name=client_name;  
  SPIFFS.begin();
   /*************************************************
   wifimanager
   *************************************************/
  WiFiManager wifiManager;
  WiFiManagerParameter custom_c("city","city","your city", 20);  
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_c);  
  while(WiFi.waitForConnectResult() != WL_CONNECTED){
   if (!wifiManager.autoConnect("Weather Widget")) {
  always_sleep();
  #ifdef debug
  Serial.println("Failed to connect WIFI and timeout");
  #endif
  
  EPD.clearbuffer();
  EPD.fontscale=2;
  EPD.SetFont(FONT12);
  EPD.DrawUTF(0,0,config_timeout_line1);
  EPD.DrawUTF(36,0,config_timeout_line2);
  EPD.DrawUTF(72,0,config_timeout_line3); 
  EPD.DrawUTF(104,0,config_timeout_line4); 
  EPD.EPD_Dis_Full((unsigned char *)EPD.EPDbuffer,1);
  EPD.deepsleep(); ESP.deepSleep(60 * 60 * 1000000);
  }
   
  }
  #ifdef debug
  Serial.printf("Wifi connected at %dms \n",millis());
  #endif
  WiFi.persistent(true);  
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
 
  city= custom_c.getValue();
   
   /*************************************************
   EPPROM
   *************************************************/
  if (city!="your city")
  {
    
         EEPROM.write(0,city.length());         
         for(int i=0;i<city.length();i++)
         {
           EEPROM.write(i+1,city[i]);        
          }
         EEPROM.commit(); 
    }
   
 byte city_length=EEPROM.read(0);
  #ifdef debug
   Serial.print("EEPROM_CITY-LENGTH:");Serial.println(city_length);
  #endif
 if (city_length>0)
 {
  String test="";
  for(int x=1;x<city_length+1;x++) 
  {test+=(char)EEPROM.read(x);
  ////Serial.print((char)EEPROM.read(x));
  }
  city=test;
  #ifdef debug
   //Serial.printf("City read from EEPROM:"+String(city));
  #endif
  heweather.city=city;
  }
/*************************************************
   update weather
*************************************************/
//heweather.city="huangdao";
heweather.EPDbuffer=&EPD.EPDbuffer[0];
avoidstuck.attach(10,check);
updating=true;
updateData();
updating=false;
updatedisplay();

}
void check()
{
  if(updating==true)
  {EPD.deepsleep(); ESP.deepSleep(timeupdateinterval * 1 * 1000000,WAKE_RF_DISABLED);}
  avoidstuck.detach();
  return;
  }

void loop() {
 
 if(digitalRead(0)==LOW)
 {
   WiFiManager wifiManager;
  wifiManager.resetSettings();
 }

    EPD.deepsleep();
    if(showtime==1)
    {
      byte seconds=timeClient.getSeconds_byte();
      if(seconds>50) timeupdateinterval=seconds+60;
      else timeupdateinterval=60-seconds;      
    }
    timeClient.localEpoc+=timeupdateinterval;
    write_time_to_rtc_mem();//save time before sleeping}
    
    #ifdef debug
     Serial.printf("Finish at %dms\n",millis());
    #endif
    ESP.deepSleep(timeupdateinterval * 1 * 1000000,WAKE_RF_DISABLED); 

}
void updatedisplay()
{
  
  EPD.clearbuffer();
  EPD.fontscale=1;
  dis_batt(3,272);//电量显示
  EPD.SetFont(ICON50); 
  EPD.DrawUTF(0,0,String(char(heweather.getMeteoconIcon(heweather.now_cond_index.toInt()))));
  
    EPD.SetFont(FONT12);//今天天气
    byte x1=68,y1=0;
    EPD.DrawUTF(x1,y1+32,(String)todaystr+" "+heweather.today_tmp_min+"°~"+heweather.today_tmp_max+"°");
    EPD.DrawUTF(x1+12,y1+32,heweather.today_txt_d+"/"+heweather.today_txt_n);
    EPD.SetFont(ICON32);
    unsigned char code2[]={0x00,heweather.getMeteoconIcon(heweather.today_cond_d_index.toInt())};
    EPD.DrawUnicodeChar(x1-3,y1,32,32,code2);
    EPD.DrawXline(0,125,63);
  
    EPD.SetFont(FONT12);//明天天气
    byte x2=98,y2=0;
    EPD.DrawUTF(x2,y2+32,(String)tomorrowstr+" "+heweather.tomorrow_tmp_min+"°~"+heweather.tomorrow_tmp_max+"°");
    EPD.DrawUTF(x2+12,y2+32,heweather.tomorrow_txt_d+"/"+heweather.tomorrow_txt_n);
    EPD.SetFont(ICON32);
    unsigned char code3[]={0x00,heweather.getMeteoconIcon(heweather.tomorrow_cond_d_index.toInt())};
    EPD.DrawUnicodeChar(x2-3,y2,32,32,code3);
    EPD.DrawXline(0,125,95);
   
  
    EPD.SetFont(FONT12);
    EPD.DrawXbm_P(48,0,12,12,(unsigned char *)city_icon);//位置图标
    EPD.DrawUTF(48,13,heweather.citystr);//城市名
    EPD.DrawXbm_P(48,60,12,12,(unsigned char *)aqi_icon);
    EPD.DrawUTF(48,74,heweather.qlty);//空气质量

    EPD.DrawUTF(5,63,"RH:"+heweather.now_hum+"%");//湿度
    EPD.DrawUTF(0,130,heweather.year);//日期
    EPD.DrawUTF(12,130,heweather.nongli);//农历
    EPD.DrawXbm_P(96,128,12,12,(unsigned char *)message);
    EPD.DrawUTF(96,140,heweather.message);//消息

    EPD.SetFont(FONT32);
    EPD.DrawUTF(16,60,heweather.now_tmp+"°");//当前温度
    EPD.DrawUTF(350,200,heweather.now_tmp+"°");//当前温度
    EPD.DrawYline(5,40,58);

   


    
    EPD.DrawXbm_P(24,132,35,72,digi_num[timeClient.getHours()[0]-0x30]);
    EPD.DrawXbm_P(24,169,35,72,digi_num[timeClient.getHours()[1]-0x30]);
    EPD.DrawXbm_P(24,206,35,72,digi_num[10]);
    EPD.DrawXbm_P(24,220,35,72,digi_num[timeClient.getMinutes()[0]-0x30]);
    EPD.DrawXbm_P(24,255,35,72,digi_num[timeClient.getMinutes()[1]-0x30]);
   
    for(int i=0;i<2048;i++) EPD.EPDbuffer[i]=~EPD.EPDbuffer[i];//左边反色
   if(heweather.city!="")
   {
     EPD.EPD_Dis_Full((unsigned char *)EPD.EPDbuffer,1);
     //EPD.EPD_init_Part();
     //EPD.EPD_Dis_Part(0,127,0,295,(unsigned char *)EPD.EPDbuffer,1);
   }
 
 }
/**
 * @brief 显示电池电量
 * @param x,y 显示位置
 */
 
 void dis_batt(int16_t x, int16_t y)
{
  /*attention! calibrate it yourself */
   float voltage;
   voltage=(float)ESP.getVcc()/1000;  
   float batt_voltage=voltage;  
 /*attention! calibrate it yourself */
  if (batt_voltage<=2.9)  {
    EPD.clearbuffer();EPD.DrawXbm_P(39,98,100,50,(unsigned char *)needcharge);
    always_sleep();
    #ifdef debug
   Serial.println("baterry low");  
    #endif  
    }
  if (batt_voltage>2.9&&batt_voltage<=2.95)  EPD.DrawXbm_P(x,y,20,10,(unsigned char *)batt_1);
  if (batt_voltage>2.95&&batt_voltage<=3.0)  EPD.DrawXbm_P(x,y,20,10,(unsigned char *)batt_2);
  if (batt_voltage>3.0&&batt_voltage<=3.05)  EPD.DrawXbm_P(x,y,20,10,(unsigned char *)batt_3);
  if (batt_voltage>3.05&&batt_voltage<=3.1)  EPD.DrawXbm_P(x,y,20,10,(unsigned char *)batt_4);
  if (batt_voltage>3.1)  EPD.DrawXbm_P(x,y,20,10,(unsigned char *)batt_5);  
  }
/*
 * @brief 显示时间
 */
 void dis_time(int16_t x, int16_t y)
 {
  x=24;y=128;
  
  EPD.fontscale=1;EPD.clearbuffer();  
  EPD.DrawXbm_P(24,132,35,72,digi_num[timeClient.getHours()[0]-0x30]);
  EPD.DrawXbm_P(24,169,35,72,digi_num[timeClient.getHours()[1]-0x30]);
  EPD.DrawXbm_P(24,206,35,72,digi_num[10]);
  EPD.DrawXbm_P(24,220,35,72,digi_num[timeClient.getMinutes()[0]-0x30]);
  EPD.DrawXbm_P(24,255,35,72,digi_num[timeClient.getMinutes()[1]-0x30]); 
  EPD.EPD_Dis_Part(24,95,128,295,(unsigned char *)EPD.EPDbuffer,1);//295
  
 }
/*
 * @brief 更新天气，校准时间
 */
void updateData() {  
  heweather.update();
  timeClient.updateTime(heweather.t);
  lastUpdate = timeClient.getHours()+":"+timeClient.getMinutes();

  byte rtc_mem[4];rtc_mem[0]=126;
  byte Hours=timeClient.getHours().toInt();  
}

void configModeCallback (WiFiManager *myWiFiManager) {
  #ifdef debug
   Serial.println("Entered config mode");  
  #endif  
  EPD.clearbuffer();
  EPD.SetFont(FONT12);
  EPD.fontscale=1;
  EPD.DrawXbm_P(6,80,32,32,one);
  EPD.DrawXbm_P(42,80,32,32,two);
  EPD.DrawXbm_P(79,80,32,32,three);
  EPD.fontscale=1;
  EPD.DrawUTF(6,112,config_page_line1);
  EPD.DrawUTF(22,112,config_page_line2);
  EPD.DrawXline(80,295,39);
  EPD.DrawUTF(42,112,config_page_line3);
  EPD.DrawUTF(58,112,config_page_line4);
  EPD.DrawXline(80,295,76);
  EPD.DrawUTF(79,112,config_page_line5);
  EPD.DrawUTF(94,112,config_page_line6);
  EPD.DrawXbm_P(6,0,70,116,phone);
  EPD.EPD_Dis_Full((unsigned char *)EPD.EPDbuffer,1);
 
}


/*
 * @brief 读取rtc mem中存储的睡眠之前的时间
 */
void read_time_from_rtc_mem()
{ 
  byte rtc_mem[4];
  ESP.rtcUserMemoryRead(8, (uint32_t*)&rtc_mem, sizeof(rtc_mem));
  timeClient.localEpoc = ((unsigned long)rtc_mem[3] << 24) | ((unsigned long)rtc_mem[2] << 16) | ((unsigned long)rtc_mem[1] << 8) | (unsigned long)rtc_mem[0];
 
  }
/*
 * @brief 将当前时间写入rtc mem
 */
void write_time_to_rtc_mem()
{
  //write time to rtc before sleep
  long time_before_sleep;
  time_before_sleep=timeClient.getCurrentEpoch();
  byte rtc_mem[4];
  rtc_mem[0] = byte(time_before_sleep);
  rtc_mem[1] = byte(time_before_sleep >> 8);
  rtc_mem[2] = byte(time_before_sleep >> 16);
  rtc_mem[3] = byte(time_before_sleep >> 24);
  ESP.rtcUserMemoryWrite(8, (uint32_t*)&rtc_mem, sizeof(rtc_mem));
}

/*
 * @brief 读取rtcmem中第六个byte
 * @detail rtcmem中第六个byte,若等于126，则代表连接WIFI超时，或者电池没电
 */
unsigned long read_config()
{
  byte rtc_mem[4];
  ESP.rtcUserMemoryRead(4, (uint32_t*)&rtc_mem, sizeof(rtc_mem));
  ////check if sleeping forever is needed
  return rtc_mem[2];
  
  }
/*
 * @brief 将标志位写入126,不再更新时间和天气，也不刷新屏幕
 */
unsigned long always_sleep()
{
  byte rtc_mem[4];
  ESP.rtcUserMemoryRead(4, (uint32_t*)&rtc_mem, sizeof(rtc_mem));
  if (rtc_mem[2]!=126)
  {  
    rtc_mem[2]=126;
    ESP.rtcUserMemoryWrite(4, (uint32_t*)&rtc_mem, sizeof(rtc_mem));   
   }  
 }
/*
 * @brief 检测是不是第一次上电运行此程序
 */
void check_rtc_mem()
{
  /*
  rtc_mem[0] sign for first run
  rtc_mem[1] how many hours left
  */
  byte rtc_mem[4];
  ESP.rtcUserMemoryRead(0, (uint32_t*)&rtc_mem, sizeof(rtc_mem));
  if (rtc_mem[0]!=126)
  {
    Serial.println("first time to run");
    byte times= byte(sleeptime*60/timeupdateinterval);
    //Serial.println("times");Serial.println(times);
    rtc_mem[0]=126;
    rtc_mem[1]=0;
    rtc_mem[2]=times;//time
    //Serial.println("rctmemblock0-2");Serial.println(rtc_mem[2]);
    ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtc_mem, sizeof(rtc_mem));
    }  
  }
 /*
  * @brief 检测更新时间还是天气
  * 
  */
  void update_time()
  {
    
   byte rtc_mem[4];
   byte times= byte(sleeptime*60/timeupdateinterval);   
   ESP.rtcUserMemoryRead(0, (uint32_t*)&rtc_mem, sizeof(rtc_mem));
   #ifdef debug
   Serial.println(timeClient.getFormattedTime());
   Serial.printf("Update weather after %d times of time update\r\n",times);
   Serial.printf("Time updated %d times\r\n",rtc_mem[2]);  
   #endif 

  
  if(rtc_mem[2]>times-1)
  //if(1)
  {
    rtc_mem[2]=0;
     #ifdef debug
     Serial.println("Need to update weather");
     #endif 
     ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtc_mem, sizeof(rtc_mem));
     EPD.EPD_init_Full();
   }
  else
   {
      if(showtime==1)
       {
         EPD.EPD_init_Part();        
        }
     
      #ifdef debug
      Serial.println("Don't need to update weather, need time");
      #endif      
       if(timeClient.getHours_byte()<end_time&&timeClient.getHours_byte()>start_time)
        {
          #ifdef debug
          Serial.println("Update at daytime");
          #endif      
          byte rct_temp=byte(rtc_mem[2]+1);rtc_mem[2]=rct_temp;
          ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtc_mem, sizeof(rtc_mem));
        }
     else
        {
          #ifdef debug
          Serial.println("Update at night");
          #endif 
          byte rct_temp=byte(times-1);rtc_mem[2]=rct_temp;
          ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtc_mem, sizeof(rtc_mem));
         }
       #ifdef debug
       Serial.printf("Epd begin at %dms\n",millis());
       #endif
       if(showtime==1)
       {
         dis_time(1, 240);       
         EPD.deepsleep();
         #ifdef debug
         Serial.printf("Epd finish at %dms\n",millis());
         #endif       
         timeupdateinterval=60;        
        }
      
       timeClient.localEpoc+=timeupdateinterval;
       write_time_to_rtc_mem();
       
       #ifdef debug
       Serial.printf("Esp finish at %dms\n",millis());
       #endif
       if((rtc_mem[2]=times-1))
       {      
        ESP.deepSleep(timeupdateinterval * 1 * 1000000,WAKE_RF_DEFAULT);
       }
       else 
       {
        ESP.deepSleep(timeupdateinterval * 1 * 1000000,WAKE_RF_DISABLED);
       }
    }
  
 }
void clear_wifi()
{
  //Serial.println("clear wifi");
  WiFiManager wifiManager;
  wifiManager.resetSettings(); 
  }
