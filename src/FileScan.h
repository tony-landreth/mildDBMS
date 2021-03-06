#ifndef FILESCAN_H
#define FILESCAN_H
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "PlanNode.h"

//  FileScan nodes scan data from disk and store it in memory.
//  Each table in the database is its own file on disk.
//  In more realistic DBMSs, only a portion of file data would be read into
//  memory. The present design is a first-pass, so whole files are read all at
//  once. One FileScan node opens one file.

class FileScan : public PlanNode {
 public:
  FileScan(const Schema schema);

  // Method declarations
  std::vector<std::string> next();
  void rewind(){};
  void scanFile();

  // Variable declarations
  Schema schema;

 private:
  // Method declarations
  std::vector<std::string> next_();

  // Variable declarations
  Schema schema_;
  int idx_for_next_;

  // This is the vector that will contain the file read
  std::unique_ptr<std::vector<std::vector<std::string> > > relation_ =
      std::make_unique<std::vector<std::vector<std::string> > >();
  std::ifstream fileStream_;
};
#endif
