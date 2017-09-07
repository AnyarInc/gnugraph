// Copyright (c) 2016-2017 Anyar, Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//      http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Some code and design is built off or inspired by Andrew Loblaw's GNUplot open source project: https://github.com/Andy11235813/GNUPlot

#pragma once

#include <iostream>
#include <windows.h>

namespace gnugraph
{
   struct GnuGraphPiping
   {
      GnuGraphPiping(const std::string& gnuplot_exe_path) : gnuplot_exe(gnuplot_exe_path)
      {
         createPipes();
         startProcess();
      }

      ~GnuGraphPiping()
      {
         write("quit\n"); // command gnuplot to quit

         // Close the handle to the process and thread
         CloseHandle(process_information.hProcess);
         CloseHandle(process_information.hThread);

         if (!CloseHandle(input_read_handle))
            errorExit("StdInWr CloseHandle");
         if (!CloseHandle(input_write_handle))
            errorExit("StdInWr CloseHandle");
         if (!CloseHandle(output_read_handle))
            errorExit("StdInWr CloseHandle");
         if (!CloseHandle(output_write_handle))
            errorExit("StdInWr CloseHandle");
      }

   private:
      //static const unsigned long buffer_size = 65536;
      static const unsigned long buffer_size = 4096;
      const std::string gnuplot_exe;

      // Pipe handle variables:
      HANDLE input_read_handle; // child process read-pipe handle
      HANDLE input_write_handle;	// parent process write-pipe handle
      HANDLE output_read_handle; // parent process read-pipe handle
      HANDLE output_write_handle; // child process write-pipe handle

      PROCESS_INFORMATION process_information; // process information struct

   protected:
      void errorExit(const std::string& description)
      {
         std::cout << "ERROR: " << description << '\n';
      }

      void write(const std::string& command)
      {
         size_t to_write = command.length();
         unsigned long written = 0;
         int success = WriteFile(input_write_handle, command.c_str(), (DWORD)to_write, &written, nullptr);
         if (!success || written != to_write)
            errorExit("GnuGraph::write");
      }

      std::string read() // Read the reply from gnuplot.exe
      {
         /* Peek the pipe to see if data is available to be read. If no data available
         *	then don't try and read (i.e. prevent ReadFile from blocking by not calling it). */
         unsigned long total_bytes_available;
         PeekNamedPipe(output_read_handle, nullptr, buffer_size, nullptr, &total_bytes_available, nullptr);

         char char_buf[buffer_size]{};

         if (total_bytes_available > 0)
         {
            unsigned long to_read = buffer_size;
            unsigned long read = 0;

            int success = ReadFile(output_read_handle, char_buf, to_read, &read, nullptr);
            if (!success)
               errorExit("GnuGraph::read");
         }

         std::string result(char_buf);
         return result;
      }

      /* This function simply creates the pipes used to interface with gnuplot.exe.
      *	It MUST be called before StartProcess(), otherwise StartProcess() will not
      * have initialized pipe handles to call gnuplot.exe with, and an error will occur.
      */
      void createPipes()
      {
         SECURITY_ATTRIBUTES saAttr;
         saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
         saAttr.bInheritHandle = true; // Set the bInheritHandle flag so pipe handles are inherited. 
         saAttr.lpSecurityDescriptor = nullptr;

         /* Create a pipe for the child process's STDIN. */
         if (!CreatePipe(&input_read_handle, &input_write_handle, &saAttr, 0))
            errorExit("Stdin CreatePipe");

         // Ensure the write handle to the pipe for STDIN is not inherited. 
         if (!SetHandleInformation(input_write_handle, HANDLE_FLAG_INHERIT, 0))
            errorExit("Stdin SetHandleInformation");

         /* Create a pipe for the child process's STDOUT. */
         if (!CreatePipe(&output_read_handle, &output_write_handle, &saAttr, 0))
            errorExit("Stdout CreatePipe");

         // Ensure the read handle to the pipe for STDOUT is not inherited. 
         if (!SetHandleInformation(output_read_handle, HANDLE_FLAG_INHERIT, 0))
            errorExit("Stdin SetHandleInformation");
      }

      /* This function starts the gnuplot.exe process and sets the appropriate pipe
      *	handles for communication. The class member variable m_ProcInfo is returned
      * containing a handle to the process, and the thread it is contained in. The
      *	path to gnuplot.exe is assumed to be "C:\Gnuplot\binary", if it is not, then
      * modify the "const TCHAR *GNUPLOT_EXE" declaration.
      */
      void startProcess()
      {
         STARTUPINFO StartInfo;
         // Set up members of the PROCESS_INFORMATION structure. 
         ZeroMemory(&process_information, sizeof(PROCESS_INFORMATION));

         // Set up members of the STARTUPINFO structure. 
         // This structure specifies the STDIN and STDOUT handles for redirection.
         ZeroMemory(&StartInfo, sizeof(STARTUPINFO));
         StartInfo.cb = sizeof(STARTUPINFO);
         StartInfo.dwX = 0; // Starting x-position in pixels
         StartInfo.dwY = 0; // Starting y-position in pixels
         StartInfo.dwXSize = 800; // Starting width in pixels
         StartInfo.dwYSize = 800; // Starting height in pixels
         StartInfo.hStdError = output_write_handle;
         StartInfo.hStdOutput = output_write_handle;
         StartInfo.hStdInput = input_read_handle;
         StartInfo.dwFlags |= STARTF_USESTDHANDLES | STARTF_USEPOSITION | STARTF_USESIZE;

         /* Initialize the Creation Flags variable with the appropriate flags set
         * The DETACHED_PROCESS flag prevents the gnuplot window from forcing it's way
         *	to the front when the command line becomes active. */
         unsigned long dwCreationFlags = DETACHED_PROCESS;

         // Create the child process. 
         if (!CreateProcess(gnuplot_exe.c_str(),
            nullptr,						// command line 
            nullptr,						// process security attributes 
            nullptr,						// primary thread security attributes 
            true,						// handles are inherited 
            dwCreationFlags,							// creation flags 
            nullptr,						// use parent's environment 
            nullptr,						// use parent's current directory 
            &StartInfo,		// STARTUPINFO pointer 
            &process_information /* receives PROCESS_INFORMATION */))
         {
            errorExit("CreateProcess");
         }
      }
   };
}