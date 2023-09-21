#include "main.h"
#include "pros/misc.h"
#include "pros/motors.hpp"
#include <string>
#include <vector>
#include <math.h>

// Wheel motor ports
pros::Motor front_left(10, false);
pros::Motor front_right(13, true);
pros::Motor back_left(17, false);
pros::Motor back_right(15, true);
pros::Motor h_wheel(18, true);

// Other motor ports
pros::Motor catapult(16, true);

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button()
{
	static bool pressed = false;
	pressed = !pressed;
	if (pressed)
	{
		pros::lcd::set_text(2, "I was pressed!");
	}
	else
	{
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous()
{
	tare_positions();

	const u_int16_t tile = 850;
	const u_int16_t turn = 350;
	const u_int16_t check_tolerance = 5;

	// forward section 1
	front_left.move_relative(tile * 3.25, 20);
	front_right.move_relative(tile * 3.25, 20);
	back_left.move_relative(tile * 3.25, 20);
	back_right.move_relative(tile * 3.25, 20);

	while (!((front_left.get_position() < tile * 3.25 + check_tolerance) && (front_left.get_position() > tile * 3.25 - check_tolerance)))
	{
		pros::delay(2);
	}

	// turn left
	front_left.move_relative(-turn, 20);
	front_right.move_relative(turn, 20);
	back_left.move_relative(-turn, 20);
	back_right.move_relative(turn, 20);

	pros::delay(5000);
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol()
{
	tare_positions();

	pros::Controller master(pros::E_CONTROLLER_MASTER);

	while (true)
	{
		u_int32_t y_stick = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
		u_int32_t x_stick = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
		u_int32_t h_stick = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);

		// drive
		u_int32_t right = x_stick - y_stick;
		u_int32_t left = x_stick + y_stick;

		front_left.move(left);
		front_right.move(right);
		back_left.move(left);
		back_right.move(right);
		h_wheel.move(h_stick);

		pros::delay(1);
	}
}

void tare_positions()
{
	front_left.tare_position();
	front_right.tare_position();
	back_left.tare_position();
	back_right.tare_position();
}