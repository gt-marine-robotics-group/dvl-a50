#ifndef DVL_A50_HPP
#define DVL_A50_HPP

/**
 * dvl-sensor.hpp
 *
 * @author     Pablo Gutiérrez
 * @date       24/11/2021
 */

// ROS 2 Headers
#include <chrono>
#include <memory>
#include <unistd.h>

#include "rclcpp/rclcpp.hpp"
#include "dvl_a50/tcpsocket.hpp"
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <geometry_msgs/msg/twist_with_covariance.hpp>
#include <geometry_msgs/msg/pose_with_covariance.hpp>
#include <nav_msgs/msg/odometry.hpp>


#include <string>
#include "dvl_msgs/msg/dvl.hpp"
#include "dvl_msgs/msg/dvl_beam.hpp"
#include "dvl_msgs/msg/dvldr.hpp"
#include "dvl_msgs/msg/config_command.hpp"
#include "dvl_msgs/msg/command_response.hpp"
#include "dvl_msgs/msg/config_status.hpp"

#include "dvl_a50/json/single_include/nlohmann/json.hpp"
#include <iomanip>

using namespace std::chrono_literals;
using std::placeholders::_1;
using std::string;
using nlohmann::json;


namespace dvl_sensor {

enum DVL_Parameters {
    speed_of_sound,
    acoustic_enabled,
    dark_mode_enabled,
    mountig_rotation_offset,
    range_mode,
    periodic_cycling_enabled,
    invalid_param
};

class DVL_A50: public rclcpp::Node
{
public:

    uint8_t ready = 0;
    uint8_t error = 0;

    DVL_A50();
    ~DVL_A50();

private:
    int fault = 1; 
    string delimiter = ",";
    double old_altitude;
    std::string ip_address;
    std::string velocity_frame_id;
    std::string position_frame_id;
    TCPSocket *tcpSocket;
    json json_data;

    DVL_Parameters resolveParameter(std::string param);
    
    std::chrono::steady_clock::time_point first_time;
    std::chrono::steady_clock::time_point first_time_error;
    
    std::string odometry_frame_id_param;
    rclcpp::TimerBase::SharedPtr timer_receive;
    rclcpp::TimerBase::SharedPtr timer_send;
    rclcpp::Publisher<dvl_msgs::msg::DVL>::SharedPtr dvl_pub_report;
    rclcpp::Publisher<dvl_msgs::msg::DVLDR>::SharedPtr dvl_pub_pos;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr dvl_pub_odometry;

    rclcpp::Publisher<dvl_msgs::msg::CommandResponse>::SharedPtr dvl_pub_command_response;
    rclcpp::Publisher<dvl_msgs::msg::ConfigStatus>::SharedPtr dvl_pub_config_status;
    rclcpp::Subscription<dvl_msgs::msg::ConfigCommand>::SharedPtr dvl_sub_config_command;
    geometry_msgs::msg::TwistWithCovariance saved_velocity;
    geometry_msgs::msg::PoseWithCovariance saved_position;

    void handle_receive();
    //Publish velocity and transducer report
    void publish_vel_trans_report();
    void publish_dead_reckoning_report();
    void publish_config_status();
    void publish_command_response();
    void publish_odometry_report();

    void command_subscriber(const dvl_msgs::msg::ConfigCommand::SharedPtr msg);
    void set_json_parameter(const std::string name, const std::string value);
    void send_parameter_to_sensor(const json &message);

};

} // namespace
#endif //DVL_A50_HPP
