// ============================================================
//  Reaction Time Game — Arduino UNO
//  OLED SSD1306 128x64 I2C
//  Bibliotecas: Adafruit SSD1306 + Adafruit GFX
// ============================================================
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- OLED ---
#define SCREEN_W  128
#define SCREEN_H   64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);

// --- Pinout ---
const uint8_t PIN_LED    = 9;
const uint8_t PIN_BUTTON = 13;

// --- Média das últimas N rodadas ---
const uint8_t  HIST_SIZE = 5;
unsigned long  history[HIST_SIZE];
uint8_t        histCount = 0;   // quantas rodadas válidas temos (máx HIST_SIZE)
uint8_t        histIndex = 0;   // próxima posição no array circular

// --- Recordes e timing ---
unsigned long bestUs     = 0;
unsigned long reactionUs = 0;
unsigned long waitUntil  = 0;
unsigned long ledOnTime  = 0;

// --- Detecção de borda (edge detection) ---
bool           prevBtnState  = HIGH;  // estado anterior do botão (HIGH = solto)
unsigned long  lastBounce    = 0;
const uint16_t DEBOUNCE_MS   = 50;

// --- Estados ---
enum State {
  STATE_MENU,
  STATE_WAITING,
  STATE_GO,
  STATE_RESULT
};
State gameState = STATE_MENU;

// ============================================================
void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  randomSeed(analogRead(A0));

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("OLED nao encontrada!"));
    while (true);
  }

  display.clearDisplay();
  drawMenu();
}

// ============================================================
void loop() {
  // Detecta apenas a borda de descida (HIGH→LOW) com debounce
  bool btnRaw = digitalRead(PIN_BUTTON);
  bool pressed = false;
  if (btnRaw == LOW && prevBtnState == HIGH && millis() - lastBounce > DEBOUNCE_MS) {
    pressed    = true;
    lastBounce = millis();
  }
  prevBtnState = btnRaw;

  switch (gameState) {

    // ── MENU ─────────────────────────────────────────────────
    case STATE_MENU:
      if (pressed) {
        startRound();
      }
      break;

    // ── AGUARDANDO LED ────────────────────────────────────────
    case STATE_WAITING:
      if (pressed) {
        // Antecipou — volta ao menu
        digitalWrite(PIN_LED, LOW);
        drawEarly();
        delay(1200);
        gameState = STATE_MENU;
        drawMenu();
        break;
      }
      if (millis() >= waitUntil) {
        digitalWrite(PIN_LED, HIGH);
        ledOnTime = micros();
        gameState = STATE_GO;
        drawGo();
      }
      break;

    // ── GO! ───────────────────────────────────────────────────
    case STATE_GO:
      if (pressed) {
        reactionUs = micros() - ledOnTime;
        blinkConfirm();
        saveHistory(reactionUs);
        if (bestUs == 0 || reactionUs < bestUs) bestUs = reactionUs;
        gameState = STATE_RESULT;
        drawResult();
      }
      break;

    // ── RESULTADO ─────────────────────────────────────────────
    case STATE_RESULT:
      if (pressed) {
        startRound();   // nova rodada direto, sem voltar ao menu
      }
      break;
  }
}

// ============================================================
//  Inicia nova rodada
// ============================================================
void startRound() {
  digitalWrite(PIN_LED, LOW);
  waitUntil = millis() + random(1000, 3001);
  gameState = STATE_WAITING;
  drawWaiting();
}

// ============================================================
//  Pisca LED 3× para confirmar o clique
// ============================================================
void blinkConfirm() {
  for (uint8_t i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, LOW);  delay(70);
    digitalWrite(PIN_LED, HIGH); delay(70);
  }
  digitalWrite(PIN_LED, LOW);
}

// ============================================================
//  Salva tempo no histórico circular
// ============================================================
void saveHistory(unsigned long us) {
  history[histIndex] = us;
  histIndex = (histIndex + 1) % HIST_SIZE;
  if (histCount < HIST_SIZE) histCount++;
}

