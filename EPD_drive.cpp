#include "FS.h"
#include "EPD_drive.h"
#include "EPD_drive_gpio.h"


unsigned char UNICODEbuffer[200];
String fontname;



 WaveShare_EPD::WaveShare_EPD(){
 
}


void WaveShare_EPD::SetFont(FONT fontindex)
{
     FontIndex=fontindex;
     switch (fontindex)
     {
     case 0:
     fontname="/font16";fontwidth=16;fontheight=16;break;
     case 1:
     fontname="/font32";fontwidth=32;fontheight=32;break;
     case 2:
     fontname="/font10";fontwidth=10;fontheight=10;break;
     case 3:
     fontname="/font12";fontwidth=12;fontheight=12;break;
     case 5:
     fontname="/font70";fontwidth=70;fontheight=70;break;
     case 6:
     fontname="/font12num";fontwidth=12;fontheight=12;break;
     case 7:
     fontname="/font24";fontwidth=24;fontheight=24;break;
     case 8:
     fontname="/font8";fontwidth=8;fontheight=8;break;
     case 9:
     fontname="/font100num";fontwidth=100;fontheight=100;break;
     case 11:
     fontname="/weathericon";fontwidth=32;fontheight=32;break;
     case 12:
     fontname="/weathericon80";fontwidth=80;fontheight=80;break;
      case 13:
     fontname="/weathericon32";fontwidth=32;fontheight=32;break;
     case 14:
     fontname="/weathericon50";fontwidth=50;fontheight=50;break;
     }
  }
  void WaveShare_EPD::DrawCircle(byte x,int y,byte r)
  {
      for(int i=0; i<360; i++)
    {
      SetPixel(round(cos(i)*r+x),round(sin(i)*r)+y);
      
    }    
 }

 void WaveShare_EPD::DrawBox(byte x,int y, int w, int h)
 {

    for(int i=x;i<x+h;i++)
  {
    DrawXline( y, y+ w-1,  i);
  }
  
 }
 void WaveShare_EPD::DrawEmptyBox(int x,int y,int w,int h)
{
   DrawXline(y,y+w, x);
   DrawXline(y,y+w, x+h);
   DrawYline(x,x+w, y);
   DrawYline(x,x+w, y+w);
  
}
 
 void WaveShare_EPD::DrawChart(int x,int y,int w, int c1, int c2, int c3, int c4, int c5,int c6)
  {
    
    int percent,sum,max_c=0;
    sum=c1+c2+c3+c4+c5+c6;
    if(sum==0) sum=1;
    max_c=max(c1,max_c);max_c=max(c2,max_c);max_c=max(c3,max_c);
    max_c=max(c4,max_c);max_c=max(c5,max_c);max_c=max(c6,max_c);
    if(max_c==0) max_c=1;
    w=w*sum/(2*max_c);
    
    SetFont(FONT12);
    DrawUTF(x,y,">0.3um");
    percent=w*c1/sum;
    DrawBox(x+1,y+41,percent,10);
    SetFont(FONT8);
    DrawUTF(x+2,y+41+percent,String(c1));
   
    SetFont(FONT12);
    DrawUTF(x+12,y,">0.5um");
    percent=w*c2/sum;
    DrawBox(x+13,y+41,percent,10);
    SetFont(FONT8);
    DrawUTF(x+14,y+41+percent,String(c2));

    SetFont(FONT12);
    DrawUTF(x+24,y,">1.0um");
    percent=w*c3/sum;
    DrawBox(x+25,y+41,percent,10);
    SetFont(FONT8);
    DrawUTF(x+26,y+41+percent,String(c3));

    SetFont(FONT12);
    DrawUTF(x+36,y,">2.5um");
    percent=w*c4/sum;
    DrawBox(x+37,y+41,percent,10);
    SetFont(FONT8);
    DrawUTF(x+38,y+41+percent,String(c4));

    SetFont(FONT12);
    DrawUTF(x+48,y,">5.0um");
    percent=w*c5/sum;
    DrawBox(x+49,y+41,percent,10);
    SetFont(FONT8);
    DrawUTF(x+50,y+41+percent,String(c5));

    SetFont(FONT12);
    DrawUTF(x+60,y,">10um");
    percent=w*c6/sum;
    DrawBox(x+61,y+41,percent,10);
    SetFont(FONT8);
    DrawUTF(x+62,y+41+percent,String(c6));
    
    
    }
 void WaveShare_EPD::DrawCircleChart(int x,int y,int r,int w,int c1,int c2,int c3)
 {
     int sum=c1+c2+c3;
     if(sum==0) sum=1;
        for(int i=0;i<360;i++)
      {
        SetPixel(-round(cos(M_PI*i/180)*r)+x,round(sin(M_PI*i/180)*r)+y);
        
      }
      
      for(int i=0;i<360;i++)
      {
        SetPixel(-round(cos(M_PI*i/180)*(r-w))+x,round(sin(M_PI*i/180)*(r-w))+y);
        
      }
      
      for(int i=0;i<c1*360/sum;i++)
      {
        for(int j=0;j<w;j++)
        { 
        SetPixel(-round(cos(M_PI*i/180)*(r-j))+x,round(sin(M_PI*i/180)*(r-j))+y);
        }
      }
      
      for(int i=(c1+c2)*360/sum-1;i<(c1+c2)*360/sum;i++)
      {
        
        for(int j=0;j<w;j++)
        { 
        SetPixel(-round(cos(M_PI*i/180)*(r-j))+x,round(sin(M_PI*i/180)*(r-j))+y);
        }
      }
      
      for(int i=(c1+c2)*360/sum;i<360;i+=10)
      {
        
        for(int j=0;j<w;j++)
        { 
        SetPixel(-round(cos(M_PI*i/180)*(r-j))+x,round(sin(M_PI*i/180)*(r-j))+y);
        }
      }
      
      y+=2;
      DrawBox(x-r,y+r+2,8,8);
      SetFont(FONT12);
      DrawUTF(x-r-2,y+r+12,"PM1.0");
      SetFont(FONT12_NUM);
      DrawUTF(x-r-2,y+r+12+30,String(c1));
      
      
      SetFont(FONT12);
      DrawEmptyBox(x-r+14,y+r+2,8,8);
      DrawUTF(x-r+12,y+r+12,"PM2.5");
      
      SetFont(FONT12_NUM);
      DrawUTF(x-r+12,y+r+12+30,String(c2));
      DrawEmptyBox(x-r+28,y+r+2,8,8);
      for(int i=y+r+2;i<y+r+2+8;i+=2)
        {
          DrawYline(x-r+28,x-r+28+7,i);
          }
      SetFont(FONT12);
      DrawUTF(x-r+26,y+r+12,"PM10");
      SetFont(FONT12_NUM);
      DrawUTF(x-r+26,y+r+12+30,String(c3));
  
  
  }
 void WaveShare_EPD::DrawXline(int start,int end, int x)
  {
    for(int i=start;i<=end;i++)
    {
      SetPixel(x,i);
      }
  }
   void WaveShare_EPD::DrawYline(int start,int end,  int y)
  {
    for(int i=start;i<=end;i++)
    {
      SetPixel(i,y);
      }
  }
