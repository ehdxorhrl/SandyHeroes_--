import os
import codecs

directories = ['SandyHeroesClient', 'SandyHeroesServer']

for directory in directories:
    if not os.path.exists(directory):
        continue
    for filename in os.listdir(directory):
        if 'AnimationState' in filename or 'AnimatorComponent' in filename:
            file_path = os.path.join(directory, filename)
            try:
                with codecs.open(file_path, 'r', encoding='euc-kr') as f:
                    content = f.read()
            except:
                with codecs.open(file_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    
            original_content = content
            
            # Replace AnimatorComponent* animator with std::shared_ptr<AnimatorComponent> animator
            content = content.replace('AnimatorComponent* animator', 'std::shared_ptr<AnimatorComponent> animator')
            
            if filename == 'AnimatorComponent.cpp':
                content = content.replace('animation_state_->Run(elapsed_time, owner_ptr, animation_tracks_[track_index_].is_end(), this)', 
                                          'animation_state_->Run(elapsed_time, owner_ptr, animation_tracks_[track_index_].is_end(), std::static_pointer_cast<AnimatorComponent>(shared_from_this()))')
                content = content.replace('animation_state_->Run(owner_ptr, animation_tracks_[track_index_].is_end(), this)', 
                                          'animation_state_->Run(owner_ptr, animation_tracks_[track_index_].is_end(), std::static_pointer_cast<AnimatorComponent>(shared_from_this()))')
                # Server commented out line
                content = content.replace('animation_state_->Run(owner_, animation_tracks_[track_index_].is_end(), this)', 
                                          'animation_state_->Run(owner_, animation_tracks_[track_index_].is_end(), std::static_pointer_cast<AnimatorComponent>(shared_from_this()))')

            if original_content != content:
                with codecs.open(file_path, 'w', encoding='euc-kr') as f:
                    f.write(content)
