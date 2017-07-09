#include <Arduino.h>

#include <RF24.h>
#include <RF24Mesh.h>
#include <RF24Network.h>

#include "Command.hpp"
#include "Color.hpp"


// TODO: Prověřit, jestli se může volat mesh.update() z interruptu

// IDEA: Při přerušení napájení bude zapojen záložní kond,
//       atmega pozná že kleslo napětí, rychle uloží data
//       do EEPROM a má vystaráno :)


class KeepCube {
    private:
        RF24 &radio;
        RF24Network &network;
        RF24Mesh &mesh;

        Color pin = {9, 5, 6}; // RGB led pins (LED strip)

        long loops = 1000;

        // Color transitions
        int calculateStep(int start, int end) {
            int step = end - start;
            if (step) {
                step = loops / step;
            }
            return step;
        }

        int calculateVal(int step, int val, int i) {
            if ((step) && i % step == 0) {
                if (step > 0) {
                    val += 1;
                } else if (step < 0) {
                    val -= 1;
                }
            }

            if (val > 255) {
                val = 255;
            } else if (val < 0) {
                val = 0;
            }

            return val;
        }


        bool LEDloopExit = false;
        int LEDloopState = -1;
        Command LEDkeyframes;


    public:

        KeepCube(RF24 &radioo, RF24Network &networkk, RF24Mesh &meshh) :
            radio(radioo),
            network(networkk),
            mesh(meshh) {}

        virtual ~KeepCube () {}


        // Constants
        static const uint8_t baseNodeID = 0x00;
        // static const uint16_t newNodeID = 0xff;


        void setLEDpins(uint8_t r, uint8_t g, uint8_t b) {
            pin.r = r;
            pin.g = g;
            pin.b = b;
        }


        void init() {
            // TODO: Config cfg = loadConfigFromEEPROM();

            // Virtual GND
            pinMode(4, OUTPUT);
            digitalWrite(4, LOW);

            pinMode(pin.r, OUTPUT);
            pinMode(pin.g, OUTPUT);
            pinMode(pin.b, OUTPUT);

            pinMode(18, OUTPUT);
            pinMode(19, OUTPUT);
            digitalWrite(18, LOW);
            status.on();

            mesh.setNodeID(1); // TODO: tahat z konfigurace

            Serial.println(F("[INFO] Connecting to the mesh... "));
            mesh.begin();
            Serial.print(F("[DUMP] Mesh node ID: "));
            Serial.println(mesh.getNodeID());
            radio.setPALevel(RF24_PA_HIGH); // hmm možná bude lepší to dát nazačátek, před setNodeID()
            Serial.println(F("[ OK ] Connected"));

            Serial.println(F("--- KClib initialized ---"));

            status.off();
        }


        void interruptHandler() {
            status.on();
            mesh.update();

            static int count;
            if (network.available()) {
                Serial.print(F("[INT0] Interrupted, stopping loops! count: "));
                LEDloopExit = true;
            }
            else {
                Serial.print(F("[INT0] Interrupted, but no message! count: "));
            }
            Serial.println(count);
            count++;
            status.off();
        }


        void update() {
            delay(1);
            mesh.update();
            updateLED();

            // static long displayTimer;
            //
            // // Check mesh connection every 1 sec
            // if (millis() - displayTimer > 1000) {
            //     displayTimer = millis();
            //     Serial.println(F("[MESH] Checking connection..."));
            //     status.on();
            //     if (!mesh.checkConnection()) {
            //         Serial.println(F("[MESH] Renewing address..."));
            //         mesh.renewAddress();
            //     }
            //     else status.off();
            // }

        }


        int messageAvailable() {
            return network.available();
        }


        String receiveMessage() {
            // TODO: nastavit velikost pole automaticky podle zbývající RAMky (nebo vymyslet lepší metodu),
            //       udělat req-res command na zjištění max. délky zprávy (zbývající RAMky (i s rezervou))

            int size = 300;
            char a[size];
            // char *a = new char[size];

            RF24NetworkHeader header;
            network.read(header, &a, sizeof a);

            // Command cmd(a);
            // return cmd;

            return String(a);
        }


        void setLEDpattern(String params) {
            LEDkeyframes.params.set(params);
            LEDloopState = LEDkeyframes.params.get(0).toInt();
        }


