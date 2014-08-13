#include "azer/render/render.h"
#include "azer/math/math.h"
#include "azer/util/util.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "wow/base/grid.h"
#include "wow/base/camera_control.h"

#include <tchar.h>
#include "grid.afx.h"
#define EFFECT_GEN_DIR "out/dbg/gen/wow/mapviewer/water/"
#define SHADER_NAME "grid.afx"
const char* kHeightmapPath = "sbox/terrain/res/heightmap01.bmp";
using base::FilePath;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  MainDelegate() {}
  virtual void OnCreate() {}

  void Init() {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    azer::Renderer* renderer = rs->GetDefaultRenderer();
    renderer->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
    CHECK(renderer->GetFrontFace() == azer::kCounterClockwise);
    CHECK(renderer->GetCullingMode() == azer::kCullBack);
    renderer->EnableDepthTest(true);
    camera_.SetPosition(azer::Vector3(0.0f, 0.0f, 5.0f));

    // grid_.Init(::base::FilePath(::base::UTF8ToWide(kHeightmapPath)));
    grid_.Init();

    azer::ShaderArray shaders;
    CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
    CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
    effect_.reset(new GridEffect(shaders.GetShaderVec(), rs));
  }
  virtual void OnUpdateScene(double time, float delta_time) {
    float rspeed = 3.14f * 2.0f / 4.0f;
    azer::Radians camera_speed(azer::kPI / 2.0f);
    UpdatedownCamera(&camera_, camera_speed, delta_time);
  }

  virtual void OnRenderScene(double time, float delta_time) {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    azer::Renderer* renderer = rs->GetDefaultRenderer();
    renderer->SetCullingMode(azer::kCullNone);
    DCHECK(NULL != rs);
    renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    renderer->ClearDepthAndStencil();

    effect_->SetWorld(camera_.GetProjViewMatrix());
    effect_->SetWorld(azer::Matrix4::kIdentity);
    effect_->Use(renderer);
    renderer->DrawIndex(grid_.GetVertexBuffer().get(),
                        grid_.GetIndicesBuffer().get(),
                        azer::kTriangleList);
  }

  virtual void OnQuit() {}
 private:
  azer::Camera camera_;
  Grid grid_;
  std::unique_ptr<GridEffect> effect_;
  DISALLOW_COPY_AND_ASSIGN(MainDelegate);
};

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
