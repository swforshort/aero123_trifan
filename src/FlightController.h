#include <string>

/*
internal members are confirgured to match
the main hardware outputs from a VTOL tiltrotor
aircraft in PX4:
https://docs.px4.io/en/airframes/airframe_reference.html#vtol-tiltrotor
*/
class FlightController
{
private:
    int motor_front_right_bottom_speed;
    int motor_front_right_top_speed;
    int motor_back_bottom_speed;
    int motor_back_top_speed;
    int motor_front_left_bottom_speed;
    int motor_front_left_top_speed;
    int tilt_servo_angle;
    int elevon_one_angle;
    int elevon_two_angle;
    int landing_gear_servo_angle;

public:
    FlightController();
    std::string getStatus();
};