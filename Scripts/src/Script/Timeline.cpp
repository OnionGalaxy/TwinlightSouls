#include "Timeline.h"

#include "Main/GameObject.h"

#include "Component/ComponentTransform.h"
#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"
#include <algorithm>
Timeline::Timeline(float duration)
	: duration(duration*1000.0f)
{
}

void Timeline::Update(float delta_time)
{
	if (playing)
	{
		current_time += delta_time;
		if (current_time > duration)
		{
			Stop();
			return;
		}
		for (auto& track : tracks)
		{
			track->Update(delta_time);
		}
	}
}

void Timeline::Play()
{
	if (!playing)
	{
		current_time = 0;
		playing = true;
	}
}

void Timeline::Stop()
{
	float delta_time = 40.f;
	while (current_time < duration)
	{
		current_time += delta_time;
		for (auto& track : tracks)
		{
			track->Update(delta_time, true);
		}
	}
	for (auto& track : tracks)
	{
		track->OnFinish();
	}
	current_time = 0;
	playing = false;
}

bool Timeline::IsPlaying() const
{
	return playing;
}


void Timeline::TranslationTrack::AddKeyframe(float time, const float4x4& transform, bool scale)
{
	TranslationKeyframe* keyframe = new TranslationKeyframe();
	keyframe->target_transform = transform;
	keyframe->time = time*1000.0f + 1.f;
	keyframe->scale = scale;
	Track::AddKeyframe(keyframe);
}

void Timeline::TranslationTrack::Update( float delta_time, bool skip)
{
	internal_current_time += delta_time;
	if (next_keyframe && time_diference > 0.0f)
	{
		float lamda = internal_current_time / time_diference;

		if (next_keyframe->scale)
		{
			float3 scale = float3::Lerp(current_transform.ExtractScale(), next_keyframe->target_transform.ExtractScale(), lamda);
			m_target->transform.SetScale(scale);
		}
		else {
			float3 scale = m_target->transform.GetGlobalModelMatrix().GetScale();

			float3 position = float3::Lerp(current_transform.TranslatePart(), next_keyframe->target_transform.TranslatePart(), lamda);
			m_target->transform.SetGlobalMatrixTranslation(position);

			Quat rotation = Quat::Lerp(current_transform.RotatePart().ToQuat(), next_keyframe->target_transform.RotatePart().ToQuat(), lamda);
			m_target->transform.SetGlobalMatrixRotation(rotation);

			//Scale correction
			m_target->transform.SetGlobalMatrixScale(scale);
		}
	
		if (lamda >= 1.0f)
		{
			NextInterval();
		}
	}
	else
	{
		NextInterval();
	}
}

void Timeline::TranslationTrack::NextInterval()
{
	time_diference = next_keyframe ? next_keyframe->time : 0.0f;
	next_keyframe = static_cast<TranslationKeyframe*>(GetNextKeyFrame());
	time_diference = next_keyframe ? next_keyframe->time - time_diference : 0.0f;
	internal_current_time = 0;
	current_transform = m_target->transform.GetGlobalModelMatrix();
}


Timeline::Keyframe* Timeline::Track::GetNextKeyFrame()
{
	for (auto& keyframe : keyframes)
	{
		if (keyframe->time > last_keyframe_time)
		{
			last_keyframe_time = keyframe->time;
			return keyframe.get();
		}
	}
	return nullptr;
}

void Timeline::Track::AddKeyframe(Timeline::Keyframe* keyframe)
{
	keyframes.emplace_back(keyframe);
	std::sort(keyframes.begin(), keyframes.end(), [](const std::unique_ptr<Timeline::Keyframe>& lhs, const std::unique_ptr<Timeline::Keyframe>& rhs)
	{
		return lhs->time < rhs->time;
	});
}

Timeline::AnimationTrack::AnimationTrack(GameObject * target)
	: Track(target)
{
	component_animation = static_cast<ComponentAnimation*>(target->GetComponent(Component::ComponentType::ANIMATION));
	component_animation->SetIgnoreTransitions(true);
}

void Timeline::AnimationTrack::AddKeyframe(float time, const std::string& state_name, float transition_interval_time)
{
	AnimationKeyframe* keyframe = new AnimationKeyframe();
	keyframe->state_name = state_name;
	keyframe->time = time * 1000.0f + 1.f;
	keyframe->interval_time = transition_interval_time;
	Track::AddKeyframe(keyframe);
}

void Timeline::AnimationTrack::Update(float delta_time, bool skip)
{
	if (!component_animation)
	{
		return;
	}
	internal_current_time += delta_time;
	if (next_keyframe)
	{
		if (next_keyframe->time <= internal_current_time || skip)
		{
			component_animation->SetActiveState(next_keyframe->state_name, next_keyframe->interval_time);
			next_keyframe = static_cast<const AnimationKeyframe*>(GetNextKeyFrame());
		}
	}
	else
	{
		next_keyframe = static_cast<const AnimationKeyframe*>(GetNextKeyFrame());
	}
}

void Timeline::AnimationTrack::OnFinish()
{
	if (component_animation)
	{
		component_animation->SetIgnoreTransitions(false);
	}
}

void Timeline::CustomTrack::AddKeyframe(float time, const std::string& trigger_name, const void* data)
{
	CustomKeyframe* keyframe = new CustomKeyframe();
	keyframe->trigger_name = trigger_name;
	keyframe->time = time * 1000.0f + 1.f;
	keyframe->data = data;
	Track::AddKeyframe(keyframe);
}

void Timeline::CustomTrack::Update(float delta_time, bool skip)
{
	internal_current_time += delta_time;
	if (next_keyframe)
	{
		if (next_keyframe->time <= internal_current_time || skip)
		{
			track_listener.Trigger(next_keyframe->trigger_name, next_keyframe->data);
			next_keyframe = static_cast<const CustomKeyframe*>(GetNextKeyFrame());
		}
	}
	else
	{
		next_keyframe = static_cast<const CustomKeyframe*>(GetNextKeyFrame());
	}
}

Timeline::AudioTrack::AudioTrack(GameObject * target)
	: Track(target)
{
	component_audio_source = static_cast<ComponentAudioSource*>(target->GetComponent(Component::ComponentType::AUDIO_SOURCE));
}

void Timeline::AudioTrack::AddKeyframe(float time, const std::string & event_name, bool play)
{
	AudioKeyframe* keyframe = new AudioKeyframe();
	keyframe->event_name = event_name;
	keyframe->time = time * 1000.0f + 1.f;
	Track::AddKeyframe(keyframe);
}

void Timeline::AudioTrack::Update(float delta_time, bool skip)
{
	if (!component_audio_source)
	{
		return;
	}
	internal_current_time += delta_time;
	if (next_keyframe)
	{
		if (skip)
		{
			auto last_keyframe = next_keyframe;
			if (next_keyframe->play)
			{
				component_audio_source->StopEvent(next_keyframe->event_name);
			}
			next_keyframe = static_cast<const AudioKeyframe*>(GetNextKeyFrame());
			if (next_keyframe == nullptr)
			{
				component_audio_source->PlayEvent(last_keyframe->event_name);
			}
			return;
		}
		if (next_keyframe->time <= internal_current_time)
		{
			if (next_keyframe->play)
			{
				component_audio_source->PlayEvent(next_keyframe->event_name);
			}
			else
			{
				component_audio_source->StopEvent(next_keyframe->event_name);
			}
			next_keyframe = static_cast<const AudioKeyframe*>(GetNextKeyFrame());
		}
	}
	else
	{
		next_keyframe = static_cast<const AudioKeyframe*>(GetNextKeyFrame());
	}
}
