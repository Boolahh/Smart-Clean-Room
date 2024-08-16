#include <pigpio.h>
#include <MFRC522.h>
#include <cstdio>

// Define the pins used for the RFID reader and the buzzer
#define RST_PIN 25    // Configurable, depending on your wiring
#define SS_PIN 8      // CE0 (Chip Enable)
#define BUZZER_PIN 17 // Example GPIO pin for the buzzer

// Create an instance of the MFRC522 class
MFRC522 mfrc522(SS_PIN, RST_PIN);

// List of valid UIDs as integers
int valid_uids[] = {
    041224,  // Example UID aka birthday 1
    040618   // Example UID aka birthday 2
};

// Variable to store the number of valid UIDs
int num_valid_uids = sizeof(valid_uids) / sizeof(valid_uids[0]);

// Function to check if a scanned UID is valid
bool check_valid_id() {
    // Check for new cards
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return false;
    }

    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
        return false;
    }

    // Assuming the UID is stored directly as an integer (simplified)
    int scanned_uid = mfrc522.uid.uidByte[0]; // Simplified, assuming UID is a single-byte integer

    // Check if the scanned UID matches any valid UID
    for (int i = 0; i < num_valid_uids; i++) {
        if (scanned_uid == valid_uids[i]) {
            return true;
        }
    }

    // Halt PICC and stop encryption
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    return false;
}

void setup() {
    // Initialize pigpio
    if (gpioInitialise() < 0) {
        printf("pigpio initialization failed\n");
        exit(1);
    }

    // Initialize SPI for MFRC522
    mfrc522.PCD_Init();

    // Initialize the buzzer pin
    gpioSetMode(BUZZER_PIN, PI_OUTPUT);

    printf("Waiting for an RFID card...\n");
}

void loop() {
    // Continuously check for a valid RFID card
    bool is_valid = check_valid_id();

    // Trigger buzzer if the card is valid
    if (is_valid) {
        gpioWrite(BUZZER_PIN, 1); // Turn on buzzer
        printf("Valid card detected! Buzzer activated.\n");
    } else {
        gpioWrite(BUZZER_PIN, 0); // Turn off buzzer
        printf("Invalid card detected!\n");
    }
}

int main() {
    setup();
    while (true) {
        loop();  // This will keep looping indefinitely
    }
    gpioTerminate();
    return 0;
}

