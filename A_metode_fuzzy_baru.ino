
// deklarasi Keanggotaan himpunan suhu
float suhuDingin[] = {26, 29};     // <24, 24, 28
float suhuNetral[] = {26, 29, 32}; // 24, 28, 32
float suhuPanas[] = {29, 32};      // 28, 28, 32>
// deklarasi Keanggotaan himpunan pH
float pHAsam[] = {6, 7};      // <6, 6, 7
float pHNetral[] = {6, 7, 8}; // 6, 7, 8
float pHBasa[] = {7, 8};      // 7, 8, 8>
// deklarasi Keanggotaan himpunan output Kualitas Air
float kuairBuruk[] = {5, 10};  // <0, 1
float kuairBaik[] = {5, 10}; // 0. 1>

float MUsuhuDin, MUsuhuNet, MUsuhuPan; // deklarasi variabel derajat keanggotaan suhu
float MUpHAs, MUpHNet, MUpHBas;        // deklarasi variabel derajat keanggotaan pH

float R1kuair, R2kuair, R3kuair, R4kuair, R5kuair, R6kuair, R7kuair, R8kuair, R9kuair;  // deklarasi variabel Rules untuk kualitas Air
float Z1kuair, Z2kuair, Z3kuair, Z4kuair, Z5kuair, Z6kuair, Z7kuair, Z8kuair, Z9kuair;  // deklarasi variabel Rules untuk kualitas Air
float OutputKuair;  // deklarasi variabel Output Kualitas Air

void setup()
{
  // Inisialisasi Serial Monitor
  Serial.begin(115200);
}

void loop()
{
  // Input nilai Variabel
  float TempValue = 29.01;
  float pHValue = 7.1;
  
  // Hitung Fuzzy Output KualitasAir menggunakan fungsi
  OutputKuair = fuzzyTsukamotoKuair(TempValue, pHValue);

  if (OutputKuair >= 5) {
    // Jika kualitasAir lebih besar dari 5
    Serial.println("Kualitas air baik");
  } else {
    // Jika kualitasAir kurang dari atau sama dengan 5
    Serial.println("Kualitas air buruk");
  }
  delay(1000  );

  Serial.print("pH Air: ");
  Serial.println(pHValue);
  Serial.print("Suhu Air: ");
  Serial.println(TempValue);
  Serial.print("Fuzzy Output Kualitas Air: ");
  Serial.println(OutputKuair);

  Serial.println();
  delay(2000);
}

// Deklarasi fungsi untuk menghitung Fuzzy Tsukamoto Kuair
float fuzzyTsukamotoKuair(float TempValue, float pHValue)
{
  // Proses Fuzzyfikasi Kendali Kuair
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
    // MUpHNet = 100;
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
  Z1kuair = kuairBuruk[1] + ((kuairBuruk[1] - kuairBuruk[0]) * R1kuair); // Mencari Z1 dari Rule 1

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
  Z3kuair = kuairBuruk[1] + ((kuairBuruk[1] - kuairBuruk[0]) * R3kuair); // Mencari Z3 dari Rule 3

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
  Z5kuair = kuairBaik[0] - ((kuairBaik[1] - kuairBaik[0]) * R5kuair); // Mencari Z5 dari Rule 5

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
  Z7kuair = kuairBuruk[1] + ((kuairBuruk[1] - kuairBuruk[0]) * R7kuair); // Mencari Z6 dari Rule 6

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
  Z9kuair = kuairBuruk[1] + ((kuairBuruk[1] - kuairBuruk[0]) * R9kuair); // Mencari Z6 dari Rule 6

  // Defuzzyfikasi Kendali kuair
  OutputKuair = ((Z1kuair * R1kuair) + (Z2kuair * R2kuair) + (Z3kuair * R3kuair) + (Z4kuair * R4kuair) + (Z5kuair * R5kuair) + (Z6kuair * R6kuair) + (Z7kuair * R7kuair) + (Z8kuair * R8kuair) + (Z9kuair * R9kuair)) / ((R1kuair + R2kuair + R3kuair + R4kuair + R5kuair + R6kuair + R7kuair + R8kuair + R9kuair));
  return OutputKuair;
  // if (OutputKuair == 0.00)
  // {
  //   return 0.00;  
  // }
  // else
  // {
  //   return OutputKuair = ((Z1kuair * R1kuair) + (Z2kuair * R2kuair) + (Z3kuair * R3kuair) + (Z4kuair * R4kuair) + (Z5kuair * R5kuair) + (Z6kuair * R6kuair) + (Z7kuair * R7kuair) + (Z8kuair * R8kuair) + (Z9kuair * R9kuair)) / ((R1kuair + R2kuair + R3kuair + R4kuair + R5kuair + R6kuair + R7kuair + R8kuair + R9kuair));
  // }
}


