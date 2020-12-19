#ifndef _TIMELINE_H_
#define _TIMELINE_H_
#include <vector>
#include <MathGeoLib.h>
#include "Main/Globals.h"
class GameObject;
class ComponentAnimation;
class ComponentAudioSource;
class Timeline
{
public:

	struct Keyframe
	{
		Keyframe() = default;
		virtual ~Keyframe() = default;
		float time = 0;
	};

	struct Track
	{
		Track(GameObject* target) : m_target(target) {};
		virtual ~Track() = default;
		virtual void Update(float delta_time, bool skip = false) = 0;
		virtual void OnFinish(){};
	protected:
		float internal_current_time = 0.0f;
		float time_diference = 0.0f;
		float last_keyframe_time = 0.0f;
		std::vector<std::unique_ptr<Keyframe>> keyframes;
		void AddKeyframe(Keyframe* keyframe);
		Keyframe* GetNextKeyFrame();
		GameObject* m_target;
	};

	struct TranslationTrack: Track
	{
		struct TranslationKeyframe : Keyframe
		{
			float4x4 target_transform;
			bool scale = false;
		};
		TranslationTrack(GameObject* target) : Track(target){};
		~TranslationTrack() = default;
		void AddKeyframe(float time, const float4x4& transform, bool scale = false);
	private:
		void Update( float delta_time, bool skip = false) override;
		void NextInterval();
		const TranslationKeyframe* next_keyframe = nullptr;
		float4x4 current_transform;
	};

	struct AnimationTrack : Track
	{
		struct AnimationKeyframe : Keyframe
		{
			float interval_time = 250.0f;
			std::string state_name;
		};
		AnimationTrack(GameObject* target);
		~AnimationTrack() = default;
		void AddKeyframe(float time,const std::string& state_name, float transition_interval_time = 250.0f);
	private:
		void Update(float delta_time, bool skip = false) override;
		void OnFinish() override;
		ComponentAnimation* component_animation = nullptr;
		const AnimationKeyframe* next_keyframe = nullptr;
	};

	struct AudioTrack : Track
	{
		struct AudioKeyframe : Keyframe
		{
			bool play = true; // if not play it will stop
			std::string event_name;
		};
		AudioTrack(GameObject* target);
		void AddKeyframe(float time, const std::string& event_name, bool play = true);
	private:
		void Update(float delta_time, bool skip = false) override;
		ComponentAudioSource* component_audio_source = nullptr;
		const AudioKeyframe* next_keyframe = nullptr;
	};

	class CustomTrackListener
	{
	public:
		CustomTrackListener() = default;
		virtual ~CustomTrackListener() = default;
		virtual void Trigger(const std::string& trigger_name, const void* data) = 0;
	};
	struct CustomTrack : Track
	{
		struct CustomKeyframe : Keyframe
		{
			std::string trigger_name;
			const void* data;
		};
		CustomTrack(CustomTrackListener& track_listener) : Track(nullptr), track_listener(track_listener){};
		~CustomTrack() = default;
		void AddKeyframe(float time, const std::string& trigger_name, const void* data = nullptr);
		CustomTrackListener& track_listener;
	private:
		void Update(float delta_time, bool skip = false) override;
		const CustomKeyframe* next_keyframe = nullptr;
	};
	Timeline(float duration);
	~Timeline() = default;

	void Update(float delta_time);
	void Play();
	void Stop();
	bool IsPlaying() const;

	std::vector<std::unique_ptr<Track>> tracks;
private:
	bool playing = false;
	float duration = 0;
	float current_time = 0.0f;
};

#endif //_TIMELINE_H_