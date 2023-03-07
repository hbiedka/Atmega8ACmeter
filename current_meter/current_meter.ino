#define TO_MILIAMPS 25 //1 means ~25 mA

#define A_VAL_MAX 128
signed int aVal[A_VAL_MAX];
uint8_t valPt = 0;

// zero-point value
signed int zeroPoint = 512;

//RMS calculating
unsigned long sumSquares = 0;
unsigned int numSqTicks = 0;

//LED last state
bool st = false;

// clock buffers
unsigned long long read_clock_buf = 0;
unsigned long long calc_rms_buf = 0;

void setup() {
  pinMode(10, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  //every 1 s
  if (millis() > calc_rms_buf+1000) {
    calc_rms_buf = millis();
    //calculate RMS

    unsigned long meanSquare = numSqTicks > 0 ? sumSquares/numSqTicks : 0; //prevent div by zero
    unsigned int RMS = sqrt(meanSquare);

    Serial.println(RMS*TO_MILIAMPS);
    
    //restart counters
    sumSquares=0;
    numSqTicks=0;

    //diode
    digitalWrite(10,st);
    st = !st;
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

