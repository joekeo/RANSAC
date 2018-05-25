/* 
Vicon Algorithm Software Engineering Test

In C++, implement the RANSAC algorithm to robustly estimate a line from noisy 2d points.

Tools: can use STL, boost, and C++ using a modern C++ compiler(up to C++ 11).
Input : a set of 2d points with unknown uncertainties, and any required algorithm parameters.
        A file containing a set of test points will be provided.
Outputs : the line parameterized as two points, and which input points are inliers.

Consider how the code might be organised to be easy to test, for others to read, and to improve later.
Use of the internet is encouraged to become familiar with details the algorithm and its variants; 
 copying/pasting directly from the internet is unlikely to be helpful.
Comments can be useful to explain design/implementation choices; there will also be an opportunity to explain
 these verbally.
Any reasonable extensions to the basic RANSAC algorithm would be welcome but please note in comments
 where these are made, and provide any relevant citations.

We have provided a simple command-line harness, including code to read and write the output.
Please feel free to remove the boost command line parser if you do not have access to boost.
*/
#include <fstream>
#include <iostream>
#include <iomanip>
#include <boost/program_options.hpp>
#include <ctime>
#include <cmath>
//to compile
//g++ main.cpp -lboost_program_options -o test
// simple point representation 
using TPoint = std::array<double, 2>;

// function to read a comma-separated list of 2d points
bool ReadInputFile(std::string const& Filename, std::vector<TPoint> & o_rData)
{
  o_rData.clear();

  std::ifstream File;
  File.open(Filename);
  if (!File.is_open())
  {
    return false;
  }

  double x, y;
  char sep;

  
//print the values on the file, as i like to test trivial stuff//remove this lines later if necessary.
//  for (size_t Index = 0; Index != Data.size(); ++Index)
//     {
//       std::cout << Data[Index][0] << " " << Data[Index][1] << " " "\n";
//     }

  
  // if successful, write the line parameterized as two po
  while (File >> x >> sep >> y)
  {
    o_rData.push_back({ x, y });
  }

  return true;
}

double GetDistance(const std::array<double, 2> & LP0,const std::array<double, 2> & LP1,const std::array<double, 2> & Point){
  
  double distance = abs(((LP1[1] - LP0[1]) * Point[0] - (LP1[0] - LP0[0]) * Point[1] + LP1[0]*LP0[1] - LP1[1]*LP0[0] )/ sqrt(pow((LP1[1]-LP0[1]),2) + pow(LP1[0]-LP0[0],2)));
  
  return distance;
}





