// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

// -- BEGIN LICENSE BLOCK ----------------------------------------------
// Copyright 2019 FZI Forschungszentrum Informatik
//
// Licensed under the Apache License, Text 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// -- END LICENSE BLOCK ------------------------------------------------

//----------------------------------------------------------------------
/*!\file
 *
 * \author  Felix Exner exner@fzi.de
 * \date    2020-04-30
 *
 */
//----------------------------------------------------------------------

#include <chrono>
#include <thread>

#include <ur_client_library/primary/primary_client.h>
#include <ur_client_library/primary/primary_shell_consumer.h>

#include <ur_client_library/comm/tcp_socket.h>

namespace urcl
{
namespace primary_interface
{
PrimaryClient::PrimaryClient(const std::string& robot_ip, const std::string& calibration_checksum) : robot_ip_(robot_ip)
{
  // connected_ = configure(robot_ip_, "");
  stream_.reset(new comm::URStream<PrimaryPackage>(robot_ip_, UR_PRIMARY_PORT));
  producer_.reset(new comm::URProducer<PrimaryPackage>(*stream_, parser_));
  producer_->setupProducer();

  consumer_.reset(new PrimaryConsumer());
  std::shared_ptr<CalibrationChecker> calibration_checker(new CalibrationChecker(calibration_checksum));
  consumer_->setKinematicsInfoHandler(calibration_checker);

  // consumer_.reset(new primary_interface::PrimaryShellConsumer());
  pipeline_.reset(new comm::Pipeline<PrimaryPackage>(*producer_, consumer_.get(), "primary pipeline", notifier_));
  pipeline_->run();
  in_remote_control_ = false;
  running_ = true;

  rc_thread_ = std::thread(&PrimaryClient::checkRemoteLocalControl, this);

  // calibration_checker->isChecked();
  // while (!calibration_checker->isChecked())
  //{
  // std::this_thread::sleep_for(std::chrono::seconds(1));
  //}
  // URCL_LOG_DEBUG("Got calibration information from robot.");
}

PrimaryClient::~PrimaryClient()
{
  URCL_LOG_DEBUG("Destructing primary client");
  stop();
}

bool PrimaryClient::sendScript(const std::string& script_code)
{
  if (stream_ == nullptr)
  {
    throw std::runtime_error("Sending script to robot requested while there is no primary interface established. This "
                             "should not happen.");
  }

  // urscripts (snippets) must end with a newline, or otherwise the controller's runtime will
  // not execute them. To avoid problems, we always just append a newline here, even if
  // there may already be one.
  auto program_with_newline = script_code + '\n';

  size_t len = program_with_newline.size();
  const uint8_t* data = reinterpret_cast<const uint8_t*>(program_with_newline.c_str());
  size_t written;

  if (stream_->write(data, len, written))
  {
    // std::cout << "in_remote_control_ write: " << in_remote_control_ << std::endl;
    if (!in_remote_control_)
    {
      // URCL_LOG_ERROR("Not in remote control. Cannot send script whilst in local control");
      return false;
    }
    // URCL_LOG_INFO("Sent program to robot:\n%s", program_with_newline.c_str());
    return true;
  }
  URCL_LOG_ERROR("Could not send program to robot");
  return false;
}

void PrimaryClient::checkCalibration(const std::string& checksum)
{
  // if (stream_ == nullptr)
  //{
  // throw std::runtime_error("checkCalibration() called without a primary interface connection being established.");
  //}

  // while (!consumer.isChecked())
  //{
  // ros::Duration(1).sleep();
  //}
  // ROS_DEBUG_STREAM("Got calibration information from robot.");
}

bool PrimaryClient::configure()
{
  URCL_LOG_DEBUG("Reconnecting to stream on %s:%d", robot_ip_.c_str(), UR_PRIMARY_PORT);
  pipeline_->stop();

  if (stream_->getState() == comm::SocketState::Connected)
    stream_->disconnect();

  bool res = stream_->connect();
  pipeline_->run();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return res;
}

void PrimaryClient::checkRemoteLocalControl()
{
  urcl::DashboardClient dashboard_client(robot_ip_);
  dashboard_client.connect();
  bool old_val = false;
  while (running_)
  {
    // if(consumer_ == nullptr)
    // {
    //   continue;
    // }

    if (dashboard_client.getState() == comm::SocketState::Connected)
    {
      std::string answer = dashboard_client.sendAndReceive("is in remote control\n");
      in_remote_control_ = answer == "true";

      if (old_val != in_remote_control_)
        configure();
    }
    else
    {
      URCL_LOG_INFO("Reconnecting to Dashboard Server");
      dashboard_client.connect();
    }

    old_val = in_remote_control_;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // if(consumer_->getLatestErrorCode() == 210)
    // {
    //   // consumer_->resetLatestErrorCode();
    //   in_remote_control_ = false;
    //   URCL_LOG_ERROR("Script code was not accepted by robot. Socket is read-only when the robot is in local (Teach
    //   pendant) control"); configure();
    // }

    // std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // uint8_t buf[4096];
    // size_t read = 0;

    // if (stream_->read(buf, sizeof(buf), read))
    // {
    //   in_remote_control_ =  true;
    //   continue;
    // }
    // URCL_LOG_ERROR("Could not read from stream");
    // in_remote_control_ = false;
    // configure();

    // if (producer_->getReconnect())
    // {
    //   in_remote_control_ = false;
    //   configure();
    // }
    // else
    // {
    //   in_remote_control_ =  true;
    // }

    // std::string program_with_newline = "\n";

    // size_t len = program_with_newline.size();
    // const uint8_t* data = reinterpret_cast<const uint8_t*>(program_with_newline.c_str());
    // size_t written;

    // stream_->write(data, len, written);
  }
}

void PrimaryClient::stop()
{
  if (rc_thread_.joinable())
  {
    rc_thread_.join();
  }
}
}  // namespace primary_interface
}  // namespace urcl
