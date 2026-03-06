#include <Bluepad32.h>
#include <math.h>
/*
    Steuerungssoftware für vordersten Robotino
    Basiert auf Controllerbeispiel von Bluepad32


*/

// Motor Variablen:
int MotorCH_R = 0;
int motorR_ENA = 32;
int motorR_IN1 = 33;
int motorR_IN2 = 25;

int MotorCH_L = 1;    
int motorL_IN3 = 14;
int motorL_IN4 = 27;
int motorL_ENB = 26;

// IR_LED
int LEDCH = 2;
int IR_LED = 12;

// RGB_LED
int RGB_RED = 4;
int RGB_BLUE = 0;
int RGB_GREEN = 2;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];


void RGB_AUS() {
    digitalWrite(RGB_RED, HIGH);
    digitalWrite(RGB_BLUE, HIGH);
    digitalWrite(RGB_GREEN, HIGH);
}


// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }

    RGB_AUS();

    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
        digitalWrite(RGB_RED, LOW);
    } else {
    digitalWrite(RGB_GREEN, LOW);    
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }

    RGB_AUS();
    digitalWrite(RGB_BLUE, LOW);
}

void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
        ctl->index(),        // Controller Index
        ctl->dpad(),         // D-pad
        ctl->buttons(),      // bitmask of pressed buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->brake(),        // (0 - 1023): brake button
        ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
        ctl->miscButtons(),  // bitmask of pressed "misc" buttons
        ctl->gyroX(),        // Gyro X
        ctl->gyroY(),        // Gyro Y
        ctl->gyroZ(),        // Gyro Z
        ctl->accelX(),       // Accelerometer X
        ctl->accelY(),       // Accelerometer Y
        ctl->accelZ()        // Accelerometer Z
    );
}





void processGamepad(ControllerPtr ctl) {
    // There are different ways to query whether a button is pressed.
    // By query each button individually:
    //  a(), b(), x(), y(), l1(), etc...
    if (ctl->a()) {
        static int colorIdx = 0;
        // Some gamepads like DS4 and DualSense support changing the color LED.
        // It is possible to change it by calling:
        switch (colorIdx % 3) {
            case 0:
                // Red
                ctl->setColorLED(255, 0, 0);
                break;
            case 1:
                // Green
                ctl->setColorLED(0, 255, 0);
                break;
            case 2:
                // Blue
                ctl->setColorLED(0, 0, 255);
                break;
        }
        colorIdx++;
    }

    if (ctl->b()) {
        // Turn on the 4 LED. Each bit represents one LED.
        static int led = 0;
        led++;
        // Some gamepads like the DS3, DualSense, Nintendo Wii, Nintendo Switch
        // support changing the "Player LEDs": those 4 LEDs that usually indicate
        // the "gamepad seat".
        // It is possible to change them by calling:
        ctl->setPlayerLEDs(led & 0x0f);
    }

    if (ctl->x()) {
        // Some gamepads like DS3, DS4, DualSense, Switch, Xbox One S, Stadia support rumble.
        // It is possible to set it by calling:
        // Some controllers have two motors: "strong motor", "weak motor".
        // It is possible to control them independently.
        ctl->playDualRumble(0 /* delayedStartMs */, 250 /* durationMs */, 0x80 /* weakMagnitude */,
                            0x40 /* strongMagnitude */);
    }

    // Another way to query controller data is by getting the buttons() function.
    // See how the different "dump*" functions dump the Controller info.
    dumpGamepad(ctl);
}


