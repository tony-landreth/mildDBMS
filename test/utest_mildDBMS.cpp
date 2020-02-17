#include <iostream>
#include "gtest/gtest.h"
#include "../src/FileScan.h"
#include "../src/Selection.h"
#include "../src/Projection.h"
#include "../src/Join.h"
#include "../src/QueryPlanner.h"
#include "../src/schema_loader.h"

class FileScanTest : public ::testing::Test {
  protected:
    FileScan fs{"test_data"};
};

TEST_F(FileScanTest, TestNext) {
  std::vector<std::string> row = fs.next();
  EXPECT_EQ(row[0], "movieId");
  EXPECT_EQ(row[1], "title");
  EXPECT_EQ(row[2], "genres");
  
  row = fs.next();
  std::vector<std::string> expectProperCommaHandling{ "1","\"A Movie Title, With Commas, In the Title\"","Adventure|Animation|Children|Comedy|Fantasy" };
  EXPECT_EQ(expectProperCommaHandling, row);
};

class SelectionTest : public ::testing::Test {
  protected:
    std::unique_ptr<FileScan> fs = std::make_unique<FileScan>("test_data");
    std::vector<std::string> triple{"title", "EQUALS","The Fall"};
    Selection select{triple, std::move( fs )};
};

TEST_F(SelectionTest, TestNext) {
  // Advance to a row where title = "The Fall"
  select.next();
  select.next();
  std::vector<std::string> row = select.next();

  std::vector<std::string> theFallRow{ "2", "The Fall", "Adventure|Fantasy" };
  EXPECT_EQ(row, theFallRow);
}

class ProjectionTest : public ::testing::Test {
  protected:
    std::map<std::string,int> schema = schema_loader("movies");
    std::unique_ptr<FileScan> fs = std::make_unique<FileScan>("test_data");
    std::vector<std::string> triple{"title", "EQUALS","The Fall"};
    std::unique_ptr<Selection> select = std::make_unique<Selection>(triple, std::move( fs ));
    std::vector<std::string> col_names{ "title" };
    Projection projection{col_names, std::move( select )};
};

TEST_F(ProjectionTest, TestNext) {
  // Advance to a row where title = "The Fall"
  projection.next();
  projection.next();
  std::vector<std::string> row = projection.next();
  std::vector<std::string> expectation{ "The Fall" };
  EXPECT_EQ(row, expectation);
}

class JoinTest : public ::testing::Test {
  protected:
  std::unique_ptr<FileScan> mfs = std::make_unique<FileScan>("test_data");
  std::unique_ptr<FileScan> rfs = std::make_unique<FileScan>("test_data");

  std::vector<std::string> triple{"title", "EQUALS","The Fall"};
  std::unique_ptr<Selection> mselect = std::make_unique<Selection>(triple, std::move( mfs ));
  std::unique_ptr<Selection> rselect = std::make_unique<Selection>(triple, std::move( rfs ));

  std::vector<std::string> col_names{ "title" };
  std::unique_ptr<Projection> mprojection = std::make_unique<Projection>(col_names, std::move( mselect ));
  std::unique_ptr<Projection> rprojection = std::make_unique<Projection>(col_names, std::move( rselect ));

  std::vector<std::string> keys{ "movieId", "movieId" };

};

TEST_F(JoinTest, TestNext) {
  Join join( std::move( mprojection ), std::move( rprojection ), keys );
  std::vector<std::string> result = join.next();
  join.next();
  result = join.next();
  std::vector<std::string> expectation{ "The Fall", "The Fall" };
  EXPECT_EQ(result, expectation);
}

class QueryPlannerTest : public ::testing::Test {
};
TEST_F(QueryPlannerTest, Run) {
  std::vector<std::vector<std::string> > expectedResult{ 
    { "movieId", "title", "genres" },
    { "1", "\"A Movie Title, With Commas, In the Title\"", "Adventure|Animation|Children|Comedy|Fantasy" },
    { "2", "The Fall", "Adventure|Fantasy" },
    { "3", "Jumanji (1995)", "Adventure|Children|Fantasy" } 
  };
  std::vector<std::string> arguments = {"./mildDBMS", "\"SELECT * FROM test_data;\""};

  std::vector<char*> argv;
  for (const auto& arg : arguments)
      argv.push_back((char*)arg.data());
  argv.push_back(nullptr);

  QueryPlanner qp(argv.size() - 1, argv.data());
  std::vector<std::vector<std::string> > result = qp.run();
  EXPECT_EQ(result, expectedResult);
}

