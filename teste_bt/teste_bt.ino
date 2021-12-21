//#include <SoftwareSerial.h>

//SoftwareSerial mySerial(0, 1); // RX, TX

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for Native USB only
//  }
//
//
//  // set the data rate for the SoftwareSerial port
//  mySerial.begin(9600);
  
    
}

void loop() // run over and over
{
//  if (mySerial.available()){
//    mySerial.write("Hello, world?");
//    mySerial.println("println");
////  Serial.write("serial");
//    delay(100);
//    Serial.write(mySerial.read());  
//    }

 if (Serial.available())
   Serial.write(Serial.read());
}
