#include "InputManager.h" 
#include <DxLib.h>
#include <cassert>


InputManager* InputManager::instance_;

void InputManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new InputManager();
	}
	instance_->Init(); // マネージャ初期化処理
}

InputManager& InputManager::GetInstance(void)
{
	if (instance_ == nullptr)
	{
		InputManager::CreateInstance();
#ifdef _DEBUG
		OutputDebugString("\n入力マネージャが生成されていません。\n入力マネージャを生成しました。\n");
#endif
	}
	return *instance_;
}

InputManager::InputManager(void)
{
	mouseInput_ = -1;
}


void InputManager::Init(void)
{
	SetInputKey(); // 入力キー登録

	SetInputMouse(); // マウス入力登録
}

void InputManager::SetInputKey(void)
{
	AddKey(KEY_INPUT_RETURN);
	AddKey(KEY_INPUT_SPACE);
	AddKey(KEY_INPUT_X);

	AddKey(KEY_INPUT_W);
	AddKey(KEY_INPUT_A);
	AddKey(KEY_INPUT_S);
	AddKey(KEY_INPUT_D);
	AddKey(KEY_INPUT_C);
	AddKey(KEY_INPUT_V);
	AddKey(KEY_INPUT_Z);

	AddKey(KEY_INPUT_UP);
	AddKey(KEY_INPUT_DOWN);
	AddKey(KEY_INPUT_LEFT);
	AddKey(KEY_INPUT_RIGHT);

	AddKey(KEY_INPUT_ESCAPE);
	AddKey(KEY_INPUT_LSHIFT);
	AddKey(KEY_INPUT_RSHIFT);
}

void InputManager::SetInputMouse(void)
{
	// 左クリック
	AddMouse(MOUSE_INPUT_LEFT);

	// 右クリック
	AddMouse(MOUSE_INPUT_RIGHT);

	// 中央クリック
	AddMouse(MOUSE_INPUT_MIDDLE);
}


void InputManager::Update(void)
{
	// キーボード入力判定
	for (auto& k : keys_)
	{
		Key& key = k.second;

		key.inputOld = key.inputNew;
		key.inputNew = CheckHitKey(key.keyType);
		key.trgDown = (key.inputNew && !key.inputOld);
		key.trgUp = (!key.inputNew && key.inputOld);
	}


	// マウス判定
	mouseInput_ = GetMouseInput();

	//マウス座標取得
	GetMousePoint(&mousePos_.x, &mousePos_.y);


	// マウス入力判定
	for (auto& m : mouses_)
	{
		Mouse& mouse = m.second;

		mouse.inputOld = mouse.inputNew;
		mouse.inputNew = ((GetMouseInput() & mouse.type) != 0);
		mouse.trgDown = (mouse.inputNew && !mouse.inputOld);
		mouse.trgUp = (!mouse.inputNew && mouse.inputOld);
	}

	// ゲームパッド入力判定更新
	SetPadInState(JOYPAD_NO::KEY_PAD1);
	SetPadInState(JOYPAD_NO::PAD1);
	SetPadInState(JOYPAD_NO::PAD2);
	SetPadInState(JOYPAD_NO::PAD3);
	SetPadInState(JOYPAD_NO::PAD4);
}

void InputManager::Destroy(void)
{
	keys_.clear();	 // 入力キー配列 解放
	mouses_.clear(); // マウス入力配列 解放

	delete instance_; // マネージャ 削除
}


#pragma region キーボード処理

void InputManager::AddKey(unsigned int type)
{
	/* 判定を行うキーを追加 */

	Key keyInfo = InputManager::Key();

	keyInfo.keyType = type;
	keyInfo.inputOld = false;
	keyInfo.inputNew = false;
	keyInfo.trgUp = false;
	keyInfo.trgDown = false;

	// 入力キー配列に格納
	keys_.emplace(type, keyInfo);
}

bool InputManager::KeyIsNewAll(void) const
{
	// いずれかのキーが入力しているか判定

	bool isInput = false;

	char key[256];
	GetHitKeyStateAll(key);
	for (int i = 0; i < 256; ++i)
	{
		if (key[i] == 1)
		{
			isInput = true;
			break;
		}
	}

	return isInput;
}

