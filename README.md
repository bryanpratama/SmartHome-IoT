# SmartHome-IoT

**SmartHome-IoT** adalah proyek sistem otomasi rumah berbasis ESP32 yang dirancang untuk memberikan solusi sederhana dan efisien dalam mengelola perangkat rumah tangga secara otomatis. Sistem ini menggunakan berbagai sensor untuk pemantauan suhu, kelembapan, arus listrik, dan deteksi gerakan, serta memungkinkan pengendalian perangkat seperti lampu dan kipas secara otomatis. Dengan memanfaatkan teknologi Internet of Things (IoT), proyek ini bertujuan untuk meningkatkan kenyamanan dan efisiensi energi di rumah dengan kontrol yang lebih mudah dan lebih cerdas.


## **Fitur**
- **Pemantauan Suhu dan Kelembapan** menggunakan **DHT22** untuk mengetahui kondisi lingkungan.  
- **Pemantauan Arus Listrik** dengan **ACS712-20**, membantu memantau penggunaan listrik.  
- **Sensor Keamanan Pintu** menggunakan **MC38 Magnetic Switch**, untuk mendeteksi status pintu (terbuka/tertutup).  
- **Deteksi Gerakan** dengan **PIR Motion Sensor**, memungkinkan sistem mengetahui adanya aktivitas di ruangan.  
- **Pengendalian Perangkat Elektronik** melalui **Relay 8 Channel**, untuk mengontrol lampu, kipas, dan perangkat lainnya.  
- **Pengukuran Tegangan** menggunakan **ZMPT101B** untuk memantau tegangan listrik rumah.  
- **Transmisi IR** melalui **KY005 IR Transmitter**, memberikan kemampuan untuk mengendalikan perangkat berbasis IR seperti TV atau AC.

## **Persyaratan Sistem**
Sebelum memulai, pastikan Anda memiliki perangkat dan perangkat lunak berikut:

- Perangkat Keras:
  - ESP32 (dapat menggunakan model apa saja, pastikan memiliki cukup pin GPIO)
  - DHT22 (untuk pengukuran suhu dan kelembapan)
  - ACS712-20 (untuk pemantauan arus listrik)
  - MC38 Magnetic Switch (untuk sensor pintu)
  - PIR Motion Sensor (untuk deteksi gerakan)
  - ZMPT101B (untuk pengukuran tegangan listrik)
  - KY005 IR Transmitter (untuk transmisi IR)
  - Relay 8 Channel (untuk kontrol perangkat rumah tangga)
- Perangkat Lunak:
  - Arduino IDE atau PlatformIO untuk pemrograman ESP32
  - Library yang dibutuhkan:
  - DHT untuk sensor suhu dan kelembapan
  - Adafruit_Sensor untuk sensor DHT22
  - IRremote untuk kontrol perangkat berbasis IR

## **Instalasi**
Berikut adalah langkah-langkah untuk menginstal dan menyiapkan proyek ini:

1. **Persiapkan Arduino IDE atau PlatformIO:**

   - Unduh dan instal Arduino IDE dari situs resmi Arduino atau pasang PlatformIO melalui Visual Studio Code.

2. **Instal Board ESP32 di Arduino IDE:**


   - Buka Arduino IDE, pergi ke File > Preferences, dan tambahkan URL board ESP32 pada Additional Boards Manager URLs:
   https://dl.espressif.com/dl/package_esp32_index.json
   - Pergi ke Tools > Board > Board Manager, cari "ESP32" dan instal.


3. **Instal Library yang Diperlukan:**

   - Instal library yang dibutuhkan melalui Sketch > Include Library > Manage Libraries, lalu cari dan instal:
     - DHT sensor library
     - ACS712 untuk sensor arus
     - IRremote untuk modul IR

4. **Koneksi Hardware:**


   - Hubungkan komponen seperti ESP32, sensor, dan relay ke papan sesuai dengan diagram pin yang telah disediakan.

5. **Upload Kode:**

      - Pilih board ESP32 yang sesuai di Tools > Board, pilih port yang digunakan, dan upload kode program.


## **Penggunaan**
Setelah proyek berhasil diinstal dan dikonfigurasi, Anda dapat menggunakan sistem otomasi rumah ini untuk mengendalikan perangkat rumah tangga dan memantau kondisi lingkungan secara otomatis. Berikut adalah contoh penggunaan:

1. **Pemantauan Suhu dan Kelembapan:** Sistem secara terus-menerus memonitor suhu dan kelembapan lingkungan rumah menggunakan sensor DHT22 dan menampilkan hasilnya di konsol atau antarmuka pengguna.

2. **Pengendalian Perangkat Listrik:** Anda dapat mengontrol perangkat seperti lampu dan kipas menggunakan Relay 8 Channel. Sistem dapat diprogram untuk menyalakan atau mematikan perangkat berdasarkan kondisi suhu atau deteksi gerakan.

3. **Peringatan Keamanan:** Jika sensor MC38 Magnetic Switch mendeteksi pintu terbuka atau sensor PIR Motion mendeteksi gerakan, sistem dapat memberikan peringatan atau melakukan tindakan tertentu (misalnya, menyalakan lampu atau mengaktifkan alarm).

4. **Kontrol Jarak Jauh:** Dengan modul KY005 IR Transmitter, sistem dapat mengendalikan perangkat berbasis IR seperti TV atau AC melalui transmisi sinyal IR yang dikirim oleh ESP32.

## **Diagram Sistem**
Berikut adalah gambaran umum arsitektur sistem:  
*(soon)*  

## **Tabel GPIO**
Berikut adalah daftar pin GPIO yang digunakan dalam proyek ini:

| **Pin GPIO** | **Fungsi**                     |
|--------------|--------------------------------|
| GPIO23       | Saklar 4                      |
| GPIO22       | Saklar 3                      |
| GPIO1        | Saklar 1                      |
| GPIO3        | Tidak digunakan               |
| GPIO21       | Saklar 2                      |
| GPIO19       | Relay 7                       |
| GPIO18       | Relay 6                       |
| GPIO5        | Relay 5                       |
| GPIO17       | Relay 4                       |
| GPIO16       | Relay 3                       |
| GPIO4        | Boot pin                      |
| GPIO2        | Boot pin                      |
| GPIO15       | DHT22                         |
| GPIO36       | zmpt101b sensor tegangan      |
| GPIO39       | Tidak digunakan               |
| GPIO34       | acs712-20 ac dc sensor arus   |
| GPIO35       | Tidak digunakan               |
| GPIO32       | ky005 infrared transmitter    |
| GPIO33       | MC38 Magnetic Switch          |
| GPIO25       | Tidak digunakan               |
| GPIO26       | Relay 1                       |
| GPIO27       | Relay 2                       |
| GPIO14       | Relay 8                       |
| GPIO12       | Boot pin                      |
| GPIO13       | ky022 infrared receiver       |
