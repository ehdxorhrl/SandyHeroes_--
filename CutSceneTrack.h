#pragma once
#include "CutSceneRecorder.h"

//컷신을 저장하고 재생하는 클래스
class CutSceneTrack
{
public:
	CutSceneTrack() = default;
	CutSceneTrack(const std::string& cut_scene_name);
	~CutSceneTrack() = default;

	void LoadCutSceneFromFile(const std::string& cut_scene_name);

	void Play(Scene* scene);
	void Pause();
	void Stop();

	void Run(float elapsed_time);

	void set_camera(Object* value);

	Object* camera() const;

private:
	std::string name_{};
	bool is_playing_{ false };
	float track_position_{};	// 현재 트랙 재생 시간
	std::vector<CutSceneData> cut_scene_data_buffer_{};

	Object* camera_;
	Object* scene_camera_;	//컷씬 재생전 씬의 카메라

	Scene* scene_{};		//컷씬 재생중인 씬
};