const InputManager::Key& InputManager::FindKey(unsigned int keyType) const
{
	auto it = keys_.find(keyType);
	if (it != keys_.end())
	{
		// キーの情報を返す
		return it->second;
	}

#ifdef _DEBUG
	OutputDebugString("\nキーの情報がありません。\nInputManagerで割り当て処理を行ってください\n");
#endif

	// 空のキー情報を返す
	return keyInfoEmpty_;
}

#pragma endregion

#pragma region マウス処理

void InputManager::AddMouse(unsigned int type)
{
	Mouse mouse = InputManager::Mouse();

	mouse.type = type;
	mouse.inputOld = false;
	mouse.inputNew = false;
	mouse.trgDown = false;
	mouse.trgUp = false;
	// 配列に格納
	mouses_.emplace(mouse.type, mouse);
}

const InputManager::Mouse& InputManager::FindMouse(unsigned int mouseType) const
{
	auto it = mouses_.find(mouseType);
	if (it != mouses_.end())
	{
		// マウスの情報を返す
		return it->second;
	}
#ifdef _DEBUG
	OutputDebugString("\nマウスの情報がありません。\nInputManagerで割り当て処理を行ってください\n");
#endif

	// 空のキー情報を返す
	return mouseInfoEmpty_;
}

#pragma endregion 


#pragma region コントローラ処理

void InputManager::SetPadInState(JOYPAD_NO jPadNum)
{
	/* コントローラを識別して取得 */

	int num = static_cast<int>(jPadNum);
	auto stateNew = GetPadInputState(jPadNum);
	auto& stateNow = padInfos_[num];

	int max = static_cast<int>(PAD_BTN::MAX);
	for (int i = 0; i < max; i++)
	{
		stateNow.ButtonOld[i] = stateNow.ButtonNew[i];
		stateNow.ButtonNew[i] = stateNew.ButtonNew[i];

		stateNow.IsOld[i] = stateNow.IsNew[i];
		stateNow.IsNew[i] = (stateNow.ButtonNew[i] > 0);

		stateNow.IsTrgDown[i] = (stateNow.IsNew[i] && !stateNow.IsOld[i]);
		stateNow.IsTrgUp[i] = (!stateNow.IsNew[i] && stateNow.IsOld[i]);
	}

	max = static_cast<int>(JOYPAD_ALGKEY::MAX);
	for (int i = 0; i < max; i++)
	{
		stateNow.AlgKeyX[i] = stateNew.AlgKeyX[i];
		stateNow.AlgKeyY[i] = stateNew.AlgKeyY[i];

		stateNow.IsOldAlgKey[i] = stateNow.IsNewAlgKey[i];
		stateNow.IsNewAlgKey[i] = (stateNow.AlgKeyX[i] != 0 || stateNow.AlgKeyY[i] != 0);

		stateNow.IsTrgDownAlgKey[i] = (stateNow.IsNewAlgKey[i] && !stateNow.IsOldAlgKey[i]);
		stateNow.IsTrgUpAlgKey[i] = (!stateNow.IsNewAlgKey[i] && stateNow.IsOldAlgKey[i]);
	}
}

