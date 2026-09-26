///////////////////////////////////////////////// SETUP //////////////////////////////////////////////////////

//calls in libraries
#include <Arduino.h>
#include <Adafruit_ST7789.h> //display screen
#include <Adafruit_BNO08x.h> //sensor
#include <math.h>
#include <AceButton.h>


//define display screen
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas(240, 135);


//define misc variables
using namespace ace_button;

#define BNO08X_RESET -1
Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

void setReports();


//defines button D1 and links it to AceButton library
int pinD1 = 1;
AceButton button(pinD1);


//define states of "AxisMode" variable
enum AxisMode {
  mode_both, //0
  mode_x, //1
  mode_y, //2
  mode_count //3
};

//set initial state of "AxisMode"
AxisMode curMode = mode_both;


//defines "change_AxisMode" - D1 double click cycles through axis modes
void change_AxisMode (AceButton* button, uint8_t eventType, uint8_t buttonState) {
  //Serial.print(F("handleEvent(): eventType: "));
  //Serial.print(AceButton::eventName(eventType));
  //Serial.print(F("; buttonState: "));
  //Serial.println(buttonState);

  if (eventType == (uint8_t)AceButton::kEventDoubleClicked) {
    curMode = (AxisMode) ((curMode + 1) % AxisMode::mode_count);
  }
}

//?// IF THIS IS A 1-TIME USE, DOES IT NEED TO BE A VOID FUNCTION //?//
//defines "rawData" - D1 long press kills LEDs and displays raw data on screen
void rawData (AceButton* button, uint8_t eventType, uint8_t buttonState) {
  //Serial.print(F("handleEvent(): eventType: "));
  //Serial.print(AceButton::eventName(eventType));
  //Serial.print(F("; buttonState: "));
  //Serial.println(buttonState);

  if (eventType == (uint8_t)AceButton::kEventLongPressed) {
    //kill LEDs
    //display raw data
  }
}






//main setup
void setup(void) {
  Serial.begin(115200);
  delay(2000);

  //setup for button D1
  pinMode(pinD1, INPUT_PULLDOWN);
  button.init(pinD1, LOW);

  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(change_AxisMode);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);

  /*
  //try to initialize
  if (!bno08x.begin_I2C()) {
    Serial.println("Failed to find BNO08x chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("BNO08x Found!");
  */

  //turn on screen
  display.init(135, 240);
  display.setRotation(3);
  canvas.setTextColor(ST77XX_GREEN);
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, 1);
  canvas.setTextSize(2);

  setReports();
}

void loop() {
  button.check(); 
  delay(10);

  if (bno08x.wasReset()) {
    Serial.print("sensor was reset ");
    setReports();
  }

  if (!bno08x.getSensorEvent(&sensorValue)) {
    return;
  }

  //define float variables "x", "y", "z", "alpha", and "beta"
  float x = sensorValue.un.accelerometer.x;
  float y = sensorValue.un.accelerometer.y;
  float z = sensorValue.un.accelerometer.z;
  float alpha = atan2(x, sqrt(y*y + z*z)) * RAD_TO_DEG; //angle about x-axis
  float beta = atan2(y, z) * RAD_TO_DEG; //angle about y-axis

  //print values
  /* 
  Serial.print("Accelerometer - x: ");
  Serial.print(x);
  Serial.print(" y: ");
  Serial.print(y);
  Serial.print(" z: ");
  Serial.print(z);
  */ 
  //prints tilt angle based corresponding "AxisMode" menu
  if (curMode == AxisMode::mode_both || curMode == AxisMode::mode_x) {
    Serial.print(" alpha: ");
    Serial.print(alpha);
  }
  if (curMode == AxisMode::mode_both || curMode == AxisMode::mode_y) {
    Serial.print(" beta: ");
    Serial.println(beta);
  }
}

void setReports(void) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(SH2_ACCELEROMETER)) {
    Serial.println("Could not enable accelerometer");
  } else {
    Serial.println("Set accelerometer report... success!");
  }
}