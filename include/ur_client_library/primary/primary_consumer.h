// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-
//
// -- BEGIN LICENSE BLOCK ----------------------------------------------
// Copyright 2020 FZI Forschungszentrum Informatik
//
// Licensed under the Apache License, Version 2.0 (the "License");
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

#ifndef UR_CLIENT_LIBRARY_PRIMARY_CONSUMER_H_INCLUDED
#define UR_CLIENT_LIBRARY_PRIMARY_CONSUMER_H_INCLUDED

#include "ur_client_library/log.h"
#include "ur_client_library/comm/pipeline.h"
#include "ur_client_library/primary/primary_package_handler.h"
#include "ur_client_library/primary/abstract_primary_consumer.h"
#include "ur_client_library/primary/key_message_handler.h"
#include "ur_client_library/primary/error_code_message_handler.h"
#include "ur_client_library/primary/robot_message/error_code_message.h"
#include "ur_client_library/primary/robot_message/key_message.h"
#include "ur_client_library/primary/robot_message/runtime_exception_message.h"
#include "ur_client_library/primary/robot_message/text_message.h"
#include "ur_client_library/primary/robot_message/version_message.h"
#include "ur_client_library/primary/robot_state/kinematics_info.h"

namespace urcl
{
namespace primary_interface
{
/*!
 * \brief Primary consumer implementation
 *
 * This class implements am AbstractPrimaryConsumer such that it can consume all incoming primary
 * messages. However, actual work will be done by workers for each specific type.
 */
class PrimaryConsumer : public AbstractPrimaryConsumer
{
public:
  PrimaryConsumer()
  {
    URCL_LOG_INFO("Constructing primary consumer");
    key_message_worker_.reset(new KeyMessageHandler());
    error_code_message_worker_.reset(new ErrorCodeMessageHandler());
    URCL_LOG_INFO("Constructed primary consumer");
  }
  virtual ~PrimaryConsumer() = default;

  virtual bool consume(RobotMessage& msg) override
  {
    URCL_LOG_INFO("---RobotMessage:---\n%s", msg.toString().c_str());
    return true;
  }
  virtual bool consume(RobotState& msg) override
  {
    // URCL_LOG_INFO("---RobotState:---\n%s", msg.toString().c_str());
    return true;
  }
  virtual bool consume(RuntimeExceptionMessage& msg) override
  {
    URCL_LOG_INFO("---RuntimeExceptionMessage---\n%s", msg.toString().c_str());
    return true;
  }
  virtual bool consume(TextMessage& msg) override
  {
    resetLatestErrorCode();
    // URCL_LOG_INFO("---TextMessage---\n%s", msg.toString().c_str());
    return true;
  }
  virtual bool consume(VersionMessage& msg) override
  {
    URCL_LOG_INFO("---VersionMessage---\n%s", msg.toString().c_str());
    return true;
  }
  virtual bool consume(ProgramStateMessage& msg) override
  {
    URCL_LOG_INFO("---ProgramStateMessage---%s", msg.toString().c_str());
    return true;
  }
  virtual bool consume(GlobalVariablesUpdateMessage& msg) override
  {
    URCL_LOG_INFO("---GlobalVariablesUpdateMessage---\n%s", msg.toString().c_str());
    return true;
  }

  /*!
   * \brief Handle a KinematicsInfo
   *
   * \returns True if there's a handler for this message type registered. False otherwise.
   */
  virtual bool consume(KinematicsInfo& pkg) override
  {
    if (kinematics_info_message_worker_ != nullptr)
    {
      kinematics_info_message_worker_->handle(pkg);
      return true;
    }
    return false;
  }

  /*!
   * \brief Handle a KeyMessage
   *
   * \returns True if there's a handler for this message type registered. False otherwise.
   */
  virtual bool consume(KeyMessage& pkg) override
  {
    if (key_message_worker_ != nullptr)
    {
      key_message_worker_->handle(pkg);
      return true;
    }
    return false;
  }

  /*!
   * \brief Handle a ErrorCodeMessage
   *
   * \returns True if there's a handler for this message type registered. False otherwise.
   */
  virtual bool consume(ErrorCodeMessage& pkg) override
  {
    latest_error_code_ = pkg.message_code_;
    if (error_code_message_worker_ != nullptr)
    {
      error_code_message_worker_->handle(pkg);
      return true;
    }
    return false;
  }
  void setKinematicsInfoHandler(const std::shared_ptr<IPrimaryPackageHandler<KinematicsInfo>>& handler)
  {
    kinematics_info_message_worker_ = handler;
  }

  void resetLatestErrorCode()
  {
    latest_error_code_ = 0;
  }

  int32_t getLatestErrorCode()
  {
    return latest_error_code_;
  }


private:
  int32_t latest_error_code_;
  std::shared_ptr<IPrimaryPackageHandler<KeyMessage>> key_message_worker_;
  std::shared_ptr<IPrimaryPackageHandler<ErrorCodeMessage>> error_code_message_worker_;
  std::shared_ptr<IPrimaryPackageHandler<KinematicsInfo>> kinematics_info_message_worker_;
};
}  // namespace primary_interface
}  // namespace urcl

#endif  // ifndef UR_CLIENT_LIBRARY_PRIMARY_CONSUMER_H_INCLUDED
