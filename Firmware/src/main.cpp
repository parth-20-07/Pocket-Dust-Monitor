/**
 * @file main.cpp
 * @author Parth Patel (parth.pmech@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-12-05
 * @copyright Copyright (c) 2021
 * ! DO NOT CHANGE
 */

//!=============================== BASIC DECLARATION ===============================//
#include <Arduino.h>
#include "Pin_Connection.h"
#include "Variable_Declaration.h"
#include "Sensor_Data_Collection.cpp"
#include <avr/sleep.h>
//!==================================================================================//

//!============================== FUNCTION DEFINITION ==============================//
void boot_screen(void);
void graphic_data_screen(void);
void drawing_battery(void);
void three_point_data_screen(void);
void draw_graph_screen(void);
void plot_graph_bar(void);
void changing_screen_display(void);
void change_screen(void);
void start_device_peripherals(void);
void shutdown_device_peripherals(void);
//!==================================================================================//

//!============================== FUNCTION DECLARATION ==============================//

/**
 * @brief Print the Boot Screen on the device startup
 */
void boot_screen(void)
{
  Serial.println("Printing Boot Screen");
  Serial.println("Drawing LOGO");
  screen.background(BLACK_COLOR);
  screen.setTextSize(2);
  screen.stroke(WHITE_COLOR);
  screen.text("ECOLOGIC", 20, (screen_height / 2) - 10);
  delay(5000);
}

/**
 * @brief Draws the bubble screen for displaying the PM 2.5 Value on the screen
 */
