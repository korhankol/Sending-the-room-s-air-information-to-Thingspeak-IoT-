#include <SoftwareSerial.h>                                   //SoftwareSerial kütüphanimizi ekliyoruz.ESP ile yazılımsal seri haberleşme için eklenir.line 22
#include <dht11.h>                                            //DHT11 sensör kütüphanemizi ekliyoruz.

String agAdi = "Mi_hobi_odasi";              //Ağımızın adını buraya yazıyoruz.    
String agSifresi = "x9cfyCYuXKD4";           //Ağımızın şifresini buraya yazıyoruz.

int rxPin = 10;                                               //ESP8266 RX pini
int txPin = 11;                                               //ESP8266 TX pini
int dht11Pin = 2;

String ip = "184.106.153.149";                                //Thingspeak ip adresi
float sicaklik, nem;

int gazsensorpin=0;                         // analog hava kalite sensorü girişi analog 


dht11 DHT11;

SoftwareSerial esp(rxPin, txPin);                             //Seri haberleşme pin ayarlarını yapıyoruz.esp ismini istediğin gibi değiştir. Nesne tanımladı.

void setup() {  
  
  Serial.begin(9600);  //Seri port ile haberleşmemizi başlatıyoruz.
  Serial.println("Started");
  esp.begin(115200);                                          //ESP8266 ile seri haberleşmeyi başlatıyoruz.Yukarıdaki yazılımsal seri haberleşme başlatılır, hızı belirlenir aynı zamanda.
  esp.println("AT");                                          //AT komutu ile modül kontrolünü yapıyoruz.esp ye gönderdiğimizde veri alıyorsa ok döner. bunu test ediyoruz aslında.
  Serial.println("AT Yollandı");
  while(!esp.find("OK")){                                     //AT den sonra ok gelmediyse while içine gir. tekrar gönder at . Modül hazır olana kadar bekliyoruz.
    Serial.println("ESP8266 Bulunamadı.");
    esp.println("AT");
    
  }
  Serial.println("OK Komutu Alındı");
  esp.println("AT+CWMODE=1");                                 //ESP8266 modülünü client olarak ayarlıyoruz. AT ile başlat CWMODE=1 ile işlemi yaptır.
  while(!esp.find("OK")){                                     //yine esp den ok dönene kadar sürekli client yapmnaya çalışır while i,çinde döner durur.Ayar yapılana kadar bekliyoruz.
    esp.println("AT+CWMODE=1");
    Serial.println("Ayar Yapılıyor....");
  }
  Serial.println("Aga Baglaniliyor...");
  Serial.println("Client olarak ayarlandı");
  
  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");    //Ağımıza bağlanıyoruz.
  while(!esp.find("OK"));                                     //Ağa bağlanana kadar bekliyoruz.Bu tek satır parantezsiz esp den ok gelene kadar burada takılı kalır döner durur.
  Serial.println("Aga Baglandi.");
  delay(1000);
}
void loop() {
  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           //Artık esp yi Thingspeak'e bağlanıyoruz. Genelde 80 kullanılır
  if(esp.find("Error")){                                      //Bağlantı hatası kontrolü yapıyoruz.
    Serial.println("AT+CIPSTART Error");
  }
  DHT11.read(dht11Pin);                                       // Bağlantı hatası yoksa artık sensörden değerleri oku
  sicaklik = (float)DHT11.temperature;
  nem = (float)DHT11.humidity;                                // okunan veriler başta tanımladığımız değişkenlere atandı
                                                               // siteye yazma işlemi yapılacağından Write API KEY buraya yapıştırılır

  int airquality=analogRead(0);                  // hava kalite sensor değeri alındı
  Serial.print("Okunan sensör değeri=");
  Serial.println(airquality);
                                                               
  String veri = "GET https://api.thingspeak.com/update?api_key=HCOCXLVY4X007R4D";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıyoruz.//Göndereceğimiz sıcaklık değişkeni
 
  veri += "&field1=";              //üstteki veriye ek bunu yaz . yani & işareti ile ek yapılır. eklenen field1 dir.
  veri += String(sicaklik);
  veri += "&field2=";
  veri += String(nem);              //Göndereceğimiz nem değişkeni
  veri += "&field3=";
  veri += int(airquality);         // hava kalitesi veri paketine eklendi.
  veri += "\r\n\r\n\r\n";          // veriyi göndermek için enter işlemi gibi bir şey gönderilir
 
  // yukarıdaki aslıda bir veri paketidir. get komutu,sensör değerleri hepsi paket olarak gider.
   
  esp.print("AT+CIPSEND=");                                   //ESP'ye göndereceğimiz veri uzunluğunu veriyoruz. !!!! esp de serial print gibi çalışıyor mantık aynı
  esp.println(veri.length()+2);
  delay(2000);
  if(esp.find(">")){                                          //ESP8266 hazır olduğunda içindeki komutlar çalışıyor.
    esp.print(veri);                                          //Veriyi gönderiyoruz.
    Serial.println(veri);
    Serial.println("Veri gonderildi.");
    delay(1000);
  }
  Serial.println("Baglantı Kapatildi.");
  esp.println("AT+CIPCLOSE");                                //Bağlantıyı kapatıyoruz
  delay(1000);                                               //Yeni veri gönderimi için 1 dakika bekliyoruz.
}
