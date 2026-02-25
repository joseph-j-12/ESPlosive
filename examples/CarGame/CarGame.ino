

#include <ESPlosive.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "CarScene.h"
#include "FinishLine.h"
#include "GInput.h"
// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

const int JX = 33;
const int JY = 32;

const int BTN_A = 26;
const int BTN_B = 27;
const int BTN_C = 14;
const int BTN_D = 12;

Vec2D convertScreenToWorld(Vec2D screenPos, int screenwidth = 320, int screenheight= 240);
Vec2D convertWorldToScreen(Vec2D worldPos, int screenwidth = 320, int screenheight= 240);

float scrollx = 0;
float scrolly = 0;

unsigned long lastMillis;
unsigned long frameCount = 0;
unsigned int framesPerSecond;

unsigned int framesPerSecondDisplay;
unsigned long lastMillisDisplay;
unsigned long frameCountDisplay = 0;
CarScene* myScene;

GInput_JoyStick joy(JX,JY);
GInput_Button btn_L(BTN_A, false);
GInput_Button btn_U(BTN_C, false);
GInput_Button btn_D(BTN_B, false);
GInput_Button btn_R(BTN_D, false);

TFT_eSprite stBar = TFT_eSprite(&tft);

SemaphoreHandle_t sceneReset;

unsigned long execTime;
unsigned long frameTime;

int conf_selection1 = 0;
bool display_changed = false;
int carGameLevelLength = 0;
int carGameLevelDiff = 1;
enum State{
    MENU,
    CarGameState,
    CarGameConfState
};

State currentState = CarGameState;

void drawStatusBar(int fps0, int fps1) {
  uint16_t barH = 12; // Very slim
  uint16_t barW = tft.width();

  // Create sprite if not already created
  if (!stBar.created()) {
    stBar.createSprite(barW, barH);
  }

  // Clear Sprite background
  stBar.fillSprite(TFT_BLACK);
  
  // Get Stats
  uint32_t tot = ESP.getHeapSize() / 1024;
  uint32_t fre = ESP.getFreeHeap() / 1024;
  uint32_t maxB = ESP.getMaxAllocHeap() / 1024;

  stBar.setTextColor(TFT_GREEN);
  stBar.setTextSize(1); // Standard small font
  
  // Use shorter labels to save space
  char buf[128];
  snprintf(buf, sizeof(buf), "T:%uK F:%uK M:%uK | C0:%d C1:%d | %uMHz | T:%u", 
           tot, fre, maxB, fps0, fps1, getCpuFrequencyMhz(),frameTime);

  stBar.drawString(buf, 2, 2); // Draw into RAM buffer
  stBar.pushSprite(0, 0);      // Push to physical screen at (0,0)
}

void Task1code( void * parameter) {
    delay(12);
  while (true)
  {
    if (currentState == CarGameState)
    CarGameDisplay();
    else if (currentState == CarGameConfState)
    CarGameConf();
    delay(5);
    
  }
}

void CarGameConf()
{
    xSemaphoreTake(sceneReset, portMAX_DELAY);
    if (display_changed)
    {
    tft.fillScreen(TFT_BLACK);
    display_changed = false;
    }
    if (currentState != CarGameConfState) {
        xSemaphoreGive(sceneReset);
        return;
    }
    // if (conf_selection1 > 1)
    // conf_selection1 = 0;
    tft.setCursor(25,25);
    tft.setTextSize(2);
    tft.print("Car Game Config");

    tft.setCursor(25,75);
    tft.setTextSize(2);
    tft.print("Level length :");
    tft.setCursor(25,95);
    if (carGameLevelLength == 0)
        tft.print("short");
    if (carGameLevelLength == 1)
        tft.print("medium");
    if (carGameLevelLength == 2)
        tft.print("long");


    tft.setCursor(25,135);
    tft.print("Level difficulty :");
    tft.setCursor(25,155);
    if (carGameLevelDiff == 0)
        tft.print("easy");
    if (carGameLevelDiff == 1)
        tft.print("medium");
    if (carGameLevelDiff == 2)
        tft.print("hard");
    
    tft.fillCircle(0,75 + conf_selection1*60, 10, TFT_WHITE);
    xSemaphoreGive(sceneReset);
}

