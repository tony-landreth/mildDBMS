#ifndef SELECTION_H
#define SELECTION_H
#include <map>
#include <string>

#include "FileScan.h"

/*
   Selection nodes process WHERE clauses in SELECT statements.
   For example, `SELECT * FROM test_data where movieId = 3;`
   will return only rows where movieId is equal to 3.
*/

class Selection : public PlanNode {
 public:
  // Method declarations
  Selection(const std::vector<std::string> keys,
            const std::unique_ptr<FileScan> fs, const Schema sch);
  std::vector<std::string> next();
  void rewind(){};  // presently used only on Projection nodes

  // Variable declarations
  std::vector<std::string> keys;

 private:
  // Method declarations

  // Variable declarations
  Schema schema_;
  std::unique_ptr<FileScan> fs_;
  int rowIdx_ = 0;
};

#endif
