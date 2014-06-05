// STD includes
#include <iostream>
#include <string>

// VruiVTK includes
#include "VruiVTK.h"

void printUsage(int argc, char* argv[])
{
  std::cout << "Usage: " << argv[0] << " filename\n" << std::endl;
  std::cout << "where," << std::endl;
  std::cout << "\tfilename = Full path for OBJ file to load using VTK";
  std::cout << std::endl;
}

/* Create and execute an application object: */
/*
 * main - The application main method.
 *
 * parameter argc - int
 * parameter argv - char**
 *
 */
int main(int argc, char* argv[])
{
  if(argc < 1)
    {
    std::cerr << "ERROR: No argument specified\n" << std::endl;
    printUsage(argc, argv);
    return 1;
    }
//  else if(std::string(argv[1]) == "--help")
//    {
//    printUsage(argc, argv);
//    return 0;
//    }

  try
    {
    VruiVTK application(argc, argv);
    if(argc > 1)
      {
      application.setFileName(argv[1]);
      }
    application.run();
    return 0;
    }
  catch (std::runtime_error e)
    {
    std::cerr << "Error: Exception " << e.what() << "!" << std::endl;
    return 1;
    }
}
