#define TO_MILIAMPS 25 //1 means ~25 mA

#define A_VAL_MAX 128
signed int aVal[A_VAL_MAX];
uint8_t valPt = 0;

// zero-point value
signed int zeroPoint = 512;

//RMS calculating
unsigned long sumSquares = 0;
unsigned int numSqTicks = 0;

//impulse LED
unsigned long long ledOffTime = 0;
unsigned long miliAmpSecs = 0;
#define MAS_IMPULSE 1000 //pulse every 1000 miliampereseconds (1 A in second)

// clock buffers
unsigned long long read_clock_buf = 0;
unsigned long long calc_rms_buf = 0;

void setup() {
  pinMode(10, OUTPUT);

  Serial.begin(9600);

  analogReference(INTERNAL);
}

void loop() {

  //if LED on and need to be reset
  if (ledOffTime > 0 && ledOffTime < millis()) {
    digitalWrite(10,LOW);
    ledOffTime = 0;
  }

  //every 1 s
  if (millis() > calc_rms_buf+1000) {
    calc_rms_buf = millis();
    //calculate RMS

    unsigned long meanSquare = numSqTicks > 0 ? sumSquares/numSqTicks : 0; //prevent div by zero
    unsigned int RMS = sqrt(meanSquare)*TO_MILIAMPS;

    Serial.println(RMS);
    
    //restart counters
    sumSquares=0;
    numSqTicks=0;

    //integrate to pulse diode
    miliAmpSecs += RMS;
    if (miliAmpSecs >= MAS_IMPULSE) {
      
      //impulse LED once
      digitalWrite(10,HIGH);

      //set time to turn LED off
      ledOffTime = millis()+100;

      //restart counter
      miliAmpSecs = 0;
    }
  }

  //every 1 ms
  if (millis() != read_clock_buf) {
    read_clock_buf = millis();

    //get analog value and store in buffer
    int read = analogRead(A1);

    if (valPt >= A_VAL_MAX) {
      //overflow
      updateZero();
      valPt = 0;
    } else {
      aVal[valPt++] = read;
    }

    //add to meanSquare
    sumSquares += pow(read-zeroPoint,2);
    numSqTicks++;

  }
}

// find and update zero point
void updateZero() {
  unsigned int min = aVal[0];
  unsigned int max = aVal[0];

  for (uint8_t i=1;i<valPt;i++) {
    if (aVal[i] < min) min = aVal[i]; 
    if (aVal[i] > max) max = aVal[i]; 
  }

  //midpoint between min and max
  zeroPoint = (min+max)/2;
}