void graphic_data_screen(void)
{
  changing_screen_display();
  change_screen_flag = false;
  Serial.println("Drawing Graphic Screen");
  uint16_t registered_level = 0;
  collect_dust_quality_data();
  screen.background(BLACK_COLOR);
  // Design variable declaration
  double indicator_height = 4;
  double indicator_bar_width = screen_width - 4;
  double green_indicator = (MAX_SAFETY_LIMIT * indicator_bar_width) / MAX_HAZARDOUS_LIMIT;
  double warning_indicator = ((MAX_WARNING_LIMIT - MAX_SAFETY_LIMIT) * indicator_bar_width) / MAX_HAZARDOUS_LIMIT;
  double severe_indicator = ((MAX_UNHEALTHY_LIMIT - MAX_WARNING_LIMIT) * indicator_bar_width) / MAX_HAZARDOUS_LIMIT;
  double danger_indicator = ((MAX_HAZARDOUS_LIMIT - MAX_UNHEALTHY_LIMIT) * indicator_bar_width) / MAX_HAZARDOUS_LIMIT;
  uint8_t indicator_x_starting_pos = 2;
  uint8_t indicator_y_starting_pos = (screen_height / 2) + 32;
  uint8_t green_indicator_height = (uint8_t)green_indicator;
  uint8_t warning_indicator_height = (uint8_t)warning_indicator;
  uint8_t severe_indicator_height = (uint8_t)severe_indicator;
  uint8_t danger_indicator_height = (uint8_t)danger_indicator;
  uint8_t seperation_line_height = 25;

  // Drawing seperation line
  screen.stroke(GREY_COLOR);
  screen.line(0, seperation_line_height, (screen_width / 2) - 30, seperation_line_height);
  screen.setTextSize(1);
  screen.text("PM2.5", (screen_width / 2) - 10, seperation_line_height - 3);
  screen.line((screen_width / 2) + 30, seperation_line_height, screen_width, seperation_line_height);

  // Drawing the indicator Bar
  screen.stroke(GREEN_COLOR);
  screen.fill(GREEN_COLOR);
  screen.rect(indicator_x_starting_pos, indicator_y_starting_pos, green_indicator_height, indicator_height);
  screen.stroke(YELLOW_COLOR);
  screen.fill(YELLOW_COLOR);
  screen.rect(indicator_x_starting_pos + green_indicator_height, indicator_y_starting_pos, warning_indicator_height, indicator_height);
  screen.stroke(ORANGE_COLOR);
  screen.fill(ORANGE_COLOR);
  screen.rect(indicator_x_starting_pos + green_indicator_height + warning_indicator_height, indicator_y_starting_pos, severe_indicator_height, indicator_height);
  screen.stroke(RED_COLOR);
  screen.fill(RED_COLOR);
  screen.rect(indicator_x_starting_pos + green_indicator_height + warning_indicator_height + severe_indicator_height, indicator_y_starting_pos, danger_indicator_height, indicator_height);
  last_millis = millis();
  bool first_time = true;
  while (1)
  {
    if ((digitalRead(INT_PIN) == LOW) || change_screen_flag)
    {
      change_screen();
      break;
    }

    // Calculating values for the design
    int arrow_x_position = 0;
    if (registered_level < MAX_HAZARDOUS_LIMIT)
      arrow_x_position = indicator_x_starting_pos + (registered_level * indicator_bar_width / MAX_HAZARDOUS_LIMIT);
    else
      arrow_x_position = indicator_x_starting_pos + indicator_bar_width;
    int arrow_y_position = indicator_y_starting_pos - 5;

    if (((registered_level > 0) && ((millis() - last_millis) > SCREEN_REFRESH_TIME)) || (first_time))
    {
      Serial.println("Registered Level: " + (String)registered_level);
      first_time = false;
      drawing_battery(); // Drawing Header: Company Name and Battery Percentage
      screen.noStroke();

      // Clearing the older values
      screen.stroke(BLACK_COLOR);
      screen.fill(BLACK_COLOR);
      screen.rect(0, arrow_y_position - 9, screen_width, 10);
      screen.rect(0, indicator_y_starting_pos + indicator_height + 5, screen_width, 15);

      // Drawing The circle with latest values
      if (registered_level < MAX_SAFETY_LIMIT)
      {
        screen.stroke(GREEN_COLOR);
        screen.fill(GREEN_COLOR);
        screen.text("SAFE", indicator_x_starting_pos, indicator_y_starting_pos + indicator_height + 5);
      }
      else if (registered_level < MAX_WARNING_LIMIT)
      {
        screen.stroke(YELLOW_COLOR);
        screen.fill(YELLOW_COLOR);
        screen.text("MODERATE", indicator_x_starting_pos, indicator_y_starting_pos + indicator_height + 5);
      }
      else if (registered_level < MAX_UNHEALTHY_LIMIT)
      {
        screen.stroke(ORANGE_COLOR);
        screen.fill(ORANGE_COLOR);
        screen.text("UNHEALTHY", indicator_x_starting_pos, indicator_y_starting_pos + indicator_height + 5);
      }
      else
      {
        screen.stroke(RED_COLOR);
        screen.fill(RED_COLOR);
        screen.text("HAZARDOUS", indicator_x_starting_pos, indicator_y_starting_pos + indicator_height + 5);
      }
      screen.circle(screen_width / 2, (screen_height / 2) - 15, screen_width / 4);

      // Draw the indicator arrow
      screen.line(arrow_x_position, arrow_y_position, arrow_x_position - 4, arrow_y_position - 8);
      screen.line(arrow_x_position, arrow_y_position, arrow_x_position + 4, arrow_y_position - 8);
      screen.line(arrow_x_position - 4, arrow_y_position - 8, arrow_x_position + 4, arrow_y_position - 8);

      // Writing the text on the circle
      screen.setTextSize(3);
      screen.stroke(BLACK_COLOR);
      screen.text(pm_ae_2_5, (screen_width / 2) - 20, (screen_height / 2) - 25);
      screen.setTextSize(1);
      screen.text("ug/m", (screen_width / 2) - 10, (screen_height / 2) + 5);
      screen.text("3", (screen_width / 2) + 13, (screen_height / 2) + 2);
      last_millis = millis();
    }
    collect_dust_quality_data();
    registered_level = atoi(pm_ae_2_5);
  }
}

