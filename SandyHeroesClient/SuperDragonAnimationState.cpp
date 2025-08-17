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
		attack_time_ = 0.f; // ���� �ð� �ʱ�ȭ
		is_attack_ = false; // ���� ���� �ʱ�ȭ
		FMODSoundManager::Instance().PlaySound("bite", false, 0.3f);
		break;
	case SuperDragonAnimationTrack::kFlyFireBreathAttackLow:
		attack_time_ = 0.f; // ���� �ð� �ʱ�ȭ
		is_attack_ = false; // ���� ���� �ʱ�ȭ
		break;

	default:
		break;
	}

}

int SuperDragonAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
{
	attack_time_ += elapsed_time; // ���� �ð� ������Ʈ
	constexpr float animation_spf = 0.03f; // ���� �ִϸ��̼� �����Ӵ� �ð�
	float start_attack_time; // ���� ���� �ð�
	float end_attack_time; // ���� ���� �ð�

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
			//TODO: ¯��� �극�� ���� ��ƼŬ ���
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
