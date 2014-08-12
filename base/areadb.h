#pragma once

#include "wow/base/dbcfile.h"
#include "base/files/file_path.h"

class AreaDB {
 public:
  AreaDB(MPQArchiveSet* set)
      : dbcfile_(::base::FilePath(FILE_PATH_LITERAL("DBFilesClient\\AreaTable.dbc"))) {
    dbcfile_.open(set);
  }

 private:
  DBCFile dbcfile_;
  DISALLOW_COPY_AND_ASSIGN(AreaDB);
};
