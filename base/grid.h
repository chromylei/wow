#pragma once

#include <string>
#include <vector>

#include "base/files/file_path.h"
#include "azer/render/render.h"
#include "wow/base/heightmap.h"

class Grid {
 public:
  Grid(const int cell = 1024) : kCellNum_(cell) {}

  struct Vertex {
    azer::Vector3 position;
    azer::Vector2 tex0;
    azer::Vector2 tex1;
    azer::Vector3 normal;
  };

  void Init();
  const std::vector<Vertex>& vertices() { return vertices_;}
  const std::vector<int32>& indices() { return indices_;}

  azer::VertexBufferPtr& GetVertexBuffer() { return vb_; }
  azer::IndicesBufferPtr&  GetIndicesBuffer() { return ib_;}
 private:
  void InitVertex();
  azer::VertexBufferPtr vb_;
  azer::IndicesBufferPtr ib_;
  std::vector<Vertex> vertices_;
  std::vector<int32> indices_;
  const int kCellNum_;
  DISALLOW_COPY_AND_ASSIGN(Grid);
};

typedef std::shared_ptr<Grid> GridPtr;
