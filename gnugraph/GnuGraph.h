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
   void addLine3D(const T& input, const std::string& title = "")
   {
      std::string formatted;
      for (unsigned i = 0; i < input.size(); ++i)
         formatted += format(input[i]) + "\n";

      data.emplace_back(formatted);
      if (!initialized)
         titles.push_back(title);
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
   std::string plotLine3D(const T& input, const std::string& title = "")
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

private:
   std::string line_type = "lines";

   std::string setup{}; // how data is to be displayed on the graph and what type of graph
   std::vector<std::string> data;
   std::vector<std::string> data_vectors; // data for drawing vectors
   std::vector<std::string> titles;

   bool mode_2D = true;
   bool initialized = false;

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
      }
      else
         write(setup + data.front() + "e\n");

      data.clear();
      data_vectors.clear();

      return read();
   }
};