// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <windows.h>

#include <vector>

static std::wstring RTrim(const std::wstring& str) {
  size_t endpos = str.find_last_not_of(L" \t\n");
  if (std::wstring::npos != endpos)
    return str.substr( 0, endpos+1 );
  return str;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("usage: graberr file_for_errors.txt\n");
    return 1;
  }

  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

#if TEST
printf(
"[next-rdhi]d:\\src\\cr3\\src>ninja -C out\\Debug content_unittests -j1\n"
"ninja: Entering directory `out\\Debug'\n"
"[13->1/14 ~0] CXX obj/content/browser/browser/loader_delegate_impl.obj\n"
"FAILED: obj/content/browser/browser/loader_delegate_impl.obj\n"
"ninja -t msvc -e environment.x86 -- \"d:\\src\\depot_tools\\win_toolchain\\vs_files\\283cc362f57dbe240e0d21f48ae45f9d834a425a\\VC\\bin\\amd64_x86/cl.exe\" /nologo /showIncludes /FC @obj/content/browser/browser/loader_delegate_impl.obj.rsp /c ../../content/browser/loader_delegate_impl.cc /Foobj/content/browser/browser/loader_delegate_impl.obj /Fd\"obj/content/browser/browser_cc.pdb\"\n"
"d:\\src\\cr3\\src\\content\\browser\\loader_delegate_impl.cc(55): error C2027: use of undefined type 'content::RenderFrameHostImpl'\n"
"d:\\src\\cr3\\src\\content\\browser\\renderer_host\\render_view_host_impl.h(319): note: see declaration of 'content::RenderFrameHostImpl'\n"
"d:\\src\\cr3\\src\\content\\browser\\loader_delegate_impl.cc(55): error C3861: 'FromID': identifier not found\n"
"d:\\src\\cr3\\src\\content\\browser\\loader_delegate_impl.cc(56): error C2065: 'WebContentsImpl': undeclared identifier\n"
"d:\\src\\cr3\\src\\content\\browser\\loader_delegate_impl.cc(56): error C2065: 'web_contents': undeclared identifier\n"
"d:\\src\\cr3\\src\\content\\browser\\loader_delegate_impl.cc(57): error C2061: syntax error: identifier 'WebContentsImpl'\n"
"d:\\src\\cr3\\src\\content\\browser\\loader_delegate_impl.cc(57): error C2653: 'WebContents': is not a class or namespace name\n"
"d:\\src\\cr3\\src\\content\\browser\\loader_delegate_impl.cc(58): error C2065: 'web_contents': undeclared identifier\n"
"d:\\src\\cr3\\src\\content\\browser\\loader_delegate_impl.cc(60): error C2065: 'web_contents': undeclared identifier\n"
"d:\\src\\cr3\\src\\content\\browser\\loader_delegate_impl.cc(60): error C2227: left of '->DidGetRedirectForResourceRequest' must point to class/struct/union/generic type\n"
"d:\\src\\cr3\\src\\content\\browser\\loader_delegate_impl.cc(60): note: type is 'unknown-type'\n"
"ninja: build stopped: subcommand failed.\n");
#endif

  CONSOLE_SCREEN_BUFFER_INFO buffer_info;
  if (!GetConsoleScreenBufferInfo(console, &buffer_info)) {
    abort();
  }

  CHAR_INFO* buffer =
      new CHAR_INFO[buffer_info.dwSize.X * buffer_info.dwSize.Y];
  COORD top_left = {0,0};
  SMALL_RECT read_region = {
      0, 0, buffer_info.dwSize.X - 1, buffer_info.dwSize.Y - 1};
  if (!ReadConsoleOutput(
          console, buffer, buffer_info.dwSize, top_left, &read_region)) {
    abort();
  }

  std::vector<std::wstring> lines;
  lines.reserve(buffer_info.dwSize.Y);
  for (int y = 0 ; y < buffer_info.dwSize.Y; ++y) {
    std::wstring line;
    line.reserve(buffer_info.dwSize.X);
    for (int x = 0; x < buffer_info.dwSize.X; ++x) {
      line.push_back(buffer[y * buffer_info.dwSize.X + x].Char.UnicodeChar);
    }
    lines.push_back(line);
  }

  std::vector<std::wstring> to_last_invocation;
  for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
    const std::wstring& line = *it;
    to_last_invocation.push_back(RTrim(line));
    if (line.find(L">ninja -C ") != std::wstring::npos)
      break;
  }

  std::reverse(to_last_invocation.begin(), to_last_invocation.end());

  FILE* out;
  if (fopen_s(&out, argv[1], "w") != 0) {
    printf("couldn't open %s\n", argv[1]);
    return 1;
  }

  for (const auto& line : to_last_invocation)
    fprintf(out, "%ls\n", line.c_str());
  fclose(out);

  return 0;
}
