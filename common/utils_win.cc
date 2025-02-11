// Copyright 2022 The Chromium Authors.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include <windows.h>
#include <sddl.h>

#include "utils_win.h"

namespace content_analysis {
namespace sdk {
namespace internal {

std::string GetUserSID() {
  std::string sid;

  HANDLE handle;
  if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &handle) &&
    !OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &handle)) {
    return std::string();
  }

  DWORD length = 0;
  std::vector<char> buffer;
  if (!GetTokenInformation(handle, TokenUser, nullptr, 0, &length)) {
    DWORD err = GetLastError();
    if (err == ERROR_INSUFFICIENT_BUFFER) {
      buffer.resize(length);
    }
  }
  if (GetTokenInformation(handle, TokenUser, buffer.data(), buffer.size(),
    &length)) {
    TOKEN_USER* info = reinterpret_cast<TOKEN_USER*>(buffer.data());
    char* sid_string;
    if (ConvertSidToStringSidA(info->User.Sid, &sid_string)) {
      sid = sid_string;
      LocalFree(sid_string);
    }
  }

  CloseHandle(handle);
  return sid;
}

std::string GetPipeName(const std::string& base, bool user_specific) {
  std::string pipename = "\\\\.\\pipe\\" + base;
  if (user_specific) {
    std::string sid = GetUserSID();
    if (sid.empty())
      return std::string();

    pipename += "." + sid;
  }

  return pipename;
}

DWORD CreatePipe(
    const std::string& name,
    bool is_first_pipe,
    HANDLE* handle) {
  DWORD err = ERROR_SUCCESS;
  DWORD mode = PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED;

  // When `is_first_pipe` is true, the agent expects there is no process that
  // is currently listening on this pipe.  If there is, CreateNamedPipeA()
  // returns with ERROR_ACCESS_DENIED.  This is used to detect if another
  // process is listening for connections when there shouldn't be.
  if (is_first_pipe) {
    mode |= FILE_FLAG_FIRST_PIPE_INSTANCE;
  }
  *handle = CreateNamedPipeA(name.c_str(), mode,
    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT |
    PIPE_REJECT_REMOTE_CLIENTS, PIPE_UNLIMITED_INSTANCES, kBufferSize,
    kBufferSize, 0, /*securityAttr=*/nullptr);
  if (*handle == INVALID_HANDLE_VALUE) {
    err = GetLastError();
  }

  return err;
}

}  // internal
}  // namespace sdk
}  // namespace content_analysis
