#include <Arduino_H7_Video.h>
#include <Arduino_GigaDisplay_GFX.h>
#include <Adafruit_GFX.h>
#include <platform/mbed_critical.h>
#include <Arduino_GigaDisplayTouch.h>
#include <SimpleTimer.h>

// Define colors
#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F


// Initialize the display object

GigaDisplay_GFX display;
Arduino_GigaDisplayTouch touchDetector;

// Global variables to track the enabled status of each tool
bool guidewireEnabled = false;
bool needleEnabled = false;
bool dilatorSheathEnabled = false;

// Add missing global definitions
int rightColumnX = 250;  // Example, adjust according to your layout
int toolBoxWidth = 270;
int toolBoxHeight = 60;

bool updateUI = false;

SimpleTimer timer(250);

void setup() {
  Serial.begin(115200);
  display.begin();
  touchDetector.begin();
  display.setRotation(1);  // Adjust rotation to fit the 480x800 display orientation
  display.fillScreen(BLACK);

  drawInitialUI();
}


void loop() {
  GDTpoint_t points[5];  // Array to store touch points
  uint8_t numTouches = touchDetector.getTouchPoints(points);

  if (numTouches > 0 && timer.isReady()) {
    for (uint8_t i = 0; i < numTouches; i++) {
      checkTouch(points[i].x, points[i].y);
      updateUI = true;
    }
    timer.reset();
  }

  if (updateUI) {
    drawInitialUI();
    updateUI = false;
  }
}

// Other necessary functions below


void drawInitialUI() {
  // Layout parameters
  int leftColumnWidth = 240;  // Width of the left column
  int rightColumnX = 250;     // X position where the right column starts

  // Left Column
  // Title
  display.setTextColor(BLUE);
  display.setTextSize(4);
  display.setCursor(240, 20);
  display.print("SEPTALPRO CONTROLLER");

  // Connection Status Box
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.fillRect(10, 80, leftColumnWidth + 120, 120, BLUE);
  display.setTextColor(WHITE);
  display.setCursor(20, 100);
  display.print("Connection Status");

  // Connection Status Details
  display.setCursor(20, 130);
  display.print("Robot: CONNECTED");
  display.setCursor(20, 160);
  display.print("Simulation: CONNECTED");

  // Tool Switching Status
  display.setTextColor(WHITE);
  display.fillRect(10, 200, leftColumnWidth + 120, 50, RED);
  display.setTextColor(WHITE);
  display.setCursor(20, 215);
  display.print("Tool Switching in Progress...");

  // Control Mode Box
  display.setTextColor(BLUE);
  display.fillRect(10, 250, leftColumnWidth + 120, 400, BLUE);
  display.setTextColor(WHITE);
  display.setCursor(20, 260);
  display.print("Control Mode:");

  // Right Column
  // Tool Status Box
  display.setTextColor(BLACK);
  display.fillRect(rightColumnX + 140, 80, 400, 400, BLUE);
  display.setTextColor(WHITE);
  display.setCursor(rightColumnX + 150, 85);
  display.print("Tool Status");

  // Guidewire Status
  drawToolStatus(rightColumnX + 150, 160, "Guidewire", guidewireEnabled, "1N");

  // Needle Status
  drawToolStatus(rightColumnX + 150, 230, "Needle", needleEnabled, "1N");

  // Dilator-Sheath Status
  drawToolStatus(rightColumnX + 150, 300, "Dilator-Sheath", dilatorSheathEnabled, "1N");
}

void checkTouch(int x, int y) {
  // Logic to check if x, y falls within any tool status box
  // Toggle the corresponding tool's enabled status if touched

  Serial.print("Touch detected at: ");
  Serial.print(x);
  Serial.print(", ");
  Serial.println(y);

  // For example:
  // guidewire
  if (x >= 280 && x <= 310 && y >= 410 && y <= 690) {
    guidewireEnabled = !guidewireEnabled;
    Serial.print("Guidewire Status now: ");
    Serial.println(guidewireEnabled ? "Enabled" : "Disabled");
  }
  // Needle
  if (x >= 200 && x <= 220 && y >= 405 && y <= 670) {
    needleEnabled = !needleEnabled;
    Serial.print("Needle Status now: ");
    Serial.println(needleEnabled ? "Enabled" : "Disabled");
  }
  //dilator sheath
  if (x >= 90 && x <= 150 && y >= 400 && y <= 670) {
    dilatorSheathEnabled = !dilatorSheathEnabled;
    Serial.print("Dilator-Sheath Status now: ");
    Serial.println(dilatorSheathEnabled ? "Enabled" : "Disabled");
  }
}

void drawToolStatus(int x, int y, const char* toolName, bool isEnabled, const char* force) {
  uint16_t fillColor = isEnabled ? GREEN : RED;
  display.fillRect(x, y, 270, 60, fillColor);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(x + 10, y + 10);
  display.print(toolName);
  display.setCursor(x + 10, y + 35);
  display.print(isEnabled ? "ENABLED" : "DISABLED");
  display.setCursor(x + 160, y + 35);
  display.print("Force: ");
  display.print(force);
  Serial.println(y);
}
