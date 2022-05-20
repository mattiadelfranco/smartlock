/* Librerie */
#include <LiquidCrystal_I2C.h> // PIN su cui è collegato il Display, SDA = 21, SCL = 22
#include <HTTPClient.h> // Client HTTP Library
#include <WiFi.h> //Wifi library
#include <MFRC522.h> //library responsible for communicating with the module RFID-RC522
#include <SPI.h> //library responsible for communicating of SPI bus

/* Costanti */
#define SS_PIN 26
#define RST_PIN 25

/* Oggetti */
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status; //authentication return status code
MFRC522 mfrc522(SS_PIN, RST_PIN);

WiFiClient client; // Oggetto client WiFi
LiquidCrystal_I2C lcd(0x27, 20, 4); //
HTTPClient http; // Client HTTP per effettuare le richieste POST

/* Variabili */
byte nuidPICC[4] = {0, 0, 0, 0}; // UID Tag NFC che viene scannerizzato
const char* ssid = "iPhone di Mattia"; // SSID Rete Wifi
const char* password = "davide500"; // Password rete Wifi
String backendUrl = "https://smartlock.exibe.it/api/nfc-access"; // Endpoint of backend
String backendSecretKey = "961bf611-a155-45bc-a768-01f47b6e2d40"; // Secret Key generated from Smartlock Backend, identifies the Doorlock
String passepartoutNfcUid = "150C93FC"; // UID del Tag NFC che può sempre aprire la serratura
String lcdCurrentText = ""; // Testo attualmente mostrato sul display
//int counter = 0; // Numero di tentativi falliti per la connessione al WiFi
int relayPin = 32; // Pin su cui è collegato il Relè che pilota la serratura

void setup() {
  Serial.begin(9600);

  // Relay init
  digitalWrite(relayPin, HIGH); // Se non lo faccio, la serratura viene sbloccata appena viene alimentato l'ESP
  pinMode(relayPin, OUTPUT);

  // NFC Reader init
  SPI.begin();
  mfrc522.PCD_Init();

  // LCD Display Init
  lcd.begin(); // inizializzazione lcd
  lcd.backlight();

  WiFi.begin(ssid, password);
}

void loop() {

  if(WiFi.status() != WL_CONNECTED) {
    readNFC(); // Dobbiamo leggere già un eventuale tag NFC avvicinato alla scatola per riconoscere il Passpartout
    delay(500);
    debugLog("Connecting to " + (String)ssid);
    printOnLcd("Connessione al WiFi in corso...");
  }

  while (WiFi.status() == WL_CONNECTED) {
    debugLog("WiFi connected with IP address: " + (String)WiFi.localIP());
    printOnLcd("Scannerizzare una chiave NFC");
    readNFC();
    delay(500);
  }

}

void debugLog(String message) {
  Serial.println(message);
}

void printOnLcd(String text) {
  if(lcdCurrentText != text) {
    lcd.clear();
    lcd.print(text);
    lcdCurrentText = text;
  }
}

int sendPostRequest(String url, String jsonBody) {
  http.begin(url); // Mi connetto all'URL del Server specificato, conterrà il nostro Endpoint
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(jsonBody); // Invio una richiesta HTTP di tipo POST che nel Body contiene il contenuto della variabile jsonBody
  http.end();  //Close connection

  return httpResponseCode;
}

void authenticateNfcKey(String nfcUid) {
  debugLog("Begin NFC Auth with Server, using NFC UID " + nfcUid);
  printOnLcd("Autenticazione chiave...");

  int statusCode;
  statusCode = sendPostRequest(backendUrl, "{\"secret_key\": \"" + backendSecretKey + "\", \"nfc_uid\": \"" + nfcUid + "\"}");
  debugLog("Got response from Backend: " + (String)statusCode);

  if(statusCode == 200) {
    debugLog("Authorized key");
    printOnLcd("Chiave NFC autorizzata.");
    openLock();
    delay(1500);
  } else {
    printOnLcd("Chiave NFC non autorizzata.");
    delay(1500);
  }
}

void openLock() {
  digitalWrite(relayPin, LOW);
  Serial.println("Serratura sbloccata");
  delay(600);
  digitalWrite(relayPin, HIGH);
}

void readNFC()
{
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }

    // Select a card
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    /* Lettura dell'UID del Tag NFC e Conversione da Byte a String dell'UID */
    String nfcUid="";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      nfcUid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      nfcUid += String(mfrc522.uid.uidByte[i], HEX);
    }
    nfcUid.toUpperCase(); // Metto tutta la stringa in caratteri MAIUSCOLI

    //instructs the PICC when in the ACTIVE state to go to a "STOP" state
    mfrc522.PICC_HaltA();
    // "stop" the encryption of the PCD, it must be called after communication with authentication, otherwise new communications can not be initiated
    mfrc522.PCD_StopCrypto1();

    // Verifichiamo se il Tag scannerizzato è il Passepartout, se lo è sblocchiamo la serratura altrimenti, se siamo connessi ad internet, autentichiamo tramite il backend remoto
    if(nfcUid == passepartoutNfcUid) {
      printOnLcd("Passepartout rilevato");
      openLock();
      delay(600);
    } else if (WiFi.status() == WL_CONNECTED) {
      authenticateNfcKey(nfcUid);
    }
}
