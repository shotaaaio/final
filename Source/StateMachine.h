#pragma once
#include <functional>

enum class StateID
{
    //�ړ��X�e�[�g
    Idle = 0,
    Jump,
    Dash,
    Walk,

    //�U���X�e�[�g
    WeakAttack,//��U��
    WeakAttackCombo2,
    WeakAttackCombo3,

    //����X�e�[�g
    BackStep,//���
    Special,//�K�E�Z(����)
    BehindEnemy,//�G�̔w��Ɉړ�

    ////����X�e�[�g
    Dodge,//���(�P�\4F)
    Guard,//�K�[�h(�󂯂�U���𔼌�)
    Parry,//�W���X�g���(�P�\3F)
};

template <typename T>
struct  StateInfo
{
	const char* name;
	std::function<void(T*, float)>onUpdate;
    std::function<void(T*)> onEnter;
    std::function<void(T*)> onExit;
	bool autoReturnToIdle;//�A�j���[�V�����I�����Ɏ����I��Idle�ɑJ�ڂ��邩
};

template <typename T>
class StateMachine {
public:
    StateMachine(T* owner, const StateInfo<T>* table)
        : owner(owner), stateTable(table)
    {
        //������Ԃ�Idle�ɃZ�b�g����
        if (stateTable[(int)currentState].onEnter){
            stateTable[(int)currentState].onEnter(owner);
        }
    }
    

    void Update(float elapsedTime) {
        if (nextState != currentState) {
            if (stateTable[(int)currentState].onExit)
                stateTable[(int)currentState].onExit(owner);
            currentState = nextState;
            if (stateTable[(int)currentState].onEnter)
                stateTable[(int)currentState].onEnter(owner);
        }

        if (stateTable[(int)currentState].onUpdate)
            stateTable[(int)currentState].onUpdate(owner,elapsedTime);

        if (stateTable[(int)currentState].autoReturnToIdle && !owner->isPlayAnimation()){
            ChangeState(StateID::Idle);
        }
    }

    void ChangeState(StateID newState) {
        nextState = newState;
    }

    StateID GetCurrentState() const { return currentState; }

    //Imgui�\���p�X�e�[�g�l�[���̃Q�b�^�[
    const char* GetCurrentStateName() const { return stateTable[(int)currentState].name; }

private:
    T* owner = nullptr;
    const StateInfo<T>* stateTable = nullptr;
    StateID currentState = StateID::Idle;
    StateID nextState = StateID::Idle;
};