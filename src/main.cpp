///////////////////////////////////////////////// SETUP //////////////////////////////////////////////////////

//calls in libraries
#include <Arduino.h>
#include <Adafruit_ST7789.h> //display screen
#include <Adafruit_BNO08x.h> //sensor
#include <math.h>
#include <AceButton.h>


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


//defines "change_mode"
void change_mode (AceButton* button, uint8_t eventType, uint8_t buttonState) {
  
  //print message
  Serial.print(F("handleEvent(): eventType: "));
  Serial.print(AceButton::eventName(eventType));
  Serial.print(F("; buttonState: "));
  Serial.println(buttonState);
  
  //D1 double click cycles through axis modes
  if (eventType == (uint8_t)AceButton::kEventDoubleClicked) {
    curMode = (AxisMode) ((curMode + 1) % AxisMode::mode_count);
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
  //buttonConfig->setEventHandler(change_mode);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  
  Serial.println("Adafruit BNO08x test!");

  //try to initialize
  if (!bno08x.begin_I2C()) {
    Serial.println("Failed to find BNO08x chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("BNO08x Found!");

  setReports();
}

///////////////////////////////////////////////// MAIN CODE //////////////////////////////////////////////////////
//main code
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
  
  /*
  //print raw data to workspace
  Serial.print("Accelerometer - x: ");
  Serial.print(x);
  Serial.print(" y: ");
  Serial.print(y);
  Serial.print(" z: ");
  Serial.print(z);
  delay(100);
  */

  //displays angle value(s) based on which "AxisMode" is selected
  if (curMode == AxisMode::mode_both || curMode == AxisMode::mode_x) {
    Serial.print("Alpha: ");
    Serial.print(alpha);
    delay(100);
  }
  if (curMode == AxisMode::mode_both || curMode == AxisMode::mode_y) {
    Serial.print("Beta: ");
    Serial.println(beta);
    delay(100);
  }
}


//defines "setReports"
void setReports(void) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(SH2_ACCELEROMETER)) {
    Serial.println("Could not enable accelerometer");
  } else {
    Serial.println("Set accelerometer report... success!");
  }
}