#include "base/base.h"
#include "base/files/file_path.h"
#include "wow/base/dbcfile.h"
#include "wow/base/mpq.h"
#include "wow/base/areadb.h"

using ::base::FilePath;

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  MPQArchiveSet archives;
  FilePath datadir(FILE_PATH_LITERAL("E:\\game\\World of Warcraft\\Data"));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("world.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("base-Win.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("alternate.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("expansion1.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("expansion2.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("expansion3.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("expansion4.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("texture.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("model.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("misc.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("interface.MPQ"))));
  CHECK(archives.AddArchive(datadir.Append(FILE_PATH_LITERAL("itemtexture.MPQ"))));
  

  AreaDB areadb(&archives);
  return 0;
}
