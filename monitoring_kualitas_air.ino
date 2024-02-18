//program penerapan IoT dalam monitoring kualitas air ikan guppy menggunakan metode fuzzy tsukamoto

// Menghubungkan Ke Web Blynk 
#define BLYNK_TEMPLATE_ID "TMPL6225Zo11k"
#define BLYNK_TEMPLATE_NAME "Monitoring Kualitas Air Ikan Guppy"
#define BLYNK_AUTH_TOKEN "FWgkW8jgZlpupAHh-6Ytot1hNVi34PyC"
#define BLYNK_PRINT Serial

// Library Sensor
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp8266.h> 
#include <Wire.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Week Days
String weekDays[7]={"Ahad", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

//Month names
String months[12]={"Januari", "Februari", "Maret", "April", "Mai", "Juni", "Juli", "Agustus", "September", "Oktober", "November", "Desember"};

// deklarasi variabel-variael logika fuzzy
// deklarasi Keanggotaan himpunan suhu
float suhuDingin[] = {26, 29};     // <26, 26, 29
float suhuNetral[] = {26, 29, 32}; // 26, 29, 32
float suhuPanas[] = {29, 32};      // 29, 29, 32>
// deklarasi Keanggotaan himpunan pH
float pHAsam[] = {6, 7};      // <6, 6, 7
float pHNetral[] = {6, 7, 8}; // 6, 7, 8
float pHBasa[] = {7, 8};      // 7, 8, 8>
// deklarasi Keanggotaan himpunan output Kualitas Air
float kuairBuruk[] = {5, 12};  // <5, 10
float kuairBaik[] = {5, 12}; // 5, 10>

// deklarasi variabel derajat keanggotaan suhu
float MUsuhuDin, MUsuhuNet, MUsuhuPan; 
// deklarasi variabel derajat keanggotaan pH
float MUpHAs, MUpHNet, MUpHBas;        

// deklarasi variabel Rules untuk kualitas Air
float R1kuair, R2kuair, R3kuair, R4kuair, R5kuair, R6kuair, R7kuair, R8kuair, R9kuair;  
// deklarasi variabel Rules untuk kualitas Air
float Z1kuair, Z2kuair, Z3kuair, Z4kuair, Z5kuair, Z6kuair, Z7kuair, Z8kuair, Z9kuair;  

// deklarasi variabel Output Kualitas Air
float OutputKuair;  

// menampung string kualitas air 
String kualitasAir; 

// Koneksi ke Blynk
char auth[] = BLYNK_AUTH_TOKEN;  
char ssid[] = "SYAKIRA";
char pass[] = "jagungmanis";        

BlynkTimer timer;

// Deklarasi Variabel Sensor Suhu
const int pinData = 2; //Inisialisasi pin komunikasi di D4
OneWire oneWire(pinData); //Mengenalkan pin komunikasi
DallasTemperature SSuhu(&oneWire); //Memasukkan pin sensor
float nilaiSuhu; // menampung nilai suhu pada function
float TempValue; // menampung value suhu

// Deklarasi Variabel Sensor pH
const int ph_Pin = A0; // pin analog untuk membaca sensor pH
float nilaipH = 0;
float PH_step;
int nilai_analog_PH;
double TeganganPh;
float PH4 = 3.3;
float PH7 = 2.8;
float pHValue; // menampung nilai pH

// Deklarasi LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// menampilkan jam pada blynk
String formattedTime;

// menampilkan tanggal  pada blynk
int currentDay;
int currentMonth;
int currentYear;

void myTimerEvent()
{
  Blynk.virtualWrite(V0, nilaiSuhu);
  Blynk.virtualWrite(V1, nilaipH);
  Blynk.virtualWrite(V2, OutputKuair);
  Blynk.virtualWrite(V3, formattedTime);
  Blynk.virtualWrite(V4, String(currentDay) + "/" + String(currentMonth) + "/" + String(currentYear));
  
  if (OutputKuair >= 7)
  {   
  Blynk.logEvent("info_baik", String("Air Baik - Fuzzy : ") + (OutputKuair) + "| Suhu : " + (nilaiSuhu) + "| pH : " + (nilaipH));
  Serial.println("Kualitas Air Baik");
  }
  else
  {
  Blynk.logEvent("peringatan", String("Air Buruk - Fuzzy : ") + (OutputKuair) + "| Suhu : " + (nilaiSuhu) + "| pH : " + (nilaipH));
  Serial.println("Kualitas Air Buruk, Tolong Di Periksa !!!");
  }
}

void setup()
{
  Serial.begin(115200); 
  SSuhu.begin(); // mulai komunikasi dengan sensor suhu
  pinMode(ph_Pin, INPUT); // inisialisasi ph sebagai input
 
  Blynk.begin(auth, ssid, pass); // in blynk
  timer.setInterval(1000L, myTimerEvent);

  // koneksi ke wifi
  WiFi.begin(ssid, pass); 
  Serial.print("Menghubungkan Ke ");
  Serial.print(ssid); Serial.print(" ...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println('\n');
  Serial.print("Koneksi Terhubung ");
  Serial.println("IP address:\t ");
  Serial.println(WiFi.localIP());

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set timezone to Asia/Jakarta (WIB)
  timeClient.setTimeOffset(25200); // Offset in seconds for GMT+7

  lcd.init();
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("KUALITAS AIR");
  lcd.setCursor(0,1);
  lcd.print("FUZZY TSUKAMOTO"); 
  delay(2000);
}

void loop()
{
  // Waktu
  timeClient.update();

  time_t epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);

  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);  

  // menampilkan jam
  // int currentHour = timeClient.getHours();
  // Serial.print("Jam: ");
  // Serial.println(currentHour);  

  // ... (tambahkan informasi waktu yang diinginkan)
  struct tm *ptm = localtime(&epochTime);
    
  // Extract date components
  int currentDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;  // Month starts from 0
  int currentYear = ptm->tm_year + 1900;  // Year is counted from 1900

  Serial.print("Tanggal: ");
  Serial.print(currentDay);
  Serial.print("/");
  Serial.print(currentMonth);
  Serial.print("/");
  Serial.println(currentYear);

   // Extract day of the week
  int currentDayOfWeek = ptm->tm_wday;

  Serial.print("Hari: ");
  Serial.println(weekDays[currentDayOfWeek]);

  // menampilkan waktu di lcd
  lcd.clear(); 
  lcd.setCursor(3,0);
  lcd.print("");
  lcd.print(formattedTime); 
  delay(2000);

  // menampilkan tanggal dan hari di lcd
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("");
  lcd.print(currentDay);
  lcd.print("/");
  lcd.print(currentMonth);
  lcd.print("/");
  lcd.print(currentYear);

  lcd.setCursor(0,1);
  lcd.print("Hari: ");
  lcd.println(weekDays[currentDayOfWeek]);
  delay(2000);

  // in blynk
  Blynk.run();
  timer.run();
  Blynk.virtualWrite(V0, nilaiSuhu);
  Blynk.virtualWrite(V1, nilaipH);
  Blynk.virtualWrite(V2, OutputKuair);
  Blynk.virtualWrite(V3, formattedTime);
  Blynk.virtualWrite(V4, String(currentDay) + "/" + String(currentMonth) + "/" + String(currentYear));

  TempValue = takeTemperature(); // ambil data suhu
  pHValue = takepH(); // ambil data ph
  
  // Hitung Fuzzy Output KualitasAir menggunakan fungsi
  OutputKuair = fuzzyTsukamotoKuair(TempValue, pHValue);

  String data = String(TempValue, 2) + "/" +
                String(pHValue, 2) + "/" +
		            kualitasAir;

  Serial.print("Suhu Air:\t\t");
  Serial.print(TempValue);
  Serial.println(" °C");
 
  Serial.print("Kadar pH Air:\t\t");
  Serial.println(pHValue, 2);

  Serial.print("Fuzzy Output Kualitas Air: ");
  Serial.println(OutputKuair);

  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("Suhu Air : ");
  lcd.print(TempValue); // cetak nilai suhu
  lcd.setCursor(0,1);
  lcd.print("PH Air   : ");
  lcd.print(pHValue); // cetak nilai ph
  delay(4000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Fuzzy : ");
  lcd.print(OutputKuair); // cetak nilai fuzzy
  lcd.setCursor(0,1); 
  lcd.print("AIR   : "); // kondisi kualitas air

  // mengirim notifikasi ke blynk sesuai nilai fuzzy
  if(OutputKuair >= 7)
  {   
    lcd.setCursor(8,1); 
    lcd.print("BAIK"); 
    Serial.println("Kualitas Air : Baik"); 
  }
  else 
  { 
    lcd.setCursor(8,1);
    lcd.print("BURUK");
    Serial.println("Kualitas Air : Buruk");
  } 
  delay(4000);
}

  // Fungsi pengambilan suhu dari sensor
  float takeTemperature()
  {                                         
    // Minta sensor untuk membaca suhu
    SSuhu.requestTemperatures();     

    // Baca suhu dalam Celsius     
    nilaiSuhu = SSuhu.getTempCByIndex(0); 
    return nilaiSuhu;
  }

  // fungsi pengambilan ph dari sensor
  float takepH()   
  {
    // baca nilai analog dari sensor ph
    nilai_analog_PH = analogRead(ph_Pin); 
    TeganganPh = 3.3 / 1024.0 * nilai_analog_PH;
    PH_step = (PH4 - PH7) / 3;
    nilaipH = 7.00 + ((PH7 - TeganganPh) / PH_step);
    return nilaipH;
  }

// Deklarasi fungsi untuk menghitung Fuzzy Tsukamoto Kuair
float fuzzyTsukamotoKuair(float TempValue, float pHValue)
{
  // Proses Fuzzyfikasi Kualitas air
  // Mencari Fungsi Keanggotaan Suhu Dingin
  if (TempValue >= suhuDingin[1])
  {
    MUsuhuDin = 0;
  }
  else if (TempValue > suhuDingin[0] && TempValue < suhuDingin[1])
  {
    MUsuhuDin = (suhuDingin[1] - TempValue) / (suhuDingin[1] - suhuDingin[0]);
  }
  else if (TempValue <= suhuDingin[0])
  {
    MUsuhuDin = 1;
  }
  // Mencari Fungsi Keanggotaan Suhu Netral
  if (TempValue <= suhuNetral[0] || TempValue >= suhuNetral[2])
  {
    MUsuhuNet = 0;
  }
  else if (TempValue > suhuNetral[0] && TempValue < suhuNetral[1])
  {
    MUsuhuNet = (TempValue - suhuNetral[0]) / (suhuNetral[1] - suhuNetral[0]);
  }
  else if (TempValue >= suhuNetral[1] && TempValue < suhuNetral[2])
  {
    MUsuhuNet = (suhuNetral[2] - TempValue) / (suhuNetral[2] - suhuNetral[1]);
  }
  // Mencari Fungsi Keanggotaan Suhu Panas
  if (TempValue <= suhuPanas[0])
  {
    MUsuhuPan = 0;
  }
  else if (TempValue > suhuPanas[0] && TempValue < suhuPanas[1])
  {
    MUsuhuPan = (TempValue - suhuPanas[0]) / (suhuPanas[1] - suhuPanas[0]);
  }
  else if (TempValue >= suhuPanas[1])
  {
    MUsuhuPan = 1;
  }
  // Mencari Fungsi Keanggotaan pH Asam
  if (pHValue >= pHAsam[1])
  {
    MUpHAs = 0;
  }
  else if (pHValue > pHAsam[0] && pHValue < pHAsam[1])
  {
    MUpHAs = (pHAsam[1] - pHValue) / (pHAsam[1] - pHAsam[0]);
  }
  else if (pHValue <= pHAsam[0])
  {
    MUpHAs = 1;
  }
  // Mencari Fungsi Keanggotaan pH Netral
  if (pHValue <= pHNetral[0] || pHValue >= pHNetral[2])
  {
    MUpHNet = 0;
  }
  else if (pHValue > pHNetral[0] && pHValue < pHNetral[1])
  {
    MUpHNet = (pHValue - pHNetral[0]) / (pHNetral[1] - pHNetral[0]);
  }
  else if (pHValue >= pHNetral[1] && pHValue < pHNetral[2])
  {
    MUpHNet = (pHNetral[2] - pHValue) / (pHNetral[2] - pHNetral[1]);
  }
  // // Mencari Fungsi Keanggotaan pH Basa
  if (pHValue <= pHBasa[0])
  {
    MUpHBas = 0;
  }
  else if (pHValue > pHBasa[0] && pHValue < pHBasa[1])
  {
    MUpHBas = (pHValue - pHBasa[0]) / (pHBasa[1] - pHBasa[0]);
  }
  else if (pHValue >= pHBasa[1])
  {
    MUpHBas = 1;
  }

  // Proses Inferensi Kualitas Air
  // [R1] Jika suhu dingin & pH asam, maka Kualitas Air buruk
  if (MUsuhuDin < MUpHAs)
  {
    R1kuair = MUsuhuDin;
  }
  else if (MUpHAs < MUsuhuDin)
  {
    R1kuair = MUpHAs;
  }
  Z1kuair = kuairBuruk[1] - ((kuairBuruk[1] - kuairBuruk[0]) * R1kuair); // Mencari Z1 dari Rule 1

  // [R2] Jika suhu dingin & pH netral, maka kuair Baik
  if (MUsuhuDin < MUpHNet)
  {
    R2kuair = MUsuhuDin;
  }
  else if (MUpHNet < MUsuhuDin)
  {
    R2kuair = MUpHNet;
  }
  Z2kuair = kuairBaik[0] + ((kuairBaik[1] - kuairBaik[0]) * R2kuair); // Mencari Z2 dari Rule 2

  // [R3] Jika suhu dingin & pH basa, maka kuair Buruk
  if (MUsuhuDin < MUpHBas)
  {
    R3kuair = MUsuhuDin;
  }
  else if (MUpHBas < MUsuhuDin)
  {
    R3kuair = MUpHBas;
  }
  Z3kuair = kuairBuruk[1] - ((kuairBuruk[1] - kuairBuruk[0]) * R3kuair); // Mencari Z3 dari Rule 3

  // [R4] Jika suhu Netral & pH Asam, maka kuair Baik
  if (MUsuhuNet < MUpHAs)
  {
    R4kuair = MUsuhuNet;
  }
  else if (MUpHAs < MUsuhuNet)
  {
    R4kuair = MUpHAs;
  }
  Z4kuair = kuairBaik[0] + ((kuairBaik[1] - kuairBaik[0]) * R4kuair); // Mencari Z4 dari Rule 4

  // [R5] Jika suhu netral & pH netral, maka kuair Baik
  if (MUsuhuNet < MUpHNet)
  {
    R5kuair = MUsuhuNet;
  }
  else if (MUpHNet < MUsuhuNet)
  {
    R5kuair = MUpHNet;
  }
  Z5kuair = kuairBaik[0] + ((kuairBaik[1] - kuairBaik[0]) * R5kuair); // Mencari Z5 dari Rule 5

  // [R6] Jika suhu netral & pH basa, maka kuair Baik
  if (MUsuhuNet < MUpHBas)
  {
    R6kuair = MUsuhuNet;
  }
  else if (MUpHBas < MUsuhuNet)
  {
    R6kuair = MUpHBas;
  }
  Z6kuair = kuairBaik[0] + ((kuairBaik[1] - kuairBaik[0]) * R6kuair); // Mencari Z6 dari Rule 6

  // [R7] Jika suhu Panas & pH asam, maka kuair buruk
  if (MUsuhuPan < MUpHAs)
  {
    R7kuair = MUsuhuPan;
  }
  else if (MUpHAs < MUsuhuPan)
  {
    R7kuair = MUpHAs;
  }
  Z7kuair = kuairBuruk[1] - ((kuairBuruk[1] - kuairBuruk[0]) * R7kuair); // Mencari Z6 dari Rule 6

  // [R8] Jika suhu panas & pH netral, maka kuair Baik
  if (MUsuhuPan < MUpHNet)
  {
    R8kuair = MUsuhuPan;
  }
  else if (MUpHNet < MUsuhuPan)
  {
    R8kuair = MUpHNet;
  }
  Z8kuair = kuairBaik[0] + ((kuairBaik[1] - kuairBaik[0]) * R8kuair); // Mencari Z6 dari Rule 6

  // [R9] Jika suhu Panas & pH basa, maka kuair buruk
  if (MUsuhuPan < MUpHBas)
  {
    R9kuair = MUsuhuPan;
  }
  else if (MUpHBas < MUsuhuNet)
  {
    R9kuair = MUpHBas;
  }
  Z9kuair = kuairBuruk[1] - ((kuairBuruk[1] - kuairBuruk[0]) * R9kuair); // Mencari Z6 dari Rule 6

  // Defuzzyfikasi Kendali kuair
  OutputKuair = ((Z1kuair * R1kuair) + (Z2kuair * R2kuair) + (Z3kuair * R3kuair) + (Z4kuair * R4kuair) + (Z5kuair * R5kuair) + (Z6kuair * R6kuair) + (Z7kuair * R7kuair) + (Z8kuair * R8kuair) + (Z9kuair * R9kuair)) / ((R1kuair + R2kuair + R3kuair + R4kuair + R5kuair + R6kuair + R7kuair + R8kuair + R9kuair));
  return OutputKuair;
}