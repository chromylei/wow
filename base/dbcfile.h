#pragma once

#include <map>
#include <string>
#include <vector>

#include "wow/base/mpq.h"

class DBCFile {
 public:
  DBCFile(const ::base::FilePath& path);
  bool open(MPQArchiveSet* set);

  class Record {
   public:
    Record(const Record& record);
    bool GetUint32(int field, uint32* val);
    bool GetInt32(int field, int32* val);
    bool GetFloat(int field, float* val);
    bool GetString(int field, char** val);
   private:
    Record(uint8* offset, DBCFile* dbcfile)
        : offset_(offset)
        , dbcfile_(dbcfile) {
    }

    uint8* offset_;
    DBCFile* dbcfile_;
    friend class DBCFile;
  };
  Record GetRecord(int num);
 private:
  bool ParseHead(MPQFile* ptr);
  ::base::FilePath raw_path_;
  ::base::FilePath act_path_;

  uint32 record_size_;
  uint32 record_num_;
  uint32 field_num_;
  uint32 string_size_;
  uint8* table_;
  uint8* string_table_;
  DISALLOW_COPY_AND_ASSIGN(DBCFile);
};
