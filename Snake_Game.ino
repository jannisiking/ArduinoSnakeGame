// Librarys
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoQueue.h>

#ifndef PSTR
#define PSTR 
#endif

// Hardware Config
#define PIN 11

#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MATRIX_WIDTH, MATRIX_HEIGHT, PIN,
                                               NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                                               NEO_GRB + NEO_KHZ800);



#define BUTTON_TOP 5
#define BUTTON_RIGHT 9
#define BUTTON_BOTTOM 6
#define BUTTON_LEFT 3

// Variablen und Konstanten, Konstrukte
#define TOP 0
#define RIGHT 1
#define BOTTOM 2
#define LEFT 3

struct coord {
  int x;
  int y;
};

ArduinoQueue<coord> snake(256);

coord snakeHead;

coord collectablePoint;

int direction;

bool lost;

//Helper-Funktionen
coord generateRandomCoord(int width, int height) {
  return {
    collectablePoint.x = random(0, width),
    collectablePoint.y = random(0, height)
  };
}

void emptySnake() {
  while (!snake.isEmpty()) {
    snake.dequeue();
  }
}

//** Snake-Game **//
void initializeSnakeGame() {

  lost = false;

  emptySnake();

  snake.enqueue({ 0, 0 });
  snake.enqueue({ 1, 0 });
  snake.enqueue({ 2, 0 });

  snakeHead.x = 4;
  snakeHead.y = 0;

  direction = RIGHT;

  collectablePoint = generateRandomCoord(MATRIX_WIDTH, MATRIX_HEIGHT);
}

void SnakeGame() {
  while (!lost) {
    // Richtung der Schlange festlegen
    if (!digitalRead(BUTTON_TOP)) {
      direction = TOP;
    }
    if (!digitalRead(BUTTON_RIGHT)) {
      direction = RIGHT;
    }
    if (!digitalRead(BUTTON_BOTTOM)) {
      direction = BOTTOM;
    }
    if (!digitalRead(BUTTON_LEFT)) {
      direction = LEFT;
    }

    // Je nach Richtung Koordinaten verändern
    switch (direction) {
      case TOP:
        snakeHead.y--;
        break;
      case RIGHT:
        snakeHead.x++;
        break;
      case BOTTOM:
        snakeHead.y++;
        break;
      case LEFT:
        snakeHead.x--;
        break;
    }

    // Bei Randwerten an anderer Seite wieder anfangen
    if (snakeHead.x > MATRIX_WIDTH - 1) {
      snakeHead.x = 0;
    }
    if (snakeHead.x < 0) {
      snakeHead.x = MATRIX_WIDTH;
    }

    if (snakeHead.y > MATRIX_HEIGHT - 1) {
      snakeHead.y = 0;
    }
    if (snakeHead.y < 0) {
      snakeHead.y = MATRIX_HEIGHT;
    }

    // Checken ob crash -> Spiel verloren
    for (int i = 0; i < snake.itemCount(); i++) {
      coord coordToCheck = snake.dequeue();
      snake.enqueue(coordToCheck);
      if (snakeHead.x == coordToCheck.x && snakeHead.y == coordToCheck.y) {
        lost = true;
      }
    }

    // Berechnete neue Kopfkoordinate an Schlange anhängen
    snake.enqueue({
      snakeHead.x,
      snakeHead.y,
    });

    // Je nachdem ob Punkt getroffen wurde Snake hinten kürzen (voran laufen) oder länger lassen und neuen Punkt generieren
    if (snakeHead.x == collectablePoint.x && snakeHead.y == collectablePoint.y) {
      collectablePoint = generateRandomCoord(MATRIX_WIDTH, MATRIX_HEIGHT);
    } else {
      snake.dequeue();
    }


    matrix.fillScreen(0);
    matrix.drawPixel(collectablePoint.x, collectablePoint.y, matrix.Color(255, 0, 0));
    for (int coordCount = snake.itemCount(); coordCount > 0; coordCount--) {
      coord coordToDraw = snake.dequeue();
      snake.enqueue(coordToDraw);
      matrix.drawPixel(coordToDraw.x, coordToDraw.y, matrix.Color(0, 255, 0));
    }

    matrix.show();
    //Delay zum prellen der Buttons
    delay(50);
  }
}

void printScore(){
matrix.fillScreen(0);
  matrix.setCursor(0, 0);
  matrix.setTextColor(matrix.Color(255, 0, 0));
  matrix.print(snake.itemCount());  
  matrix.show();
}
//** ENDE Snake-Game **//

// Hauptsetup des Programms
void setup() {
  pinMode(BUTTON_TOP, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_BOTTOM, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
}

// Main-Loop
void loop() {
  initializeSnakeGame();
  SnakeGame();
  printScore();
  delay(5000);
}