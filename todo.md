* Write test data to sqlite database
* Have a function or class which can return a list of test files for given folders.
  * Find all *.m file
  * Have a test file filter to extract t*.m file (use std::regexp)
* Write test data to rocksdb.
  * Value need below fields:
    * Test file paths
    * Date or time stamp.
    * Test log data.
    * Change list info: Files that are openned in the given sandbox.
* Need to have a simple test log parser using spirit.
* A simple writer to convert test data to HTML format (whuang).
* A simple report of the test status (whuang).
