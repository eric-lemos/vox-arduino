//==================== INCLUDE ====================|
#include <EEPROM.h>

//==================== CONST ======================|
//-------------------- AUDIO ----------------------|
const uint8_t AUDIO_1 = A6;
const uint8_t AUDIO_2 = A7;
const uint8_t RELAY_1 = 2;
const uint8_t RELAY_2 = 3;

//-------------------- LEDS -----------------------|
const uint8_t LED_SEL_VOX_1 = A3;
const uint8_t LED_SEL_VOX_2 = A1;
const uint8_t LED_RELAY_1 = A2;
const uint8_t LED_RELAY_2 = A0;
const uint8_t LED_SYSTEM = 9;

//-------------------- ROTARY ---------------------|
const uint8_t SWITCH = 6;
const uint8_t ROT_A = 7;
const uint8_t ROT_B = 8;

//-------------------- CONFIG ---------------------|
const int PEAK_TIME = 40;
const int SWITCH_TIME = 300;
const int EPROM_TIME = 10000;
const int BLINK_NORMAL = 500;
const int BLINK_ROTARY = 100;
const int DEFAULT_RELEASE = 2000;
const int DEFAULT_THRESHOLD = 540;
const int ROTARY_BLINK_TIME = 1000;

//==================== STRUCT =====================|
struct Vox {
  int analog=0, peak=0, new_peak=0, eprom_addr, threshold=0;
  unsigned long release_time=0, last_peak=0;
  bool switch_state, eprom_state=false;
};


//==================== VARS =======================|
int release_interval=0, blink_time=0, vox_selected=1, release_addr;
unsigned long last_blink=0, last_rotary=0, last_switch=0, last_eprom=0;
bool blink_state, rotary_last_state, new_rotary;
String command;
Vox vox1, vox2;

//==================== FUNCS ======================|
//-------------------- ROTARY ---------------------|
void Switch(unsigned long now) {
  int switch_data = digitalRead(SWITCH);

  if(last_switch < now) {
    if(switch_data == false) {
      if(vox1.switch_state == HIGH) {
        vox1.switch_state = !vox1.switch_state;
        vox2.switch_state = !vox2.switch_state;
        vox_selected = 2;

        // Serial.print("VOX 2 selected => ");
        // Serial.println(vox2.threshold);

      } else if(vox2.switch_state == HIGH) {
        vox1.switch_state = !vox1.switch_state;
        vox2.switch_state = !vox2.switch_state;
        vox_selected = 1;

        // Serial.print("VOX 1 selected => ");
        // Serial.println(vox1.threshold);
      }
    }

    digitalWrite(LED_SEL_VOX_1, vox1.switch_state);
    digitalWrite(LED_SEL_VOX_2, vox2.switch_state);
    last_switch = now+SWITCH_TIME;
  }

}

void Rotary(unsigned long now) {
  if(rotary_last_state == true && new_rotary == false) {

    if(digitalRead(ROT_B) == false) {
      if(vox_selected == 1) vox1.threshold++;
      else vox2.threshold++;

    } else {
      if(vox_selected == 1) vox1.threshold--;
      else vox2.threshold--;

    }

    if(vox_selected == 1) {
      vox1.eprom_state = true;
      // Serial.print("VOX 1 => threshold ");
      // Serial.println(vox1.threshold);
    } else {
      vox2.eprom_state = true;
      // Serial.print("VOX 2 => threshold ");
      // Serial.println(vox2.threshold);
    }

    last_rotary = now+ROTARY_BLINK_TIME;
    last_eprom  = now+EPROM_TIME;
  }

  rotary_last_state = new_rotary;
}

//-------------------- EPROM ----------------------|
void Eprom(unsigned long now) {
  if(vox_selected == 1) {
    if(vox1.eprom_state == true && last_eprom < now) {
      EEPROM.put(vox1.eprom_addr, vox1.threshold);
      vox1.eprom_state = false;

      // Serial.print("VOX 1 => threshold ");
      // Serial.print(vox1.threshold);
      // Serial.println(" has been stored in EEPROM.");
    }

  } else {
    if(vox2.eprom_state == true && last_eprom < now) {
      EEPROM.put(vox2.eprom_addr, vox2.threshold);
      vox2.eprom_state = false;

      // Serial.print("VOX 2 => threshold ");
      // Serial.print(vox2.threshold);
      // Serial.println(" has been stored in EEPROM.");
    }

  }
}

