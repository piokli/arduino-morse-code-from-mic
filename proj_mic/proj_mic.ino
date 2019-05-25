/*
      Morse Code to English Alphabet

      In a few words this sketch allows to interpret series of buzzer's "beeps" as morse code and
      then translate them to idividual english alphabet letters (and numbers).

      The circuit:
      * components: buzzer, electrolyt microphone, button, 1k resistor
      * circuit template will be attached as an .png file
      
      Last modified 06.05.2019
*/

/* PINs */
static const int MIC_PIN = A0;

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
float inputFromMic[] = {0, 0}; // {<current_probe>, <last_probe>}
float filterOutput[] = {0, 0};
float absoluteValue[] = {0, 0};
float avg = 0;
const int wielkoscBufora = 20;
float bufor[wielkoscBufora];
int k = 0;
int discr[] = {0, 0};

/* Used for measuring beep's time length */
double t_01;
double t_10;



/* M A I N */



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
  /* Getting absolute */
  absoluteValue[n-1] = abs(filterOutput[n-1]);
  absoluteValue[n] = abs(filterOutput[n]);
  /* Filling the bufor */
  bufor[k] = absoluteValue[n];
  k++;
  if (k >= wielkoscBufora) {
    k = 0;
  }
  /* Averaging signal */
  avg = 0;
  for (int i = 0; i < wielkoscBufora; i++) {
    avg += bufor[i];
  }
  /* Filtered analog signal to discrete (0 and 1) */
  discr[n-1] = discr[n];
  if (avg > 800) {
    discr[n] = 1;
  } else {
    discr[n] = 0;
  }
  
  discrToMorse(discr);
}



/* High Pass Filter function gets ... */
float highPassFilter(float x[], float y[], float dt, float fc) {
  float a = 1/(2*3.14*dt*fc + 1);
  return y[n] = a * (y[n-1] + x[n] - x[n-1]);
}


/* blabla... */
/* przerwa miedzy literami > 1s
   '-' od 200ms do 1s 
   '.' od 50ms do 200ms*/
void discrToMorse(int d[2])
{
  // najpierw wykrywamy zmianę z 0 na 1 i na odwrót
  if (d[n-1] != d[n]) {
    double dt;
    if (d[n-1] == 0 and d[n] == 1) {
      /* leading edge 0 -> 1 */
      t_01 = micros();
      dt = t_01 - t_10;
      if (dt > 1000000) {
        //Serial.println('|');
        //Serial.print(msg);
        morseCodeToEnglishAlphabet(msg);
        Serial.println(msg);
        strcpy(msg, "");
      }
    } else if(d[n-1] == 1 and d[n] == 0) {
      /* trailing edge 1 -> 0 */
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
  int alphabetLength = 37;
  for (int i = 0; i < alphabetLength - 1; i++) {
    if (strcmp(morseCode[i], m) == 0) {
      strcpy(m, englishAlphabet[i]);
    }
  }
}