void WaveShare_EPD::DrawUTF(int16_t x,int16_t y,String code)
{
  char buffer[200];
  code.toCharArray(buffer,200);
  DrawUTF(x,y,fontwidth,fontheight,(unsigned char *)buffer);
  }
void WaveShare_EPD::DrawUTF(int16_t x,int16_t y,byte width,byte height,unsigned char *code)
{
  int charcount;
  charcount=UTFtoUNICODE((unsigned char*)code);
  DrawUnicodeStr(x,y,width,height,charcount,(unsigned char *)UNICODEbuffer);
  
  }
int WaveShare_EPD::UTFtoUNICODE(unsigned char *code)
{ 
  int i=0;int charcount=0;
  while(code[i]!='\0')
  { 
    //Serial.println("current codei");
      //Serial.println(code[i],HEX);
      //  Serial.println(code[i]&0xf0,HEX);
    if(code[i]<=0x7f)  //ascii
    {
      
      UNICODEbuffer[charcount*2]=0x00;
      UNICODEbuffer[charcount*2+1]=code[i];
        // Serial.println("english or number");
      // Serial.println(UNICODEbuffer[charcount*2],HEX);
       //  Serial.println(UNICODEbuffer[charcount*2+1],HEX);
      i++;charcount++;
   
      }
    else if((code[i]&0xe0)==0xc0)
    {
    
      UNICODEbuffer[charcount*2+1]=(code[i]<<6)+(code[i+1]&0x3f);
      UNICODEbuffer[charcount*2]=(code[i]>>2)&0x07;
      i+=2;charcount++;
     // Serial.println("two bits utf-8");
      }
     else if((code[i]&0xf0)==0xe0)
    {
      
      UNICODEbuffer[charcount*2+1]=(code[i+1]<<6)+(code[i+2]&0x7f);
      UNICODEbuffer[charcount*2]=(code[i]<<4)+((code[i+1]>>2)&0x0f);
      
       //Serial.println("three bits utf-8");
       // Serial.println(UNICODEbuffer[charcount*2],HEX);
        // Serial.println(UNICODEbuffer[charcount*2+1],HEX);
        i+=3;charcount++;
      }
     else
     {
      return 0;}
  }
  UNICODEbuffer[charcount*2]='\0';
  return charcount;
  }
