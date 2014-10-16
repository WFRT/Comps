Description of verification system
----------------------------------

Code in this directory can be used to verify NetCDF files produced by COMPS. COMPS outputs
these files by including 'outputs=verif' in the run specification. The verification files
will be located in $COMPS/results/<runName>/verif/.

./verif is the main program for extracting verification results from these NetCDF files. For a full
description of its features, run the command without arguments.

Verification consists of two components: Metrics and Outputs.
- Metrics define procedures for computing verification scores
  - such as root mean squared error or correlation
- Outputs describe how the scores are displayed.
  - This can include plots as well as text

Metric or Output?
-----------------
The rule of thumb is that a simple metric can usually be implemented in Metric, whereas a complex
one often requires a new output. Metrics can implement any scoring function that gives a single
value given a sequence of forecast and observation pairs.

The reliability diagram is implemented in Output, as the metric is not a single value for a given
sequence of forecast/observation pairs.

Adding a new Metric
-------------------
1) Set up class in Metric.py
   Most metrics have the following format:
   `
   class MyMetric(Metric):
      def computeCore(self, data, tRange):
         <code here>
      def description():
         return "description of metric"
   `
   The `computeCore(self, data, tRange)` function consists of code that computes the score, based on
   forecasts and observations from `data`. You can access data from the data object. 'data' will
   return values for a pre-set range of locations, dates, and offsets. Also, values from only
   one verification file is returned. This flexibility allows the system to plot verification
   scores as a function of date or location, for example.

   `[obs,fcst] = data.getScores(["obs", "fcst"])` will return arrays of matching observations and
   forecasts.

   You should compute the score based on the lower and upper threshold specified in 'tRange'. For
   many metrics, these values can be ignored, as the score is independent of any threshold. For
   other scores, such as hit rate, 'tRange' specifies that the hit rate should be computed based on
   forecasts landing within the threshold range. This flexibility allows the system to show hit rate
   as a function of threshold.

   Implement description()
   Running ./verif without arguments gives a list of available metrics. If the description function
   is implemented, then this metric will appear here.

2) Implement optional functions
   A number of other optional functions can be implemented, which affect how the metric is plotted.
   For example the 'min' function represents the lowest value that the metric can take on. Plotting
   the metric will set the lower y-axis limit to this value. Check the documentation in Metric.py.
   If your metric is bounded from below by 0, use this:
   `def min(self):
       return 0`

3) Add a line in ./verif to indicate that if the user passes `-m mymetric` to ./verif, then it
   should use your metric:
   `elif(metric == "mymetric"):
       m = Metric.MyMetric() `

Adding a new Output
-------------------
1) Set up class in Output.py
   `
   class MyOutput(Output):
      def _plotCore(self, data):
         F = data.getNumFiles()
         for f in range(0, F):
            data.setFileIndex(f)
      def _textCore(self, data):
   `
   To be continued...

Testing
-------
A simple test script is included to check that the system is working. The test script produces
graphs of many different metrics and plotting options, and are places in ./testPlots/. The tests are
not very thorough however.
