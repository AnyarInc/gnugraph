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

#include "gnugraph/GnuGraph.h"

using namespace std;

inline void pressEnter(void)
{
   cout << "Press ENTER to continue . . ." << '\n';
   cin.get();
}

void test1()
{
   GnuGraph graph("C:/Users/sberry/Desktop/Ascent/gnuplot/bin");
   string output = graph.plot("1 1\n2 4\n3 9\n4 16\n5 25\n6 36\n7 49\n8 64\n9 81\n10 100\n");
   cout << "Test1:\n" << output << '\n';
   pressEnter();
}

void test2()
{
   GnuGraph graph;

   vector<double> x, y;
   for (size_t i = 0; i < 200; ++i)
   {
      x.push_back(double(i));
      y.push_back(sqrt(x[i]));
   }

   string output = graph.plot(x, y, "y = sqrt(x)");
   //string output = graph.animate(x, y);
   cout << "Test2:\n" << output << '\n';
   pressEnter();
}

void test3()
{
   GnuGraph graph;

   vector<vector<double>> vec;
   vec.push_back({ 1.0, 1.0, 1.0 });
   vec.push_back({ 2.0, 2.0, 2.0 });
   vec.push_back({ 3.0, 4.0, 4.0 });
   vec.push_back({ 4.0, 8.0, 8.0 });
   vec.push_back({ 5.0, 16.0, 16.0 });
   vec.push_back({ 5.0, 32.0, 32.0 });
   cout << "Test3:\n" << graph.plotLine3D(vec) << '\n';
   pressEnter();
}

void test4()
{
   GnuGraph graph;

   vector<double> x0, y0, x1, y1;
   for (size_t i = 0; i < 200; ++i)
   {
      x0.push_back(double(i));
      y0.push_back(sqrt(x0[i]));

      x1.push_back(double(i));
      y1.push_back(pow(x1[i], 1.0 / 3.0));
   }

   graph.addPlot(x0, y0, "y = sqrt(x)");
   graph.addPlot(x1, y1, "y = x^(1/3)");
   string output = graph.plot();
   cout << "Test4:\n" << output << '\n';
   pressEnter();
}

void test5()
{
   GnuGraph graph;

   vector<vector<double>> vec;
   vec.push_back({ 1.0, 1.0, 1.0 });
   vec.push_back({ 2.0, 2.0, 2.0 });
   vec.push_back({ 3.0, 4.0, 4.0 });
   vec.push_back({ 4.0, 8.0, 8.0 });
   vec.push_back({ 5.0, 16.0, 16.0 });
   vec.push_back({ 5.0, 32.0, 32.0 });

   graph.addLine3D(vec);

   const vector<double> start{ 1.0, 1.0, 1.0 };
   const vector<double> direction{ 1.0, 0.0, 0.0 };

   graph.addVector3D(start, direction);

   cout << "Test5:\n" << graph.plot3D() << '\n';
   pressEnter();
}

void test6()
{
   GnuGraph graph;

   const size_t n = 200;
   vector<vector<double>> vec(n);
   for (size_t i = 0; i < n; ++i)
   {
      vec[i] = { cos(i / 20.0), sin(i / 30.0), cos(i / 50.0) };
   }

   graph.addLine3D(vec);
   cout << graph.plot3D() << '\n';
   pressEnter();
}

int main(int argc, char* argv[])
{
   test1();
   test2();
   test3();
   test4();
   test5();
   test6();

   return 0;
}