#include "stdafx.h"
#include "SuperDragonAnimationState.h"
#include "Object.h"

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
		//TODO: ¯��� ���� ���� ��� 1ȸ
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
			//TODO: ¯��� �극�� ���� ���� ��� Loop
			//TODO: ¯��� �극�� ���� ��ƼŬ ���
			auto breath_frame = object->FindFrame("Breath");
			if(!breath_frame)
			{
				OutputDebugString(L"SuperDragonAnimationState::Run: Breath frame not found.\n");
			}

		}

		if(is_attack_ && attack_time_ > end_attack_time)
		{
			is_attack_ = false;
			//TODO: ¯��� �극�� ���� ���� ��� Stop
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
