Rogue_Boss_13_MapScripts::
	.byte 0


Rogue_Battle_Boss13::
	lockall
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss13
	waitmovement 0
	buffertrainername STR_VAR_1, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	msgbox Rogue_Battle_Boss13_Text_0
	call Rogue_SetDesiredWeather
	applymovement 1, BossToFightBoss13
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss13
	waitmovement 0
	msgbox Rogue_Battle_Boss13_Text_1
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_Battle_Boss13_Text_2
	call Rogue_BattleDefeated_Boss13
	return


Rogue_BattleDefeated_Boss13::
	fadescreen FADE_TO_BLACK
	special GameClear
	waitstate
	releaseall
	return


CameraToBattleFieldBoss13:
	walk_up
	walk_up
	walk_up
	step_end

PlayerToFightBoss13:
	walk_up
	step_end

BossToFightBoss13:
	face_down
	step_end

Rogue_Battle_Boss13_Text_0:
	.string "...\p"
	.string "...\p"
	.string "...\p"
	.string "{STR_VAR_1}: You did well to make it\n"
	.string "here...\p"
	.string "Now...\p"
	.string "For one last time...$"

Rogue_Battle_Boss13_Text_1:
	.string "Let's battle!$"

Rogue_Battle_Boss13_Text_2:
	.string "Congratulations {PLAYER}!$"