/**
 * @brief Draws the battery level after measuring the battery level on the screen in upper right corner of screen when called.
 */
void drawing_battery(void)
{
  // Writing Company Logo
  screen.stroke(WHITE_COLOR);
  screen.setTextSize(2);
  screen.text(company_name, 2, MARGIN_BUFFER);

  int xStart = screen_width - BATTERY_SYMBOL_WIDTH - MARGIN_BUFFER;
  int yStart = MARGIN_BUFFER;
  // Clearing previous battery value
  screen.noStroke();
  screen.fill(BLACK_COLOR);
  screen.rect(xStart, yStart, BATTERY_SYMBOL_WIDTH, BATTERY_SYMBOL_HEIGHT);
  // Creating battery Outline
  screen.stroke(WHITE_COLOR);
  screen.rect(xStart, yStart, BATTERY_SYMBOL_WIDTH, BATTERY_SYMBOL_HEIGHT);
  screen.fill(WHITE_COLOR);
  screen.rect(xStart + BATTERY_SYMBOL_WIDTH, yStart + MARGIN_BUFFER, MARGIN_BUFFER, (BATTERY_SYMBOL_HEIGHT - (2 * MARGIN_BUFFER)));

  // Creating Battery Fill
  float val1 = (analogRead(BATTERY_INPUT) - MIN_BATTERY_ADC_VALUE);
  float available_battery_percentage = (val1 * 100) / MAX_BATTERY_ADC_VALUE;
  // Serial.println("Raw Battery Analog value:" + (String)analogRead(BATTERY_INPUT));
  Serial.println("Battery Level: " + (String)available_battery_percentage + "%");
  float bat_bar_width = (available_battery_percentage * BATTERY_SYMBOL_WIDTH) / 100;
  int battery_bar_width = bat_bar_width;
  // Serial.println("Integer Bar Width:" + (String)battery_bar_width);
  screen.noStroke();

  if (available_battery_percentage < LOW_BATTERY_PERCENT)
    screen.fill(RED_COLOR);
  else if (available_battery_percentage < MEDIUM_BATTERY_PERCENT)
    screen.fill(ORANGE_COLOR);
  else
    screen.fill(GREEN_COLOR);
  screen.rect(xStart + 1, yStart + 1, battery_bar_width - 2, BATTERY_SYMBOL_HEIGHT - 2);
  if (digitalRead(CONNECTED_SUPPLY_PIN))
  {
    Serial.println("Charger Connected");
    int batxStart = screen_width - (BATTERY_SYMBOL_WIDTH / 2) - MARGIN_BUFFER;
    int batyStart = MARGIN_BUFFER + 1;
    int bat_symbol_width = 2;
    screen.stroke(WHITE_COLOR);
    screen.line(batxStart, batyStart, batxStart - bat_symbol_width, batyStart + (BATTERY_SYMBOL_HEIGHT / 2));
    screen.line(batxStart - bat_symbol_width, batyStart + (BATTERY_SYMBOL_HEIGHT / 2) - 1, batxStart + bat_symbol_width, batyStart + (BATTERY_SYMBOL_HEIGHT / 2) - 1);
    screen.line(batxStart + bat_symbol_width, batyStart + (BATTERY_SYMBOL_HEIGHT / 2) - 1, batxStart, batyStart + BATTERY_SYMBOL_HEIGHT - 2);
    screen.line(batxStart, batyStart + BATTERY_SYMBOL_HEIGHT - 2, batxStart, batyStart);
  }
}

/**
 * @brief Draws the screen and the values of PMS 1.0, PM 2.5 and PM 10.0 together on a single screen in a grid manner
 */
