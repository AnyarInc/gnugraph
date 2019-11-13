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

#include "gnugraph/GnuGraphFormatter.h"
#include "gnugraph/GnuGraphPiping.h"

#include <vector>
#include <filesystem>

struct GnuGraph : public gnugraph::GnuGraphFormatter, public gnugraph::GnuGraphPiping
{
   GnuGraph(const std::string& gnuplot_exe_path = "C:/Program Files/gnuplot/bin/gnuplot.exe") : gnugraph::GnuGraphPiping(gnuplot_exe_path) {}

   void lineType(const std::string& line_type) { this->line_type = line_type; }

   //void clear()
   //{
   //   //initialized = false;
   //   write("clear\n");
   //}

   std::string plot()
   {
      setup2D();
      return writeRead();
   }

   std::string plot3D()
   {
      setup3D();
      return writeRead();
   }
   
   std::string plot(const std::string& input)
   {
      data.push_back(input);

      setup2D();
      return writeRead();
   }

   std::string plot(const double& x, const double& y)
   {
      data.emplace_back(format(x, y) + '\n');

      setup2D();
      return writeRead();
   }

   template <typename T> // designed for std::container<double>
   void addPlot(T& x, T& y, const std::string& title = "") // adds data to be plotted and doesn't plot yet
   {
      std::string formatted;
      for (unsigned i = 0; i < x.size(); ++i)
         formatted += format(x[i], y[i]) + "\n";

      data.emplace_back(formatted);
      if (!initialized)
         titles.push_back(title);
   }

   template <typename T> // designed for std::container<double>
   std::string plot(T& x, T& y, const std::string& title = "")
   {
      addPlot(x, y, title);
      return plot();
   }

   template <typename T> // designed for a 2D point (i.e. Eigen::Vector2d)
   void addPlot2D(const T& input, const std::string& title = "")
   {
      data.emplace_back(format(input) + "\n");
      if (!initialized)
         titles.push_back(title);
   }

   template <typename T> // designed for a 3D point (i.e. Eigen::Vector3d)
   void addPlot3D(const T& input, const std::string& title = "")
   {
      data.emplace_back(format(input) + "\n");
      if (!initialized)
         titles.push_back(title);
   }

   template <typename T> // designed for a 3D point (i.e. Eigen::Vector3d)
   std::string plot3D(const T& input)
   {
      data.emplace_back(format(input) + "\n");
      return plot3D();
   }

   template <typename T> // designed for a std::container of vectors (i.e. std::container<Eigen::Vector3d>)
   void addLine3D(const T& input)
   {
      std::string formatted;
      for (unsigned i = 0; i < input.size(); ++i)
      {
         formatted += format(input[i]) + "\n";
      }

      data.emplace_back(formatted);
      if (!initialized)
      {
         titles.push_back("");
      }
   }

   template <typename T> // designed for a std::container of vectors (i.e. std::container<Eigen::Vector3d>)
   void addLine3DTitle(const T& input, const std::string& title)
   {
      std::string formatted;
      for (unsigned i = 0; i < input.size(); ++i)
      {
         formatted += format(input[i]) + "\n";
      } 

      data.emplace_back(formatted);
      if (!initialized)
      {
         titles.push_back(title);
      }   
   }

   template <typename T> // designed for a std::container of vectors (i.e. std::container<Eigen::Vector3d>)
   void addLineSparse3D(const T& input, const unsigned r, const std::string& title = "")
   {
      std::string formatted;
      unsigned i = 0;
      for (; i < input.size(); i += r)
         formatted += format(input[i]) + "\n";

      // we want to still plot all the most recent samples for a smooth front end
      i -= r;
      ++i;
      while (i < input.size())
      {
         formatted += format(input[i++]) + "\n";
      }

      data.emplace_back(formatted);
      if (!initialized)
         titles.push_back(title);
   }

   template <typename T, typename... Trest> // designed for std::container<Eigen::Vector3d>
   void addLine3D(const T& input, const Trest&... rest)
   {
      addLine3D(input);
      addLine3D(rest...);
   }

   template <typename T> // designed for std::container<Eigen::Vector3d>
   std::string plotLine3D(const T& input)
   {
      addLine3D(input);
      return plot3D();
   }

   template <typename T> // designed for std::container<Eigen::Vector3d>
   std::string plotLine3DTitle(const T& input, const std::string& title = "")
   {
      addLine3D(input, title);
      return plot3D();
   }

   template <typename T, typename... Trest> // designed for std::container<Eigen::Vector3d>
   std::string plotLine3D(const T& input, const Trest&... rest)
   {
      addLine3D(input);
      addLine3D(rest...);
      return plot3D();
   }

   template <typename T> // designed for std::container<double>
   std::string animate(const T& x, const T& y)
   {
      T x_segment, y_segment;

      std::string result;

      for (unsigned i = 0; i < x.size(); ++i)
      {
         x_segment.push_back(x[i]);
         y_segment.push_back(y[i]);
         result += plot(x_segment, y_segment);
      }

      return result;
   }

   template <typename T> // designed for std::container<Eigen::Vector3d>
   std::string animateLine3D(const T& input)
   {
      T segment;

      string result;

      for (unsigned i = 0; i < input.size(); ++i)
      {
         segment.push_back(input[i]);
         result += plot4D(segment);
      }

      return result;
   }