void WaveShare_EPD::DrawUnicodeChar(int16_t x,int16_t y,byte width,byte height,unsigned char *code)
 { 
 
  int offset;
  int sizeofsinglechar;
  if (height%8==0) sizeofsinglechar=(height/8)*width;
  else sizeofsinglechar=(height/8+1)*width;
 offset=(code[0]*0x100+code[1])*sizeofsinglechar;
  // Serial.println("code[1]");
  // Serial.println(code[1]);
   // Serial.println("sizeofsinglechar");
  // Serial.println(sizeofsinglechar);
  File f=SPIFFS.open(fontname,"r");
  f.seek(offset,SeekSet);
  char zi[sizeofsinglechar];
  f.readBytes(zi,sizeofsinglechar);
 /*for(int i=0;i<32;i++)
  {
     
   Serial.println(zi[i],HEX);
    }*/
  // Serial.println("offset");
   //Serial.println(offset);
   if (offset<0xff*sizeofsinglechar && FontIndex<10) 
   {drawXbm(x,y,width,height,(unsigned char *)zi); }
   else  {drawXbm(x,y,width,height,(unsigned char *)zi);}
  
//SPIFFS.end();
}

void WaveShare_EPD::DrawUnicodeStr(int16_t x,int16_t y,byte width,byte height,byte strlength,unsigned char *code)
{
  CurrentCursor=0;
   byte sizeofsinglechar;
  if (height%8==0) sizeofsinglechar=(height/8)*width;
  else sizeofsinglechar=(height/8+1)*width;
  int ymove=0;
  int xmove=0;
  strlength*=2;
  int i=0;
  while(i<strlength)
  {
      int offset;
      offset=(code[i]*0x100+code[i+1])*sizeofsinglechar;
      if (offset<0xff*sizeofsinglechar&&fontscale==1) 
      {
      
      DrawUnicodeChar(x+xmove,y+ymove,width,height,(unsigned char *)code+i);
      ymove+=CurrentCursor+1;
      if((y+ymove+width/2)>=yDot-1) {xmove+=height+1;ymove=0;CurrentCursor=0;}
    }
    else if(fontscale==2)
    {
      DrawUnicodeChar(x+xmove,y+ymove,width,height,(unsigned char *)code+i);
      ymove+=width*2;
      if((y+ymove+width*2)>=yDot-1) {xmove+=height*2+2;ymove=0;CurrentCursor=0;}
      }
    else
    {
      DrawUnicodeChar(x+xmove,y+ymove,width,height,(unsigned char *)code+i);
      ymove+=width;
      if((y+ymove+width)>=yDot-1) {xmove+=height+1;ymove=0;CurrentCursor=0;}
      }
    i++;i++;
    }
  
  }
void WaveShare_EPD::drawXbm(int16_t xMove, int16_t yMove, int16_t width, int16_t height,unsigned char *xbm) {
  int16_t heightInXbm = (height + 7) / 8;
  uint8_t Data;
  for(int16_t x = 0; x < width; x++) {
    for(int16_t y = 0; y < height; y++ ) {
      if (y & 7) {
        Data <<= 1; // Move a bit
      } else {  // Read new Data every 8 bit
        Data = xbm[(y / 8) + x * heightInXbm];
      }
      // if there is a bit draw it
      if (((Data & 0x80)>>7)) {
       if(fontscale==1) {SetPixel(xMove + y, yMove + x);CurrentCursor=x;}
       if(fontscale==2) {SetPixel(xMove + y*2, yMove + x*2);SetPixel(xMove + y*2+1, yMove + x*2);SetPixel(xMove + y*2, yMove + x*2+1);SetPixel(xMove + y*2+1, yMove + x*2+1);}
      }
    }
  }
}

