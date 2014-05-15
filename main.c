 //*********************************************************************************************************//
//  Code will startup in deg F mode.                                                                       //
//                                                                                                         //
//  Press and hold the button until you see both LEDs go solid for 2 seconds to change between F and C.    //
//  A 1 second solid LED color will indicate the new mode (deg F or C):                                    //
//  Red is for F, and Green is for C.                                                                      //
//                                                                                                         //
//  The Red LED will blink the temp tens digit after a 1 second strobe followed by a 1 second pause.       //
//  The Green LED will then blink the ones digit and the code will repeat.                                 //
//*********************************************************************************************************//

#define CAL_ADC_25T30 (*((unsigned int *) (0x10E8))) //The temp calibration data for 30 degC
#define CAL_ADC_25T85 (*((unsigned int *) (0x10EA))) //The temp calibration data for 85 degC

float t30 = CAL_ADC_25T30; //can only read this once otherwise the RAM access gets messed up
float t85 = CAL_ADC_25T85; //can only read this once otherwise the RAM access gets messed up
float cal_1, cal_2, cal_3, cal_4, cal_5, cal_6; //steps to apply the calibration data to the raw ADC value

int readADCvalue = 0; //raw ADC value
int mode = 0; //0 for F and 1 for C

void setup() {
  pinMode(RED_LED, OUTPUT); //mux the pin for output
  pinMode(GREEN_LED, OUTPUT); //mux the pin for output
  pinMode(PUSH2, INPUT_PULLUP); //mux the pin for intput with debounce enabled
  analogReference(INTERNAL2V5); //set internal reference voltage (set to 2.5V)
  Serial.begin(9600); //start serial interface at 9600 bps
}

void loop() 
{

  buttonSense(); //check to see if the button is pressed
  blinkLED(); //blink the LEDs to a predetermined pattern
  delay(1000); //pause for 1 second

  readADCvalue = analogRead(138); //get the raw ADC data. Must use "pin" 138 due to backend coding located in pins_energia.h
  
  //below formulas were taken from the MSP430 user guide for calibrating internal temp sensor
  cal_1 = t85 - t30;
  cal_2 = 85-30;
  cal_3 = cal_1 / cal_2;
  cal_4 = readADCvalue - t30;
  cal_5 = cal_3 * cal_4;
  cal_6 = cal_5 + 30; //final temp result in degC
    
  displayTemp(cal_6, mode); //pass temp info and mode info to be displayed on LEDs
  delay(1000); //pause for 1 second
  
}

void blinkLED(void)
{
  for(int i = 0; i <= 17; i++) //strobe the LEDs for ~ 1 second
  {
    Serial.println("************************");
    Serial.println("blinkLED");
    Serial.println("************************");
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    delay(30);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    delay(30);
  }
}

void displayTemp(float tempDisplay, int modeSel)
{
  int tensDigit = 0;
  int onesDigit = 0;
  
    Serial.println("************************");
    Serial.println("displayTemp");
    Serial.println("************************");
  
  
  Serial.print("Temperature: ");
  
  if(modeSel == 0) //if F we need to convert
  {
    tempDisplay = ((tempDisplay * 1.8) + 32);
    Serial.print(tempDisplay);
    Serial.println(" deg F");
  }
  else //this is just to make it easy to confirm the correct info in the serial monitor
  {
    Serial.print(tempDisplay);
    Serial.println(" deg C");
  }
  
  tensDigit = tempDisplay / 10; //get the 10s number
  onesDigit = (int) tempDisplay % 10; //divide by ten and then store the remainder in onesDigit
  
  while(tensDigit != 0) //blink the LED to match the 10s digit
  {
    digitalWrite(RED_LED, HIGH);
    delay(500);
    digitalWrite(RED_LED, LOW);
    delay(500);
    tensDigit--; //subtract 1 after every cycle
  }
  while(onesDigit != 0) //blink the LED to match the 1s digit
  {
    digitalWrite(GREEN_LED, HIGH);
    delay(500);
    digitalWrite(GREEN_LED, LOW);
    delay(500);
    onesDigit--; //subtact 1 after every cycle
  }
  
}

void buttonSense(void)
{

    Serial.println("************************");
    Serial.println("buttonSense");
    Serial.println("************************");

  if(PUSH2 == 0) //button is pressed
  {
    digitalWrite(RED_LED, HIGH); //turn on both LEDs for 2 seconds to indicate program sees the button is pressed
    digitalWrite(GREEN_LED, HIGH);
    delay(2000);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
  }
  
  if(mode == 1) //if we are currently in deg C
  {
    mode = 0; //change mode to deg F
    Serial.println("Switching to degF");
  }
  else //if we are currently in deg F
  {
    mode = 1; //change mode to deg C
    Serial.println("Switching to degC");
  }
  
  if(mode == 0) //temp will display in F
  {
  digitalWrite(RED_LED, HIGH);
  delay(1000);
  digitalWrite(RED_LED, LOW);
  }
  else //temp will display in C
  {
  digitalWrite(GREEN_LED, HIGH);
  delay(1000);
  digitalWrite(GREEN_LED, LOW); 
  }

}

