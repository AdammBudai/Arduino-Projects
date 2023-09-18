#include "funshield.h"


unsigned long previous_millis;
unsigned long current_millis;
unsigned long animation_interval = 100;
int check = 0;


int previous_throw;
constexpr int display_length = 4;
bool first_press = false;

int values[display_length-1]= {100,10,1};

constexpr int d = 0b10100001;
int first_num_pos;

constexpr int buttons_length = 3;
int buttons[buttons_length] = {button1_pin, button2_pin,button3_pin};

int gathered_number;
int counter = 0;
int index = 3;
int pos = 3;

constexpr int throw_border = -1;
constexpr int config_border = 1;
constexpr int max_displayed = 10000;

int throws = 1; 
constexpr int maxthrow = 9; 

int visible_dice[display_length] = {0,0,0,0};
int gathered[display_length] = {0,0,0,0};
int visible_throws[1] = {throws};

int animation_counter = 0;
int ani_num = 1234;
int animation_array[display_length] = {0,0,0,0};

int dice_pointer = 0; 
int max_dice = 100; 
constexpr int dice_types = 7;
int dices[dice_types] = {4,6,8,10,12,20,100};

struct Button {
  bool state;
  int pin;
  void (*action)();
  void (*released_action)();
  unsigned long pressed_time;
};

Button buttons_structures[buttons_length];

enum states {
  normal_mode = 0,
  configuration_mode = 1, 
  animation_mode = 2,
  }current_state ;


int answer(unsigned long seed,uint64_t max)
{
  unsigned long multiplier = 134775813;

  uint64_t  RandomNumber = (((seed + previous_throw)* multiplier + 1) * max) >> 32;
  previous_throw = RandomNumber;
  return static_cast<int>(RandomNumber + 1);
    
}

void data_gathering()
{
  if(current_state != normal_mode)
  {
    current_state = normal_mode;
  }

  int sum = 0;
  unsigned long seed = current_millis - buttons_structures[0].pressed_time;
  for (int i = 0; i < throws; i++)
   {
    int throwResult = answer(seed, dices[dice_pointer]);
    sum += throwResult;
  }
  gathered_number = sum;
}


void increment_throws() {
   if(current_state != configuration_mode)
  {
    current_state = configuration_mode;
  }
  else
  {
    throws += 1;
  if(throws > maxthrow)
  {
    throws = 1;
  }
  }
}


void dice_modifier()
 {
   if(current_state != configuration_mode)
  {
    current_state = configuration_mode;
  }
  else
  {
    dice_pointer = (dice_pointer + 1) % dice_types;
  }
}


void update(int number, int border, int array[]){
  int temp = number;

  for(int i = display_length-1; i > border; i--)
  {
    array[i] = temp % 10;
    if(temp % 10 != 0 && first_num_pos > i)
    {
      first_num_pos = i;
    }
    temp = temp/10;
  }
}

void change_animation()
{
  int temp = random(1,  max_displayed + 1);
  ani_num = (ani_num + temp) % max_displayed;
}


void display_number(int position, int array[]) {
  digitalWrite(latch_pin, LOW);
  if(array == visible_dice)
  {
    if (position == 1) {
      shiftOut(data_pin, clock_pin, MSBFIRST, d);
      shiftOut(data_pin, clock_pin, MSBFIRST, 1 << position);
    } else {
      shiftOut(data_pin, clock_pin, MSBFIRST, digits[array[position]]);
      shiftOut(data_pin, clock_pin, MSBFIRST, digit_muxpos[position]);
    }
  }
  else
  {
    if(position >= first_num_pos)
    {
      shiftOut(data_pin, clock_pin, MSBFIRST, digits[array[position]]);
      shiftOut(data_pin, clock_pin, MSBFIRST, digit_muxpos[position]);
    }
  }
  
  digitalWrite(latch_pin, HIGH);
}

void nothing()
{}

void animation()
{
  current_state = animation_mode; 
}

void detect_IfPressed_button(int button_pos)
{
  bool button_state = digitalRead(buttons_structures[button_pos].pin);
     
      if(button_state == ON && buttons_structures[button_pos].state == OFF)
      {
        buttons_structures[button_pos].action();
        check = 1;
        buttons_structures[button_pos].pressed_time = current_millis;
      }
      else if(button_state == OFF && buttons_structures[button_pos].state == ON && check != 0)
      {
        buttons_structures[button_pos].released_action();
      }
      buttons_structures[button_pos].state = button_state;
}


void update_position()
{
  pos = ( pos + display_length-1) % display_length;
}


void handleConfigurationMode() {
    update(dices[dice_pointer], config_border, visible_dice);
    display_number(pos, visible_dice);
}

void handleNormalMode() {
    update(gathered_number, throw_border, gathered);
    display_number(pos, gathered);
}

void handleAnimationMode() {
    if (current_millis - previous_millis > animation_interval) {
        previous_millis = current_millis;
        change_animation();
    }
    update(ani_num, throw_border, animation_array);
    display_number(pos, animation_array);
}



void setup() {

  pinMode(clock_pin, OUTPUT);
  pinMode(latch_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  for(int i = 0; i < buttons_length; i++)
  {
    buttons_structures[i].pin = buttons[i];
    pinMode(buttons_structures[i].pin, INPUT);
  }
   buttons_structures[0].action = animation;
   buttons_structures[0].released_action = data_gathering;

   buttons_structures[1].action = increment_throws;
   buttons_structures[1].released_action = nothing;

   buttons_structures[2].action = dice_modifier;
   buttons_structures[2].released_action = nothing;

   current_state = configuration_mode;
   update(dices[dice_pointer], config_border, visible_dice);
   display_number(pos, visible_dice);
}


void loop() {
  current_millis = millis();
  first_num_pos = 5;
  visible_dice[0] = throws;
  previous_throw = 0;
  
  for(int i = 0; i < buttons_length; i++)
    {
      detect_IfPressed_button(i);
    }
    
if (current_state == configuration_mode) {
    handleConfigurationMode();
} else if (current_state == normal_mode) {
    handleNormalMode();
} else if (current_state == animation_mode) {
    handleAnimationMode();
}

  update_position();
}