/*
      Morse Code to English Alphabet

      In a few words this sketch allows to interpret series of buzzer's "beeps" as morse code and
      then translate them to idividual english alphabet letters (and numbers).

      The circuit:
      * components: buzzer, electrolyt microphone, button, 1k resistor
      * circuit template will be attached as an .png file
      
      Last modified 25.05.2019
*/

/* PINs */
const int MIC_PIN = A0;

/* Arrays and a buffer for translating morse code */
const char *englishAlphabet[] = {"A", "B", "C", "D", "E", "F", "G",
                                 "H", "I", "J", "K", "L", "M", "N",
                                 "O", "P", "Q", "R", "S", "T", "U",
                                 "V", "W", "X", "Y", "Z",
                                 "1", "2", "3", "4", "5",
                                 "6", "7", "8", "9", "0"
                                };
const char *morseCode[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
                           "....", "..", ".---.", "-.-", ".-..", "--", "-.",
                           "---", ".--.", "--.-", ".-.", "...", "-", "..-",
                           "..._", ".--", "-..-", "-.--", "--..",
                           ".----", "..---", "...--", "....-", ".....",
                           "-....", "--...", "---..", "----.", "-----"
                          };
char msg[32];

/* Variables for getting data and filtering */
const int n = 1;
const int SIGN_BUFF_SIZE = 20;

float inputFromMic[] = {0, 0}; // {<current_probe>, <last_probe>} == {n, n-1}
float filterOutput[] = {0, 0};
float absoluteValue[] = {0, 0};
int discreteSignal[] = {0, 0};
float avg = 0;
float signalBuffer[SIGN_BUFF_SIZE];


void setup() {
  pinMode(MIC_PIN, INPUT);
  Serial.begin(115200);
}


void loop() {
  /* Acquiring data from microphone */
  inputFromMic[n-1] = inputFromMic[n];
  inputFromMic[n] = analogRead(MIC_PIN);
  /* Passing through high pass filter */
  filterOutput[n-1] = filterOutput[n];
  filterOutput[n] = highPassFilter(inputFromMic, filterOutput, 0.000145, 2000);
  /* Getting absolute value of filtered signal */
  absoluteValue[n-1] = abs(filterOutput[n-1]);
  absoluteValue[n] = abs(filterOutput[n]);
  
  /* Averaging signal using a buffer */
  static int k = 0;
  signalBuffer[k] = absoluteValue[n];
  k++;
  if (k >= SIGN_BUFF_SIZE) {
    k = 0;
  }

  avg = 0;
  for (int i = 0; i < SIGN_BUFF_SIZE; i++) {
    avg += signalBuffer[i];
  }

  /* Discretization of signal */
  discreteSignal[n-1] = discreteSignal[n];
  if (avg > 800) {
    discreteSignal[n] = 1;
  } else {
    discreteSignal[n] = 0;
  }
  
  discreteSignalToMorseCode(discreteSignal);
}


/* High Pass Filter function gets ... - działanie filtru?? jak obliczony? */
float highPassFilter(float x[], float y[], float dt, float fc) {
  float a = 1/(2*3.14*dt*fc + 1);
  return y[n] = a * (y[n-1] + x[n] - x[n-1]);
}


/* blabla... */
/* przerwa miedzy literami > 1s
   '-' od 200ms do 1s 
   '.' od 50ms do 200ms*/
void discreteSignalToMorseCode(int d[2])
{
  /* variables for measuring beep's duration */
  static double t_01;
  static double t_10;

  if (d[n-1] != d[n]) {                    //if signal has changed then:
    double dt;
    if (d[n-1] == 0 and d[n] == 1) {       //if leading edge (0 -> 1) then:
      t_01 = micros();
      dt = t_01 - t_10;
      if (dt > 1000000) {
        //Serial.println('|');
        //Serial.print(msg);
        morseCodeToEnglishAlphabet(msg);
        Serial.println(msg);
        strcpy(msg, "");
      }
    } else if(d[n-1] == 1 and d[n] == 0) {  //if trailing edge (1 -> 0) then:
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
}


/* Fucnction... */
void morseCodeToEnglishAlphabet(char m[]) {
  const int ALPHABET_LENGTH = 37;
  for (int i = 0; i < ALPHABET_LENGTH - 1; i++) {
    if (strcmp(morseCode[i], m) == 0) {
      strcpy(m, englishAlphabet[i]);
    }
  }
}