#include "stdafx.h"
#include "CutSceneRecorder.h"
#include "Scene.h"
#include "CameraComponent.h"
#include "Object.h"

CutSceneRecorder::CutSceneRecorder(const std::string& name) : name_(name)
{

}

CutSceneRecorder::~CutSceneRecorder()
{
}

void CutSceneRecorder::Start()
{
	is_recording_ = true;
}

void CutSceneRecorder::Pause()
{
	is_recording_ = false;
}

void CutSceneRecorder::Stop()
{
	is_recording_ = false;
	WriteCutScene(name_);
	key_time_ = 0.f;
	cut_scene_data_list_.clear();
}

void CutSceneRecorder::CaptureCameraTransform(float elapsed_time)
{
	if (!is_recording_)
	{
		return;
	}
	key_time_ += elapsed_time;
	XMFLOAT4X4 world = camera_object_->world_matrix();
	cut_scene_data_list_.emplace_back(world, key_time_);
}

void CutSceneRecorder::WriteCutScene(const std::string& cut_scene_name)
{
	std::ofstream cut_scene_file{ "./Resource/CutScene/" + cut_scene_name + ".bin", std::ios::binary};
	auto size{ cut_scene_data_list_.size() };
	cut_scene_file.write((const char*)&size, sizeof(size_t));

	for (auto& cut_scene_data : cut_scene_data_list_)
	{
		cut_scene_file.write((const char*)&cut_scene_data, sizeof(cut_scene_data));
	}

}
