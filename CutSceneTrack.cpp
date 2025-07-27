#include "stdafx.h"
#include "CutSceneTrack.h"
#include "Object.h"
#include "Scene.h"
#include "CameraComponent.h"

CutSceneTrack::CutSceneTrack(const std::string& cut_scene_name) : name_(cut_scene_name)
{
	LoadCutSceneFromFile(cut_scene_name);
}

void CutSceneTrack::LoadCutSceneFromFile(const std::string& cut_scene_name)
{
	std::ifstream cut_scene_file{ "./Resource/CutScene/" + cut_scene_name + ".bin", std::ios::binary};
	if (!cut_scene_file.is_open())
	{
		return;
	}

	size_t size{ 0 };
	cut_scene_file.read((char*)&size, sizeof(size_t));
	cut_scene_data_buffer_.resize(size);
	cut_scene_file.read((char*)cut_scene_data_buffer_.data(), sizeof(CutSceneData) * size);
}

void CutSceneTrack::Play(Scene* scene)
{
	is_playing_ = true;
	scene_ = scene;
	scene_camera_ = scene->main_camera()->owner();
	auto camera_component = Object::GetComponent<CameraComponent>(camera_);
	scene->set_main_camera(camera_component);
}

void CutSceneTrack::Pause()
{
	is_playing_ = false;
}

void CutSceneTrack::Stop()
{
	is_playing_ = false;
	track_position_ = 0.f;
	auto camera_component = Object::GetComponent<CameraComponent>(scene_camera_);
	scene_->set_main_camera(camera_component);
	scene_camera_ = nullptr;
	scene_ = nullptr;
}

void CutSceneTrack::Run(float elapsed_time)
{
	if (!camera_ || !is_playing_)
		return;

	if (track_position_ > cut_scene_data_buffer_.back().key_time)
	{
		Stop();
	}

	for (int i = 0; i < cut_scene_data_buffer_.size() - 1; ++i)
	{
		const auto& data = cut_scene_data_buffer_[i];
		const auto& next_data = cut_scene_data_buffer_[i + 1];
		if (data.key_time <= track_position_ &&
			next_data.key_time >= track_position_)
		{
			float t = (track_position_ - data.key_time) / (next_data.key_time - data.key_time);
			auto camera_world = xmath_util_float4x4::Interpolate(data.world_matrix, next_data.world_matrix, t);
			camera_->set_transform_matrix(camera_world);
			break;
		}
	}

	track_position_ += elapsed_time;

}

void CutSceneTrack::set_camera(Object* value)
{
	camera_ = value;
}

Object* CutSceneTrack::camera() const
{
	return camera_;
}