void WaveShare_EPD::DrawXbm_P(int16_t xMove, int16_t yMove, int16_t width, int16_t height,const unsigned char *xbm) {
 int16_t heightInXbm = (height + 7) / 8;
 uint8_t Data;
 //unsigned char temp[heightInXbm*width];
 //memcpy_P(temp, xbm, heightInXbm*width);
  
  for(int16_t x = 0; x < width; x++) {
    for(int16_t y = 0; y < height; y++ ) {
      if (y & 7) {
        Data <<= 1; // Move a bit
      } else {  // Read new Data every 8 bit
        Data = pgm_read_byte(xbm+(y / 8) + x * heightInXbm);
      }
      // if there is a bit draw it
      if (((Data & 0x80)>>7)) {
       if(fontscale==1) {SetPixel(xMove + y, yMove + x);CurrentCursor=x;}
       if(fontscale==2) {SetPixel(xMove + y*2, yMove + x*2);SetPixel(xMove + y*2+1, yMove + x*2);SetPixel(xMove + y*2, yMove + x*2+1);SetPixel(xMove + y*2+1, yMove + x*2+1);}
      }
    }
  }
}
void WaveShare_EPD::SetPixel(int16_t x, int16_t y)
{ 
    
    #ifdef OPM42
    int16_t temp=x;
    x=y;y=yDot-1-temp;
    if(x<xDot&&y<yDot)  EPDbuffer[x/8+y*xDot/8]&=~(0x80>>x%8) ;
    #else
    if(x<xDot&&y<yDot)  EPDbuffer[x/8+y*xDot/8]&=~(0x80>>x%8) ;
    #endif
  }
void WaveShare_EPD::clearbuffer()
{
  for(int i=0;i<(xDot*yDot/8);i++) EPDbuffer[i]=0xff;
  
  }

unsigned char WaveShare_EPD::ReadBusy(void)
{
  unsigned long i=0;
  for(i=0;i<400;i++){
	//	println("isEPD_BUSY = %d\r\n",isEPD_CS);
#if(defined WX)||(defined OPM42)
      if(isEPD_BUSY==EPD_BUSY_LEVEL) {
				//Serial.println("Busy is Low \r\n");
      	return 1;
      }
#endif
#if(defined WF)||(defined WF5X83)
if(isEPD_BUSY!=EPD_BUSY_LEVEL) {
				//Serial.println("Busy is Low \r\n");
      	return 1;
      }
#endif
	  delay(2);
   //Serial.println("epd is Busy");
  }
  return 0;
}

void WaveShare_EPD::EPD_WriteCMD(unsigned char command)
{ 
  #if(defined WX)||(defined OPM42)
  ReadBusy();
  #endif       
	
	EPD_CS_0;	
	EPD_DC_0;    // command write
	SPI_Write(command);
	EPD_CS_1;
}
void WaveShare_EPD::EPD_WriteData (unsigned char Data)
{
 
  EPD_CS_0; 
  EPD_DC_1;     
  SPI_Write(Data);  
  EPD_CS_1;
}

void WaveShare_EPD::EPD_WriteCMD_p1(unsigned char command,unsigned char para)
{
	#if(defined WX)||(defined OPM42)
  ReadBusy();
  #endif      
  EPD_CS_0; 
  EPD_DC_0;    // command write		
	SPI_Write(command);
	EPD_DC_1;		// command write
	SPI_Write(para);
  EPD_CS_1;	
}

void WaveShare_EPD::deepsleep(void)
{
  #if(defined WX)||(defined OPM42)
    ReadBusy();
	  EPD_WriteCMD_p1(0x10,0x01);
	  //EPD_WriteCMD_p1(0x22,0xc0);//power off
	  //EPD_WriteCMD(0x20);
  #endif
  #ifdef WF
	  ReadBusy();
	  EPD_WriteCMD(0x50); 
	  EPD_WriteData (0xf7);//border floating
	  EPD_WriteCMD(0x02);//power off
	  EPD_WriteCMD(0x07);//sleep
	  EPD_WriteData(0xa5);
  #endif
 
}


