#include "util.hpp"
#include "Arduino.h"
#include "hal_trng.h"
#include "hal_wdt.h"
#include "uint256.hpp"
#include "hal_sha.h"


void sha256_data( uint8_t *message, uint8_t len, uint8_t *digest)
{
  hal_sha256_context_t context = {0};
  hal_sha256_init(&context);
  hal_sha256_append(&context, message, len);
  hal_sha256_end(&context, digest);
}

uint16_t BinaryStringtoArray(String data){
  uint16_t output = 0;
  if (data.length()>16)
  {
    Serial.println("BinaryStringtoArray length too large");
    return 0;
  }
  for (int i = 0; i < data.length(); ++i)
  {
    if (data[i] == '1')
    {
      output |= (uint16_t)1<<(data.length()-i-1);
    }
  }
  return output;
}
String ArraytoString_binary(uint8_t* buffer,uint32_t len){
  String data;
  for (int i = 0; i < len; ++i)
  {
    String binary_string = String(buffer[i],BIN);
    uint8_t append0 = 8 - binary_string.length();
    for (int i = 0; i < append0; ++i)
    {
      data += "0";
    }
    data += binary_string;
  }
  return data;
}


String ArraytoString(uint8_t* buffer,uint32_t len,uint32_t String_len){
  if (String_len<len)
  {
    Serial.println("Error String_len< buffer len");
    return "";
  }
  String data;
  uint32_t append_0 = String_len - len;
  for (int i = 0; i < append_0; ++i)
  {
    data+= "00";
  }
  data +=ArraytoString(buffer,len);
  return data;
}


String ArraytoString(uint8_t* buffer,uint32_t len){
  String data;
  for (int i = 0; i < len; ++i)
  {
    if (buffer[i]<=0x0F)
    {
      data += "0";
    }
    data += String(buffer[i],HEX);
  }
  return data;
}

void print_vector(std::vector<uint8_t> VEC){
  for(int i=0;i<VEC.size();i++){
    if(VEC[i]<16){
    Serial.print("0");
    }
    Serial.print(VEC[i],HEX);
  }
}
String vector2string(std::vector<uint8_t> VEC){
    String s;
    for(int i=0;i<VEC.size();i++){
      if(VEC[i]<16){
        s+="0";
      }
      s+=String(VEC[i],16);
    }
  return s;
}
void randomNumbergenerator_init(){
  hal_trng_status_t ret = hal_trng_init();
}
uint32_t randomNumbergenerator(uint8_t digit){
  uint32_t number = 0;
  uint32_t max = pow(10,digit);
  
  hal_trng_get_generated_random_number(&number); 
  return number%max;
}

uint32_t randomUint32_t_generator(){
  uint32_t number = 0;
  hal_trng_get_generated_random_number(&number); 
  return number;
}


void random_UUID_generator(char* array){
  uint32_t number[4] = {0};
  for (int i = 0; i < 4; ++i)
  {
    hal_trng_get_generated_random_number(&number[i]); 
  }

  uint16_t buffer[4] = {0};
  buffer[0] = number[1]>>16;
  buffer[1] = number[1];
  buffer[2] = number[2]>>16;
  buffer[3] = number[2];
  

  sprintf (array, "%08x-%04x-%04x-%04x-%04x%08x",number[0], buffer[0], buffer[1],buffer[2],buffer[3], number[3]);
  //Serial.println((char*)array);
}

void random_UUID_generator_head(char* array){
  uint32_t number = 0;
  hal_trng_get_generated_random_number(&number); 
  sprintf (array, "%08x",number);
  //Serial.println((char*)array);
}


void WDT_Reset(){
  Serial.println("Reset");
  pinMode(A0,OUTPUT);
  hal_wdt_software_reset();
  while(1){
    digitalWrite(A0,HIGH);
    delay(100);
    digitalWrite(A0,LOW);
    delay(100);

  }
}


String vector2string_DEC(std::vector<uint8_t> data,uint8_t quanty){

  uint256_t value;
  convertUint256BE(&data[0],data.size(), &value);
  char value_str[100] = {0};
  tostring256(&value, 10, value_str,100);
  String value_string = String(value_str);
  //Serial.println(value_string);
  //Serial.println(value_string.length());
  //Serial.println(quanty);


  switch(quanty){
    case 0: //raw output
      return value_string;
      break;
    case 1:{
      // Gwei = 10^9 = 10 char
      if (value_string.length()>9)
      {
        value_string = value_string.substring(0,value_string.length()-9) + "." + value_string.substring(value_string.length()-9);
      }
      else{
        String append_0;
        for (int i = 0; i < 10 - value_string.length(); ++i)
        {
          append_0 += "0";
        }
        value_string = "0." + append_0 + value_string;
      }
      break;
    }
    case 2:{
      // eth = 10^18 = 19 char 
      if (value_string.length()>18)
      {
        value_string = value_string.substring(0,value_string.length()-18) + "." + value_string.substring(value_string.length()-18);
      }
      else{
        String append_0;
        for (int i = 0; i < 18 - value_string.length(); ++i)
        {
          append_0 += "0";
        }
        value_string =  "0." +append_0+ value_string;
      }
      break;
    }

  }
  //strip 0
  for (int i = value_string.length()-1; i>0; i--)
  {
    if (value_string.charAt(i) == '0')
    {
      value_string.remove(i);
    }
    else
    {
      break;
    }
  }
  //strip . if only . is at the end of string
  if (value_string.charAt(value_string.length()-1) == '.')
  {
    value_string.remove(value_string.length()-1);
  }

  return value_string;
}


void convertUint256BE(uint8_t *data, uint32_t length, uint256_t *target) {
    uint8_t tmp[32];
    memset(tmp, 0, 32);
    memmove(tmp + 32 - length, data, length);
    readu256BE(tmp, target);
}

