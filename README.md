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