void WaveShare_EPD::EPD_Write(unsigned char *value, unsigned char Datalen)
{
  
	unsigned char i = 0;
	unsigned char *ptemp;
	ptemp = value;
	#ifdef WX
  ReadBusy();
  #endif        
	EPD_CS_0;
	EPD_DC_0;		// When DC is 0, write command 
	SPI_Write(*ptemp);	//The first byte is written with the command value
	ptemp++;
	EPD_DC_1;		// When DC is 1, write Data
	for(i= 0;i<Datalen-1;i++){	// sub the Data
		SPI_Write(*ptemp);
		ptemp++;
	}
	EPD_CS_1;
}

void WaveShare_EPD::EPD_WriteDispRam(unsigned int XSize,unsigned int YSize,unsigned char *Dispbuff,unsigned int offset,byte label)
{
	
	int i = 0,j = 0;
	
	
  //byte *temp=Dispbuff+offset;
  //#ifdef WF
  #if (defined WF)||(defined WF5X83)
      EPD_WriteCMD(0x10);
      EPD_CS_0; 
      EPD_DC_1;
     for(i=0;i<YSize;i++){
    for(j=0;j<XSize;j++){
      SPI_Write(0x00);      
       }
     }    
    EPD_CS_1; 
    #endif
  #if(defined WX)||(defined OPM42)
  ReadBusy();  
	EPD_DC_0;    //command write
	EPD_CS_0;	
	SPI_Write(0x24);
	#endif
  #ifdef WF
  EPD_CS_0;  
  EPD_DC_0;   //command write
  SPI_Write(0x13);
  #endif
  #ifdef WF5X83
  EPD_CS_0;  
  EPD_DC_0;   //command write
  SPI_Write(0x13);
  #endif
	EPD_DC_1;		//Data write
  //Serial.printf("Xsize=%dYsize=%d\r\n",XSize,YSize);
 
  if(label!=1)
  {
    for(i=0;i<YSize;i++){
    for(j=0;j<XSize;j++){
      SPI_Write(label);
     
       }
     }    
     
    }
  else
  {
    Dispbuff+=offset;
	for(i=0;i<YSize;i++){
		for(j=0;j<XSize;j++){
			SPI_Write(*Dispbuff);
			Dispbuff++;
		}
   Dispbuff+=xDot/8-XSize;
	}
  }
	EPD_CS_1;

}




void WaveShare_EPD::EPD_SetRamArea(uint16_t Xstart,uint16_t Xend,
						unsigned char Ystart,unsigned char Ystart1,unsigned char Yend,unsigned char Yend1)
{
  #if(defined WX)||(defined OPM42)
  unsigned char RamAreaX[3];	// X start and end
	unsigned char RamAreaY[5]; 	// Y start and end
	RamAreaX[0] = 0x44;	// command
	RamAreaX[1] = Xstart/8;
	RamAreaX[2] = Xend/8;
	RamAreaY[0] = 0x45;	// command
	RamAreaY[1] = Ystart;
	RamAreaY[2] = Ystart1;
	RamAreaY[3] = Yend;
  RamAreaY[4] = Yend1;
	EPD_Write(RamAreaX, sizeof(RamAreaX));
	EPD_Write(RamAreaY, sizeof(RamAreaY));
  Serial.printf("set ram area%d %d %d %d %d %d %d %d %d\n",RamAreaX[0],RamAreaX[1],RamAreaX[2],RamAreaY[0],RamAreaY[1],RamAreaY[2],RamAreaY[3],RamAreaY[4]);

  #endif
  #ifdef WF
      EPD_WriteCMD(0x91); //enter partial refresh mode
      EPD_WriteCMD(0x90); 
      EPD_WriteData (Xstart);
      EPD_WriteData (Xend);
      EPD_WriteData (Ystart);
      EPD_WriteData (Ystart1);
      EPD_WriteData (Yend);
      EPD_WriteData (Yend1);
      EPD_WriteData (0x0);
  #endif
}

void WaveShare_EPD::EPD_SetRamPointer(uint16_t addrX,unsigned char addrY,unsigned char addrY1)
{
  unsigned char RamPointerX[2];  // default (0,0)
  unsigned char RamPointerY[3];
  //Set RAM X address counter
  RamPointerX[0] = 0x4e;
  RamPointerX[1] = addrX;
  //RamPointerX[1] = 0xff;
  //Set RAM Y address counter
  RamPointerY[0] = 0x4f;
  RamPointerY[1] = addrY;
  RamPointerY[2] = addrY1;
  //RamPointerY[1] = 0x2b;
  //RamPointerY[2] = 0x02;
  
  EPD_Write(RamPointerX, sizeof(RamPointerX));
  EPD_Write(RamPointerY, sizeof(RamPointerY));
}


