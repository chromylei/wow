#include "wow/base/grid.h"

using azer::GpuConstantsTable;

void ComputeNormal(Grid::Vertex* v, int vertex_num, DWORD* indices,
                   int indices_num) {
  std::unique_ptr<uint32> used(new uint32[vertex_num]);
  memset(used.get(), 0, sizeof(uint32) * vertex_num);
  for (int i = 0; i < indices_num; i+=3) {
    Grid::Vertex* p1 = v + indices[i];
    Grid::Vertex* p2 = v +indices[i+1];
    Grid::Vertex* p3 = v + indices[i+2];
    used.get()[indices[i]]++;
    used.get()[indices[i+1]]++;
    used.get()[indices[i+2]]++;
    azer::Vector3 normal = azer::CalcPlaneNormal(p1->position,
                                                 p2->position,
                                                 p3->position);
    p1->normal += normal;
    p2->normal += normal;
    p3->normal += normal;
  }

  for (int i = 0; i < vertex_num; ++i) {
    v[i].normal /= (float)used.get()[i];
  }
}

void Grid::Init() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  InitVertex();
}


void Grid::InitVertex() {
  const int kRow = kCellNum_;
  const int kColumn = kCellNum_;
  const int kVertexNum = kRow * kColumn;
  for (int i = 0; i < kColumn; ++i) {
    for (int j = 0; j < kRow; ++j) {
      int idx = i * kRow + j;
      Vertex v;
      v.position = azer::vec3((float)i, 0.0f, (float)j);
      v.tex0 = azer::Vector2(j / (float)kRow, i / (float)kColumn);
      v.tex1 = azer::Vector2(j, i);
      v.normal = azer::vec3(0.0f, 1.0f, 0.0f);
      vertices_.push_back(v);
    }
  }

  const int kTriangleNum = (kRow - 1) * (kColumn - 1) * 2;
  for (int i = 0; i < kColumn-1; ++i) {
    for (int j = 0; j < kRow-1; ++j) {
      indices_.push_back(i * kRow + j);
      indices_.push_back((i + 1) * kRow + j + 1);
      indices_.push_back((i + 1) * kRow + j);
      indices_.push_back(i * kRow + j);
      indices_.push_back(i * kRow + j + 1);
      indices_.push_back((i + 1) * kRow + j + 1);
    }
  }

  CHECK_EQ((int)indices_.size(), kTriangleNum * 3);
  ::ComputeNormal((Vertex*)&(vertices_[0]), kColumn * kRow,
                  (DWORD*)&(indices_[0]), kTriangleNum * 3);
}