void three_point_data_screen(void)
{
  changing_screen_display();
  change_screen_flag = false;
  Serial.println("Drawing 3 Data Monitor Screen");
  collect_dust_quality_data();

  screen.background(BLACK_COLOR);

  // Drawing the layout
  drawing_battery();
  int seperation_line_height = 25;
  screen.stroke(GREY_COLOR);
  screen.line(0, seperation_line_height, screen_width, seperation_line_height);
  screen.line(screen_width / 2, seperation_line_height, screen_width / 2, screen_height);
  screen.line(screen_width / 2, (screen_height + seperation_line_height) / 2, screen_width, (screen_height + seperation_line_height) / 2);
  screen.stroke(ORANGE_COLOR);
  screen.setTextSize(1);
  screen.text("PM1.0", (3 * screen_width / 4) - 10, seperation_line_height + 5);
  screen.text("PM10", (3 * screen_width / 4) - 10, ((screen_height + seperation_line_height) / 2) + 5);
  screen.text("ug/m", (screen_width / 4) - 15, screen_height - 10);
  screen.text("3", (screen_width / 4) + 10, screen_height - 15);
  screen.setTextSize(2);
  screen.text("PM2.5", (screen_width / 4) - 27, seperation_line_height + 5);
  int text_seperation_height = 45;

  // Writing the data
  bool first_time = true;
  last_millis = millis();
  Serial.println("Starting Unit");
  while (1)
  {
    if ((digitalRead(INT_PIN) == LOW) || change_screen_flag)
    {
      change_screen();
      break;
    }

    while (((millis() - last_millis) > SCREEN_REFRESH_TIME) || (first_time))
    {
      first_time = false;

      if (atoi(pm_ae_1_0) > 0)
      {
        Serial.println("PM AE 1.0: " + String(atoi(pm_ae_1_0)));
        Serial.println("PM AE 2.5: " + String(atoi(pm_ae_2_5)));
        Serial.println("PM AE 10: " + String(atoi(pm_ae_10_0)));
        Serial.println();
        drawing_battery();
        // Cleaning Previous Values
        screen.noStroke();
        screen.fill(BLACK_COLOR);
        screen.rect((screen_width / 4) - 30, text_seperation_height + 25, 60, 30);
        screen.rect((3 * screen_width / 4) - 25, ((screen_height + text_seperation_height) / 2) + 5, 60, 30);
        screen.rect((3 * screen_width / 4) - 25, text_seperation_height - 5, 60, 30);

        // Writing New Values
        screen.setTextSize(3);
        screen.stroke(YELLOW_COLOR);
        screen.text(pm_ae_2_5, (screen_width / 4) - 25, text_seperation_height + 30);
        screen.text(pm_ae_10_0, (3 * screen_width / 4) - 20, ((screen_height + text_seperation_height) / 2) + 10);
        screen.stroke(PINK_COLOR);
        screen.text(pm_ae_1_0, (3 * screen_width / 4) - 20, text_seperation_height);
        last_millis = millis();
      }
      Serial.println();
      collect_dust_quality_data();
    }
  }
}

/**
 * @brief Draws the background to plot the values of PM 2.5
 */
