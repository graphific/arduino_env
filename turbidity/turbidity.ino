//KEYESTUDIO Ds18b20 Digital Temperature Sensor(Waterproof)
//https://wiki.keyestudio.com/index.php/KS0329_Keyestudio_18B20_Waterproof_Temperature_Sensor_%2B_4.7K_Resistor_(3PCS)
// digital pin 3
#include <OneWire.h>
int DS18S20_Pin = 2; //DS18S20 Signal pin on digital pin 2
//Temperature chip i/o
OneWire ds(DS18S20_Pin);  // on digital pin 2

//https://www.teachmemicro.com/arduino-turbidity-sensor/
int sensorPin = A0;
float volt;
float ntu;

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}


//https://wiki.keyestudio.com/KS0414_Keyestudio_Turbidity_Sensor_V1.0
// the loop routine runs over and over again forever:
void loop() {
  float temperature = getTemp();
  
  Serial.print("temperature: ");  
  Serial.println(temperature);  
  
  // read the input on analog pin 0:
  //int sensorValue = analogRead(A0);
  // print out the value you read:
  //Serial.println(sensorValue);

  //float voltage = sensorValue * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V)
  //Serial.print("turbidity: ");
  //Serial.print(voltage);
  //Serial.println("% NTU");
  //3.5%=35000ppm=35000mg/L=4550NTU 
  //10-6 (PPM)=1ppm=1mg/L=0.13NTU (empirical formula) 
  //https://wiki.dfrobot.com/Turbidity_sensor_SKU__SEN0189
  //pure water, that is NTU < 0.5, it should output “4.1±0.3V” when temperature is 10~50℃.
  

  volt = 0;
  for(int i=0; i<800; i++)
  {
      volt += ((float)analogRead(sensorPin)/1023)*5;
  }
  volt = volt/800;
  //R:mine goes to 4.8 = 
  volt = volt-0.6;
  
  volt = round_to_dp(volt,1);
  if(volt < 2.5){
    ntu = 3000;
  }else{
    //equation on the graph is only true within the range of 2.5 V to 4.2 V. 
    //difference between the original equation and the equation here: 
    //(4352.9 versus 4353.8). 
    //This is because at 4.2 V, the NTU should be zero but mine gives 0.9.
    //This could be a rounding error on the ADC's part. 
    //I decided to filter out this 0.9 by subtracting it from the original equation. 
    //ntu = -1120.4*square(volt)+5742.3*volt-4353.8;  
    
    ntu = -1120.4*square(volt)+5742.3*volt-4353.8;  
  }
  //Serial.print("volt (turbidity sensor): ");  
  //Serial.println(volt);  
  Serial.print("NTU: ");  
  Serial.println(ntu);  
  delay(1000);        // delay in between reads for stability
}

float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius 
  byte data[12];
  byte addr[8];
  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  } 
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad   
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  ds.reset_search();   
  byte MSB = data[1];
  byte LSB = data[0];
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  return TemperatureSum;  
}
