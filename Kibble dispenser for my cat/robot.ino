#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>


// Initialization of the LCD:
LiquidCrystal_I2C lcd(0x20,20,4);

// Initialization of the servos:
Servo myservo;

#define nb_eat 3 

// Variables:
uint8_t actual_h;  // 0 => 23
uint8_t actual_m;  // 0 => 59
uint8_t actual_s;  // 0 => 59

const char msg[20] = "Need to update time.";
uint8_t flag = 0;
uint8_t eat[nb_eat][3] = {{8, 0 , 0},
                {12, 0, 0},
                {18, 0, 0}};


void setup() 
{
  // Initilization of the Serial Port:
  Serial.begin(9600);

  // Attach the servo the the pin 9:
  myservo.attach(9);

  // Initilization of the servo to 0°:
  myservo.write(0);

  // Initialization of the LCD:
  lcd.init();
  lcd.clear();

  // Initialization of the system:
  init_system(); 
}

void loop() 
{
  cat_bowl();
} 

void init_system()
{
  // Test of the LCD:
  lcd.backlight();    
  lcd.setCursor(0, 0);   
  lcd.print(" LCD ===> Ok"); 
  
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);   
  
  // Need to set the time (Because we don't have a RTC clock):
  Scroll_txt(19 ,msg, 0);
  delay(2000);
  lcd.clear();

  // The user need to set the time on the serial monitor:
  Serial.println("Need the configure the time:");
  Serial.println("Enter hour:");
  while (Serial.available() == 0) {
    // Wait...
  }
  // Retrieve the hour:
  actual_h = Serial.parseInt();
  // Clear the Serial buffer.
  flushSerialBuffer(); 

  // Same for the minute and second:
  Serial.println("Enter minutes : ");
  while (Serial.available() == 0) {
    // Wait...
  }
  actual_m = Serial.parseInt();
  flushSerialBuffer(); // Clear buffer

  Serial.println("Enter seconds : ");
  while (Serial.available() == 0) {
    // Wait...
  }
  actual_s = Serial.parseInt();
  flushSerialBuffer(); // Clear Serial Buffer...

  // Print the time on the serial monitor:
  Serial.print(actual_h);
  Serial.print(" : ");
  Serial.print(actual_m);
  Serial.print(" : ");
  Serial.println(actual_s);
}

void cat_bowl()
{
  lcd.clear();
  
  // Update the time:
  // Add a second for each iteration of the "loop" function:
  actual_s = actual_s + 1;
  // Check if:
  // 0 <= seconds < 60  
  // 0<= minutes < 60
  // 0<= hour < 24
  if (actual_s == 60){
    actual_s = 0;
    actual_m ++;
  }
  if (actual_m == 60)
  {
    actual_m = 0;
    actual_h ++;
  }
  if (actual_h == 24)
  {
    actual_s = 0;
    actual_m = 0;
    actual_h = 0;
  }

  // Print the time on the LCD:
  lcd.setCursor(0, 0);   
  if (actual_h < 10){
    // For having a 2 numbers hour
    lcd.print(0);
    lcd.print(actual_h);
  }
  else{
    lcd.print(actual_h);
  }
  
  lcd.print(" : ");
  
  // Same for minutes and secondes:
  if (actual_m < 10){
    lcd.print(0);
    lcd.print(actual_m);
  }
  else{
    lcd.print(actual_m);
  }
  lcd.print(" : ");
  
  if (actual_s < 10){
    lcd.print(0);
    lcd.print(actual_s);
  }
  else{
    lcd.print(actual_s);
  }

  // Test if its time to eat:
  // If it is time: the servos will be at 180 for 2 seconds, and the cat food falls.
  
  // 4 types of flag:
  // 0 : it is not time to eat   | 1: it is time, servo needs to be at 180°  |
  // 2 : Waiting, servos at 180° the cat food falls  |  3: Stop, servo needs to go at 0°
  for (uint8_t i = 0; i < nb_eat; i ++)
  {
    if (actual_h == eat[i][0] && actual_m == eat[i][1]  && actual_s == eat[i][2]) 
    {
      flag = 1;
      break;
    }
    else if (actual_h == eat[i][0] && actual_m == eat[i][1]  && actual_s == eat[i][2] + 1) 
    {
      flag = 2;
      break;
    } 
    else if (actual_h == eat[i][0] && actual_m == eat[i][1]  && actual_s == eat[i][2] + 2)
    {
      flag = 3;
      break;
    }
    else {
      flag = 0;
    }
  }

  // Check the flag, and move the servo:  
  lcd.setCursor(0, 1); 
  switch (flag){
    case 1: 
      lcd.print("It s time");
      for (uint8_t pos = 0; pos <= 180; pos += 1) {
        myservo.write(pos);              
        delay(5.555);    // 180 x 5.555 = 1000 ms = 1 s                 
      }
      break;
    case 2:
      lcd.print("Wait...");
      delay(1000);
      break;
    case 3:
      lcd.print("close");
      for (uint8_t pos = 180; pos > 0; pos -= 1) { 
        myservo.write(pos);             
        delay(5.555);     // 180 x 5.555 = 1000 ms = 1 s                 
      }
      break;
    default: 
      lcd.print("Not time");
      delay(1000);        // Wait a second
  } 
}

// Clear Serial:
void flushSerialBuffer() {
  while (Serial.available() > 0) {
    char c = Serial.read();
  }
}

// Make the text on the LCD move to the left:
void Scroll_txt(int8_t size, const char msg[50], uint8_t line)
{
  for (uint8_t i = 0; i < size; i++)
  {
    lcd.setCursor(15-i, line);
    lcd.print(msg);
    delay(250);
  }
}