void WaveShare_EPD::EPD_Init(void)
{
  EPD_RST_0;   
  driver_delay_xms(5);  
  EPD_RST_1;
  //driver_delay_xms(100);
	#ifdef WX	
    EPD_Write(GateVol, sizeof(GateVol));
  	EPD_Write(GDOControl, sizeof(GDOControl));	// Pannel configuration, Gate selection
  	EPD_Write(softstart, sizeof(softstart));	// X decrease, Y decrease
  	EPD_Write(VCOMVol, sizeof(VCOMVol));		// VCOM setting
  	EPD_Write(DummyLine, sizeof(DummyLine));	// dummy line per gate
  	EPD_Write(Gatetime, sizeof(Gatetime));		// Gage time setting
  	EPD_Write(RamDataEntryMode, sizeof(RamDataEntryMode));	// X increase, Y decrease
    EPD_WriteCMD_p1(0x22,0xc0);//poweron
    EPD_WriteCMD(0x20); 
  	
	#endif
	
	#ifdef WF
 
     EPD_WriteCMD(0x01);     
     EPD_WriteData (0x03);
     EPD_WriteData (0x00);
     EPD_WriteData (0x26);//default26 max2b
     EPD_WriteData (0x26);//default26 max2b
     EPD_WriteData (0x03);
     
     EPD_WriteCMD(0x06);
     EPD_WriteData (0x17);
     EPD_WriteData (0x17);
     
     EPD_WriteCMD(0x04);
     //
     
     EPD_WriteCMD(0x00); 
     EPD_WriteData (0xb7);
      
     EPD_WriteCMD(0x30);  //PLL
     EPD_WriteData (0x3a);
     
     EPD_WriteCMD(0x61); //resolution
     EPD_WriteData (0x80);
     EPD_WriteData (0x01);
     EPD_WriteData (0x28);
     
     EPD_WriteCMD(0X82);
     EPD_WriteData (0x20);
     
     EPD_WriteCMD(0X50);
     EPD_WriteData (0x97);
	#endif
  
  #ifdef WF5X83
    EPD_WriteCMD(0x01); 
    EPD_WriteData (0x37);     //POWER SETTING
    EPD_WriteData (0x00);

    EPD_WriteCMD(0X00);     //PANNEL SETTING
    EPD_WriteData(0xb7);
    //EPD_WriteData(0x08);
    
      EPD_WriteCMD(0x30);     //PLL setting
    EPD_WriteData(0x3a);   //PLL:    0-15¦:0x3C, 15+:0x3A  
     EPD_WriteCMD(0X82);     //VCOM VOLTAGE SETTING
    EPD_WriteData(0x28);    //all temperature  range
    
    EPD_WriteCMD(0x06);         //boost
    EPD_WriteData (0xc7);     
    EPD_WriteData (0xcc);
    EPD_WriteData (0x28);

    EPD_WriteCMD(0X50);     //VCOM AND Data INTERVAL SETTING
    EPD_WriteData(0x77);

    EPD_WriteCMD(0X60);     //TCON SETTING
    EPD_WriteData(0x22);

    EPD_WriteCMD(0X65);     //FLASH CONTROL
    EPD_WriteData(0x00);

    EPD_WriteCMD(0x61);         //tres      
    EPD_WriteData (0x02);   //source 600
    EPD_WriteData (0x88);
    EPD_WriteData (0x01);   //gate 448
    EPD_WriteData (0xe0);

    EPD_WriteCMD(0xe5);     //FLASH MODE        
    EPD_WriteData(0x03);  

    EPD_WriteCMD(0X00);     //PANNEL SETTING
    EPD_WriteData(0x17);
    
    EPD_WriteCMD(0x04);       //POWER ON
    ReadBusy();
   #endif

   #ifdef OPM42 1
    EPD_WriteCMD(0x74);       // 
    EPD_WriteData(0x54);    // 
    EPD_WriteCMD(0x7E);       // 
    EPD_WriteData(0x3B);    // 
    EPD_WriteCMD(0x01);       // 
    EPD_WriteData(0x2B);    // 
    EPD_WriteData(0x01);
    EPD_WriteData(0x00);    // 

    EPD_WriteCMD(0x0C);       // 
    EPD_WriteData(0x8B);    // 
    EPD_WriteData(0x9C);    // 
    EPD_WriteData(0xD6);    //     
    EPD_WriteData(0x0F);    // 

    EPD_WriteCMD(0x3A);       // 
    EPD_WriteData(0x2C);    // 
    EPD_WriteCMD(0x3B);       // 
    EPD_WriteData(0x0A);    // 
    EPD_WriteCMD(0x3C);       // 
    EPD_WriteData(0x03);    // 

    EPD_WriteCMD(0x11);       // data enter mode
    EPD_WriteData(0x01);    // 01 –Y decrement, X increment,

    EPD_WriteCMD(0x2C);       // 
    EPD_WriteData(0x6C);    //


    EPD_WriteCMD(0x37);       // 
    EPD_WriteData(0x00);    // 
    EPD_WriteData(0x00);    // 
    EPD_WriteData(0x00);    // 
    EPD_WriteData(0x00);    // 
    EPD_WriteData(0x80);    // 
  
    EPD_WriteCMD(0x21);       // 
    EPD_WriteData(0x40);    // 
    EPD_WriteCMD(0x22);
    EPD_WriteData(0xC7);    // 
  
   #endif
}