//-------------------- BLINK ----------------------|
void Blink(unsigned long now) {
  if(last_blink < now) {
    digitalWrite(LED_SYSTEM, blink_state);
    blink_state = !blink_state;

    if(last_rotary > now) 
      last_blink = now+BLINK_ROTARY;
    else last_blink = now+BLINK_NORMAL;
  }
}

//-------------------- PEAK1 ----------------------|
void Peak1(unsigned long now) {
  if(vox1.analog > vox1.new_peak)
    vox1.new_peak = vox1.analog;
  
  if(vox1.analog > vox1.peak) {
    vox1.last_peak = now+PEAK_TIME;
    vox1.peak = vox1.analog;
    vox1.new_peak = 0;

  } else if(vox1.last_peak < now) {
    vox1.last_peak = now+PEAK_TIME;
    vox1.peak = vox1.new_peak;
    vox1.new_peak = 0;
  }
}

//-------------------- PEAK2 ----------------------|
void Peak2(unsigned long now) {
  if(vox2.analog > vox2.new_peak)
    vox2.new_peak = vox2.analog;
  
  if(vox2.analog > vox2.peak) {
    vox2.last_peak = now+PEAK_TIME;
    vox2.peak = vox2.analog;
    vox2.new_peak = 0;

  } else if(vox2.last_peak < now) {
    vox2.last_peak = now+PEAK_TIME;
    vox2.peak = vox2.new_peak;
    vox2.new_peak = 0;
  }
}

//-------------------- THRESHOLD1 -----------------|
void Threshold1(unsigned long now) {
  if(vox1.peak > vox1.threshold) {
    digitalWrite(RELAY_1, LOW);
    digitalWrite(LED_RELAY_1, HIGH);
    vox1.release_time = now+release_interval;

  } else if(vox1.release_time < now) {
    digitalWrite(RELAY_1, HIGH);
    digitalWrite(LED_RELAY_1, LOW);
  }
}

//-------------------- THRESHOLD2 -----------------|
void Threshold2(unsigned long now) {
  if(vox2.peak > vox2.threshold) {
    digitalWrite(RELAY_2, LOW);
    digitalWrite(LED_RELAY_2, HIGH);
    vox2.release_time = now+release_interval;

  } else if(vox2.release_time < now) {
    digitalWrite(RELAY_2, HIGH);
    digitalWrite(LED_RELAY_2, LOW);
  }
}