// ============================================================
//  Calcula média do histórico (em ms)
// ============================================================
uint32_t calcAverage() {
  if (histCount == 0) return 0;
  unsigned long sum = 0;
  for (uint8_t i = 0; i < histCount; i++) sum += history[i];
  return (sum / histCount) / 1000;
}

// ============================================================
//  TELAS OLED
// ============================================================

void drawMenu() {
  display.clearDisplay();

  // Título
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(14, 4);
  display.print(F("REACTION TIME GAME"));

  // Linha separadora
  display.drawLine(0, 15, 127, 15, SSD1306_WHITE);

  // Recorde
  display.setCursor(4, 20);
  display.print(F("Recorde: "));
  if (bestUs == 0) {
    display.print(F("--"));
  } else {
    display.print(bestUs / 1000);
    display.print(F(" ms"));
  }

  // Media
  display.setCursor(4, 31);
  display.print(F("Media ("));
  display.print(histCount);
  display.print(F("x): "));
  if (histCount == 0) {
    display.print(F("--"));
  } else {
    display.print(calcAverage());
    display.print(F(" ms"));
  }

  // Linha separadora
  display.drawLine(0, 48, 127, 48, SSD1306_WHITE);

  // Instrução
  display.setCursor(8, 53);
  display.print(F("[BTN] Iniciar"));

  display.display();
}

// ─────────────────────────────────────────────────────────────
void drawWaiting() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(22, 10);
  display.print(F("Prepare-se..."));

  // Três pontos grandes centralizados
  display.setTextSize(3);
  display.setCursor(36, 28);
  display.print(F(". . ."));

  display.display();
}

// ─────────────────────────────────────────────────────────────
void drawGo() {
  display.clearDisplay();

  // "VAI!" em fonte grande centralizada
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(22, 10);
  display.print(F("VAI!"));

  display.setTextSize(1);
  display.setCursor(20, 50);
  display.print(F("Aperte o botao!"));

  display.display();
}

// ─────────────────────────────────────────────────────────────
void drawResult() {
  uint32_t ms  = reactionUs / 1000;
  uint32_t avg = calcAverage();
  bool     rec = (bestUs == reactionUs); // acabou de bater recorde

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Destaque de recorde
  if (rec) {
    display.setTextSize(1);
    display.setCursor(16, 0);
    display.print(F("** NOVO RECORDE **"));
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  }

  // Tempo de reação em fonte grande
  display.setTextSize(2);
  int16_t x = rec ? 14 : 8;
  int16_t y = rec ? 13 : 4;
  display.setCursor(x, y);
  display.print(ms);
  display.print(F(" ms"));

  // Linha divisória
  display.drawLine(0, rec ? 33 : 27, 127, rec ? 33 : 27, SSD1306_WHITE);

  // Recorde e média
  display.setTextSize(1);
  int16_t yInfo = rec ? 37 : 31;
  display.setCursor(4, yInfo);
  display.print(F("Recorde: "));
  display.print(bestUs / 1000);
  display.print(F(" ms"));

  display.setCursor(4, yInfo + 11);
  display.print(F("Media ("));
  display.print(histCount);
  display.print(F("x): "));
  display.print(avg);
  display.print(F(" ms"));

  // Instrução
  display.drawLine(0, 53, 127, 53, SSD1306_WHITE);
  display.setCursor(4, 56);
  display.print(F("[BTN] Jogar de novo"));

  display.display();
}

// ─────────────────────────────────────────────────────────────
void drawEarly() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(4, 10);
  display.print(F("Cedo!"));

  display.setTextSize(1);
  display.setCursor(4, 34);
  display.print(F("Aguarde o LED"));
  display.setCursor(4, 45);
  display.print(F("antes de clicar."));

  display.display();
}
