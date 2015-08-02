#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>


int ReadInputSampleFile   (const std::string & ,std::vector<std::string> &, std::vector<std::string> &, std::vector<int> &,  std::vector<double> &, std::vector<int> &);

int ReadInputVariableFile (const std::string & ,std::vector<std::string> &, std::vector<int> &, std::vector<double> & ,std::vector<double> &, std::vector<std::string> & );

int ReadInputVariableFile (const std::string & ,std::vector<std::string> &);

int ReadInputVariableBlindedFile( const std::string & , std::vector<std::string> & , std::vector<int> & , std::vector<double> & , std::vector<double> & ,  
                                  std::vector<double> & , std::vector<double> & , std::vector<std::string> & );

int ReadInputCutFile      (const std::string & ,std::vector<std::string> &);