void CarGameDisplay()
{
    xSemaphoreTake(sceneReset, portMAX_DELAY);
    frameCountDisplay++; 
    if ((millis() - lastMillisDisplay) >= 1000) {
        framesPerSecondDisplay = frameCountDisplay;
        frameCountDisplay = 0;
        lastMillisDisplay = millis();
        drawStatusBar(framesPerSecondDisplay, framesPerSecond);
    }

    clearColliders(myScene, tft);
    renderColliders(myScene, tft);

    if (myScene)
    {
        if (myScene->finishLine->player_reached)
        {
            tft.setTextSize(5);
            tft.setCursor(25,100);
            tft.print("You Won!");
        }
    }     
    xSemaphoreGive(sceneReset);
}
void setup() {

    TaskHandle_t Task1;
    sceneReset = xSemaphoreCreateMutex();
    pinMode(BTN_A, INPUT_PULLUP);
    pinMode(BTN_B, INPUT_PULLUP);
    pinMode(BTN_C, INPUT_PULLUP);
    pinMode(BTN_D, INPUT_PULLUP);

    tft.init();
    tft.invertDisplay(true);
    //tft.setRotation(2);
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    Serial.begin(115200);
    lastMillis = millis();
    myScene =  new CarScene();
    myScene->terrain_length = 25 + carGameLevelLength*30;
    myScene->difficulty = carGameLevelDiff;
    
    bool buttonVal = false;
    myScene->Begin();
    myScene->joy = &joy;
    myScene->btn_U = &btn_U;
    myScene->btn_L = &btn_L;

    xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */
    

    joy.errorH = -0.1;
    joy.errorV = -0.1;
    joy.deadZoneH = 0.06;
    joy.deadZoneV = 0.06;
    
    while(true)
    {
        execTime = micros();

        btn_L.Tick();
        btn_U.Tick();
        btn_D.Tick();
        btn_R.Tick();
        
        if (currentState == CarGameState)
        CarGame(myScene, tft);

        else if (currentState == CarGameConfState)
        CarGameConf(myScene, tft);

        frameCount++; 
        
        
        if ((millis() - lastMillis) >= 1000)
        {
            framesPerSecond = frameCount; 
            // printMemoryUsage();
            frameCount = 0;
            lastMillis = millis();
        }
        
        

        frameTime = micros() - execTime;
        if (frameTime >= 2000){
            frameTime = 2000;
            continue;
        }
        delayMicroseconds(2000 - (micros() - execTime));
        
  }
    

}

void CarGameConf(CarScene* myScene, TFT_eSPI& tft)
{
    if (btn_D.myButtonState == GInput_Button::ButtonState::ONRELEASE)
    {
        xSemaphoreTake(sceneReset, portMAX_DELAY);  
        delay(15);
        // delete myScene;
        myScene =  new CarScene();
        myScene->terrain_length = 25 + carGameLevelLength*30;
        myScene->difficulty = carGameLevelDiff;
        myScene->Begin();
        myScene->joy = &joy;
        currentState = CarGameState;
        tft.fillScreen(TFT_BLACK);
        xSemaphoreGive(sceneReset);
    }

    if (btn_R.myButtonState == GInput_Button::ButtonState::ONRELEASE)
    {
        if (conf_selection1 == 0)
        {
            carGameLevelLength += 1;
            carGameLevelLength %= 3;
            display_changed = true;
        }
        else if (conf_selection1 == 1)
        {
            carGameLevelDiff += 1;
            carGameLevelDiff %= 3;
            Serial.println(carGameLevelDiff);
            display_changed = true;
        }
    }

    if (btn_U.myButtonState == GInput_Button::ButtonState::ONRELEASE)
    {
        conf_selection1 += 1;
        conf_selection1 %= 2;
        display_changed = true;
    }
}

void CarGame(CarScene* myScene, TFT_eSPI& tft)
{
    scrollx = myScene->cam.position.X;
    scrolly = myScene->cam.position.Y;

    myScene->Tick_Physics(0.014);
    myScene->Tick_Objects(0.014);

    if (btn_L.myButtonState == GInput_Button::ButtonState::ONRELEASE)
    {
        xSemaphoreTake(sceneReset, portMAX_DELAY);  
        delay(15);
        delete myScene;
        myScene =  new CarScene();
        myScene->terrain_length = 25 + carGameLevelLength*30;
        myScene->difficulty = carGameLevelDiff;
        myScene->Begin();
        myScene->joy = &joy;
        Serial.println("restart");
        tft.fillScreen(TFT_BLACK);
        xSemaphoreGive(sceneReset);
    }

    if (btn_U.myButtonState == GInput_Button::ButtonState::ONRELEASE)
    {
        xSemaphoreTake(sceneReset, portMAX_DELAY);  
        delay(15);
        myScene->restart_level();
        tft.fillScreen(TFT_BLACK);
        xSemaphoreGive(sceneReset);
    }

    if (btn_D.myButtonState == GInput_Button::ButtonState::ONRELEASE)
    {
        xSemaphoreTake(sceneReset, portMAX_DELAY);  
        delay(15);
        delete myScene;
        currentState = CarGameConfState;
        tft.fillScreen(TFT_BLACK);
        display_changed = true;
        xSemaphoreGive(sceneReset);
    }

}

void loop()
{
   
}

void drawCircleWithAxis(
    TFT_eSPI& tft,
    int cx, int cy,
    int r,
    uint16_t color
)
{
    tft.drawCircle(cx, cy, r, color);
    //tft.drawLine(cx, cy, cx + r, cy, color);
}


