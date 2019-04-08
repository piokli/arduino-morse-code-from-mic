static const int MIC_PIN = A0;
float inputFromMic[] = {0, 0};
float filterOutput[] = {0, 0};
float secondFilterOutput[] = {0, 0};
float absoluteValue[] = {0, 0};
const int n = 1;

void setup() {
  pinMode(MIC_PIN, INPUT);
  Serial.begin(115200);
}

void loop() {
  inputFromMic[n-1] = inputFromMic[n];
  inputFromMic[n] = analogRead(MIC_PIN);

  filterOutput[n-1] = filterOutput[n];
  filterOutput[n] = highPassFilter(inputFromMic, filterOutput, 0.3);

  absoluteValue[n-1] = abs(filterOutput[n-1]);
  absoluteValue[n] = abs(filterOutput[n]);

  /*zamiast drugiego filtra chyba lepiej "moving average" 
    np. na 10 ostatnich próbek - chyba ze nie będzie zadnych błędów
  */
  secondFilterOutput[n-1] = secondFilterOutput[n];
  secondFilterOutput[n] = lowPassFilter(4*absoluteValue, secondFilterOutput, 0.3);

  Serial.print(inputFromMic[n]);
  Serial.print(',');
  Serial.print(filterOutput[n]);
  Serial.print(',');
  Serial.println(secondFilterOutput[n]);
  delay(1);
}

float highPassFilter(float x[], float y[], float a) {
  return y[n] = a * (y[n-1] + x[n] - x[n-1]);
}

float lowPassFilter(float x[], float y[], float a) {
  return y[n] = y[n-1] + a * (x[n] - y[n-1]);
}
