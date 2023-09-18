#include "funshield.h"

constexpr int display_length = 4;
int my_number[display_length] = {0,0,0,0};

enum states {
  stopped = 0,
  running = 1, 
  lapped = 2
  }current_state ;




constexpr int dot_position = 2;

constexpr int wait_time = 100;

unsigned long first_millis;
unsigned long previous_millis;
unsigned long current_millis;

constexpr int mod = 100;
long previous; 
unsigned long counter = 0;
int index = 3;
int pos = 3;

constexpr int buttons_length = 3;
int buttons[buttons_length] = {button1_pin, button2_pin, button3_pin};

struct Button {
  bool state;
  int pin;
  void (*action)();
};

Button buttons_structures[buttons_length];


void update(unsigned long number){
  number /= mod;
  int stop = 0;
  for (int i = display_length - 1; i > -1; i--) 
  {
    my_number[i] = (stop == 0) ? digits[number % 10] : 0xff;
    number = number / 10;
    if (number == 0 && dot_position >= i)
     {
        stop = 1; 
     }
  }
  my_number[dot_position] &= 0x7f;

}


void display_number(int pos) 
{
  shiftOut(data_pin, clock_pin, MSBFIRST, my_number[pos]);
  shiftOut(data_pin, clock_pin, MSBFIRST, digit_muxpos[pos]);
  digitalWrite(latch_pin, LOW);
  digitalWrite(latch_pin, HIGH);
}

void start_stop() {

  if(current_state == running)
  {
    current_state = stopped;
  }
  else if(current_state == stopped){
    current_state = running;
    if (counter == 0) {
      first_millis = current_millis;
    }
  }
  

}
  

void silent_stop() {
  switch(current_state)
  {
    case running:

      current_state = lapped;
      update(counter);
      break;

    case lapped:

      current_state = running;
      update(counter);
      break;

    case stopped:
      break;
  }

  

}
  


void restart()
{

  if(current_state == stopped)
    {
      counter = 0;
      update(counter);
    }
  
}


void setup() {
  current_state = stopped;

  pinMode(clock_pin, OUTPUT);
  pinMode(latch_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  for(int i = 0; i < buttons_length; i++)
  {
    buttons_structures[i].pin = buttons[i];
    pinMode(buttons_structures[i].pin, INPUT);
  }
   buttons_structures[0].action = start_stop;
   buttons_structures[1].action = silent_stop;
   buttons_structures[2].action = restart;


  update(counter);
  
}



void loop() {
  previous_millis = current_millis;
  current_millis = millis();
  
  for(int i = 0; i < buttons_length; i++)
    {
      bool current_state = digitalRead(buttons_structures[i].pin);
      

      if(current_state == ON && buttons_structures[i].state == OFF)
      {
        buttons_structures[i].action();
        
        
      }
      buttons_structures[i].state = current_state;
    }


  

  if(current_state != stopped){  
    counter += current_millis - previous_millis;
    if (counter - first_millis >= wait_time && current_state!= lapped) {
      first_millis += wait_time;
      update(counter);
    }
  }
      
    
    
  display_number(index);
  index = ( index + display_length-1) % display_length;
  

}