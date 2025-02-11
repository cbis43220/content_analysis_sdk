// Copyright 2022 The Chromium Authors.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <fstream>
#include <iostream>
#include <string>

#include "content_analysis/sdk/analysis_agent.h"
#include "demo/handler.h"

int main(int argc, char* argv[]) {
  // Each agent uses a unique name to identify itself with Google Chrome.
  content_analysis::sdk::ResultCode rc;
  auto agent = content_analysis::sdk::Agent::Create({"content_analysis_sdk"},
      std::make_unique<Handler>(), &rc);
  if (!agent || rc != content_analysis::sdk::ResultCode::OK) {
    std::cout << "[Demo] Error starting agent: "
              << content_analysis::sdk::ResultCodeToString(rc)
              << std::endl;
    return 1;
  };

  std::cout << "[Demo] " << agent->DebugString() << std::endl;

  // Blocks, sending events to the handler until agent->Stop() is called.
  rc = agent->HandleEvents();
  if (rc != content_analysis::sdk::ResultCode::OK) {
    std::cout << "[Demo] Error from handling events: "
              << content_analysis::sdk::ResultCodeToString(rc)
              << std::endl;
    std::cout << "[Demo] " << agent->DebugString() << std::endl;
  }

  return 0;
};
