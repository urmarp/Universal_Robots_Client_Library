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
#ifndef UR_CLIENT_LIBRARY_PRIMARY_CLIENT_H_INCLUDED
#define UR_CLIENT_LIBRARY_PRIMARY_CLIENT_H_INCLUDED

#include <ur_client_library/primary/primary_parser.h>
#include <ur_client_library/comm/producer.h>
#include <ur_client_library/comm/stream.h>
#include <ur_client_library/comm/pipeline.h>
#include <ur_client_library/ur/calibration_checker.h>
#include <ur_client_library/primary/primary_consumer.h>
#include <ur_client_library/primary/primary_shell_consumer.h>
#include <ur_client_library/ur/dashboard_client.h>

namespace urcl
{
namespace primary_interface
{
class PrimaryClient
{
public:
  PrimaryClient() = delete;
  PrimaryClient(const std::string& robot_ip, const std::string& calibration_checksum);
  // virtual ~PrimaryClient() = default;
  virtual ~PrimaryClient();

  /*!
   * \brief Sends a custom script program to the robot.
   *
   * The given code must be valid according the UR Scripting Manual.
   *
   * \param script_code URScript code that shall be executed by the robot.
   *
   * \returns true on successful upload, false otherwise.
   */
  bool sendScript(const std::string& script_code);

  /*!
   * \brief Checks if the kinematics information in the used model fits the actual robot.
   *
   * \param checksum Hash of the used kinematics information
   */
  void checkCalibration(const std::string& checksum);

  /*!
   * \brief Configures the primary client
   *
   * Creates a connection to the stream and sets up producer, consumer and pipeline
   */
  bool configure();

   /*!
   * \brief Checks if the robot is in local or remote control
   *
   * Checks for package with error code determining if robot is in remote or local control
   */
  void checkRemoteLocalControl();

  /*!
   * \brief Stops the primary client
   *
   * Closes the thread checking for remote or local control
   */
  void stop();

private:
  std::string robot_ip_;
  PrimaryParser parser_;
  std::unique_ptr<PrimaryConsumer> consumer_;
  comm::INotifier notifier_;
  bool connected_;
  std::atomic<bool> running_, in_remote_control_;
  std::unique_ptr<comm::URProducer<PrimaryPackage>> producer_;
  std::unique_ptr<comm::URStream<PrimaryPackage>> stream_;
  std::unique_ptr<comm::Pipeline<PrimaryPackage>> pipeline_;
  std::thread rc_thread_;
};

}  // namespace primary_interface
}  // namespace urcl

#endif  // ifndef UR_CLIENT_LIBRARY_PRIMARY_CLIENT_H_INCLUDED