int main(int argc, char** argv)
{

  // setup the input parameters 
  // filename of input data
  std::string InputFilename;

  // filename of output data
  std::string OutputFilename("output.txt");

  // required probability that a result will be generated from inliers-only
  double Confidence = 0.99;

  // threshold on error metric 
  double Threshold = 0.1;

  // approximate expected inlier fraction
  double ApproximateInlierFraction = 0.5;


  // setup the command line arguments
  namespace po = boost::program_options;
  po::options_description OptionsDescription("Allowed options");
  OptionsDescription.add_options()
    ("help", "produce help message")
    ("filename", po::value<std::string>(&InputFilename), "filename for input point data")
    ("out,o", po::value<std::string>(&OutputFilename)->default_value(OutputFilename), "filename for output data")
    ("confidence,c", po::value<double>(&Confidence)->default_value(Confidence), "Confidence")
    ("threshold,t", po::value<double>(&Threshold)->default_value(Threshold), "Threshold")
    ("inlier,i", po::value<double>(&ApproximateInlierFraction)->default_value(ApproximateInlierFraction), "Approximate Inlier Fraction")
    ;

  po::positional_options_description PositionalOptionsDescription;
  PositionalOptionsDescription.add("filename", -1);

  po::variables_map VariablesMap;
  po::store(po::command_line_parser(argc, argv).
    options(OptionsDescription).positional(PositionalOptionsDescription).run(), VariablesMap);
  po::notify(VariablesMap);

  if (VariablesMap.count("help"))
  {
    std::cout << OptionsDescription << "\n";
    return 1;
  }
  
  std::vector<TPoint> Data;
  if (!ReadInputFile(InputFilename, Data))
  {
    std::cerr << "Unable to read point data from file '" << InputFilename << "'" << std::endl;
    return 1;
  }

  

  // Output variables ... 
  std::vector<bool> Inliers;
  TPoint Point0;
  TPoint Point1;
  bool bSuccess(false);

  //Got some of the RANSAC basics from this paper
  //A Comparative Analysis of RANSAC
  //Techniques Leading to Adaptive Real-Time
  //Random Sample Consensus

  //randomize the random number generator
  //I am using the time because it is a number that changes almost every
  //time the program is initialised
  srand(time(NULL));

  //TPoints to store the
  //current selected points that make the test line
  //Line Point 1 and Line Point 2, and a the test point to test ,TP
  //using TPoint = std::array<double, 2>;
  std::array<double, 2> LPoint0; std::array<double, 2> LPoint1; std::array<double, 2> TP;
  std::vector<bool> TInliers;

  //lets create a variable to count how many of the points are inliners
  int numOfIn = 0;

  //and another variable to store the max number of inliners acheived by the last 
  //selection of points
  int maxNumOfIn = 0;

  //number of inliners / number of data points
  double w = 0.00; double p = 0.00;
  
  //Set the number of iterations to do based on the 
  //confidence required and the approximated inliner fraction
  int k = log(1-Confidence)/log(1-pow(ApproximateInlierFraction,2));

  for(int i = 0; i <= k; i++){
    //choose tw o random points from the sample points
    //using a randomised random number generator
    int testP0 = (rand() % Data.size());
    LPoint0[0] = Data[testP0][0];   LPoint0[1] = Data[testP0][1];

    int testP1 = (rand() % Data.size());
    LPoint1[0] = Data[testP1][0];   LPoint1[1] = Data[testP1][1];
  
    //reset the number of inliner for the nex iteration
    numOfIn = 0;
    //clear the list on inliers for this iteration
    TInliers.clear();

    for (size_t Index = 0; Index != Data.size(); ++Index)
      {
        
        //test the choosen test point
        TP[0] = Data[Index][0]; TP[1] = Data[Index][1];
        if(GetDistance(LPoint0,LPoint1,TP) <= Threshold){
          //if the point is within the desired distance to the line
          //add assing a true to the same index of the inliers vector
          TInliers.push_back(true);
          //and add to the count of number of inliers for this line model
          numOfIn++;
        }
        else TInliers.push_back(false); 
      }
    //save the maximum number of inliners found in the data set and the points that form them
    if(maxNumOfIn < numOfIn){
      maxNumOfIn = numOfIn;
      Point0[0] = LPoint0[0]; Point0[1] = LPoint0[1]; Point1[0] = LPoint1[0]; Point1[1] = LPoint1[1];
      //print the current champion
      //std::cout << "X1: " << Point0[0] << " Y1: " << Point0[1] << "  X2: " << Point1[0] << " Y2: " << Point1[1] << '\n';
      //copy the inliners to the Otput inliners
      Inliers = TInliers;
    }
    
  } 
  //it is always a success because it is running for a certain
  //numner of iterations
  bSuccess = true; 
  //prunt the number of inliners detected
  //std::cout << maxNumOfIn << '\n';
  // if successful, write the line parameterized as two points, and each input point along with its inlier status
  if (bSuccess)
  {
    std::ofstream Out(OutputFilename);
    Out << Point0[0] << " " << Point0[1] << "\n";
    Out << Point1[0] << " " << Point1[1] << "\n";
    assert(Inliers.size() == Data.size());
    for (size_t Index = 0; Index != Data.size(); ++Index)
    {
      Out << Data[Index][0] << " " << Data[Index][1] << " " << Inliers[Index] << "\n";
    }
    Out.close();
  }

  return 0;
}