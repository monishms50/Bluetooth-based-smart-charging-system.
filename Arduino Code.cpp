#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 7, 6, 5, 4);
int Rly = A1; 

char a_byte = 0;// defining a character data for a variable
String a_str;// creating a string having space
int ay=100;//charging per
char lat[25];
  
void setup() {
  pinMode(Rly, OUTPUT);
 digitalWrite(Rly, LOW);
Serial.begin(9600);// initializing the Serial communication
lcd.begin(16,2);// initializing the dimensions of LCD
//Serial.println("Enter a website");// data to be printed
lcd.clear();
  }
void loop() {
   ay = analogRead(A0);
// lcd.setCursor(0,0);
//lcd.print("BAT:");
//lcd.print(a_str);lcd.print(" Per..."); 
ay = map(ay, 0, 1023, 0, 100);
lcd.setCursor(0,1);
lcd.print("SET:");
lcd.print(ay);  
lcd.print(" % ");  
  if (Serial.available() > 0) { /*checking if any input is given on the Serial monitor*/    
a_byte = Serial.read(); /*read if there is any input and save it in the character data type variable*/    

    if (a_byte != '\n') {/*to display the data on the next line one line space is added to the string */
a_str += a_byte;
    }
    else {
      lcd.clear();
Serial.println(a_str); // print the string on the serial monitor
 lcd.setCursor(0,0);
lcd.print("BAT:");
lcd.print(a_str);lcd.print(" % "); 

if (a_str.toInt() >= ay){digitalWrite(Rly, LOW);Serial.println("CHARGER OFF");
lcd.setCursor(9,0);
lcd.print("CHARGER");
lcd.setCursor(9,1);
lcd.print("  OFF  ");}
else{digitalWrite(Rly, HIGH);Serial.println("CHARGER ON");
lcd.setCursor(9,0);
lcd.print("CHARGER");
lcd.setCursor(9,1);
lcd.print("  ON   ");}
a_str = "";
Serial.println("");// printing the string having space
    }
  }  
}
void Lcd_Decimal3(unsigned char com,unsigned char com1,unsigned int val)
  {
      unsigned int Lcd_h,Lcd_hr,Lcd_t,Lcd_o;
      lcd.setCursor(com,com1);
      Lcd_h=val/100;
      Lcd_hr=val%100;
      Lcd_t=Lcd_hr/10;
      Lcd_o=Lcd_hr%10;
    
      lcd.setCursor(com,com1);
      lcd.write(Lcd_h+0x30);
      lcd.setCursor(com+1,com1);
      lcd.write(Lcd_t+0x30);
      lcd.setCursor(com+2,com1);
      lcd.write(Lcd_o+0x30);
  }
