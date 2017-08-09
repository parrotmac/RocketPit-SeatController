
#define BUFFER_SIZE 12
#define SERIAL_BAUD 115200

char incomingBuffer[BUFFER_SIZE];
int bufferPosition = 0;

class MotorController {
  private:
  int positivePin;
  int negativePin;
  public:
  MotorController(int positive, int negative);
  void activatePositive();
  void activateNegative();
  void stopMotor();
  void interpretCommand(char dir);
};

MotorController::MotorController(int positive, int negative) {
  this->positivePin = positive;
  this->negativePin = negative;
  
  pinMode(this->positivePin, INPUT);
  pinMode(this->negativePin, INPUT);
}

void MotorController::activatePositive() {
  // Deactivate negative
  digitalWrite(this->negativePin, HIGH);
  pinMode(this->negativePin, INPUT);

  // Activate positive
  pinMode(this->positivePin, OUTPUT);
  digitalWrite(this->positivePin, LOW);
}

void MotorController::activateNegative() {
  // Deactivate positive
  digitalWrite(this->positivePin, HIGH);
  pinMode(this->positivePin, INPUT);

  // Activate negative
  pinMode(this->negativePin, OUTPUT);
  digitalWrite(this->negativePin, LOW);
}

void MotorController::stopMotor() {
  
  // Deactivate positive
  digitalWrite(this->positivePin, HIGH);
  pinMode(this->positivePin, INPUT);
  
  // Deactivate negative
  digitalWrite(this->negativePin, HIGH);
  pinMode(this->negativePin, INPUT);
  
}


MotorController* tiltController;
MotorController* frontController;
MotorController* distanceController;
MotorController* rearController;

void MotorController::interpretCommand(char dir) {
  if(dir == '+') {
    this->activatePositive();
  } else if(dir == '-') {
    this->activateNegative();
  } else {
    this->stopMotor();
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD);

  tiltController = new MotorController(4, 5);
  frontController = new MotorController(6, 7);
  distanceController = new MotorController(8, 9);
  rearController = new MotorController(10, 11);
}



void loop() {

  while(Serial.available() > 0) {
    
    char readByte = Serial.read();

    if(bufferPosition < BUFFER_SIZE) { 
      incomingBuffer[bufferPosition++] = readByte;
    }

    if(readByte == '\n') {
      
      switch(incomingBuffer[0]) {
        case 'T':
          tiltController->interpretCommand(incomingBuffer[2]);
          break;
        case 'F':
          frontController->interpretCommand(incomingBuffer[2]);
          break;
        case 'R':
          rearController->interpretCommand(incomingBuffer[2]);
          break;
        case 'D':
          distanceController->interpretCommand(incomingBuffer[2]);
          break;
         default:
         break;
      }
      
      bufferPosition = 0;
    }
    
  }

  

}
