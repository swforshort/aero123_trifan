#include "FlightController.h"
#include "FlightLog.h"
#include "Gps.h"
#include "Simulator.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

void loggerThread(FlightLog* fPtr){
    fPtr->run();
}

void simThread(Simulator* sim){
    sim->run();
}

void printHelp()
{
    std::cout << "************ TRIFAN HELP *************** \n";
    std::cout << "Command Options: \n";
    std::cout << "  'help'           -> print this message \n";
    std::cout << "  'shutdown'       -> kill the program \n";
    std::cout << "  'takeoff'        -> turn motors up to gain hover altitude\n";
    std::cout << "  'land'           -> turn motors down to shed hover altitude\n";
    std::cout << "  'forward_flight' -> flip propellors to 90deg (forward)\n";
    std::cout << "  'rotors_forward' -> nudge propellors towards forward flight\n";
    std::cout << "  'rotors_up'      -> nudge propellors towards hover\n";
    std::cout << "  'hover'          -> flip props to 0 (hover) \n";
    std::cout << "  'elvs_up'        -> nudge angle of elevons for climb\n";
    std::cout << "  'elvs_down'      -> nudge angle of elevons for dive\n";
    std::cout << "  'elvs_level'     -> snap elevons to neutral\n";
    std::cout << "  'throttle_up'    -> turn motors up by 250 RPM\n";
    std::cout << "  'throttle_down'  -> turn motors down by 250 RPM\n";
    std::cout << "  'gear_up'        -> stow landing gear\n";
    std::cout << "  'gear_down'      -> deploy landing gear\n";
    std::cout << "  'status'         -> print flight log current entry\n";
    std::cout << "**************************************** \n\n\n";
}

int main()
{
    std::cout << "Trifan initializing...\n";
    Gps* gps = new Gps(stable);
    FlightController* ctrl = new FlightController(gps);
    std::string log_name = "flight.log";
    int log_interval = 3; // write status ever 3 seconds
    FlightLog* fLog = new FlightLog(ctrl, log_name);
    Simulator* sim = new Simulator(ctrl, gps);
    std::cout << "\n" << "...Initialized!\n" << "Logging to " << log_name << "\n\n";
    printHelp();
    std::string flight_command;
    // background threads
    // the first one writes to the flight log every three seconds
    std::thread lt(loggerThread, fLog);
    // this one updates the GPS position from the
    // flight configuration (approximately)
    std::thread st(simThread, sim);
    while(true) {
        std::cout << "Input command...\n";
        std::getline(std::cin, flight_command);
        std::cout << "Executing Command: " << flight_command << "\n";
        if(flight_command == "shutdown")
        {
            std::cout << "Aborting...\n";
            break;
        }
        else if(flight_command == "help")
        {
            printHelp();
        }
        else if(flight_command == "takeoff")
        {
            // probably should check arm/disarm here and abort if
            // disarmed
            ctrl->updateMotors(3500);
        }
        else if(flight_command == "land")
        {
            if(ctrl->getTiltAngle() == 0) //checks to see if the dron is in hover
            {
                // So a thought occurs that at different levels of wind and such these values wouldn't always work.
                // 2500 might keep you at the same altitude as there is an updraft.
                // the comments "stable - ###" is the values off the motor speed that would allow for descent

                std::cout << "Landing...\n";

                while(gps->getAltitude()>0.0)
                {
                    std::cout << "Current Altitude: " << gps->getAltitude() << " \n"; //this isn't printing

                    if(gps->getAltitude() >50.0)                                //if drone is currently over 50 units in altitude
                        ctrl->updateMotors(2500); 
                        // stable - 500 OR 0.8333 of stable 
                    if(gps->getAltitude() <=50.0 && gps->getAltitude() > 20.0)  //if drone is currently between 50 and 20 units
                        ctrl->updateMotors(2750); 
                        // stable - 250 OR 0.9167 of stable
                    if(gps->getAltitude() >0.0 && gps->getAltitude() <= 20)     //if drone is below 20 units but not on the ground
                        ctrl->updateMotors(2900); 
                        // stable - 100 OR 0.9667 of stable

                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

                std::cout << "Landed. \n";
                ctrl->updateMotors(3000);
            }
            else
                std::cout << "Please return to hover, before attempting to land. \n";

        }
        else if(flight_command == "forward_flight")
        {
            // snap props to forward configuration
            ctrl->tiltProps(90);
        }
        else if(flight_command == "rotors_forward")
        {
            ctrl->tiltProps(ctrl->getTiltAngle() + 10);
        }
        else if(flight_command == "rotors_up")
        {
            ctrl->tiltProps(ctrl->getTiltAngle() - 10);
        }
        else if(flight_command == "hover")
        {
            // snap props to hover configuration
            ctrl->tiltProps(0);
        }
        else if(flight_command == "elvs_up")
        {
            // use elevons to gain altitude
            ctrl->setElevons(ctrl->getElevonAngle(0) + 3);
        }
        else if(flight_command == "elvs_down")
        {
            // use elevons to shed altitude
            ctrl->setElevons(ctrl->getElevonAngle(0) - 3);
        }
        else if(flight_command == "elvs_level")
        {
            // bring elevons to neutral
            ctrl->setElevons(0);
        }
        else if(flight_command == "throttle_up")
        {
            // increments motor speed by 250 RPM
            ctrl->updateMotors(ctrl->getMotorSpeed(0) + 250);
        }
        else if(flight_command == "throttle_down")
        {
            // decrements motor speed by 250 RPM
            ctrl->updateMotors(ctrl->getMotorSpeed(0) - 250);
        }
        else if(flight_command == "gear_up")
        {
            ctrl->setGearSrv(90);
        }
        else if(flight_command == "gear_down")
        {
            ctrl->setGearSrv(0);
        }
        else if(flight_command == "status")
        {
            std::cout << ctrl->getStatus() << "\n" << sim->getStatus() << "\n";
        }
        else
        {
            std::cout << "NO SUCH COMMAND!! " << flight_command << "\n\n";
            printHelp();
        }
        
    }
    fLog->signalStop();
    sim->signalStop();
    lt.join();
    st.join();
    delete ctrl;
    return 0;
}