void drawPolygon(TFT_eSPI& tft, Vec2D* points, int count, uint16_t color)
{
    for (int i = 0; i < count; i++)
    {
        int j = (i + 1) % count;
        tft.drawLine(
            points[i].X, points[i].Y,
            points[j].X, points[j].Y,
            color
        );
    }
}

void renderColliders(GScene* scene, TFT_eSPI& tft)
{
    int count = 0;
    for (auto& obj : scene->sceneObjects)
    {
        if (!obj->enabled) continue;
        for (auto& coll : obj->myComponents)
        {
            if (coll->_myCompType != GComponent::ComponentType::Collider) continue;
            // if (!static_cast<GColliderComp*>(coll.get()))
            //    continue;

            auto* col = static_cast<GColliderComp*>(coll.get());

            if (!col) continue;
            uint16_t outlineColor =
                obj->getPhysicsEnabled() ? TFT_RED : TFT_BLUE;

            if (obj->my_id == 15)    
            {
                if(FinishLineObject* f = static_cast<FinishLineObject*>(obj.get()))
                    if (f->player_reached == true)
                        outlineColor = TFT_GREEN;
                    else
                        outlineColor = TFT_YELLOW;
            }

            if (col->myColliderType == GColliderComp::ColliderType::Polygon)
            {
                int n = col->myShape->numPoints;
                static Vec2D screenPts[16]; // adjust max if needed
                count += 1;
                for (int i = 0; i < n; i++)
                {
                    Vec2D p =
                        obj->transform.local_to_scene(
                            col->myShape->points[i]
                        );

                    screenPts[i] = convertWorldToScreen(p);
                    // if (obj->my_id == 5) {
                    //     Serial.print(p.Y);
                    //     Serial.println();
                    // }
                    
                }

                // Draw polygon outline
                drawPolygon(tft, screenPts, n, outlineColor);

                // ---- Bounding box (optional)
                col->CalculateBoundingBox();
                col->lastSeenBoundingBoxMin = convertWorldToScreen(col->boundingBoxMin);
                col->lastSeenBoundingBoxMax = convertWorldToScreen(col->boundingBoxMax);
                Vec2D bTop =
                    convertWorldToScreen(
                        Vec2D(col->boundingBoxMin.X, col->boundingBoxMax.Y)
                    );

                Vec2D bBottom =
                    convertWorldToScreen(
                        Vec2D(col->boundingBoxMax.X, col->boundingBoxMin.Y)
                    );

                int w = bBottom.X - bTop.X;
                int h = bBottom.Y - bTop.Y;

                // Uncomment if you want bounding boxes
                 //tft.drawRect(bTop.X, bTop.Y, w, h, TFT_GREEN);
            }

           
            else if (col->myColliderType == GColliderComp::ColliderType::Circle)
            {
                Vec2D centerWorld =
                    obj->transform.local_to_scene(col->myCenter);

                Vec2D center =
                    convertWorldToScreen(centerWorld);
                
                int r = (int)col->circleRadius;

                col->CalculateBoundingBox();
                col->lastSeenBoundingBoxMin = convertWorldToScreen(col->boundingBoxMin);
                col->lastSeenBoundingBoxMax = convertWorldToScreen(col->boundingBoxMax);

                drawCircleWithAxis(
                    tft,
                    center.X,
                    center.Y,
                    r,
                    outlineColor
                );
            }
        }
    }
}

void clearColliders(GScene* scene, TFT_eSPI& tft)
{
    int count = 0;
    for (auto& obj : scene->sceneObjects) {
        if (!obj->enabled) continue;
        for (auto& coll : obj->myComponents) {
            auto* col = static_cast<GColliderComp*>(coll.get());
            if (!col) continue;
            if (coll->_myCompType != GComponent::ComponentType::Collider) continue;
            // Clear the exact area we occupied last frame
            Vec2D p1 = col->lastSeenBoundingBoxMin;
            Vec2D p2 = col->lastSeenBoundingBoxMax;
            // Serial.println(p2.Y-p1.Y);
            tft.fillRect(
                p1.X-3, 
                p2.Y-3, 
                p2.X-p1.X+6, 
                p1.Y-p2.Y+6, 
                TFT_BLACK
            );
        }
        // Update physics transform history
        //obj->transform.calculatePrevCosAndSine();
    }
}


Vec2D convertScreenToWorld(Vec2D screenPos, int screenwidth, int screenheight)
{
    return Vec2D(screenPos.X-screenwidth/2,-(screenPos.Y-screenheight/2));
}

Vec2D convertWorldToScreen(Vec2D worldPos, int screenwidth, int screenheight)
{
    return Vec2D((worldPos.X-scrollx)+screenwidth/2, (-worldPos.Y+scrolly)+screenheight/2);
}