        void updateLED() {
            if (LEDloopState == -1) return;

            // int loop = LEDkeyframes.params.get(0).toInt();
            int keyframes = LEDkeyframes.params.get(1).toInt();

            int times[keyframes];
            Color colors[keyframes];

            for (auto i = 2; i < keyframes * 2 + 2; i++) {
                if (i % 2 == 0) colors[i / 2 - 1].fromCompound(LEDkeyframes.params.get(i));  // Even
                else times[i / 2 - 1] = LEDkeyframes.params.get(i).toInt();  // Odd
            }

            for (auto i = 0; i < keyframes && !LEDloopExit; i++)
                led(colors[i], times[i]);

            if (LEDloopExit) LEDloopExit = !LEDloopExit;
            if (LEDloopState == 0) LEDloopState = -1;
        }


        void led(uint8_t r, uint8_t g, uint8_t b, int time) {
            led({r, g, b}, time);
        }

        void led(Color color, int time) {
            if (time == 0) {
                analogWrite(pin.r, color.r);
                analogWrite(pin.g, color.g);
                analogWrite(pin.b, color.b);
                return;

            } else if (time < 160) {
                time = 1;
            } else {
                // sem prijde neco na upravu toho delaye, zatim se na to sere
                // time /= 1.075; // bere se v potaz pomalost atmegy
                // time -= 155;
            }

            struct step_t {
                int r;
                int g;
                int b;
            } step;

            static Color val;
            static Color prev;

            step.r = calculateStep(prev.r, color.r);
            step.g = calculateStep(prev.g, color.g);
            step.b = calculateStep(prev.b, color.b);

            for (int i = 0; i <= loops && !LEDloopExit; i++) {
                val.r = calculateVal(step.r, val.r, i);
                val.g = calculateVal(step.g, val.g, i);
                val.b = calculateVal(step.b, val.b, i);

                analogWrite(pin.r, val.r);
                analogWrite(pin.g, val.g);
                analogWrite(pin.b, val.b);

                delayMicroseconds(time);
            }

            prev.r = val.r;
            prev.g = val.g;
            prev.b = val.b;
        }


        struct status {
            void on() {
                digitalWrite(19, HIGH);
            }
            void off() {
                digitalWrite(19, LOW);
            }
        } status;


        String deltaRAM() {
            extern int __bss_end;
            extern void *__brkval;
            int freeValue;
            static int lastram;
            if ((int)__brkval == 0)
                freeValue = ((int)&freeValue) - ((int)&__bss_end);
            else
                freeValue = ((int)&freeValue) - ((int)__brkval);

            int x = freeValue - lastram;
            if (lastram == 0) {
                lastram = freeValue;
                return "RAM deltax: 0";
            }
            lastram = freeValue;


            return "RAM deltax: " + String(-x);
        }

        String totalRAM() {
            extern int __bss_end;
            extern void *__brkval;
            int freeValue;
            if ((int)__brkval == 0)
                freeValue = ((int)&freeValue) - ((int)&__bss_end);
            else
                freeValue = ((int)&freeValue) - ((int)__brkval);

            return "RAM totalx: " + String(freeValue);
        }

};


// Color black   = {0x00, 0x00, 0x00};
// Color white   = {0xff, 0xff, 0xff};
// Color red     = {0xff, 0x00, 0x00};
// Color green   = {0x00, 0xff, 0x00};
// Color blue    = {0x00, 0x00, 0xff};
// Color yellow  = {0x80, 0x80, 0x00};
// Color cyan    = {0x00, 0x80, 0x80};
// Color magenta = {0x80, 0x00, 0x80};


// TODO: Naplácat tam toho co nejvíc, pak to půjde nastavit na dálku
struct LEDboardConfig {
    bool firstRun;
    int restartCause;
    int whatToDo;
    int whoami; // Jestli base, led board nebo něco dalšího
    long EEPROMWrites;
    unsigned int meshAddress;
    Color ledPins;
    int ledStripType;
    int capSensitivity; // ten typ...

    // TODO: Data o poslední animaci

};


// WTF, to to tu dělá???
int easeOutExpo(int a) {
    return 1 == a ? 1 : 1 - pow(2, -10 * a);
}
