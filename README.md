# SmartHome-IoT

**SmartHome-IoT** adalah proyek sistem otomasi rumah berbasis ESP32 yang menggunakan berbagai sensor untuk pemantauan dan pengendalian. Proyek ini dirancang untuk memberikan solusi sederhana dan efisien bagi pengelolaan perangkat rumah tangga secara otomatis melalui integrasi sensor dan modul.

## **Fitur**
- **Pemantauan Suhu dan Kelembapan** menggunakan **DHT22** untuk mengetahui kondisi lingkungan.  
- **Pemantauan Arus Listrik** dengan **ACS712-20**, membantu memantau penggunaan listrik.  
- **Sensor Keamanan Pintu** menggunakan **MC38 Magnetic Switch**, untuk mendeteksi status pintu (terbuka/tertutup).  
- **Deteksi Gerakan** dengan **PIR Motion Sensor**, memungkinkan sistem mengetahui adanya aktivitas di ruangan.  
- **Pengendalian Perangkat Elektronik** melalui **Relay 8 Channel**, untuk mengontrol lampu, kipas, dan perangkat lainnya.  
- **Pengukuran Tegangan** menggunakan **ZMPT101B** untuk memantau tegangan listrik rumah.  
- **Transmisi IR** melalui **KY005 IR Transmitter**, memberikan kemampuan untuk mengendalikan perangkat berbasis IR seperti TV atau AC.

## **Komponen Utama**
1. **ESP32**: Mikrokontroler yang menjadi inti pengolahan dan komunikasi.  
2. **Sensor:**
   - **DHT22**: Mengukur suhu dan kelembapan.  
   - **ACS712-20**: Mengukur arus listrik.  
   - **MC38 Magnetic Switch**: Sensor pintu.  
   - **PIR Motion Sensor**: Deteksi gerakan.  
   - **ZMPT101B**: Mengukur tegangan listrik.  
   - **KY005 IR Transmitter**: Modul pengirim sinyal IR.  
3. **Relay 8 Channel**: Mengontrol berbagai perangkat elektronik.  

## **Diagram Sistem**
Berikut adalah gambaran umum arsitektur sistem:  
*(soon)*  

## **Tabel GPIO Lengkap**
Berikut adalah daftar pin GPIO yang digunakan dan tidak digunakan dalam proyek ini:

| **Pin GPIO** | **Fungsi**                      | **Deskripsi**                     |
|--------------|---------------------------------|------------------------------------|
| GPIO0        | Tidak Digunakan                | Pin boot strapping                |
| GPIO1        | Tidak Digunakan                | TX0 (UART TX default)             |
| GPIO2        | Relay 1                        | Output untuk relay                |
| GPIO3        | Tidak Digunakan                | RX0 (UART RX default)             |
| GPIO4        | Relay 2                        | Output untuk relay                |
| GPIO5        | Relay 5                        | Output untuk relay                |
| GPIO6        | Tidak Digunakan                | Terhubung ke flash internal       |
| GPIO7        | Tidak Digunakan                | Terhubung ke flash internal       |
| GPIO8        | Tidak Digunakan                | Terhubung ke flash internal       |
| GPIO9        | Tidak Digunakan                | Terhubung ke flash internal       |
| GPIO10       | Tidak Digunakan                | Terhubung ke flash internal       |
| GPIO11       | Tidak Digunakan                | Terhubung ke flash internal       |
| GPIO12       | Saklar 2                       | Input untuk kontrol saklar        |
| GPIO13       | KY-022 Infrared Receiver       | Menerima sinyal infrared          |
| GPIO14       | Relay 8                        | Output untuk relay                |
| GPIO15       | Tidak Digunakan                | Pin boot strapping                |
| GPIO16       | Relay 3                        | Output untuk relay                |
| GPIO17       | Relay 4                        | Output untuk relay                |
| GPIO18       | Relay 6                        | Output untuk relay                |
| GPIO19       | Relay 7                        | Output untuk relay                |
| GPIO20       | Tidak Digunakan                | Tidak tersedia pada header ESP32  |
| GPIO21       | Saklar 5                       | Input untuk kontrol saklar        |
| GPIO22       | Saklar 3                       | Input untuk kontrol saklar        |
| GPIO23       | Saklar 4                       | Input untuk kontrol saklar        |
| GPIO24       | Tidak Digunakan                | Tidak tersedia pada header ESP32  |
| GPIO25       | DHT Sensor                     | Mengukur suhu dan kelembapan      |
| GPIO26       | Saklar 7                       | Input untuk kontrol saklar        |
| GPIO27       | Saklar 6                       | Input untuk kontrol saklar        |
| GPIO28       | Tidak Digunakan                | Tidak tersedia pada header ESP32  |
| GPIO29       | Tidak Digunakan                | Tidak tersedia pada header ESP32  |
| GPIO30       | Tidak Digunakan                | Tidak tersedia pada header ESP32  |
| GPIO31       | Tidak Digunakan                | Tidak tersedia pada header ESP32  |
| GPIO32       | KY-005 Infrared Transmitter    | Mengirim sinyal infrared          |
| GPIO33       | Door Sensor                    | Mendeteksi status pintu           |
| GPIO34       | ACS712-20 Sensor Arus          | Mengukur arus AC/DC               |
| GPIO35       | Saklar 1                       | Input untuk kontrol saklar        |
| GPIO36       | ZMPT101B Sensor Tegangan       | Mengukur tegangan AC              |
| GPIO37       | Tidak Digunakan                | Tidak tersedia pada header ESP32  |
| GPIO38       | Tidak Digunakan                | Tidak tersedia pada header ESP32  |
| GPIO39       | Tidak Digunakan                | Input-only, tidak digunakan       |