   // Draw a vector in 3D cartesian coordinates from two 3D points (i.e. use Eigen::Vector3d)
   template <typename T>
   void addVector3D(const T& start, const T& direction, const std::string& title = "")
   {
      data_vectors.emplace_back(format(start) + format(direction) + '\n');
      if (!initialized)
         titles.push_back(title);
   }

   // Activates gif output from gnuplot, call this before any graphing has occured. This will save a .gif file
   //    in the active directory.
   // Note: Only one output format is allowed to be activated at a time. If more than one is called, system will
   //    only use the first output format called.
   void addGif(const std::string& file_name)
   {
      if (add_image_sequence)
      {
         std::cout << "Warning: gnuplot only supports one output format at a time\n";
         return;
      }

      output_name = file_name;
      add_gif = true;
   }

   // Activates image sequence output from gnuplot, call this before any graphing has occured. This will dump all
   //    .png frames into a folder called 'output' in the active directory.
   // Note: Only one output format is allowed to be activated at a time. If more than one is called, system will
   //    only use the first output format called.
   void addImageSequence(const std::string& file_name)
   {
      if (add_gif)
      {
         std::cout << "Warning: gnuplot only supports one output format at a time\n";
         return;
      }

      output_name = file_name;
      std::filesystem::create_directories("./output");
      add_image_sequence = true;
   }

   // Closes output file in gnuplot. Call this at end of graph processing to save output file.
   // Note: Works for both gif and image sequence
   std::string closeOutput()
   {
      write("unset output\n");
      return read();
   }

private:
   std::string line_type = "lines";

   std::string setup{}; // how data is to be displayed on the graph and what type of graph
   std::vector<std::string> data;
   std::vector<std::string> data_vectors; // data for drawing vectors
   std::vector<std::string> titles;

   bool mode_2D = true;
   bool initialized = false;

   // GIF and Image Sequence Parameters
   bool add_image_sequence = false;  // Flag for if image sequence output is activated
   bool add_gif = false;  // Flag for if gif output is activated
   std::string output_name{};  // Name (without extension) of output file
   size_t frame_id = 1;  // Id number of current frame, starts at 1. Maximum frame number is 99999 (~100 secs @ 0.01 dt)
   std::string frame;  // String version of frame ID, used internally to name image output

   void setup2D()
   {
      if (!mode_2D)
      {
         initialized = false;
         mode_2D = true;
         write("clear\n");
      }

      if (!initialized)
      {
         //setup += "set term windows\n"; // gnuplot command
         std::string title;
         if (titles.size() > 0)
            title = titles.front();

         setup += "plot '-' ";	// "-" for realtime plotting
         setup += "using 1:2 ";
         setup += "title '" + title + "' ";
         setup += "with " + line_type;

         for (size_t i = 1; i < data.size(); ++i)
         {
            if (titles.size() == data.size())
               title = titles[i];
            setup += ", '-' using 1:2 title '" + title + "' with " + line_type;
         }

         setup += "\n";
         initialized = true;
      }
      else
         setup = "replot\n";
   }

   void setup3D()
   {
      if (mode_2D)
      {
         initialized = false;
         mode_2D = false;
         write("clear\n");
      }

      if (!initialized)
      {
         // Check for output options
         if (add_gif)
            setupGif();
         else if (add_image_sequence)
            setupImageSequence();

         //setup += "set term windows\n"; // gnuplot command
         std::string title;
         if (titles.size() > 0)
            title = titles.front();

         setup += "splot '-' ";	// "-" for realtime plotting
         setup += "using 1:2:3 ";
         setup += "title '" + title + "' ";
         setup += "with " + line_type;

         for (size_t i = 1; i < data.size(); ++i)
         {
            if (titles.size() == data.size())
               title = titles[i];
            setup += ", '-' using 1:2:3 title '" + title + "' with " + line_type;
         }

         for (size_t i = 0; i < data_vectors.size(); ++i)
         {
            if (titles.size() == data_vectors.size())
               title = titles[i];
            setup += ", '-' using 1:2:3:4:5:6 title '" + title + "' with vectors filled head lw 2";
         }

         setup += "\n";
         initialized = true;
      }
      else
         setup = "replot\n";
   }

   std::string writeRead()
   {
      std::string input{};

      if (data.size() > 1 || data_vectors.size() > 0)
      {
         for (const auto& i : data)
            input += i + "e\n";

         for (const auto& i : data_vectors)
            input += i + "e\n";

         write(setup + input);

         // export frame
         if (add_image_sequence)
            exportImageFrame();
      }
      else
      {
         write(setup + data.front() + "e\n");
         
         // export frame
         if (add_image_sequence)
            exportImageFrame();
      }
      data.clear();
      data_vectors.clear();

      return read();
   }

   // Calls necessary commands to enable gif output for gnuplot
   std::string setupGif()
   {
      std::string result = "";
      write("set terminal gif animate delay .001\n");
      result = read();
      write("set output '" + output_name + ".gif'\n");
      result += read();

      return result;
   }

   // Calls necessary commands to enable png output for gnuplot
   std::string setupImageSequence()
   {
      std::string result = "";
      write("set terminal pngcairo\n");
      result = read();
      write("set output 'output/" + output_name + "00001.png'\n");
      result += read();
      add_image_sequence = true;

      return result;
   }

   // Increments frame count and sets up next frame to be saved, this simultaneously saves the previous frame in gnuplot
   void exportImageFrame()
   {
      ++frame_id;
      std::string frame_number = std::to_string(frame_id);
      write("set output 'output/" + output_name + std::string(5 - frame_number.length(), '0') + frame_number + ".png'\n");
   }
};