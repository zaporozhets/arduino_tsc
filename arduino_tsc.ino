#include <Wire.h>

#define X1 (A0)
#define X2 (A2)
#define Y1 (A1)
#define Y2 (A3)

#define IRQ_PIN (13)

#define GUARD_TIME (50)

volatile int isTouchedFlag = 0;
volatile unsigned long lastTouchTimeStamp = 0;
volatile int xCoord = 0;
volatile int yCoord = 0;

void setup() {
  Serial.begin(115200);
  Serial.print("Hello world\n");

  Wire.begin(0x55);
  Wire.onRequest(requestEvent);

  pinMode(IRQ_PIN, OUTPUT);
  digitalWrite(IRQ_PIN, LOW);
}

void requestEvent()
{
  uint8_t data[] = {0x80 | isTouchedFlag,
                    (xCoord) & 0xFF, (xCoord >> 8) & 0xFF,
                    (yCoord) & 0xFF, (yCoord >> 8) & 0xFF
                   };
  Wire.write(data, sizeof(data));

#if 0
  Serial.print(__func__);
  Serial.print(": X = ");
  Serial.print(xCoord);

  Serial.print(" Y = ");
  Serial.print(yCoord);

  Serial.print(" touch = ");
  Serial.println(isTouchedFlag);
#endif
}

void loop() {
  if (isTouched()) {

    isTouchedFlag = 1;
    xCoord = readX();
    yCoord = readY();
    lastTouchTimeStamp = millis();

    sendInterrupt();
  } else {
    if (isTouchedFlag && ((millis() - lastTouchTimeStamp) > GUARD_TIME)) {
      isTouchedFlag = 0;
      sendInterrupt();
    }
  }
}

int isTouched(void)
{
  pinMode(Y1, INPUT_PULLUP);
  pinMode(Y2, INPUT_PULLUP);

  pinMode(X1, OUTPUT);
  digitalWrite(X1, LOW);
  pinMode(X2, OUTPUT);
  digitalWrite(X2, LOW);

  delayMicroseconds(100); // wait for charge

  return !digitalRead(Y1);
}

int readX(void)
{
  pinMode(Y1, INPUT);
  pinMode(Y2, INPUT);

  pinMode(X1, OUTPUT);
  digitalWrite(X1, LOW);
  pinMode(X2, OUTPUT);
  digitalWrite(X2, HIGH);

  return analogRead(Y1);
}

int readY(void)
{
  pinMode(X1, INPUT);
  pinMode(X2, INPUT);

  pinMode(Y1, OUTPUT);
  digitalWrite(Y1, LOW);
  pinMode(Y2, OUTPUT);
  digitalWrite(Y2, HIGH);

  return analogRead(X1);
}

void sendInterrupt(void) {
  digitalWrite(IRQ_PIN, HIGH);
  delay(1); 
  digitalWrite(IRQ_PIN, LOW);
  delay(5);
}
