// Copyright 2022 The Chromium Authors.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "agent_win.h"
#include "session_win.h"

namespace content_analysis {
namespace sdk {

const DWORD kBufferSize = 4096;

// static
std::unique_ptr<Agent> Agent::Create(const Uri& uri) {
  return std::make_unique<AgentWin>(uri);
}

AgentWin::AgentWin(const Uri& uri) : AgentBase(uri) {
  pipename_ = "\\\\.\\pipe\\" + uri;
  CreatePipe(&hPipe_);
}

AgentWin::~AgentWin() {
  Shutdown();
}

std::unique_ptr<Session> AgentWin::GetNextSession() {
  std::unique_ptr<SessionWin> session;

  // Wait for a client to connect to the pipe.
  DWORD err = ConnectPipe(hPipe_);
  if (err == ERROR_SUCCESS) {
    // Create a session with the existing pipe handle.  The session now owns
    // the handle.
    session = std::make_unique<SessionWin>(hPipe_);
    hPipe_ = INVALID_HANDLE_VALUE;

    err = session->Init();
    if (err == ERROR_SUCCESS) {
      // Create a new pipe to accept a new client connection.
      err = CreatePipe(&hPipe_);
    }

    if (err != ERROR_SUCCESS) {
      session.reset();
    }
  }

  return session;
}

int AgentWin::Stop() {
  Shutdown();
  return AgentBase::Stop();
}

DWORD AgentWin::CreatePipe(HANDLE* handle) {
  DWORD err = ERROR_SUCCESS;

  *handle = CreateNamedPipe(pipename_.c_str(),
    PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT |
        PIPE_REJECT_REMOTE_CLIENTS,
    PIPE_UNLIMITED_INSTANCES, kBufferSize, kBufferSize, 0, nullptr);
  if (*handle == INVALID_HANDLE_VALUE) {
    err = GetLastError();
  }

  return err;
}

DWORD AgentWin::ConnectPipe(HANDLE handle) {
  BOOL connected = ConnectNamedPipe(handle, nullptr) ||
      (GetLastError() == ERROR_PIPE_CONNECTED);
  return connected ? ERROR_SUCCESS :  GetLastError();
}

void AgentWin::Shutdown() {
  if (hPipe_ != INVALID_HANDLE_VALUE) {
    CloseHandle(hPipe_);
  }
}

}  // namespace sdk
}  // namespace content_analysis