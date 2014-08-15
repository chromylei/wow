#include "azer/render/render.h"
#include "azer/math/math.h"
#include "azer/util/util.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "wow/base/tile.h"
#include "wow/base/camera_control.h"

#include <tchar.h>
#include "tile.afx.h"
#define EFFECT_GEN_DIR "out/dbg/gen/wow/effect/tile/"
#define SHADER_NAME "tile.afx"
const char* kHeightmapPath = "sbox/terrain/res/heightmap01.bmp";
using base::FilePath;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  MainDelegate() : grid_(256) {}
  virtual void OnCreate() {}

  void Init();
  virtual void OnUpdateScene(double time, float delta_time);
  virtual void OnRenderScene(double time, float delta_time);
  virtual void OnQuit() {}
 private:
  void InitPhysicsBuffer(azer::RenderSystem* rs);
  azer::Camera camera_;
  Tile grid_;
  std::unique_ptr<TileEffect> effect_;
  azer::VertexBufferPtr vb_;
  azer::IndicesBufferPtr ib_;
  DISALLOW_COPY_AND_ASSIGN(MainDelegate);
};

void MainDelegate::InitPhysicsBuffer(azer::RenderSystem* rs) {
  azer::VertexDataPtr vdata(
      new azer::VertexData(effect_->GetVertexDesc(), grid_.vertices().size()));
  TileEffect::Vertex* v = (TileEffect::Vertex*)vdata->pointer();
  for (int i = 0; i < grid_.GetCellNum(); ++i) {
    for (int j = 0; j < grid_.GetCellNum(); ++j) {
      int index = i * grid_.GetCellNum() + j;
      const azer::Vector3& grid_pos = grid_.vertices()[index];
      v->position = azer::Vector4(grid_pos, 1.0f) * 0.05f;
      v->tex0 = azer::Vector2(j, i);
      v->tex1 = azer::Vector2(j, i);
      v->normal = azer::Vector4(0.0, 1.0f, 0.0f, 0.0f);
      v++;
    }
  }

  azer::IndicesDataPtr idata_ptr(
      new azer::IndicesData(grid_.indices().size(), azer::IndicesData::kUint32,
                            azer::IndicesData::kMainMemory));
  memcpy(idata_ptr->pointer(), &(grid_.indices()[0]),
         sizeof(int32) * grid_.indices().size());

  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), vdata));
  ib_.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), idata_ptr));
}

void MainDelegate::Init() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::Renderer* renderer = rs->GetDefaultRenderer();
  renderer->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
  CHECK(renderer->GetFrontFace() == azer::kCounterClockwise);
  CHECK(renderer->GetCullingMode() == azer::kCullBack);
  renderer->EnableDepthTest(true);
  azer::ShaderArray shaders;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
  effect_.reset(new TileEffect(shaders.GetShaderVec(), rs));

  grid_.Init();
  InitPhysicsBuffer(rs);

  camera_.SetPosition(azer::Vector3(0.0f, 2.0f, 0.0f));
  camera_.SetLookAt(azer::Vector3(0.0f, 2.0f, -1.0f));
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  float rspeed = 3.14f * 2.0f / 4.0f;
  azer::Radians camera_speed(azer::kPI / 2.0f);
  UpdatedownCamera(&camera_, camera_speed, delta_time);
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::Renderer* renderer = rs->GetDefaultRenderer();
  renderer->SetCullingMode(azer::kCullNone);
  DCHECK(NULL != rs);
  renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  renderer->ClearDepthAndStencil();

  effect_->SetPVW(camera_.GetProjViewMatrix());
  effect_->SetWorld(azer::Matrix4::kIdentity);
  effect_->Use(renderer);
  renderer->DrawIndex(vb_.get(), ib_.get(), azer::kTriangleList);
}

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  
  MainDelegate delegate;
  azer::WindowHost win(azer::WindowHost::Options(), &delegate);
  win.Init();
  CHECK(azer::LoadRenderSystem(&win));
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();
  delegate.Init();
  win.Show();
  azer::MainRenderLoop(&win);
  return 0;
}