void WaveShare_EPD::EPD_Update(void)
{
  #ifdef OPM42
  EPD_WriteCMD(0x20);
  #endif
	#ifdef WX
	EPD_WriteCMD_p1(0x22,0x04);
	EPD_WriteCMD(0x20);
	//EPD_WriteCMD(0xff);
	#endif
	#if(defined WF)||(defined WF5X83)
	EPD_WriteCMD(0x12);
  //delay(100);
  //EPD_WriteCMD(0x92);
	#endif
}
void WaveShare_EPD::EPD_Update_Part(void)
{
  #ifdef WX
	EPD_WriteCMD_p1(0x22,0x04);
	EPD_WriteCMD(0x20);
	//EPD_WriteCMD(0xff);
  #endif
  #ifdef WF
  EPD_WriteCMD(0x12);
  EPD_WriteCMD(0x92);
  
  #endif
}

void WaveShare_EPD::EPD_init_Full(void)
{	
	#if (defined WX)||(defined OPM42)
	EPD_Init();		
	EPD_Write((unsigned char *)LUTDefault_full,sizeof(LUTDefault_full));
	
	#endif
	
	#ifdef WF
	EPD_Init();
  EPD_Write((unsigned char *)lut_vcomDC,sizeof(lut_vcomDC));
  EPD_Write((unsigned char *)lut_ww,sizeof(lut_ww));
  EPD_Write((unsigned char *)lut_bw,sizeof(lut_bw));
  EPD_Write((unsigned char *)lut_wb,sizeof(lut_wb));
  EPD_Write((unsigned char *)lut_bb,sizeof(lut_bb));
	#endif

  #ifdef WF5X83
  EPD_Init();

  #endif
}


void WaveShare_EPD::EPD_init_Part(void)
{		
   #ifdef WX
	EPD_Init();			// display
	EPD_Write((unsigned char *)LUTDefault_part,sizeof(LUTDefault_part));
	EPD_WriteCMD_p1(0x22,0xc0);//poweron
  EPD_WriteCMD(0x20);   		
	#endif 
	
	#ifdef WF
	  EPD_Init();
	  EPD_WriteCMD(0x50); 
    EPD_WriteData(0xb7);
    EPD_Write((unsigned char *)lut_vcomDC1,sizeof(lut_vcomDC1));
    EPD_Write((unsigned char *)lut_ww1,sizeof(lut_ww1));
    EPD_Write((unsigned char *)lut_bw1,sizeof(lut_bw1));
    EPD_Write((unsigned char *)lut_wb1,sizeof(lut_wb1));
    EPD_Write((unsigned char *)lut_bb1,sizeof(lut_bb1));
	#endif
}

