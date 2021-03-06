/*
      Morse Code to English Alphabet

      In a few words this sketch allows to interpret series of buzzer's "beeps"
      as morse code and then translate them to idividual english 
      alphabet letters (and numbers).

      The circuit:
      * components: buzzer, electrolyt microphone, button, 1k resistor
      * circuit template is attached as an .png file
      
      Last modified 25.05.2019
*/

/* PINs */
const int MIC_PIN = A0;

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


/* 
  Acts like a discrete high pass filter.

  Parameters:
  'x' input
  'y' output
  'dt' time interval == average time between function uses
  'fc' frequency cutoff
  
  Returns:
  'y' filtering result 

  'a' is calculated from 'dt' and 'fc'.
  'dt' was measured and averaged and 'fc' was chosen
  knowing that the buzzer can generate 4 kHz ± 500 Hz.
  From measured 'dt' we know that overall sampling rate is around 6.5 GHz. 
  */
float highPassFilter(float x[], float y[], float dt, float fc) {
  float a = 1/(2*3.14*dt*fc + 1);
  return y[n] = a * (y[n-1] + x[n] - x[n-1]);
}


/*
  Converts binary signal to morse code.
  
  Parameters:
  d discrete signal to analyze

  Returns:
  nothing

  Function analyzes processed signal. Checks for dots and dashes.
  If "beep" classifies as a dot or a dash a '.' or a '-' is written
  to 'msg_buffer'.
  When signal is down for over 1s the 'msg_buffer' is passed
  to 'morseCodeToEnglishAlphabet()' function and the 'msg_buffer' is cleared.
*/
void discreteSignalToMorseCode(int d[2])
{
  /* Constants defining dot and dash */
  const int MIN_DOT_LENGTH = 50000;
  const int MAX_DOT_LENGTH = 200000;
  const int MAX_DASH_LENGTH = 1000000;
  /* Variables for measuring beep's duration and a buffer*/
  static double t_01;
  static double t_10;
  static double dt;
  static char msg_buffer[32];

  if (d[n-1] == 0 and d[n] == 1) { //if leading edge (0 -> 1) then:
    t_01 = micros();
    dt = t_01 - t_10;
    if (dt > MAX_BEEP_LENGTH) {
      morseCodeToEnglishAlphabet(msg_buffer);
      Serial.println(msg_buffer);
      strcpy(msg_buffer, "");
    }
  } else if(d[n-1] == 1 and d[n] == 0) { //if trailing edge (1 -> 0) then:
      t_10 = micros();
      dt = t_10 - t_01;
      if (dt > MAX_DOT_LENGTH and dt < MAX_DASH_LENGTH) { //'-' when between 200ms-1s
        strcat(msg_buffer, "-");
      } else if (dt > MIN_DOT_LENGTH and dt <= MAX_DOT_LENGTH) { //'.' when between
        strcat(msg_buffer, ".");                                 // 50ms-200ms
      }
    }
}


/*
  Translates series of dots and dashes to english alphabet and numbers.
  
  Parameters:
  m buffer containing dots and dashes

  Returns:
  nothing

  Function compares series of dots and dashes from 'm' to the ones
  from 'englishAlphabet' array.
  When the same, a correct letter or number is then overwritten to 'm'. 
*/
void morseCodeToEnglishAlphabet(char m[]) {
  /* Arrays for translating morse code */
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
  const int ALPHABET_LENGTH = 37;

  for (int i = 0; i < ALPHABET_LENGTH - 1; i++) {
    if (strcmp(morseCode[i], m) == 0) {    //compares message to morse code
      strcpy(m, englishAlphabet[i]);       //if found then translates
    }
  }
}