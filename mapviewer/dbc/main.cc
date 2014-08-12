#include "base/base.h"
#include "base/files/file_path.h"
#include "wow/base/dbcfile.h"
#include "wow/base/mpq.h"

using ::base::FilePath;

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  MPQArchiveSet archives;
  FilePath datadir(FILE_PATH_LITERAL("E:\\game\\World of Warcraft\\Data"));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("world.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("texture.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("model.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("misc.MPQ"))));
  return 0;
}
