#include "stdafx.h"
#include "SuperDragonAnimationState.h"
#include "Object.h"
#include "FMODSoundManager.h"
#include "AnimatorComponent.h"
#include "GameFramework.h"
#include "BaseScene.h"
#include "ParticleComponent.h"

SuperDragonAnimationState::SuperDragonAnimationState()
{
	set_animation_track((int)SuperDragonAnimationTrack::kFlyCastSpell);
	animation_loop_type_ = 1; //Once

}

void SuperDragonAnimationState::Enter(int animation_track, Object* object, AnimatorComponent* animator)
{
	
	switch ((SuperDragonAnimationTrack)animation_track)
	{
	case SuperDragonAnimationTrack::kFlyCastSpell:
	case SuperDragonAnimationTrack::kFlyTakeDamage:	
	case SuperDragonAnimationTrack::kFlyDie:
		animation_loop_type_ = 1; //Once
		break;
	case SuperDragonAnimationTrack::kFlyIdle:
		animation_loop_type_ = 0; //loop
		break;
	case SuperDragonAnimationTrack::kFlyBiteAttackLow:
		attack_time_ = 0.f; // 공격 시간 초기화
		is_attack_ = false; // 공격 상태 초기화
		FMODSoundManager::Instance().PlaySound("bite", false, 0.3f);
		break;
	case SuperDragonAnimationTrack::kFlyFireBreathAttackLow:
		attack_time_ = 0.f; // 공격 시간 초기화
		is_attack_ = false; // 공격 상태 초기화
		break;

	default:
		break;
	}

}

int SuperDragonAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
{
	attack_time_ += elapsed_time; // 공격 시간 업데이트
	constexpr float animation_spf = 0.03f; // 공격 애니메이션 프레임당 시간
	float start_attack_time; // 공격 시작 시간
	float end_attack_time; // 공격 종료 시간

	switch ((SuperDragonAnimationTrack)animation_track())
	{
	case SuperDragonAnimationTrack::kFlyCastSpell:
		if (is_end)
		{
			ChangeAnimationTrack((int)SuperDragonAnimationTrack::kFlyIdle, object, animator);
		}
		break;
	case SuperDragonAnimationTrack::kFlyDie:
		if (is_end)
		{
			object->set_is_dead(true);
		}
		break;
	case SuperDragonAnimationTrack::kFlyBiteAttackLow:
	{
		if (is_end)
		{
			ChangeAnimationTrack((int)SuperDragonAnimationTrack::kFlyIdle, object, animator);
		}
	}
		break;
	case SuperDragonAnimationTrack::kFlyFireBreathAttackLow:
	{
		start_attack_time = animation_spf * 13.f;
		end_attack_time = animation_spf * 40.f;

		if (!is_attack_ && attack_time_ > start_attack_time)
		{
			is_attack_ = true;
			FMODSoundManager::Instance().PlaySound("breath", false, 0.3f);
			//TODO: 짱쎄용 브레스 공격 파티클 재생
			auto breath_frame = object->FindFrame("Breath");
			if(!breath_frame)
			{
				OutputDebugString(L"SuperDragonAnimationState::Run: Breath frame not found.\n");
			}
			auto base_scene = dynamic_cast<BaseScene*>(GameFramework::Instance()->scene());
			if (base_scene)
			{
				auto particle = base_scene->dragon_particle();
				if (particle)
				{
					particle->set_hit_position(breath_frame->world_position_vector());
					particle->set_direction_pivot_object(breath_frame);
					particle->Play(200);
				}
			}
		}

		if(is_attack_ && attack_time_ > end_attack_time)
		{
			is_attack_ = false;
		}

		if (is_end)
		{
			ChangeAnimationTrack((int)SuperDragonAnimationTrack::kFlyIdle, object, animator);
		}
	}
	break;

	default:
		break;
	}
	return animation_track();
}

void SuperDragonAnimationState::Exit(int animation_track, Object* object, AnimatorComponent* animator)
{
	if((SuperDragonAnimationTrack)animation_track == SuperDragonAnimationTrack::kFlyBiteAttackLow)
	{
		animation_loop_type_ = 0; //loop
	}
}

AnimationState* SuperDragonAnimationState::GetCopy()
{
	return new SuperDragonAnimationState(*this);
}

int SuperDragonAnimationState::GetDeadAnimationTrack() const
{
	return (int)SuperDragonAnimationTrack::kFlyDie;
}