void draw_graph_screen(void)
{
  Serial.println("Draw Graph Background");
  collect_dust_quality_data();

  screen.background(BLACK_COLOR);
  screen.stroke(GREY_COLOR);
  screen.line(GRAPH_BUFFER_SCREEN, GRAPH_BUFFER_SCREEN, GRAPH_BUFFER_SCREEN, screen_height - GRAPH_BUFFER_SCREEN);
  screen.line(GRAPH_BUFFER_SCREEN, screen_height - GRAPH_BUFFER_SCREEN, screen_width - GRAPH_BUFFER_SCREEN, screen_height - GRAPH_BUFFER_SCREEN);
  int x_dots = ((screen_width - (2 * GRAPH_BUFFER_SCREEN)) / GRAPH_BUFFER_SCREEN + 1);
  int y_dots = ((screen_height - (2 * GRAPH_BUFFER_SCREEN)) / GRAPH_BUFFER_SCREEN) + 1;
  for (size_t i = 0; i < y_dots; i++)
    for (size_t j = 0; j < x_dots; j++)
      screen.point((j * GRAPH_BUFFER_SCREEN) + GRAPH_BUFFER_SCREEN, (i * GRAPH_BUFFER_SCREEN) + GRAPH_BUFFER_SCREEN);

  screen.stroke(WHITE_COLOR);
  screen.setTextSize(1);
  screen.text("PM2.5", screen_width - 40, 10);
  screen.text("50 divs", 1, 1);
  screen.text("5 mins", screen_width - 40, screen_height - 10);
  for (size_t i = 0; i < (y_dots / 2) + 1; i++)
    screen.line(GRAPH_BUFFER_SCREEN, screen_height - (2 * i * GRAPH_BUFFER_SCREEN) - GRAPH_BUFFER_SCREEN, GRAPH_BUFFER_SCREEN / 2, screen_height - (2 * i * GRAPH_BUFFER_SCREEN) - GRAPH_BUFFER_SCREEN);
  for (size_t i = 0; i < (x_dots / 2); i++)
    screen.line((2 * i * GRAPH_BUFFER_SCREEN) + GRAPH_BUFFER_SCREEN, screen_height - GRAPH_BUFFER_SCREEN, (2 * i * GRAPH_BUFFER_SCREEN) + GRAPH_BUFFER_SCREEN, screen_height - (GRAPH_BUFFER_SCREEN / 2));
}

/**
 * @brief Plots the graph bars corresponding to value of PM 2.5 after drawing the graph background
 */
void plot_graph_bar(void)
{
  draw_graph_screen();
  change_screen_flag = false;
  Serial.println("Plot Graph");
  last_millis = millis();
  int x_levels = ((screen_width - (2 * GRAPH_BUFFER_SCREEN)) / GRAPH_BUFFER_SCREEN) / 2;
  int y_levels = ((screen_height - (2 * GRAPH_BUFFER_SCREEN)) / (GRAPH_BUFFER_SCREEN * 2));
  int last_graph_position = 0;
  uint16_t registered_level = 0;
  float graph_divisions = 100;
  float scaling_factor = screen_height / (y_levels * graph_divisions);
  screen.stroke(BLACK_COLOR);

  int x_last_pos, y_last_pos;

  while (last_graph_position <= x_levels)
  {
    if (last_graph_position == x_levels)
    {
      draw_graph_screen();
      last_graph_position = 0;
    }

    if ((digitalRead(INT_PIN) == LOW) || change_screen_flag)
    {
      change_screen();
      break;
    }

    registered_level = 0;
    collect_dust_quality_data();
    registered_level = atoi(pm_ae_2_5);

    if ((((millis() - last_millis) > GRAPH_PLOT_TIME) || (last_graph_position == 0)) && (registered_level > 10))
    {
      Serial.println("PM AE 2.5: " + (String)registered_level);
      // Calculating Bar Values
      int scaled_value = 0;
      int height = 0;
      if (registered_level < MAX_HAZARDOUS_LIMIT)
      {
        scaled_value = registered_level * scaling_factor;
        height = scaled_value;
      }
      else
      {
        scaled_value = screen_height - (2 * GRAPH_BUFFER_SCREEN);
        height = screen_height - (2 * GRAPH_BUFFER_SCREEN);
      }
      int x_pos = GRAPH_BUFFER_SCREEN + (2 * GRAPH_BUFFER_SCREEN * last_graph_position) + 1;
      int y_pos = screen_height - GRAPH_BUFFER_SCREEN - scaled_value;
      int width = (2 * GRAPH_BUFFER_SCREEN) - 1;

      if (registered_level < MAX_SAFETY_LIMIT)
        screen.fill(GREEN_COLOR);
      else if (registered_level < MAX_WARNING_LIMIT)
        screen.fill(YELLOW_COLOR);
      else if (registered_level < MAX_UNHEALTHY_LIMIT)
        screen.fill(ORANGE_COLOR);
      else
        screen.fill(RED_COLOR);
      screen.stroke(BLACK_COLOR);
      screen.rect(x_pos, y_pos, width, height);

      if (last_graph_position == 0)
      {
        x_last_pos = x_pos + (width / 2);
        y_last_pos = y_pos;
      }
      else
      {
        x_pos = x_pos + (width / 2);
        screen.fill(WHITE_COLOR);
        screen.stroke(WHITE_COLOR);
        screen.circle(x_last_pos, y_last_pos, 2);
        screen.circle(x_pos, y_pos, 2);
        screen.line(x_last_pos, y_last_pos, x_pos, y_pos);
        x_last_pos = x_pos;
        y_last_pos = y_pos;
      }

      last_millis = millis();
      last_graph_position++;
    }
  }
}

