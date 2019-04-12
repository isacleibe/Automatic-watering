/*
 *
 *  Arduino project : Automatic Plant Watering
 *  Copyright : Fran Alasan, 2019.
 *  GitHub : isacleibe
 * 
 */

#define MOISTURE_PIN A0
#define MOISTURE_PERMS 1500

#define MONITOR_PERMS 1500

#define WATERING_PIN 4
#define WATERING_PERMS 3000
#define WATERING_LEVEL 1000
#define WATERING_DURATION 12000
//===============================MyFunctions

//===============================analogInput

struct analogInput {
    byte pin;
    int state;
    int state_b;
    unsigned long lastms;
    unsigned long perms;
};
void setup_analogInput(struct analogInput& d, byte pin, unsigned long perms, unsigned long currms) {
    d.pin = pin;
    pinMode(d.pin, INPUT);
    d.state = 0;
    d.state_b = d.state;
    d.perms = perms;
    d.lastms = currms - d.perms;
}
void loop_analogInput(struct analogInput& d, unsigned long currms) {
    if (currms - d.lastms >= d.perms)
    {
        d.lastms += d.perms;
        d.state_b = d.state;
        d.state = analogRead(d.pin);
    }    
}
//===============================digitalOut

struct digitalOut {
    byte pin;
    byte state;
    byte state_b;
    unsigned long perms;
    unsigned long lastms;
    unsigned long flagms;
};
void setup_digitalOut(struct digitalOut& d, byte pin, unsigned long perms, unsigned long currms) {
    d.pin = pin;
    pinMode(d.pin, OUTPUT);
    d.state = LOW;    
    d.state_b = d.state;
    d.perms = perms;
    d.lastms = currms - d.perms;
    d.flagms = currms;
}
void loop_digitalOut(struct digitalOut& d, struct analogInput& a, unsigned long currms) {
    if (currms - d.lastms >= d.perms)
    {
        d.lastms += d.perms;
        d.state_b = d.state;
        
        if(a.state > WATERING_LEVEL) {
            d.state = HIGH;
        }else
        {
            if(currms - d.flagms >= WATERING_DURATION) {
                d.state = LOW;
            }
        }

        if(d.state != d.state_b) {
            digitalWrite(d.pin,d.state);
            d.flagms = currms;
        }
    }    
}

//===============================monitor

struct monitor {
    int state;
    int state_b;
    unsigned long lastms;
    unsigned long perms;
};
void setup_monitor(struct monitor& d, unsigned long perms, unsigned long currms) {
    Serial.begin(9600);
    d.state = 0;
    d.state_b = d.state;
    d.perms = perms;
    d.lastms = currms - d.perms;
}
void loop_monitor(struct monitor& d, struct analogInput& a, unsigned long currms) {
    if (currms - d.lastms >= d.perms)
    {
        d.lastms += d.perms;
        d.state_b = d.state;
        
        d.state = a.state;
        Serial.println(d.state);
    }    
}
//===============================main

struct analogInput moistureSensor;
struct digitalOut pomp;
struct monitor m;

void setup() {    
    unsigned long currms = millis();
    setup_analogInput(moistureSensor, MOISTURE_PIN, MOISTURE_PERMS, currms);
    setup_digitalOut(pomp, WATERING_PIN, WATERING_PERMS, currms);
    setup_monitor(m, MONITOR_PERMS, currms);
}

void loop() {
    unsigned long currms = millis();
    loop_analogInput(moistureSensor, currms);    
    loop_digitalOut(pomp, moistureSensor, currms);
    loop_monitor(m, moistureSensor, currms);
}