InputManager::JOYPAD_IN_STATE& InputManager::GetPadInputState(JOYPAD_NO padNum)
{
	/* コントローラ入力取得処理 */

	JOYPAD_IN_STATE ret = InputManager::JOYPAD_IN_STATE();

	auto type = GetPadType(padNum);

	switch (type)
	{
		/* XBOX入力 */
	case InputManager::JOYPAD_TYPE::XBOX_ONE:
	{
		auto d = GetPadDInputState(padNum);
		auto x = GetPadXInputState(padNum);

		int index;

		//  Y
		// X  B
		//  A

		// A
		index = static_cast<int>(PAD_BTN::DOWN);
		ret.ButtonNew[index] = d.Buttons[0];

		// B
		index = static_cast<int>(PAD_BTN::RIGHT);
		ret.ButtonNew[index] = d.Buttons[1];

		// X
		index = static_cast<int>(PAD_BTN::LEFT);
		ret.ButtonNew[index] = d.Buttons[2];

		// Y
		index = static_cast<int>(PAD_BTN::UP);
		ret.ButtonNew[index] = d.Buttons[3];

		// L
		index = static_cast<int>(PAD_BTN::L_BUTTON);
		ret.ButtonNew[index] = d.Buttons[4];

		// R
		index = static_cast<int>(PAD_BTN::R_BUTTON);
		ret.ButtonNew[index] = d.Buttons[5];

		// BACK
		index = static_cast<int>(PAD_BTN::BACK);
		ret.ButtonNew[index] = d.Buttons[6];

		// START
		index = static_cast<int>(PAD_BTN::START);
		ret.ButtonNew[index] = d.Buttons[7];


		// RTrigger
		index = static_cast<int>(PAD_BTN::L_TRIGGER);
		ret.ButtonNew[index] = x.LeftTrigger;

		// RTrigger
		index = static_cast<int>(PAD_BTN::R_TRIGGER);
		ret.ButtonNew[index] = x.RightTrigger;



		// 左スティック
		index = static_cast<int>(JOYPAD_ALGKEY::LEFT);
		ret.AlgKeyX[index] = d.X;
		ret.AlgKeyY[index] = d.Y;

		// 右スティック
		index = static_cast<int>(JOYPAD_ALGKEY::RIGHT);
		ret.AlgKeyX[index] = d.Rx;
		ret.AlgKeyY[index] = d.Ry;

		// Dパッド(十字スティック)
		index = static_cast<int>(JOYPAD_ALGKEY::D_PAD);
		int inputX = 0;
		int inputY = 0;
		unsigned int left = 27000;
		unsigned int right = 9000;
		unsigned int up = 0;
		unsigned int down = 18000;

		if (d.POV == &left)  inputX = -1;
		if (d.POV == &right) inputX = 1;
		if (d.POV == &up)    inputY = -1;
		if (d.POV == &down)  inputY = 1;

		ret.AlgKeyX[index] = inputX;
		ret.AlgKeyY[index] = inputY;
	}
	break;

	/* PlayStationコントローラ入力 */
	case InputManager::JOYPAD_TYPE::DUAL_SENSE:
	{
		/*
		auto d = GetPadDInputState(padNum);

		int index;

		// 　△
		// □　○
		// 　×

		//// △
		index = static_cast<int>(JOYPAD_BTN::TOP);
		ret.ButtonNew[index] = d.Buttons[3];

		//// ○
		index = static_cast<int>(JOYPAD_BTN::RIGHT);
		ret.ButtonNew[index] = d.Buttons[2];

		//// ×
		index = static_cast<int>(JOYPAD_BTN::DOWN);
		ret.ButtonNew[index] = d.Buttons[1];

		//// □
		index = static_cast<int>(JOYPAD_BTN::LEFT);
		ret.ButtonNew[index] = d.Buttons[0];


		// 左スティック
		index = static_cast<int>(JOYPAD_ALGKEY::LEFT);
		ret.AlgKeyX[index] = d.X;
		ret.AlgKeyY[index] = d.Y;

		// 右スティック
		index = static_cast<int>(JOYPAD_ALGKEY::RIGHT);
		ret.AlgKeyX[index] = d.Rx;
		ret.AlgKeyY[index] = d.Ry;
		*/
	}
	break;


	/* その他入力 */
	case InputManager::JOYPAD_TYPE::XBOX_360:
	case InputManager::JOYPAD_TYPE::DUAL_SHOCK_4:
	case InputManager::JOYPAD_TYPE::SWITCH_JOY_CON_L:
	case InputManager::JOYPAD_TYPE::SWITCH_JOY_CON_R:
	case InputManager::JOYPAD_TYPE::SWITCH_PRO_CTRL:
	case InputManager::JOYPAD_TYPE::OTHER:
	case InputManager::JOYPAD_TYPE::MAX:
	{
		// 入力なし
	}
	break;
	}

	return ret;
}

InputManager::JOYPAD_TYPE InputManager::GetJPadType(JOYPAD_NO jPadNo)
{
	int num = GetJoypadType(static_cast<int>(jPadNo));
	InputManager::JOYPAD_TYPE type = static_cast<InputManager::JOYPAD_TYPE>(num);
	return type;
}


