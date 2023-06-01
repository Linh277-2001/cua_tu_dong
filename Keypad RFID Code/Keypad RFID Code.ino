#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;  //four rows
const byte COLS = 3;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};                                     // Ma trận Keypad
byte rowPins[ROWS] = { A0, 4, 3, 2 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 8, 7, 6 };      //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char key_code[3];                      // Mảng chứa mật khẩu nhập vào
char password[3] = { '1', '2', '3' };  // Initial Password
int i = 0;                             // Biến chạy của chuỗi ký tự

int card1[4]{ 54, 205, 29, 31 };  // UID thẻ từ được nhận diện
int card2[4]{ 90, 33, 66, 178 };


constexpr uint8_t RST_PIN = 9;  // Define SS_PIN of the RC522 RFID Reader to digital pin 10 of the Arduino
constexpr uint8_t SS_PIN = 10;  // Define RST_PIN of the RC522 RFID Reader to digital pin 9 of the Arduino
MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position

int Buzzer = A1;    // Buzzer output is analog pin A1
int RedLed = A2;    // Red LED output is analog pin A2
int GreenLed = A3;  // Green LED output is analog pin A3

unsigned int k = 0;
byte nuidPICC[4];  // UID number of your Mifare card. This is a UID number with 4 pairs of digits. Example: EB 70 C0 BC

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(RedLed, OUTPUT);    // Define RedLed as OUTPUT
  pinMode(GreenLed, OUTPUT);  // Define GreenLed as OUTPUT
  pinMode(Buzzer, OUTPUT);    // Define Buzzer as OUTPUT
  //digitalWrite(lock, HIGH);
  myservo.attach(5);  // attaches the servo on pin 5 to the servo object
  SPI.begin();        // Init SPI bus
  rfid.PCD_Init();    // Init MFRC522
  // =====================================================
  lcd.init();  // Khoi tao LCD
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  RFID & KEYPAD ");
  lcd.setCursor(0, 1);
  lcd.print("  Lock Project  ");
  // lcd.clear();
  // ====================================================
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;  // Mảng chứa UID của RFID
  }
  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  // =====================================================
  // lcd.setCursor(0, 0);
  // lcd.print("Scan Your Card");
  // lcd.setCursor(0, 1);
  // lcd.print("Enter the Pass");
}

void loop() {
  char key = keypad.getKey();
  if (key != NO_KEY) {  // Kiểm tra có nhấn phím k?
    key_code[i++] = key;
    k = i;
    tone(Buzzer, 3000);  // Make a short beep for a DigitKey press on the keypad
    delay(100);
    noTone(Buzzer);
    delay(100);
  }

  if (k == 3) {
    if (!strncmp(password, key_code, 3))  // Mã nhập = password
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Access Granted");   // Hiển thị thành công
      digitalWrite(GreenLed, HIGH);  // Make a nice beep for the correct Pincode and set the Green LED high for 1 second
      delay(300);
      tone(Buzzer, 3000);
      delay(500);
      noTone(Buzzer);
      delay(100);
      tone(Buzzer, 3000);
      delay(500);
      noTone(Buzzer);
      delay(100);
      myservo.write(110);
      delay(2000);
      myservo.write(200);
      delay(200);
      myservo.write(200);
      digitalWrite(GreenLed, LOW);
      delay(200);
      // =============== RESET======================
      i = k = 0;
      lcd.setCursor(0, 0);
      lcd.print("Scan Your Card");
      lcd.setCursor(0, 1);
      lcd.print("Enter the Pass");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Access Denied");  // Hiển thị thất bại
      digitalWrite(RedLed, HIGH);  // Make a nice beep for the correct Pincode and set the Green LED high for 1 second
      delay(300);
      tone(Buzzer, 500, 900);
      delay(1500);
      noTone(Buzzer);
      delay(200);
      tone(Buzzer, 500, 900);
      delay(1500);
      noTone(Buzzer);
      delay(500);
      digitalWrite(RedLed, LOW);
      delay(200);
      // =============== RESET======================
      i = k = 0;
      lcd.setCursor(0, 0);
      lcd.print("Scan Your Card");
      lcd.setCursor(0, 1);
      lcd.print("Enter the Pass");
    }
  }
  // Look for new cards
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
    Serial.println(nuidPICC[i]);
  }

  if (card1[0] == nuidPICC[0] && card1[1] == nuidPICC[1] && card1[2] == nuidPICC[2] && card1[3] == nuidPICC[3]) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    digitalWrite(GreenLed, HIGH);  // Make a nice beep for the correct Pincode and set the Green LED high for 1 second
    delay(300);
    tone(Buzzer, 3000);
    delay(500);
    noTone(Buzzer);
    delay(100);
    tone(Buzzer, 3000);
    delay(500);
    noTone(Buzzer);
    delay(100);
    myservo.write(110);
    delay(2000);
    myservo.write(200);
    delay(200);
    myservo.write(200);
    digitalWrite(GreenLed, LOW);
    delay(200);
    //=====================RESET======================
    lcd.setCursor(0, 0);
    lcd.print("Scan Your Card");
    lcd.setCursor(0, 1);
    lcd.print("Enter the Pass");
  }

  else if (card2[0] == nuidPICC[0] && card2[1] == nuidPICC[1] && card2[2] == nuidPICC[2] && card2[3] == nuidPICC[3]) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    digitalWrite(GreenLed, HIGH);  // Make a nice beep for the correct Pincode and set the Green LED high for 1 second
    delay(300);
    tone(Buzzer, 3000);
    delay(500);
    noTone(Buzzer);
    delay(100);
    tone(Buzzer, 3000);
    delay(500);
    noTone(Buzzer);
    delay(100);
    myservo.write(110);
    delay(2000);
    myservo.write(200);
    delay(200);
    myservo.write(200);
    digitalWrite(GreenLed, LOW);
    delay(200);
    //=============================RESET==================
    lcd.setCursor(0, 0);
    lcd.print("Scan Your Card");
    lcd.setCursor(0, 1);
    lcd.print("Enter the Pass");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    digitalWrite(RedLed, HIGH);  // Make a nice beep for the correct Pincode and set the Green LED high for 1 second
    delay(300);
    tone(Buzzer, 500, 900);
    delay(1500);
    noTone(Buzzer);
    delay(200);
    tone(Buzzer, 500, 900);
    delay(1500);
    noTone(Buzzer);
    delay(500);
    digitalWrite(RedLed, LOW);
    delay(200);
    //=============================RESET==================
    lcd.setCursor(0, 0);
    lcd.print("Scan Your Card");
    lcd.setCursor(0, 1);
    lcd.print("Enter the Pass");
  }
}