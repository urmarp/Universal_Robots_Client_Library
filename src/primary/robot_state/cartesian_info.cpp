// -- BEGIN LICENSE BLOCK ----------------------------------------------
// Copyright 2022 Universal Robots A/S
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the {copyright_holder} nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// -- END LICENSE BLOCK ------------------------------------------------

#include "ur_client_library/log.h"
#include "ur_client_library/primary/robot_state/cartesian_info.h"
#include "ur_client_library/primary/abstract_primary_consumer.h"

#include <iomanip>

namespace urcl
{
namespace primary_interface
{
bool CartesianInfo::parseWith(comm::BinParser& bp)
{
  bp.parse(flange_coordinates_);
  bp.parse(tcp_offset_coordinates_);

  return true;
}

bool CartesianInfo::consumeWith(AbstractPrimaryConsumer& consumer)
{
  return consumer.consume(*this);
}

std::string CartesianInfo::toString() const
{
  std::stringstream os;
  os << "flange_coordinates: [";
  for (size_t i = 0; i < flange_coordinates_.size(); ++i)
  {
    os << std::setprecision(15) << flange_coordinates_[i] << " ";
  }
  os << "]" << std::endl;
  os << "tcp_offset_coordinates: [";
  for (size_t i = 0; i < tcp_offset_coordinates_.size(); ++i)
  {
    os << std::setprecision(15) << tcp_offset_coordinates_[i] << " ";
  }
  os << "]" << std::endl;

  return os.str();
}

// std::string CartesianInfo::toHash() const
// {
//   std::stringstream ss;
//   for (size_t i = 0; i < 6; ++i)
//   {
//     ss << flange_coordinates_[i];
//     ss << tcp_offset_coordinates_[i];
//   }
//   std::hash<std::string> hash_fn;
//   return "calib_" + std::to_string(hash_fn(ss.str()));
// }
}  // namespace primary_interface
}  // namespace urcl