void put_device_to_sleep(void)
{
  Serial.println("Entering Sleep mode");
  screen.fill(GREY_COLOR);
  screen.rect(10, (screen_height / 2) - 40, screen_width - 20, 80);
  screen.fill(BLACK_COLOR);
  screen.stroke(BLACK_COLOR);
  screen.setTextSize(1);
  screen.text("SHUTTING DOWN", 30, (screen_height / 2));
  delay(2000);
  boot_screen();
  screen_counter = 0;
  change_screen_flag = false;
  shutdown_device_peripherals();
  cli();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();
  sleep_enable();
  cli();
  Serial.println("Placing Device in sleep mode");
  Serial.flush();
  delay(2000);
  cli();
  attachInterrupt(digitalPinToInterrupt(INT_PIN), change_screen, LOW);
  cli();
  interrupts();
  sleep_cpu();
}

void changing_screen_display(void)
{
  screen.fill(GREY_COLOR);
  screen.rect(10, (screen_height / 2) - 40, screen_width - 20, 80);
  screen.fill(BLACK_COLOR);
  screen.stroke(BLACK_COLOR);
  screen.setTextSize(1);
  screen.text("CHANGING SCREEN", 20, (screen_height / 2));
}

void change_screen(void)
{
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(INT_PIN));
  Serial.println("Button pressed");
  delay(500);

  change_screen_flag = true;
  uint64_t sleep_timer = millis();
  while (digitalRead(INT_PIN) == LOW)
    if ((millis() - sleep_timer) > 2000)
    {
      screen_counter = 4;
      return;
    }

  if (screen_counter < 3)
    screen_counter++;
  else
    screen_counter = 1;
  return;
}

void start_device_peripherals(void)
{
  Serial.println("Starting Device Peripherals");
  digitalWrite(PMS_CTRL, HIGH);
  digitalWrite(TFT_DISPLAY_LED, HIGH);
  screen.begin();
  screen.setRotation(180);
  screen_height = screen.height();
  screen_width = screen.width();
  screen.background(BLACK_COLOR);
  screen.stroke(WHITE_COLOR);
  boot_screen();
}

void shutdown_device_peripherals(void)
{
  digitalWrite(PMS_CTRL, LOW);
  digitalWrite(TFT_DISPLAY_LED, LOW);
  Serial.println("Stopping Device Peripherals");
}

//!==================================================================================//

void setup()
{
  Serial.begin(9600);
  Serial.println("\n\n\n----------------------------------------------------------------------------------------");
  Serial.println("Starting Device");
  setup_sensors();
  setup_peripheral_pin_mode();
  start_device_peripherals();
  Serial.println("Setup Done");
  start_device_peripherals();
}

void loop()
{
  changing_screen_display();
  switch (screen_counter)
  {
  case 1:
    Serial.println("\n----------------------------------------------------------------------------------------");
    graphic_data_screen();
    break;

  case 2:
    Serial.println("\n----------------------------------------------------------------------------------------");
    three_point_data_screen();
    break;

  case 3:
    Serial.println("\n----------------------------------------------------------------------------------------");
    plot_graph_bar();
    break;

  case 4:
    Serial.println("\n----------------------------------------------------------------------------------------");
    put_device_to_sleep();
    break;
  }
}