// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

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
 * \author  Felix Exner mauch@fzi.de
 * \date    2020-09-11
 *
 */
//----------------------------------------------------------------------

#include <ur_client_library/comm/pipeline.h>
#include <ur_client_library/comm/producer.h>
#include <ur_client_library/primary/primary_shell_consumer.h>
#include <ur_client_library/primary/primary_parser.h>
#include <ur_client_library/primary/primary_client.h>

using namespace urcl;

// In a real-world example it would be better to get those values from command line parameters / a better configuration
// system such as Boost.Program_options
const std::string ROBOT_IP = "10.53.253.82";
// const std::string ROBOT_IP = "localhost";

int main(int argc, char* argv[])
{
  // First of all, we need to create a primary client that connects to the robot
  primary_interface::PrimaryClient primary_client(ROBOT_IP, "");

  // std::stringstream cmd;
  // cmd.imbue(std::locale::classic());  // Make sure, decimal divider is actually '.'
  // cmd << "sec setup():" << std::endl
  //     // << " set_payload(" << 0.1 << ", [" << 0 << ", " << 0 << ", " << 0 << "])" << std::endl
  //     << " textmsg(\"Command through primary interface complete\")" << std::endl
  //     << "end";

  // std::string script_code = cmd.str();

  // auto program_with_newline = script_code + '\n';

  // primary_client.sendScript(program_with_newline);
  

  // First of all, we need a stream that connects to the robot
  // comm::URStream<primary_interface::PrimaryPackage> primary_stream(ROBOT_IP, urcl::primary_interface::UR_PRIMARY_PORT);

  // // This will parse the primary packages
  // primary_interface::PrimaryParser parser;

  // // The producer needs both, the stream and the parser to fully work
  // comm::URProducer<primary_interface::PrimaryPackage> prod(primary_stream, parser);
  // prod.setupProducer();

  // // The shell consumer will print the package contents to the shell
  // std::unique_ptr<comm::IConsumer<primary_interface::PrimaryPackage>> consumer;
  // consumer.reset(new primary_interface::PrimaryShellConsumer());

  // // The notifier will be called at some points during connection setup / loss. This isn't fully
  // // implemented atm.
  // comm::INotifier notifier;

  // // Now that we have all components, we can create and start the pipeline to run it all.
  // comm::Pipeline<primary_interface::PrimaryPackage> pipeline(prod, consumer.get(), "Pipeline", notifier);
  // pipeline.run();

  // std::this_thread::sleep_for(std::chrono::seconds(2));

  // std::stringstream cmd;
  // cmd.imbue(std::locale::classic());  // Make sure, decimal divider is actually '.'
  // cmd << "sec setup():" << std::endl
  //     << " set_payload(" << 4 << ", [" << 0 << ", " << 0 << ", " << 0 << "])" << std::endl
  //     << "end";

  // std::string script_code = cmd.str();

  // auto program_with_newline = script_code + '\n';

  // size_t len = program_with_newline.size();
  // const uint8_t* data = reinterpret_cast<const uint8_t*>(program_with_newline.c_str());
  // size_t written;

  // primary_stream.write(data, len, written);

  // Package contents will be printed while not being interrupted
  // Note: Packages for which the parsing isn't implemented, will only get their raw bytes printed.

  int i = 0;
  while (true)
  {

    std::stringstream cmd;
    cmd.imbue(std::locale::classic());  // Make sure, decimal divider is actually '.'
    cmd << "sec setup():" << std::endl
        << " textmsg(\"Command through primary interface complete " << i++ << "\")" << std::endl
        << "end";

    std::string script_code = cmd.str();

    auto program_with_newline = script_code + '\n';

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    primary_client.sendScript(program_with_newline);
  }
  return 0;
}
