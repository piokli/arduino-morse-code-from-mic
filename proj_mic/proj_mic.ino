static const int MIC_PIN = A0;

const char *englishAlphabet[] = {"A", "B", "C", "D", "E", "F", "G", 
                          "H", "I", "J", "K", "L", "M", "N",
                          "O", "P", "Q", "R", "S", "T", "U", 
                          "V", "W", "X", "Y", "Z"};
                                
const char *morseCodes[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
                            "....", "..", ".---.", "-.-", ".-..", "--", "-.",
                            "---", ".--.", "--.-", ".-.", "...", "-", "..-", "..._",
                            ".--", "-..-", "-.--", "--.."};

char msg[32];

float bufor[20];
float x = 0;
float y = 0;
float h = 0;

double t_01;
double t_10;
double dt;

int n = 0;
int avg = 0;
int discr = 0;
int discrLast = 0;

void setup() { 
  pinMode(MIC_PIN, INPUT);
  Serial.begin(115200);
}

void loop() {
  x = analogRead(MIC_PIN);  // input
  y = LowPassFilter(x, y, 0.7); // after LPF
  
  h = x-y;  // "after" HPF
  h = 2 * sqrt(h*h);

  // Moving average
  bufor[n] = h;
  n++;
  if (n >= 19) {
    n = 0;
  }
  avg = 0;
  for (int i = 0; i < 20; i++) {
    avg = avg + bufor[i];
  }
  avg = avg / 20;

  // to {1, 0}
  if (avg > 50) {
    discr = 1; 
  } else {
    discr = 0;
  }

  // Signal to '.' and '_'
  if (discrLast != discr) {
    if (discrLast == 0 and discr == 1) { // if from 0 to 1
      t_01 = micros();
      dt = t_01 - t_10;
      if (dt > 1000000) {
        //Serial.println('|');
        Serial.println(msg);
        MorseCodeToEnglishAlphabet(msg);
        strcpy(msg, "");
      }
    } else if (discrLast == 1 and discr ==0) { // if from 1 to 0
      t_10 = micros();
      dt = t_10 - t_01;
      if (dt > 200000 and dt < 1000000) {
          //Serial.print('-');
          strcat(msg, "-");
      } else if (dt <= 200000 and dt > 50000) {
          //Serial.print('.');
          strcat(msg, ".");
      }   
    }
  } 

  

  discrLast = discr;
  delay(1);
}

// LowPassFilter()
float LowPassFilter(float input, float output, float alpha) {
  return output + alpha * (input - output);
}

void MorseCodeToEnglishAlphabet(char m[]) {
  int alphabetLength = 27;
  for (int i = 0; i < alphabetLength - 1; i++) {
    if (strcmp(morseCodes[i], m) == 0) {
      strcat(m, englishAlphabet[i]);
    }
  }
  Serial.println(m);
}
