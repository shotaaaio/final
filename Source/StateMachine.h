#pragma once
#include <functional>

enum class StateID
{
    //移動ステート
    Idle = 0,
    Jump,
    Dash,
    Walk,

    //攻撃ステート
    WeakAttack,//弱攻撃
    WeakAttackCombo2,
    WeakAttackCombo3,

    //特殊ステート
    BackStep,//後退
    Special,//必殺技(未定)
    BehindEnemy,//敵の背後に移動

    ////回避ステート
    Dodge,//回避(猶予4F)
    Guard,//ガード(受ける攻撃を半減)
    Parry,//ジャスト回避(猶予3F)
};

template <typename T>
struct  StateInfo
{
	const char* name;
	std::function<void(T*, float)>onUpdate;
    std::function<void(T*)> onEnter;
    std::function<void(T*)> onExit;
	bool autoReturnToIdle;//アニメーション終了時に自動的にIdleに遷移するか
};

template <typename T>
class StateMachine {
public:
    StateMachine(T* owner, const StateInfo<T>* table)
        : owner(owner), stateTable(table)
    {
        //初期状態をIdleにセットする
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

    //Imgui表示用ステートネームのゲッター
    const char* GetCurrentStateName() const { return stateTable[(int)currentState].name; }

private:
    T* owner = nullptr;
    const StateInfo<T>* stateTable = nullptr;
    StateID currentState = StateID::Idle;
    StateID nextState = StateID::Idle;
};