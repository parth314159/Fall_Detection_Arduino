#include <SD.h> //Included SD Card Library
#include <SPI.h>


//.......................Accelerometer Variables...........................................//

//Analog input pins 0, 1, and 2
//are what I send the x,y, and z
//accelerometer outputs to, respectively
int xAxisPin = A3;
int yAxisPin = A2;
int zAxisPin = A1;
 
//Variables to hold the returned
//ADC data from the analog input
//pins
int xAxisValADC = 0;
int yAxisValADC = 0;
int zAxisValADC = 0;
 
//Variables to hold the voltage
//values after converting from ADC
//units to mV
float xAxisValmV = 0;
float yAxisValmV = 0;
float zAxisValmV = 0;


//variables to hold final acceleration 
float fxAxis = 0.0;
float fyAxis = 0.0;
float fzAxis = 0.0;

 
//My Arduino Uno has a 10-bit
//AD converter, with a max value
//of 1023
int ADCMaxVal = 1023;
 
//The AD converter voltage
//is powered by 5V
float mVMaxVal = 5000;
 
//I measured the power going to the
//accelerometer as actually being 
//3230 mV, so I use this value to 
//define the mid-point
float supplyMidPointmV = 3230 / 2;
 
//Since the supply is actually 3230
//mV, I know the output will be 323mV
//per 1g detected
int mVperg = 323;
 
//Multiply any acquired ADC value
//by mVPerADC to convert to mV
float mVPerADC = mVMaxVal / ADCMaxVal;


//..................................................SD Card Variables..................................................................//
const int SD_pin = 10;

//variables to manage delay
unsigned long SD_start_time = millis();
unsigned long SD_wait_time = 400;
unsigned long SD_target_time = millis()+SD_wait_time;

String SD_file_ext = ".txt";

String SD_report_file_name = "";
String SD_report_string = "";


void setup() {
  // Setting up serial port
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Intializing SD Card....");

  if (!SD.begin(SD_pin))
  {
    Serial.println("SD Card Initialization Failed");
  }
  else
  {
    Serial.println("SD Card Initialization completed");
  }
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
}

///*check number and format for a string*/
//String num_check(int num)
//{
//  if (num < 10)
//    return "0"+(String)num;
//  else
//    return (String)num;
//}
//
///*convert hr,min and sec to string*/
//String get_time_string(int hour, int minute, int second)
//{
//  return num_check(hour)+":"+num_check(minute)+":"+num_check(second);
//}


/*function to queue report message */
void log_report(String msg){
  SD_report_string += msg + "\n";
}


void log_file_sd(String filename,String msg)
{
  File file = SD.open(filename, FILE_WRITE);
  if(file)
  {
    file.print(msg);
    Serial.print(msg); //printing on serial for testing, don't forget to remove it if not needed
    file.close();
  }
  else
  {
    Serial.println("Error in opening "+filename+" file");
  }
}

void log_sd()
{
  SD_report_file_name =(String) "Data" + SD_file_ext;
  log_file_sd(SD_report_file_name,SD_report_string);
  SD_report_string = "";
}

void prepare_report()
{
  xAxisValADC = analogRead(xAxisPin);
  yAxisValADC = analogRead(yAxisPin);
  zAxisValADC = analogRead(zAxisPin);
   
  xAxisValmV = xAxisValADC * mVPerADC;
  yAxisValmV = yAxisValADC * mVPerADC;
  zAxisValmV = zAxisValADC * mVPerADC;

  fxAxis = (xAxisValmV - supplyMidPointmV) / mVperg;
  fyAxis = (yAxisValmV - supplyMidPointmV) / mVperg;
  fzAxis = (zAxisValmV - supplyMidPointmV) / mVperg;

  String temp = (String) fxAxis + "," + (String) fyAxis+ "," + (String) fzAxis + "\n";
//  
//  Serial.print();
//  Serial.print(",");
//  Serial.print();
//  Serial.print(",");
//  Serial.print();
//  Serial.println();

  log_report(temp);
  //Serial.println(SD_report_string);
}


void loop() {

  if(SD_target_time <= millis())
  {
      prepare_report();
      log_sd(); //log to SD
      SD_target_time += SD_wait_time;
  }
}
