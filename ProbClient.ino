#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define bleServerName "ESP32 Charles et Vincent"

// The UUID of the service and characteristic
static BLEUUID ServiceUUID("912071e2-fdb5-4a2e-b420-0ffb542b0e05");

static BLEUUID baro_temp_UUID("41bcf98a-fa02-49bd-9365-9f8c3c311b1b");
static BLEUUID baro_pressure_UUID("99570e93-fa45-4bb9-a30a-d0408cf7fa37");
static BLEUUID light_UUID("b2640133-2d8d-4c19-bc4b-c493fddeed76");
static BLEUUID temp_UUID("f1949579-c2b8-4f9b-91ba-7f7257f8d84d");
static BLEUUID humi_UUID("03823d56-a16a-4551-ad7c-d96dc4d937bf");
static BLEUUID wind_speed_UUID("1c8e4ae2-87dc-4010-bcbc-291ccf23144c");
static BLEUUID wind_dir_UUID("7e54e529-dc81-4b97-8952-d50ed0cdaf50");
static BLEUUID rain_mm_UUID("e278d5b5-f233-4cb9-935c-d5d43d1f9371");

static boolean doConnect = false;
static boolean connected = false;

static BLEAddress *pServerAddress;

static BLEAdvertisedDevice* myDevice;

BLERemoteCharacteristic* baro_temp_characteristic;
BLERemoteCharacteristic* baro_pressure_characteristic;
BLERemoteCharacteristic* light_characteristic;
BLERemoteCharacteristic* temp_characteristic;
BLERemoteCharacteristic* humi_characteristic;
BLERemoteCharacteristic* wind_speed_characteristic;
BLERemoteCharacteristic* wind_dir_characteristic;
BLERemoteCharacteristic* rain_mm_characteristic;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == bleServerName) { //Check if the name of the advertiser matches
      advertisedDevice.getScan()->stop(); //Scan can be stopped, we found what we are looking for
      pServerAddress = new BLEAddress(advertisedDevice.getAddress()); //Address of advertiser is the one we need
      doConnect = true; //Set indicator, stating that we are ready to connect
      Serial.println("Device found. Connecting!");
    }
  }
};

// fonction pour rechercher le Serveur "ESP32 Charles et Vincent"

bool connectToServer(BLEAddress pAddress) {
   BLEClient* Client = BLEDevice::createClient();
 
  // Connect to the remove BLE Server.
  Client->connect(pAddress);
  Serial.println("Connecter au server");
  Client->setMTU(517);
 
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* RemoteService = Client->getService(ServiceUUID);
  if (RemoteService == nullptr) {
    Serial.print("Failed pour trouver le service UUID: ");
    Serial.println(ServiceUUID.toString().c_str());
    return (false);
  }
 
  // Obtiente la characteristics du service regard s'il peut notify
  light_characteristic = RemoteService->getCharacteristic(light_UUID);
  if(light_characteristic->canNotify()){
    light_characteristic->registerForNotify(light_characteristicCallback);
  }

  temp_characteristic = RemoteService->getCharacteristic(temp_UUID);
  if(temp_characteristic->canNotify()){
    temp_characteristic->registerForNotify(temp_characteristicCallback);
  }

  baro_temp_characteristic = RemoteService->getCharacteristic(baro_temp_UUID);
  if(baro_temp_characteristic->canNotify()){
    baro_temp_characteristic->registerForNotify(baro_temp_characteristicCallback);
  }

  // comme le notify ne prend que 1 seul characteristique ceux-ci sont mis en commentaire
  // humi_characteristic =  RemoteService->getCharacteristic(humi_UUID);
  // baro_temp_characteristic = RemoteService->getCharacteristic(baro_temp_UUID);
  // baro_pressure_characteristic = RemoteService->getCharacteristic(baro_pressure_UUID);
  // wind_speed_characteristic = RemoteService->getCharacteristic(wind_speed_UUID);
  // rain_mm_characteristic = RemoteService->getCharacteristic(rain_mm_UUID);

  // comme le notify ne prend que 1 seul characteristique ceux-ci sont mis en commentaire
  // humi_characteristic->registerForNotify(humi_characteristicCallback);
  // baro_temp_characteristic->registerForNotify(baro_temp_characteristicCallback);
  // baro_pressure_characteristic->registerForNotify(baro_pressure_characteristicCallback);
  // wind_speed_characteristic->registerForNotify(wind_speed_characteristicCallback);
  //rain_mm_characteristic->registerForNotify(rain_mm_characteristicCallback);

  return true;
}


// Call back pour voir si la fonction peut être lu.
void light_characteristicCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
{
  if(light_characteristic->canRead()){
    Serial.print("Lumière de la journée: ");
    Serial.write(pData, length);
    Serial.println();
  }
}

void temp_characteristicCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
{
  if(temp_characteristic->canRead()){
    Serial.print("Température venant du capteur d'humidité: ");
    Serial.write(pData, length);
    Serial.println();
  }
}

void baro_temp_characteristicCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
{
  if(baro_temp_characteristic->canRead()){
    Serial.print("Température venant du baromètre: ");
    Serial.write(pData, length);
     Serial.println();
  }
}

// Comme peux pas lire plus que 4
// void humi_characteristicCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
// {
//   if(humi_characteristic->canRead()){
//       Serial.write(pData, length);
//   }
// }

// void wind_speed_characteristicCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
// {
//   if(wind_speed_characteristic->canRead()){
//       Serial.write(pData, length);
//   }
// }

// void baro_pressure_characteristicCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
// { 
//   if(baro_pressure_characteristic->canRead()){
//       Serial.write(pData, length);
//   }
// }

// void wind_dir_characteristicCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
// {
//   if(wind_dir_characteristic->canRead()){
//       Serial.write(pData, length);
//   }
// }

// void rain_mm_characteristicCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
// {
//   if(rain_mm_characteristic->canRead()){
//     Serial.write(pData, length);
//   }
// }

void setup() {
  Serial.begin(115200);
  Serial.println("Start Client");

  BLEDevice::init("");


  // Scanning pour trouver le serveur
  Serial.println("Scanning pour un BLE Server...");
  BLEScan* BLEScan = BLEDevice::getScan();
  BLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  BLEScan->setInterval(1349);
  BLEScan->setWindow(449);
  BLEScan->setActiveScan(true);
  BLEScan->start(2, false);
}

void loop() {
  if (doConnect == true) {
    if (connectToServer(*pServerAddress)) {
      Serial.println("Échange établie");
    } else {
      Serial.println("Connection Perdu");
    }
    doConnect = false;
  }

  delay(2000);
}