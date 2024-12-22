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

## **Tabel GPIO**
Berikut adalah daftar pin GPIO yang digunakan dalam proyek ini:

| **Pin GPIO** | **Fungsi**                         |
|--------------|------------------------------------|
| GPIO23       | Saklar 4                          |
| GPIO22       | Saklar 3                          |
| GPIO21       | Saklar 2                          |
| GPIO19       | Relay 7                           |
| GPIO18       | Relay 6                           |
| GPIO5        | Relay 5                           |
| GPIO17       | Relay 4                           |
| GPIO16       | Relay 3                           |
| GPIO15       | DHT sensor                        |
| GPIO36       | zmpt101b sensor tegangan          |
| GPIO34       | acs712-20 ac dc sensor arus       |
| GPIO35       | Tidak digunakan                   |
| GPIO32       | ky005 infrared transmitter        |
| GPIO33       | Door sensor                       |
| GPIO13       | ky022 infrared receiver           |
| GPIO14       | Relay 8                           |
| GPIO12       | Tidak digunakan                   |
| GPIO25       | Tidak digunakan                   |
| GPIO26       | Tidak digunakan                   |
| GPIO27       | Tidak digunakan                   |

### **GPIO Tidak Digunakan**
| **Pin GPIO**          |
|-----------------------|
| GPIO0 (Boot pin)      |
| GPIO2 (Boot pin)      |
| GPIO4 (Boot pin)      |
| GPIO6–11 (Flash pins) |
| GPIO39                |
| GPIO35                |
| GPIO12                |
| GPIO25                |
| GPIO26                |
| GPIO27                |


