# gnugraph

A header only interface to gnuplot.exe, to plot 2D and 3D graphs from C++ code.

## Basic Example
``` C++
int main()
{
   GnuGraph graph("C:/Program Files/gnuplot/bin/gnuplot.exe"); // provide path to executable

   vector<double> x, y;
   for (size_t i = 0; i < 200; ++i)
   {
      x.push_back(double(i));
      y.push_back(sqrt(x[i]));
   }

   const string output = graph.plot(x, y, "y = sqrt(x)");
   cout << output << '\n'; // print any errors to console
   cin.get(); // keep the window open until the user presses ENTER
   return 0;
}
```

> Resulting graph
<img src="https://github.com/AnyarInc/gnugraph/wiki/graphics/gnugraph-sqrt.PNG" width="60%">

## Features
- Supports runtime plotting from C++
- Supports animated plotting
- Supports std container plotting
- Supports Eigen linear algebra vector plotting

### Currently supports Gnuplot 4.6
### Currently Windows only support (uses Windows piping)

## 3D Plotting Example
``` C++
int main()
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
   cin.get();
   return 0;
}
```

> Resulting graph
<img src="https://github.com/AnyarInc/gnugraph/wiki/graphics/gnugraph-3D.PNG" width="60%">