void driveHandler(ControllerPtr ctl) {
    float motor_R_Speed = 0;
    float motor_L_Speed = 0;
    float robot_Speed = 0;
    float tempR = 0;
    float tempL = 0;
    float tempGes = 0;
    int x = ctl->axisX();
    int y = -ctl->axisY();      // Stick nach oben gibt negativen y-wert

    if((x > -80) && (x < 80)) x = 0;     //stickdrift bereinigung
    if((y > -40) && (y < 40)) y = 0;

    //skalieren
    float fx = float(x) / 500;
    float fy = float(y) / 500;

    if(fx > 1.0f) fx = 1.0f;
    if(fy > 1.0f) fy = 1.0f;

    motor_R_Speed = fy - fx /*  * abs(fy)   */;
    motor_L_Speed = fy + fx;

    float maxMag = max(abs(motor_L_Speed), abs(motor_R_Speed));
    if (maxMag > 1.0f) {
        motor_R_Speed /= maxMag;
        motor_L_Speed /= maxMag;
    }

/*
    robot_Speed = sqrt(y*y + x*x);
    if(robot_Speed > 500.0) robot_Speed = 500.0;
    
    
    motor_R_Direction = y - x;
    motor_L_Direction = y + x;

    tempR = y - x;
    tempL = y + x;
    tempGes = abs(tempR) + abs(tempL);
    
    motor_R_Speed = (abs(tempR)/ tempGes) * robot_Speed;
    motor_L_Speed = (abs(tempL)/ tempGes) * robot_Speed;
*/   

    Serial.printf("Motor L: %.4f | Motor R: %.4f\n", motor_L_Speed, motor_R_Speed);

    ledcWrite(MotorCH_R, ((abs(motor_R_Speed) - 0.3) * 255));
    ledcWrite(MotorCH_L, ((abs(motor_L_Speed) - 0.3) * 255));

    //Serial.printf("Berechnet: Speed: %.4f Motor L: %.4f | Motor R: %.4f\n", robot_Speed, ((motor_L_Speed * 255) / 500), ((motor_R_Speed * 255) / 500));

    if(motor_R_Speed == 0) {
        
        digitalWrite(motorR_IN1, LOW);
        digitalWrite(motorR_IN2, LOW);

    } else if(motor_R_Speed > 0) {
        
        digitalWrite(motorR_IN1, HIGH);
        digitalWrite(motorR_IN2, LOW);

    } else if(motor_R_Speed < 0) {

        digitalWrite(motorR_IN1, LOW);
        digitalWrite(motorR_IN2, HIGH);
    }

    if(motor_L_Speed == 0) {
        
        digitalWrite(motorL_IN3, LOW);
        digitalWrite(motorL_IN4, LOW);

    } else if(motor_L_Speed > 0) {
        
        digitalWrite(motorL_IN3, HIGH);
        digitalWrite(motorL_IN4, LOW);

    } else if(motor_L_Speed < 0) {

        digitalWrite(motorL_IN3, LOW);
        digitalWrite(motorL_IN4, HIGH);
    }

}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
                driveHandler(myController);
            } else {
                Serial.println("Unsupported controller");
                RGB_AUS();
                digitalWrite(RGB_RED, LOW);
            }
        }
    }
}



// Arduino setup function. Runs in CPU 1
void setup() {
    Serial.begin(115200);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // "forgetBluetoothKeys()" should be called when the user performs
    // a "device factory reset", or similar.
    // Calling "forgetBluetoothKeys" in setup() just as an example.
    // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
    // But it might also fix some connection / re-connection issues.
    BP32.forgetBluetoothKeys();

    // Enables mouse / touchpad support for gamepads that support them.
    // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
    // - First one: the gamepad
    // - Second one, which is a "virtual device", is a mouse.
    // By default, it is disabled.
    BP32.enableVirtualDevice(false);

    // setup der Motorausgänge
    pinMode(motorR_ENA, OUTPUT);
    pinMode(motorR_IN1, OUTPUT);
    pinMode(motorR_IN2, OUTPUT);
    pinMode(motorL_IN3, OUTPUT);
    pinMode(motorL_IN4, OUTPUT);
    pinMode(motorL_ENB, OUTPUT);

    digitalWrite(motorR_IN1, LOW);
    digitalWrite(motorR_IN2, LOW);
    digitalWrite(motorL_IN3, LOW);
    digitalWrite(motorL_IN4, LOW);

    // PWM setup
    ledcSetup(MotorCH_R, 20000, 8);
    ledcAttachPin(motorR_ENA, MotorCH_R);

    ledcSetup(MotorCH_L, 20000, 8);
    ledcAttachPin(motorL_ENB, MotorCH_L);

    ledcWrite(MotorCH_R, 0);
    ledcWrite(MotorCH_L, 0);

    // setup des IR-LED-Ausgangs
    pinMode(IR_LED, OUTPUT);
    
    ledcSetup(LEDCH, 38000, 8);
    ledcAttachPin(IR_LED, LEDCH);
    ledcWrite(LEDCH, 127);
    
    // setup RGB_LED
    pinMode(RGB_RED, OUTPUT);
    pinMode(RGB_BLUE, OUTPUT);
    pinMode(RGB_GREEN, OUTPUT);

    //RGB_LED ist Low-Aktiv
    digitalWrite(RGB_RED, HIGH);
    digitalWrite(RGB_BLUE, HIGH);
    digitalWrite(RGB_GREEN, HIGH);



    //Während noch kein Controller Connected ist: LED leuchtet Blau
    digitalWrite(RGB_BLUE, LOW);    
}

// Arduino loop function. Runs in CPU 1.
void loop() {
    // This call fetches all the controllers' data.
    // Call this function in your main loop.
    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers();
        

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    //     vTaskDelay(1);
    delay(10);
    //Für Debug sollte man den Delay höher stellen, ca. 150 funktioniert gut
}
