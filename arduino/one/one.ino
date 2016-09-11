//2016年9月11号第一次调试(youngda)
//{"datastreams":[{"stu_id":"201415541001","test_id":"SitAndReach","datapoints":[{"value":21.32}]}]}
#include <SoftwareSerial.h>
String comdata = "";
SoftwareSerial WIFISerial(8, 9);     // RX, TX
const int TrigPin = 2;                // 设定SR04连接的Arduino引脚
const int EchoPin = 3; 
const int key = 4;
int a = 250;                          //a是光照参考值
float distance, max_t = -20.00; 
#define test_id "SitAndReach"
void wifi_data(){
      
  while (Serial.available() > 0)  
    {
        comdata += char(Serial.read());
        delay(20);
    }
  if (comdata.length() > 0)
    {
        Serial.println(comdata);
        //WIFISerial.println(comdata);
       // delay(500);
        comdata = "";
    }  

}
bool doCmdOk(String data, char *keyword)
{
  bool result = false;
  if (data != "")   //对于tcp连接命令，直接等待第二次回复
  {
    WIFISerial.println(data);  //发送AT指令
    Serial.print("SEND: ");
    Serial.println(data);
  }
  if (data == "AT")   //检查模块存在
    delay(2000);
  else
    while (!WIFISerial.available());  // 等待模块回复

  delay(200);
  if (WIFISerial.find(keyword))   //返回值判断
  {
    Serial.println("do cmd OK");
    result = true;
  }
  else
  {
    Serial.println("do cmd ERROR");
    result = false;
  }
  while (WIFISerial.available()) WIFISerial.read();   //清空串口接收缓存
  delay(500); //指令时间间隔
  return result;
}
void send_data(String student_id,String testmode_id,float thisdata){
  Serial.print("{\"datastreams\":[{\"stu_id\":\"");
  Serial.print(student_id);
  Serial.print("\",\"test_id\":\"");
  Serial.print(testmode_id);
  Serial.print("\",\"datapoints\":[{\"value\":");
  Serial.print(thisdata);
  Serial.print("}]}]}");
  }
void setup() 
{   // 初始化串口通信及连接SR04的引脚
        Serial.begin(9600);
        pinMode(TrigPin, OUTPUT); 
        pinMode(key,INPUT);
    // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
        pinMode(EchoPin, INPUT); 
    //  以下部分为wifi的配置    
        WIFISerial.begin(9600);
        WIFISerial.setTimeout(3000);
        delay(3000);
        Serial.println("Ready to receive data:");
        delay(2000);
        while (!doCmdOk("AT", "OK"));
        while (!doCmdOk("AT+CWMODE=1", "OK"));            //工作模式
        while (!doCmdOk("AT+CWJAP=\"TP-LINK_B06460\",\"xgxy901901\"", "OK"));
        while (!doCmdOk("AT+CIPSTART=\"TCP\",\"192.168.1.101\",8081", "OK"));
        while (!doCmdOk("AT+CIPMODE=1", "OK"));           //透传模式
        while (!doCmdOk("AT+CIPSEND", ">"));              //开始发送
        WIFISerial.println("00000000000");
        delay(500);                
}
void distance_max()
{
      // 产生一个10us的高脉冲去触发TrigPin 
        digitalWrite(TrigPin, LOW); 
        delayMicroseconds(2); 
        digitalWrite(TrigPin, HIGH); 
        delayMicroseconds(10);
        digitalWrite(TrigPin, LOW); 
     //检测脉冲宽度，并计算出距离
        distance = 50-(pulseIn(EchoPin, HIGH) / 58.00);
        if(distance>max_t)
        {
          delay(50);
          if(distance>max_t)
          max_t = distance; 
        }   
}
void loop() 
{
 if (WIFISerial.available()){           //软串口
  Serial.write(WIFISerial.read());     //写入软串口读取的数据 ————>  串口
  }
 wifi_data();                           //发送学号信息
 if(analogRead(A0) >= a ) {          
    delay(10); 
    if(analogRead(A0) >= a ){             
    distance_max();                                                       
        }
      }
    if(digitalRead(key) == HIGH){
      delay(10);
        if(digitalRead(key) == HIGH){
        delay(10);
        while(digitalRead(key) == HIGH);
        send_data(comdata,test_id,max_t);
        //WIFISerial.println(max_t);
       // delay(500); 
        max_t = -20;  
   } 
 }      
delay(100); 
}
