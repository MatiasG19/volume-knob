#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>

#define ENCODER_CLK A0
#define ENCODER_DT A1
#define ENCODER_SW A2
#define ENCODER_STEPS 4

ClickEncoder *encoder;
int16_t last, value, lastEncoderValue;
unsigned long previousMillis = 0;

void timerIsr() {
  encoder->service();
}

void setup() {
  Serial.begin(9600);
  Consumer.begin();
  encoder = new ClickEncoder(ENCODER_DT, ENCODER_CLK, ENCODER_SW);

  Timer1.initialize(100);
  Timer1.attachInterrupt(timerIsr); 
  last = -1;
} 

void loop() {  
  // Encoder
  value += encoder->getValue();
  if((value - last) >= ENCODER_STEPS) {
    last = value; 
    Consumer.write(MEDIA_VOLUME_UP);
  }
  else if((last - value) >= ENCODER_STEPS) {
    last = value; 
    Consumer.write(MEDIA_VOLUME_DOWN);
  }

  // Compensate encoder inaccuracy
  if (lastEncoderValue == value) {
    unsigned long currentMillis = millis(); 
    if(currentMillis - previousMillis >= 500) {
      previousMillis = currentMillis;
      last = value;
    }
  }
  lastEncoderValue = value;

  // Button
  ClickEncoder::Button b = encoder->getButton();
  if (b != ClickEncoder::Open) {
    switch (b) {
      case ClickEncoder::Clicked:
        Consumer.write(MEDIA_PLAY_PAUSE);
        break;      
      case ClickEncoder::DoubleClicked:
        Consumer.write(MEDIA_VOLUME_MUTE);
        break;      
    }
  }

  delay(10);
}
