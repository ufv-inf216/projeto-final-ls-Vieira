//
// Created by julio on 11/11/23.
//

#include "Character.h"

Character::Character() {

}

const std::vector<int> Character::GetStateArray(CharacterSelect character, CharacterState state) {
    switch (character) {
        case CharacterSelect::Goku:
            switch (state) {
                case CharacterState::Idle:
                    return {10, 11, 12, 13};
                case CharacterState::Move:
                    return {18, 19, 20, 21, 22, 23};
                case CharacterState::Jump:
                    return {33, 34, 35, 36, 37, 38, 39, 40};
                case CharacterState::Down:
                    return {41, 42, 43, 44, 45, 46, 47, 48};
                case CharacterState::Punch:
                    return {49, 50, 51, 52, 53, 54, 55, 56};
                case CharacterState::Kick:
                    return {57, 58, 59, 60, 61, 62, 63, 64};
                case CharacterState::Dead:
                    return {65, 66, 67, 68, 69, 70, 71, 72};
            }
        case CharacterSelect::Vegeta:
            switch (state) {
                case CharacterState::Idle:
                    return {10, 11, 12, 13};
                case CharacterState::Move:
                    return {18, 19, 20, 21, 22, 23};
                case CharacterState::Jump:
                    return {33, 34, 35, 36, 37, 38, 39, 40};
                case CharacterState::Down:
                    return {41, 42, 43, 44, 45, 46, 47, 48};
                case CharacterState::Punch:
                    return {49, 50, 51, 52, 53, 54, 55, 56};
                case CharacterState::Kick:
                    return {57, 58, 59, 60, 61, 62, 63, 64};
                case CharacterState::Dead:
                    return {65, 66, 67, 68, 69, 70, 71, 72};
            }
    }
}

const std::string Character::GetCharacterSpriteSheetPath(CharacterSelect character) {
    switch (character) {
        case CharacterSelect::Goku:
            return "../Assets/Sprites/Goku/Goku.png";
        case CharacterSelect::Vegeta:
            return "../Assets/Sprites/Vegeta/Vegeta.png";
    }
}

const std::string Character::GetCharacterSpriteSheetJSON(CharacterSelect character) {
    switch (character) {
        case CharacterSelect::Goku:
            return "../Assets/Sprites/Goku/Goku.json";
        case CharacterSelect::Vegeta:
            return "../Assets/Sprites/Vegeta/Vegeta.json";
    }
}