void WaveShare_EPD::EPD_Dis_Full(unsigned char *DisBuffer,unsigned char Label)
{
	#if (defined WX)||(defined OPM42)
  unsigned int yStart=0;
  unsigned int yEnd=yDot-1;
  unsigned int xStart=0;
  unsigned int xEnd=xDot-1;
  unsigned long temp=yStart;
  #ifdef WX
  yStart=yDot-1-yEnd;yEnd=yDot-1-temp;
  EPD_SetRamPointer(xStart/8,yEnd%256,yEnd/256);
  EPD_SetRamArea(xStart,xEnd,yEnd%256,yEnd/256,yStart%256,yStart/256);
  #endif
  #ifdef OPM42
  yStart=yDot-1-yEnd;yEnd=yDot-1-temp;
  EPD_SetRamPointer(xStart/8,yEnd%256,yEnd/256);
  EPD_SetRamArea(xStart,xEnd,yEnd%256,yEnd/256,yStart%256,yStart/256);
  #endif
	if(Label == 2){
		EPD_WriteDispRam(xDot/8, yDot, (unsigned char *)DisBuffer, 0,0x00);	// white	
	}
	else if(Label==3)
	{
	  EPD_WriteDispRam(xDot/8, yDot, (unsigned char *)DisBuffer,0,0xff);  // black
	  }
	else{
		EPD_WriteDispRam(xDot/8, yDot, (unsigned char *)DisBuffer,0,1);	
	}	
	EPD_Update();
  ReadBusy();
  //EPD_SetRamPointer(xStart/8,yEnd%256,yEnd/256);
  //EPD_WriteDispRam(xDot/8, yDot, (unsigned char *)DisBuffer,0,1); 
	#endif
	
	#if (defined WF)||(defined WF5X83)
	if(Label == 2){
    EPD_WriteDispRam(xDot/8, yDot,(unsigned char *)DisBuffer,0, 0xff); // white  
  }
  else if(Label==3)
  {
    EPD_WriteDispRam(xDot/8, yDot, (unsigned char *)DisBuffer,0,0x00);  // black
    }else{
		EPD_WriteDispRam(xDot/8, yDot, (unsigned char *)DisBuffer,0,1);	// white
	}	
	EPD_Update();	
	#endif
	  
}


void WaveShare_EPD::EPD_Dis_Part(unsigned char xStart,unsigned char xEnd,unsigned long yStart,unsigned long yEnd,unsigned char *DisBuffer,unsigned char Label)
{
   unsigned int Xsize=xEnd-xStart;
      unsigned int Ysize=yEnd-yStart+1;
     if(Xsize%8!= 0){
      Xsize = Xsize+(8-Xsize%8);
      }
      Xsize = Xsize/8;
  unsigned int offset=yStart*16+xStart/8; 
	#ifdef WX
  unsigned long temp=yStart;
  yStart=295-yEnd;yEnd=295-temp;
  EPD_SetRamArea(xStart,xEnd,yEnd%256,yEnd/256,yStart%256,yStart/256);
  EPD_SetRamPointer(xStart/8,yEnd%256,yEnd/256);
	if(Label==2)  EPD_WriteDispRam(xEnd-xStart, yEnd-yStart+1,(unsigned char *)DisBuffer,offset,0x00);
  else if(Label==3) EPD_WriteDispRam(xEnd-xStart, yEnd-yStart+1,(unsigned char *)DisBuffer,offset,0xff);
  else  EPD_WriteDispRam(Xsize, Ysize,(unsigned char *)DisBuffer,offset,1);  
  EPD_Update_Part();
  ReadBusy();
  //EPD_SetRamArea(xStart,xEnd,yEnd%256,yEnd/256,yStart%256,yStart/256);
  //EPD_SetRamPointer(xStart/8,yEnd%256,yEnd/256);
  //EPD_WriteDispRam(Xsize, Ysize,(unsigned char *)DisBuffer,offset,1);  
	#endif
	
	#ifdef WF    
	EPD_SetRamArea(xStart,xEnd,yStart/256,yStart%256,yEnd/256,yEnd%256);
  if(Label==2)  EPD_WriteDispRam(xEnd-xStart, yEnd-yStart+1,(unsigned char *)DisBuffer,offset,0xff);
  else if(Label==3) EPD_WriteDispRam(xEnd-xStart, yEnd-yStart+1,(unsigned char *)DisBuffer,offset,0x00);
  else  EPD_WriteDispRam(Xsize, Ysize,(unsigned char *)DisBuffer,offset,1);  
  EPD_Update_Part();
	#endif
}
