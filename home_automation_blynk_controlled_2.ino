#define BLYNK_TEMPLATE_ID "TMPL3PjRQDat9"
#define BLYNK_TEMPLATE_NAME "HOME AUTOMATION"
#define BLYNK_AUTH_TOKEN "ZNLS2Mo2CkLeUJOfvhIOIWAj2d6STz0a"

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Include your custom libraries here
#include "main.h"
#include "temperature_system.h"
#include "ldr.h"
#include "serial_tank.h"

char auth[] = BLYNK_AUTH_TOKEN;

bool heater_sw, inlet_sw, outlet_sw, cooler_sw;
unsigned int tank_volume;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Handle cooler control
BLYNK_WRITE(COOLER) 
{
    cooler_sw = param.asInt();
    if(cooler_sw)
    {
        cooler_control(ON);
        lcd.setCursor(8, 0);
        lcd.print("CLR_ON ");
    }
    else
    {
        cooler_control(OFF);
        lcd.setCursor(8, 0);
        lcd.print("CLR_OFF");
    }
}

// Handle heater control
BLYNK_WRITE(HEATER) 
{
    heater_sw = param.asInt();
    if(heater_sw)
    {
        heater_control(ON);
        lcd.setCursor(8, 0);
        lcd.print("HTR_ON ");
    }
    else
    {
        heater_control(OFF);
        lcd.setCursor(8, 0);
        lcd.print("HTR_OFF");
    }
}

// Handle inlet valve control
BLYNK_WRITE(INLET_V_PIN) 
{
    inlet_sw = param.asInt();
    if(inlet_sw)
    {
        enable_inlet();
        lcd.setCursor(8, 1);
        lcd.print("IN_FL_ON ");
    }
    else
    {
        disable_inlet();
        lcd.setCursor(8, 1);
        lcd.print("IN_FL_OFF");
    }
}

// Handle outlet valve control
BLYNK_WRITE(OUTLET_V_PIN) 
{
    outlet_sw = param.asInt();
    if(outlet_sw)
    {
        enable_outlet();
        lcd.setCursor(8, 1);
        lcd.print("OUT_FL_ON ");
    }
    else
    {
        disable_outlet();
        lcd.setCursor(8, 1);
        lcd.print("OUT_FL_OFF");
    }
}


void update_temperature_reading() 
{
    Blynk.virtualWrite(TEMPERATURE_GAUGE, read_temperature());
}

void handle_temp(void) 
{
    if (read_temperature() > 35.0 && heater_sw) 
    {
        heater_sw = 0;
        heater_control(OFF);
        Blynk.virtualWrite(HEATER, 0);
        lcd.setCursor(8, 0);
        lcd.print("HTR_OFF ");
    }
}

void handle_tank(void) 
{
    if ((tank_volume < 2000) && (inlet_sw == 0)) 
    {
        enable_inlet();
        inlet_sw = 1;
        Blynk.virtualWrite(INLET_V_PIN, 1);
        lcd.setCursor(8, 1);
        lcd.print("IN_FL_ON ");
    
    
    if ((tank_volume >= 3000) && (inlet_sw == 1)) 
    {
        disable_inlet();
        inlet_sw = 0;
        Blynk.virtualWrite(INLET_V_PIN, 0);
        lcd.setCursor(8, 1);
        lcd.print("IN_FL_OFF");
    }
    }
}

void setup(void) 
{
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.home();

    lcd.setCursor(0, 0);
    lcd.print("Home Automation");
    delay(1000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("T=");
    lcd.setCursor(0, 1);
    lcd.print("V=");

    init_ldr();
    init_temperature_system();
    
    Blynk.begin(BLYNK_AUTH_TOKEN);
    timer.setInterval(1000, update_temperature_reading );
}

void loop(void) 
{
    brightness_control();

    String dis = String(read_temperature(), 2);
    lcd.setCursor(2, 0);
    lcd.print(dis);

    tank_volume = volume();
    lcd.setCursor(2, 1);
    lcd.print(tank_volume);

    Blynk.run();
    timer.run();
    handle_temp();
    handle_tank();
}
