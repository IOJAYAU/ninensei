#include <Wire.h>
#include <AccelStepper.h>

#define SLAVE_ADDRESS 0x08  // I2C address of the Arduino Uno
#define MotorInterfaceType 4
#define heatPlate 10

byte templ[6];  // data buffer to store the received data
byte time[8];

String result = "", min = "";
int incomingByte = 0, tetime;
int setTime = 0, run = false;

AccelStepper stepper = AccelStepper(MotorInterfaceType, 2, 3, 4, 5);
AccelStepper stepperH = AccelStepper(MotorInterfaceType, 6, 7, 8, 9);

void setup() {
  setTime = 0;
  tetime = 0;
  Wire.begin(SLAVE_ADDRESS);  // initialize I2C communication as slave
  Wire.onReceive(receiveData);  // register receiveData function to be called when data is received
  Serial.begin(9600);
  stepper.setMaxSpeed(1000);
  stepperH.setMaxSpeed(750);
  stepperH.setAcceleration(500);
  pinMode(heatPlate,OUTPUT);
}

void loop() {
  stepper.setSpeed(500); // max at 900 is the best case, but 500 is the best case for all
  stepper.runSpeed();
  if(run == true){
      Serial.println("  POURING  ");
      setTime = 0;
      Serial.println(millis());
      stepperH.setCurrentPosition(0);
      stepperH.moveTo(2600); //7000 for 25%, 8000 for 50% volume
      stepperH.runToPosition();
      stepperH.moveTo(0);// 550 for 0, 25 and 700 for 50
      stepperH.runToPosition();
      stepperH.disableOutputs();
      run = false;
      delay(1000);
  }
}

void receiveData(int byteCount) {  
  if (run == false){
    float temp = getTemp();
    int mini = getMin();

    Serial.print("Time: ");
    Serial.print(mini);

    Serial.print(" Temperature: ");
    Serial.print(temp);  

    digitalWrite(heatPlate, 1);

    if(setTime == 0 && temp >= 100){
      tetime = mini + 1;
      if(tetime >= 60){
        tetime -= 60;
      }
      Serial.print(" finaleTime: ");
      Serial.print(tetime);
      Serial.print("  ");
      setTime = 1;
      run = 0;
    }

    Serial.print(" setTime: ");
    Serial.print(setTime);
    Serial.print(" finaleTime: ");
    Serial.println(tetime);

    if(temp > 130){
      digitalWrite(heatPlate, 0);
    }else if(temp < 120){
      digitalWrite(heatPlate, 1);
    }

    if(run == 0 && setTime == 1 && mini == tetime) {
      digitalWrite(heatPlate, 1);
      // a90Step();
      run = true;
    }
  } 
}

void a90Step(){
  stepperH.setCurrentPosition(0);
  stepperH.moveTo(7000); //7000 for 25%, 8000 for 50% volume
  stepperH.runToPosition();
  stepperH.moveTo(550);// 550 for 0, 25 and 700 for 50
  stepperH.runToPosition();
}

float getTemp(){
  int i = 0;
  while (Wire.available()) {  // loop through all received bytes
    templ[i] = Wire.read();  // read each byte and store it in the data buffer
    i++;
  }

  // int lenght = sizeof(templ);
  // for (int i = 0; i <= lenght; i++) {
  //   Serial.println(templ[i]);
  // }
  
  for (int i = 1; i <= 5; i++) {
    result.concat(templ[i]);
  }
  
  float temperature = result.toFloat()/1000;
  Serial.println(temperature);

  if(temperature < 18){
    temperature *= 10;
    result = "";
    return temperature;
  }else{
    result = "";
    return temperature;
  }
  // Serial.println(temperature);
  // result = "";
}

int getMin(){
  int i = 0;
  while (Wire.available()) {  // loop through all received bytes
    time[i] = Wire.read();  // read each byte and store it in the data buffer
    i++;
  }
  
  int lenght = sizeof(time);
  // Serial.println(lenght);
  
  for (int i = 15; i <= 17; i++) {
    min.concat(time[i]);
  }

  for (int i = 15; i <= 17; i++) {
    Serial.print(time[i]);
    Serial.print(", ");
  }

  int minutes = min.toFloat();
  Serial.println(minutes);

  if(minutes > 60){
    minutes = minutes/10;
    min = "";
  } else{
    minutes = minutes;
    min = "";
  }

  return minutes;
}
