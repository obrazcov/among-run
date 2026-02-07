#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int buttonPin = 7;
const int speakerPin = 8;

static const char kAppVersion[] = "v4.4";

// --- Symbols & Animation Frames ---
byte crew_L[8] = {0b01110, 0b10001, 0b10111, 0b11111, 0b11111, 0b11111, 0b10000, 0b10001};
byte crew_R[8] = {0b01110, 0b10001, 0b10111, 0b11111, 0b11111, 0b11111, 0b00001, 0b10001};
byte crew_J[8] = {0b01110, 0b10001, 0b10111, 0b11111, 0b11111, 0b11111, 0b10001, 0b11011};
byte monster[8]= {0b10001, 0b01110, 0b10101, 0b11111, 0b10101, 0b01110, 0b11111, 0b10001};
byte tree[8]   = {0b00100, 0b01110, 0b01110, 0b11111, 0b11111, 0b00100, 0b00100, 0b00000}; 
byte bat[8]    = {0b01010, 0b10001, 0b10101, 0b11111, 0b11111, 0b10101, 0b01010, 0b00100}; 
byte build[8]  = {0b11111, 0b10101, 0b11111, 0b11111, 0b10101, 0b11111, 0b11111, 0b11111};
byte bolt[8]   = {0b00010, 0b00100, 0b01000, 0b11111, 0b00100, 0b01000, 0b10000, 0b00000};
byte heart[8]  = {0b00000, 0b01010, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000, 0b00000};
byte potion[8] = {0b01110, 0b00100, 0b01010, 0b10001, 0b11111, 0b10101, 0b11111, 0b00000};
byte clockSym[8]={0b00000, 0b01110, 0b10101, 0b10111, 0b10001, 0b01110, 0b00000, 0b00000};

// --- Music ---
int melody[] = {294, 330, 349, 392, 440, 349, 440, 0, 415, 330, 415, 0, 392, 311, 392, 0};
int monsterMelody[] = {82, 82, 82, 164, 155, 146, 138, 130, 82, 82, 82, 164, 155, 146, 138, 130};
int noteIndex = 0;

// --- State ---
int statsHearts = 0, statsPotions = 0, statsJumps = 0, statsTreesEaten = 0;
unsigned long gameStartTime, lastMoveTime, jumpStartTime, heroStartTime, undergroundStartTime;
int playerLevel = 0, health = 5, currentSpeed = 200;
int obsRow = 1, obsCol = -1, platCol = -10, platWidth = 0, itemCol = -10, itemType, itemRow;
bool isJumping = false, isHero = false, isUnderground = false, gameStarted = false;

void setup() {
  lcd.begin(16, 2);
  loadDefaultSprites();
  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(speakerPin, OUTPUT);
  randomSeed(analogRead(0));
  showTutorial();
}

void loadDefaultSprites() {
  lcd.createChar(0, crew_L);  lcd.createChar(1, tree);   lcd.createChar(2, heart);
  lcd.createChar(3, monster); lcd.createChar(4, potion); lcd.createChar(5, build);
  lcd.createChar(6, crew_R);  lcd.createChar(7, crew_J);
}

void showTutorial() {
  unsigned long lastToggle = 0;
  int page = 0;
  while(digitalRead(buttonPin) == HIGH) {
    if (millis() - lastToggle > 2500) {
      lastToggle = millis();
      lcd.clear();
      if (page == 0) {
        lcd.print("CITY RUN ");
        lcd.print(kAppVersion);
        lcd.setCursor(0,1); lcd.print("PRESS TO START");
      } else {
        lcd.write(4); lcd.print("=Monster "); lcd.write(2); lcd.print("=Life");
        lcd.setCursor(0,1); lcd.createChar(4, bolt); lcd.write(4); lcd.print("=Subway  "); 
        lcd.createChar(4, clockSym); lcd.write(4); lcd.print("=Slow");
        lcd.createChar(4, potion); 
      }
      page = (page + 1) % 2;
    }
  }
  tone(speakerPin, 600, 100); delay(300);
  startGame();
}

void startGame() {
  statsHearts = 0; statsPotions = 0; statsJumps = 0; statsTreesEaten = 0;
  gameStarted = true; gameStartTime = millis();
  playerLevel = 0; isJumping = false; isHero = false; isUnderground = false; health = 5;
  obsCol = -1; itemCol = -10; platCol = -10; noteIndex = 0;
}