//==================== SETUP ======================|
void setup() {
  Serial.begin(115200);

  //------------------ VOX1 -----------------------|
  vox1.eprom_addr = 0;
  vox1.switch_state = HIGH;

  pinMode(AUDIO_1, INPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(LED_RELAY_1, OUTPUT);
  pinMode(LED_SEL_VOX_1, OUTPUT);
  digitalWrite(LED_SEL_VOX_1, vox1.switch_state);

  EEPROM.get(vox1.eprom_addr, vox1.threshold);
  if(vox1.threshold == 0) {
    EEPROM.put(vox1.eprom_addr, DEFAULT_THRESHOLD);
    EEPROM.get(vox1.eprom_addr, vox1.threshold);  
  }

  //------------------ VOX2 -----------------------|
  vox2.eprom_addr = 350;
  vox2.switch_state = LOW;

  pinMode(AUDIO_2, INPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(LED_RELAY_2, OUTPUT);
  pinMode(LED_SEL_VOX_2, OUTPUT);
  digitalWrite(LED_SEL_VOX_2, vox2.switch_state);

  EEPROM.get(vox2.eprom_addr, vox2.threshold);
  if(vox2.threshold == 0) {
    EEPROM.put(vox2.eprom_addr, DEFAULT_THRESHOLD);
    EEPROM.get(vox2.eprom_addr, vox2.threshold);
  }

  //------------------ SYSTEM ---------------------|
  pinMode(LED_SYSTEM, OUTPUT);
  release_addr = 700;
  blink_time = 500;
  
  EEPROM.get(release_addr, release_interval);
  if(release_interval == 0) {
    EEPROM.put(release_addr, DEFAULT_RELEASE);
    EEPROM.get(release_addr, release_interval);
  }

  //------------------ LOADED ---------------------|
  Serial.println("(? for help)");
}

//==================== LOOP =======================|
void loop() {
  unsigned long now = millis();
  new_rotary = digitalRead(ROT_A);
  vox1.analog = analogRead(AUDIO_1);
  vox2.analog = analogRead(AUDIO_2);
  
  Switch(now);
  Rotary(now);
  Eprom(now);
  Blink(now);
  Peak1(now);
  Peak2(now);
  Threshold1(now);
  Threshold2(now);

  // Serial.print(vox1.analog);
  // Serial.print(",");
  // Serial.print(vox1.threshold);
  // Serial.print(",");
  // Serial.println(vox1.peak);

  // Serial.print(vox2.analog);
  // Serial.print(",");
  // Serial.print(vox2.threshold);
  // Serial.print(",");
  // Serial.println(vox2.peak);
}

//==================== SERIAL =====================|
int DbuToArduino(int x) {
  return ((13*pow(x,3))/6000)+((47*pow(x,2))/200)+(131*x/15)+631; 
} 

void serialEvent() {
  if(Serial.available()) {
    command = Serial.readStringUntil('\n');
    command.trim();

    //------------------ HELP -----------------------|
    if(command.equals("?")) {
      Serial.println("\n==================================================");
      Serial.println(" => Press ENTER to get vox info.");
      Serial.println("--------------------------------------------------");
      Serial.println(" => Changes the release time interval.");
      Serial.println(" => Syntax: r [interval in milliseconds]");
      Serial.println("--------------------------------------------------");
      Serial.println(" => Change the threshold values ​​for the VOX.");
      Serial.println(" => Syntax: t[vox number] [threshold value]");
      Serial.println("==================================================\n");

    //------------------ ENTER ----------------------|    
    } else if(command.equals("")) {
      Serial.print("VOX #1 threshold => ");
      Serial.println(vox1.threshold);
      
      Serial.print("VOX #2 threshold => ");
      Serial.println(vox2.threshold);
      
      Serial.print("Release interval => ");
      Serial.print(release_interval);
      Serial.println("ms");

      Serial.println("(? for help)\n");

    //------------------ RELEASE --------------------|
    } else if(command.substring(0, 1).equals("r")) {
      int serial_interval = command.substring(1).toInt();

      if(serial_interval) {
        int old_release_value = release_interval;
        EEPROM.put(release_addr, serial_interval);
        EEPROM.get(release_addr, release_interval);
      
        Serial.print("Release interval (");
        Serial.print(old_release_value);
        Serial.print("ms) changed => ");
        Serial.print(release_interval);
        Serial.println("ms");
      
      }  else {
        Serial.print("The value(");
        Serial.print(serial_interval);
        Serial.println(") entered to change interval is invalid!");   
      }

    //------------------ THRESHOLD ------------------|
    } else if(command.substring(0, 1).equals("t")) {
      int serial_vox_selected = command.substring(1,2).toInt();
      int serial_threshold = command.substring(3).toInt();

      //------------------ VOX1 ---------------------|
      if(serial_vox_selected == 1) {
        vox1.threshold = DbuToArduino(serial_threshold);
        EEPROM.put(vox1.eprom_addr, vox1.threshold);
        EEPROM.get(vox1.eprom_addr, vox1.threshold);

        Serial.print("VOX #1 threshold changed => ");
        Serial.print(serial_threshold);
        Serial.print("dBu (");
        Serial.print(vox1.threshold);
        Serial.println(")");

      //------------------ VOX2 ---------------------|
      } else if(serial_vox_selected == 2) {
        vox2.threshold = DbuToArduino(serial_threshold);
        EEPROM.put(vox2.eprom_addr, vox2.threshold);
        EEPROM.get(vox2.eprom_addr, vox2.threshold);

        Serial.print("VOX #2 threshold changed => ");
        Serial.print(serial_threshold);
        Serial.print("dBu (");
        Serial.print(vox2.threshold);
        Serial.println(")");

      } else {
        Serial.print("The value(");
        Serial.print(command);
        Serial.println(") entered to change interval is invalid!"); 
      }

    } else {
      Serial.print("The '");
      Serial.print(command);
      Serial.println("' command is invalid!");
    } 
  }
}
