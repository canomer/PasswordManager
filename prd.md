# Password Manager - Ürün Gereksinim Dokümanı (PRD)

## 1. Amaç
Kullanıcıların cihazlarındaki tarayıcılardan (Chrome, Firefox, Edge, vs.) kayıtlı site, kullanıcı adı (id) ve şifreleri otomatik olarak aktarabilen, güvenli bir şekilde saklayabilen ve yönetebilen, kullanıcı girişi olan, Qt arayüzlü, C++ ile yazılmış, CMake ile derlenen ve hem Windows hem de Linux'ta çalışabilen bir şifre yöneticisi yazılımı geliştirmek.

---

## 2. Hedef Kitle
- Bireysel kullanıcılar
- Farklı platformlarda (Windows, Linux) çalışan açık kaynak şifre yöneticisi arayanlar
- Tarayıcı şifrelerini merkezi olarak yönetmek isteyenler
- Güvenli şifre yönetimi ihtiyacı olan kullanıcılar

---

## 3. Temel Özellikler

### 3.1. Kullanıcı Girişi
- Ana parola ile uygulamaya giriş
- Yeni kullanıcı kaydı ve parola sıfırlama
- Güvenli parola hash'leme (SHA-256)
- Oturum yönetimi

### 3.2. Şifre Aktarımı
- Chrome, Firefox ve Edge tarayıcılarından şifre aktarımı
- Platform bağımsız tarayıcı profil tespiti
- Toplu içe aktarma desteği
- Aktarım durumu bildirimleri

### 3.3. Şifre Yönetimi
- Site, kullanıcı adı ve şifre ekleme, silme, düzenleme
- AES-256-GCM ile şifreleme
- PBKDF2 ile güvenli anahtar türetme
- Rastgele şifre üretme
- Şifre arama ve filtreleme
- Şifre görünürlük kontrolü

### 3.4. Arayüz
- Modern Qt arayüzü
- Sezgisel kullanıcı deneyimi
- Durum bildirimleri
- İşlem onayları
- Sistem teması desteği

### 3.5. Platform Desteği
- Windows ve Linux üzerinde sorunsuz çalışabilme
- CMake ile kolay derlenebilirlik
- SQLite veritabanı entegrasyonu
- OpenSSL şifreleme desteği

### 3.6. Güvenlik
- AES-256-GCM şifreleme
- Güvenli anahtar türetme (PBKDF2)
- Rastgele IV ve tuz üretimi
- Yerel veritabanı şifreleme
- Ana parola hash'leme

---

## 4. Teknik Gereksinimler

- **Programlama Dili:** C++17
- **Arayüz:** Qt6
- **Derleme Sistemi:** CMake (>= 3.16)
- **Veritabanı:** SQLite3
- **Şifreleme:** OpenSSL (AES-256-GCM, PBKDF2)
- **Çapraz Platform:** Windows 10+, Linux (Ubuntu, Fedora, vs.)

---

## 5. Yapıldı

- [x] Proje yapısı oluşturuldu (CMake, temel klasörler)
- [x] Qt ile temel arayüz tasarlandı
- [x] Kullanıcı girişi ve kayıt sistemi tamamlandı
- [x] SQLite veritabanı entegrasyonu yapıldı
- [x] AES-256-GCM şifreleme implementasyonu tamamlandı
- [x] PBKDF2 ile anahtar türetme eklendi
- [x] Şifre ekleme/düzenleme/silme diyalogları oluşturuldu
- [x] Rastgele şifre üretme özelliği eklendi
- [x] Şifre arama ve filtreleme implementasyonu tamamlandı
- [x] Tarayıcılardan şifre aktarımı için temel altyapı hazırlandı
- [x] Sistem teması desteği eklendi
- [x] Windows ve Linux için kurulum talimatları hazırlandı

---

## 6. Yapılacak

- [ ] Tarayıcı şifre aktarımı için platform-spesifik implementasyonlar
  - [ ] Chrome için Windows DPAPI entegrasyonu
  - [ ] Firefox için NSS kütüphane entegrasyonu
  - [ ] Edge için Windows DPAPI entegrasyonu
- [ ] Otomatik güncelleme sistemi
- [ ] Yedekleme ve geri yükleme özellikleri
- [ ] İki faktörlü kimlik doğrulama (2FA) desteği
- [ ] Parola sağlamlık analizi
- [ ] Parola sıfırlama mekanizması
- [ ] Oturum zaman aşımı
- [ ] Çevrimdışı mod
- [ ] Dışa aktarma özelliği
- [ ] Çoklu dil desteği
- [ ] Kapsamlı kullanıcı dokümantasyonu
- [ ] Birim testleri
- [ ] Entegrasyon testleri
- [ ] Güvenlik denetimi
- [ ] Performans optimizasyonları

---

## 7. Güvenlik Özellikleri

### 7.1. Şifreleme
- AES-256-GCM ile şifreleme
- PBKDF2 ile güvenli anahtar türetme
- Rastgele IV ve tuz üretimi
- OpenSSL kütüphanesi kullanımı

### 7.2. Veri Güvenliği
- Tüm şifreler şifreli olarak saklanır
- Ana parola hash'lenerek saklanır
- Veritabanı şifreleme
- Bellek güvenliği

### 7.3. Kullanıcı Güvenliği
- Güçlü parola zorunluluğu
- Başarısız giriş denemesi sınırlaması
- Otomatik oturum kapatma
- Güvenli parola üreteci

---

## 8. Performans Hedefleri

- Uygulama başlatma süresi < 3 saniye
- Şifre arama yanıt süresi < 500ms
- Şifre ekleme/düzenleme yanıt süresi < 1 saniye
- Tarayıcıdan içe aktarma süresi < 5 saniye (100 şifre için)
- RAM kullanımı < 200MB
- Disk kullanımı < 100MB

---

## 9. Gelecek Özellikler (v2.0)

- Mobil uygulama desteği
- Bulut senkronizasyon
- Parola paylaşımı
- Güvenli not saklama
- Kredi kartı bilgisi saklama
- Kimlik bilgisi saklama
- Browser eklentisi
- Otomatik form doldurma
- Parola ihlal kontrolü
- Gelişmiş raporlama

---

## 10. Katkıda Bulunma

Projeye katkıda bulunmak için:
1. Fork edin
2. Feature branch oluşturun
3. Değişikliklerinizi commit edin
4. Branch'inizi push edin
5. Pull request açın

---

## 11. Lisans

Bu proje MIT lisansı altında lisanslanmıştır. Detaylar için [LICENSE](LICENSE) dosyasına bakın.