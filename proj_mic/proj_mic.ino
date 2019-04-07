static const int MIC_PIN = A0;
float x = 0;
float y = 0;
float h = 0;
float bufor[20];
int n = 0;
int avg = 0;
double t_01;
double t_10;
double dt;

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
  h = 2 * sqrt(h*h); // 2 razy wartosc bezwzgledna

  // Bufor w celu uśrednienia sygnału
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

  // Dyskretyzacja {1, 0}
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
        Serial.println('|');
      }
    } else if (discrLast == 1 and discr ==0) { // if from 1 to 0
      t_10 = micros();
      dt = t_10 - t_01;
      if (dt > 200000 and dt < 1000000) {
          Serial.print('-');
      } else if (dt <= 200000 and dt > 50000) {
          Serial.print('.');
      }   
    }
  } 
  
//  Serial.print(x);
//  Serial.print(',');
//  Serial.print(avg);
//  Serial.print(',');
//  Serial.println(temp*300);

  discrLast = discr;
  delay(1);
}

// LowPassFilter()

float LowPassFilter(float input, float output, float alpha) {
  return output + alpha * (input - output);
}
