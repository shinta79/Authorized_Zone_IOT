#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <Adafruit_Fingerprint.h>

//------------------Define Components---------------
// Define keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 2, 3, 4, 5 };
byte colPins[COLS] = { 6, 7, 8, 9 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);  // Define LCD
Servo myServo;                              // Define servo

volatile int finger_status = -1;  //Define fingerprint sensor
SoftwareSerial mySerial(12, 13);  // TX/RX on fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int pos = 0;
int servo = 9;
int greenLED = 10;
int redLED = 11;
String password = "12345";  // Define password
String input = "";
int currentposition = 0;

//-----------------Set Up----------------
void setup() {
  mainScreen();
  Serial.begin(9600);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  myServo.attach(servo);  // SERVO ATTACHED
  myServo.write(0);

  lcd.begin(16, 2);

  while (!Serial)
    ;  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains ");
  Serial.print(finger.templateCount);
  Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
}

void loop() {
  if (currentposition == 0) {
    mainScreen();
    input = "";
  }

  finger_status = getFingerprintID();
  if (finger_status != -1 and finger_status != -2) {
    Serial.println("Match");
    digitalWrite(greenLED, HIGH);
    unlockdoor();
    currentposition = 0;
  } else {
    if (finger_status == -2) {
      Serial.println("NOT Match");
      unauthorized();
      currentposition = 0;
    }
  }
  delay(100);

  char code = keypad.getKey();

  if (code != NO_KEY) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PASSWORD:");
    lcd.setCursor(7, 1);
    lcd.print(" ");
    lcd.setCursor(7, 1);

    switch (code) {
      case '*':
        if (input == password) {
          digitalWrite(greenLED, HIGH);
          unlockdoor();
          currentposition = 0;
        } else {
          unauthorized();
          currentposition = 0;
        }
        break;
      case '#':
        currentposition = 0;
        break;
      default:
        for (int i = 0; i <= currentposition; ++i) {
          lcd.print("*");
        }
        input = input + code;
        Serial.println(input);
        currentposition++;
        break;
    }
  }
  // LOOP END
}

//----------------Unlock the door---------------
void unlockdoor() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println(" ");
  lcd.setCursor(1, 0);
  lcd.print("Access Granted");
  lcd.setCursor(4, 1);
  lcd.println("WELCOME!!");
  lcd.setCursor(15, 1);
  lcd.println(" ");
  lcd.setCursor(16, 1);
  lcd.println(" ");
  lcd.setCursor(14, 1);
  lcd.println(" ");
  lcd.setCursor(13, 1);
  lcd.println(" ");
  Serial.println("CODE CORRECT YOU ARE AUTHORIZED");
  delay(1000);

  for (pos = 0; pos < 90; pos += 5)  // goes from 180 degrees to 0 degrees
  {
    myServo.write(pos);  // tell servo to go to position in variable 'pos'
    delay(5);            // waits 15ms for the servo to reach the position
  }
  delay(3000);
  countDown();
  delay(1000);

  lcd.clear();
}

//-----------------Unauthorized access--------------
void unauthorized() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("UNAUTHORIZED!");
  lcd.setCursor(15, 1);
  lcd.println(" ");
  lcd.setCursor(4, 1);
  lcd.println("GET AWAY!");

  lcd.setCursor(13, 1);
  lcd.println(" ");
  Serial.println("CODE INCORRECT YOU ARE UNAUTHORIZED");

  digitalWrite(redLED, HIGH);
  delay(500);
  digitalWrite(redLED, LOW);
  delay(500);
  digitalWrite(redLED, HIGH);
  delay(500);
  digitalWrite(redLED, LOW);
  delay(500);
  digitalWrite(redLED, HIGH);
  delay(500);
  digitalWrite(redLED, LOW);

  mainScreen();
}

//------------------Clear Screen----------------
void clearscreen() {
  lcd.setCursor(0, 0);
  lcd.println(" ");
  lcd.setCursor(0, 1);
}

//-----------------Main Screen----------------
void mainScreen() {
  lcd.setCursor(0, 0);
  lcd.println("*ENTER THE CODE*");
  lcd.setCursor(4, 1);
  lcd.println("TO OPEN!!");
}

//-----------------Count Down To Re-Lock---------------
void countDown() {
  delay(1200);

  lcd.clear();
  digitalWrite(greenLED, HIGH);


  delay(200);
  // 1
  lcd.setCursor(2, 0);
  lcd.println("GET IN WITHIN:::");
  lcd.setCursor(4, 1);
  lcd.print("5");
  delay(1000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.println("GET IN WITHIN:");
  digitalWrite(greenLED, LOW);
  // 2
  digitalWrite(greenLED, HIGH);
  lcd.setCursor(2, 0);
  lcd.println("GET IN WITHIN:");
  lcd.setCursor(4, 1);
  lcd.print("4");
  delay(1000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.println("GET IN WITHIN:");
  digitalWrite(greenLED, LOW);
  // 3
  digitalWrite(greenLED, HIGH);
  lcd.setCursor(2, 0);
  lcd.println("GET IN WITHIN:");
  lcd.setCursor(4, 1);
  lcd.print("3");
  delay(1000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.println("GET IN WITHIN:");
  digitalWrite(greenLED, LOW);
  // 4
  digitalWrite(greenLED, HIGH);
  lcd.setCursor(2, 0);
  lcd.println("GET IN WITHIN:");
  lcd.setCursor(4, 1);
  lcd.print("2");
  delay(1000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.println("GET IN WITHIN:");
  digitalWrite(greenLED, LOW);
  // 5
  digitalWrite(greenLED, HIGH);
  lcd.setCursor(4, 1);
  lcd.print("1");
  delay(1000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.println("GET IN WITHIN::");
  digitalWrite(greenLED, LOW);
  // 6
  digitalWrite(greenLED, HIGH);
  delay(40);
  digitalWrite(greenLED, LOW);
  delay(40);
  digitalWrite(greenLED, HIGH);
  delay(40);
  digitalWrite(greenLED, LOW);
  delay(40);
  digitalWrite(greenLED, HIGH);
  delay(40);
  digitalWrite(greenLED, LOW);
  delay(40);
  digitalWrite(greenLED, HIGH);
  delay(40);
  digitalWrite(greenLED, LOW);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("RE-LOCKING");
  delay(500);
  lcd.setCursor(12, 0);
  lcd.print(".");
  delay(500);
  lcd.setCursor(13, 0);
  lcd.print(".");
  delay(500);
  lcd.setCursor(14, 0);
  lcd.print(".");
  delay(400);
  lcd.clear();
  lcd.setCursor(4, 0);

  for (pos = 90; pos > 0; pos -= 5)  // goes from 0 degrees to 180 degrees
  {                                  // in steps of 1 degree
    myServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);
  }
  lcd.print("LOCKED!");
  delay(440);
}

//--------------------Get Fingerprint ID--------------------
int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != 2) {
    Serial.println(p);
  }
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != 2) {
    Serial.println(p);
  }
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -2;

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}