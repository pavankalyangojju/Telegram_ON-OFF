#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// =================================
// WIFI DETAILS
// =================================

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// =================================
// TELEGRAM DETAILS
// =================================

#define BOT_TOKEN "YOUR_BOT_TOKEN"

// Add your Telegram Chat ID
#define CHAT_ID "Telegram_Chat_ID"

// =================================
// LED CONFIGURATION
// =================================

#define LED_PIN 2

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long lastTime = 0;
const unsigned long interval = 1000;

bool ledState = false;

// =================================
// SETUP
// =================================

void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");

  client.setInsecure();

  Serial.println("Telegram Bot Ready!");
}

// =================================
// HANDLE TELEGRAM MESSAGES
// =================================

void handleNewMessages(int numNewMessages) {

  for (int i = 0; i < numNewMessages; i++) {

    String chat_id = bot.messages[i].chat_id;
    String message = bot.messages[i].text;

    // Chat ID verification
    if (chat_id != String(CHAT_ID)) {
      bot.sendMessage(chat_id, "Unauthorized Access!", "");
      continue;
    }

    Serial.println("Received: " + message);

    if (message == "/start") {

      String welcome = "ESP32 LED CONTROL\n\n";
      welcome += "/on - Turn LED ON\n";
      welcome += "/off - Turn LED OFF\n";
      welcome += "/status - Check LED Status";

      bot.sendMessage(chat_id, welcome, "");
    }

    else if (message == "/on") {

      digitalWrite(LED_PIN, HIGH);
      ledState = true;

      bot.sendMessage(chat_id, "LED is ON 🟢", "");
    }

    else if (message == "/off") {

      digitalWrite(LED_PIN, LOW);
      ledState = false;

      bot.sendMessage(chat_id, "LED is OFF 🔴", "");
    }

    else if (message == "/status") {

      if (ledState) {
        bot.sendMessage(chat_id, "LED Status: ON 🟢", "");
      }
      else {
        bot.sendMessage(chat_id, "LED Status: OFF 🔴", "");
      }
    }

    else {

      bot.sendMessage(chat_id,
        "Invalid Command!\nUse /on, /off or /status",
        "");
    }
  }
}

// =================================
// MAIN LOOP
// =================================

void loop() {

  if (millis() - lastTime >= interval) {

    int numNewMessages =
      bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {

      handleNewMessages(numNewMessages);

      numNewMessages =
        bot.getUpdates(bot.last_message_received + 1);
    }

    lastTime = millis();
  }
}