DINPUT_JOYSTATE InputManager::GetPadDInputState(InputManager::JOYPAD_NO padNum)
{
	// DirectInput情報取得
	GetJoypadDirectInputState(static_cast<int>(padNum), &joyDInState_);
	return joyDInState_;
}

XINPUT_STATE InputManager::GetPadXInputState(InputManager::JOYPAD_NO padNum)
{
	// ボタン入力情報取得
	GetJoypadXInputState(static_cast<int>(padNum), &joyXInState_);
	return joyXInState_;
}


InputManager::JOYPAD_TYPE InputManager::GetPadType(JOYPAD_NO padNo)
{
	return static_cast<InputManager::JOYPAD_TYPE>(GetJoypadType(static_cast<int>(padNo)));
}


bool InputManager::PadIsBtnNew(int padNum, PAD_BTN button) const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	int btnType = static_cast<int>(button);
	bool ret = (padInfos_[padNum].IsNew[btnType]);
	return ret;
}
bool InputManager::PadIsBtnNew(int padNum, int button) const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	bool ret = (padInfos_[padNum].IsNew[button]);
	return ret;
}

bool InputManager::PadIsBtnTrgDown(int padNum, PAD_BTN button) const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	int btnType = static_cast<int>(button);
	bool ret = (padInfos_[padNum].IsTrgDown[btnType]);
	return ret;
}
bool InputManager::PadIsBtnTrgDown(int padNum, int button) const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	bool ret = (padInfos_[padNum].IsTrgDown[button]);
	return ret;
}

bool InputManager::PadIsBtnTrgUp(int padNum, PAD_BTN button) const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	int btnType = static_cast<int>(button);
	bool ret = (padInfos_[padNum].IsTrgUp[btnType]);
	return ret;
}
bool InputManager::PadIsBtnTrgUp(int padNum, int button) const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	bool ret = (padInfos_[padNum].IsTrgUp[button]);
	return ret;
}

bool InputManager::PadIsAlgKeyNew(int padNum, JOYPAD_ALGKEY algKey)const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	int keyType = static_cast<int>(algKey);
	bool ret = (padInfos_[padNum].IsNewAlgKey[keyType]);
	return ret;
}
bool InputManager::PadIsAlgKeyNew(int padNum, int algKey)const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	bool ret = (padInfos_[padNum].IsNewAlgKey[algKey]);
	return ret;
}

bool InputManager::PadIsAlgKeyTrgDown(int padNum, JOYPAD_ALGKEY algKey)const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	int keyType = static_cast<int>(algKey);
	bool ret = (padInfos_[padNum].IsTrgDownAlgKey[keyType]);
	return ret;
}
bool InputManager::PadIsAlgKeyTrgDown(int padNum, int algKey)const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	bool ret = (padInfos_[padNum].IsTrgDownAlgKey[algKey]);
	return ret;
}

bool InputManager::PadIsAlgKeyTrgUp(int padNum, JOYPAD_ALGKEY algKey)const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	int keyType = static_cast<int>(algKey);
	bool ret = (padInfos_[padNum].IsTrgUpAlgKey[keyType]);
	return ret;
}
bool InputManager::PadIsAlgKeyTrgUp(int padNum, int algKey)const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	bool ret = (padInfos_[padNum].IsTrgUpAlgKey[algKey]);
	return ret;
}

int InputManager::PadAlgKeyX(int padNum, JOYPAD_ALGKEY algKey)const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	int key = static_cast<int>(algKey);
	return padInfos_[padNum].AlgKeyX[key];
}
int InputManager::PadAlgKeyX(int padNum, int algKey)const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	return padInfos_[padNum].AlgKeyX[algKey];
}

int InputManager::PadAlgKeyY(int padNum, JOYPAD_ALGKEY algKey)const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	int key = static_cast<int>(algKey);
	return padInfos_[padNum].AlgKeyY[key];
}
int InputManager::PadAlgKeyY(int padNum, int algKey)const
{
	// パッドが未割当時、false
	if (GetJoypadNum() < padNum) return false;

	return padInfos_[padNum].AlgKeyY[algKey];
}

#pragma endregion