void loop() {
  if (!gameStarted) return;
  unsigned long now = millis();
  
  if (now - gameStartTime < 300000) currentSpeed = 200 - ((now - gameStartTime) / 2140);

  int uT = 0; if (isUnderground) {
    uT = 20 - ((now - undergroundStartTime) / 1000);
    if (uT <= 0) { isUnderground = false; loadDefaultSprites(); }
  }
  
  int hT = 0; if (isHero) {
    hT = 10 - ((now - heroStartTime) / 1000);
    if (hT <= 0) isHero = false;
  }

  if (digitalRead(buttonPin) == LOW && !isJumping) {
    isJumping = true; jumpStartTime = now; statsJumps++;
    tone(speakerPin, isHero ? 150 : 800, 20);
    playerLevel++; if (playerLevel > 2) playerLevel = 2;
  }
  if (isJumping && (now - jumpStartTime > 1200)) isJumping = false;

  if (!isJumping) {
    bool onPlat = (platCol <= 2 && (platCol + platWidth) > 2);
    if (onPlat && playerLevel >= 1) playerLevel = 1; else playerLevel = 0;
  }

  if (now - lastMoveTime > currentSpeed) {
    lastMoveTime = now;
    int note = isHero ? monsterMelody[noteIndex % 16] : melody[noteIndex % 16];
    tone(speakerPin, isUnderground ? note/2 : note, 40);
    noteIndex++;

    lcd.clear();
    obsCol--; platCol--; itemCol--;

    if (obsCol + 3 < 0 && platCol + platWidth < 0) {
      if (random(0, 5) == 0) { obsCol = 15; obsRow = random(0, 2); } 
      else if (!isUnderground && random(0, 4) == 0) { platCol = 15; platWidth = random(3, 7); }
    }
    
    // ITEM SPAWNING LOGIC: Potions removed in Underground
    if (itemCol < -10 && random(0, 35) == 0) {
      itemCol = 15; itemRow = random(0, 2); 
      itemType = random(0, 4);
      // If we roll a Potion (1) while underground, change it to a Clock (3)
      if (isUnderground && itemType == 1) itemType = 3; 
    }

    if (isUnderground) lcd.createChar(1, bat); else lcd.createChar(1, tree); 
    
    if (itemType == 3) lcd.createChar(4, clockSym); 
    else if (itemType == 2) lcd.createChar(4, bolt);
    else lcd.createChar(4, potion);
    
    drawFrame(uT, hT);
  }
  checkCollisions();
}

void drawFrame(int uT, int hT) {
  lcd.setCursor(14, 0); lcd.print(health); lcd.write(2);
  if (isUnderground) { lcd.setCursor(0, 0); lcd.print("U:"); lcd.print(uT); }
  else if (isHero) { lcd.setCursor(0, 0); lcd.print("M:"); lcd.print(hT); }

  if (obsCol >= 0 && obsCol < 16) { lcd.setCursor(obsCol, obsRow); lcd.write(1); }

  for (int i = 0; i < platWidth; i++) {
    int p = platCol + i;
    if (p >= 0 && p < 16) { lcd.setCursor(p, 1); lcd.write(5); }
  }

  if (itemCol >= 0 && itemCol < 16) {
    lcd.setCursor(itemCol, itemRow);
    if (itemType == 0) lcd.write(2); 
    else lcd.write(4); 
  }

  lcd.setCursor(2, (playerLevel == 0 ? 1 : 0));
  if (isHero) lcd.write(3);
  else if (isJumping) lcd.write(7);
  else lcd.write((noteIndex % 2 == 0) ? 0 : 6);
}

void checkCollisions() {
  int pR = (playerLevel == 0 ? 1 : 0);
  if (obsCol == 2 && obsRow == pR) {
    if (isHero) { obsCol = -1; statsTreesEaten++; tone(speakerPin, 60, 40); }
    else handleCrash();
  }
  if (itemCol == 2 && itemRow == pR) {
    if (itemType == 0) { statsHearts++; if (health < 5) health++; tone(speakerPin, 1200, 50); }
    else if (itemType == 1) { statsPotions++; isHero = true; heroStartTime = millis(); }
    else if (itemType == 2) { isUnderground = true; undergroundStartTime = millis(); }
    else if (itemType == 3) { gameStartTime = millis(); }
    itemCol = -10;
  }
}

void handleCrash() {
  health--; tone(speakerPin, 100, 300);
  if (health <= 0) showStats();
  else {
    lcd.setCursor(5, 0); lcd.print("OUCH!"); delay(800);
    obsCol = -1; itemCol = -10; platCol = -10; isJumping = false; playerLevel = 0;
  }
}

void showStats() {
  gameStarted = false;
  unsigned long t = (millis() - gameStartTime) / 1000;
  unsigned long lastToggle = 0; bool page = true;
  while(digitalRead(buttonPin) == HIGH) {
    if (millis() - lastToggle > 2000) {
      lastToggle = millis(); lcd.clear();
      if (page) { 
        lcd.setCursor(3,0); lcd.print("GAME OVER!");
        lcd.setCursor(1,1); lcd.print("TIME: "); lcd.print(t/60); lcd.print("m "); lcd.print(t%60); lcd.print("s");
      } else { 
        lcd.write(2); lcd.print(":"); lcd.print(statsHearts); 
        lcd.setCursor(8,0); lcd.createChar(4, potion); lcd.write(4); lcd.print(":"); lcd.print(statsPotions);
        lcd.setCursor(0,1); lcd.print("TREES EATEN: "); lcd.print(statsTreesEaten);
      }
      page = !page;
    }
  }
  loadDefaultSprites(); startGame();
}
