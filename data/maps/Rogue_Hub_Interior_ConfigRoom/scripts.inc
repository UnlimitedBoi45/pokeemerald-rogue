Rogue_Hub_Interior_ConfigRoom_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_ConfigRoom_RedrawMap
	.byte 0


Rogue_ConfigRoom_TalkLanette::
	lock
	faceplayer
	msgbox Rogue_ConfigRoom_TalkLanette_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_ConfigRoom_TalkAssistant::
	lock
	faceplayer
	goto_if_unset FLAG_SET_SEED_ENABLED, Rogue_ConfigRoom_TalkAssistant_2
Rogue_ConfigRoom_TalkAssistant_1:
	clearflag FLAG_TEMP_2
	call Rogue_ConfigRoom_TalkAssistantInternal
	release
	return

Rogue_ConfigRoom_TalkAssistant_2:
	msgbox Rogue_ConfigRoom_TalkAssistant_Text_0, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_TalkAssistant_1


Rogue_ConfigRoom_SpeakSeedMode::
	lock
	faceplayer
	goto_if_unset FLAG_SET_SEED_ENABLED, Rogue_ConfigRoom_SpeakSeedMode_2
Rogue_ConfigRoom_SpeakSeedMode_1:
	clearflag FLAG_TEMP_2
	call Rogue_ConfigRoom_TalkAssistantInternal
	release
	return

Rogue_ConfigRoom_SpeakSeedMode_2:
	msgbox Rogue_ConfigRoom_SpeakSeedMode_Text_0, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_SpeakSeedMode_1


Rogue_ConfigRoom_SpeakBasicBag::
	lock
	faceplayer
	goto_if_set FLAG_ROGUE_FORCE_BASIC_BAG, Rogue_ConfigRoom_SpeakBasicBag_2
	msgbox Rogue_ConfigRoom_SpeakBasicBag_Text_2
	msgbox Rogue_ConfigRoom_SpeakBasicBag_Text_3, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_SpeakBasicBag_7
Rogue_ConfigRoom_SpeakBasicBag_1:
	release
	return

Rogue_ConfigRoom_SpeakBasicBag_2:
	msgbox Rogue_ConfigRoom_SpeakBasicBag_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_SpeakBasicBag_5
	goto Rogue_ConfigRoom_SpeakBasicBag_1

Rogue_ConfigRoom_SpeakBasicBag_5:
	msgbox Rogue_ConfigRoom_SpeakBasicBag_Text_1, MSGBOX_DEFAULT
	clearflag FLAG_ROGUE_FORCE_BASIC_BAG
	goto Rogue_ConfigRoom_SpeakBasicBag_1

Rogue_ConfigRoom_SpeakBasicBag_7:
	msgbox Rogue_ConfigRoom_SpeakBasicBag_Text_4, MSGBOX_DEFAULT
	setflag FLAG_ROGUE_FORCE_BASIC_BAG
	goto Rogue_ConfigRoom_SpeakBasicBag_1


Rogue_ConfigRoom_TalkAssistantInternal::
	goto_if_set FLAG_SET_SEED_ENABLED, Rogue_ConfigRoom_TalkAssistantInternal_1
	msgbox Rogue_ConfigRoom_TalkAssistantInternal_Text_2, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_TalkAssistantInternal_6
	goto_if_set FLAG_TEMP_2, Rogue_ConfigRoom_TalkAssistantInternal_14
	return

Rogue_ConfigRoom_TalkAssistantInternal_1:
	msgbox Rogue_ConfigRoom_TalkAssistantInternal_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_TalkAssistantInternal_4
	return

Rogue_ConfigRoom_TalkAssistantInternal_4:
	msgbox Rogue_ConfigRoom_TalkAssistantInternal_Text_1, MSGBOX_DEFAULT
	clearflag FLAG_SET_SEED_ENABLED
	goto_if_set FLAG_TEMP_2, Rogue_ConfigRoom_TalkAssistantInternal_9
	return

Rogue_ConfigRoom_TalkAssistantInternal_6:
	msgbox Rogue_ConfigRoom_TalkAssistantInternal_Text_3, MSGBOX_DEFAULT
	setvar VAR_0x8004, EASY_CHAT_TYPE_TRENDY_PHRASE
	call Common_ShowEasyChatScreen
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_ConfigRoom_TalkAssistantInternal_11
	msgbox Rogue_ConfigRoom_TalkAssistantInternal_Text_5, MSGBOX_DEFAULT
	goto_if_set FLAG_TEMP_2, Rogue_ConfigRoom_TalkAssistantInternal_16
	return

Rogue_ConfigRoom_TalkAssistantInternal_9:
	applymovement OBJ_EVENT_ID_PLAYER, ConfigRoom_WalkLeft
	applymovement 1, ConfigRoom_LookLeft
	waitmovement 0
	return

Rogue_ConfigRoom_TalkAssistantInternal_11:
	msgbox Rogue_ConfigRoom_TalkAssistantInternal_Text_4, MSGBOX_DEFAULT
	setflag FLAG_SET_SEED_ENABLED
	return

Rogue_ConfigRoom_TalkAssistantInternal_14:
	applymovement OBJ_EVENT_ID_PLAYER, ConfigRoom_WalkLeft
	applymovement 1, ConfigRoom_LookLeft
	waitmovement 0
	return

Rogue_ConfigRoom_TalkAssistantInternal_16:
	applymovement OBJ_EVENT_ID_PLAYER, ConfigRoom_WalkLeft
	applymovement 1, ConfigRoom_LookLeft
	waitmovement 0
	return


Rogue_ConfigRoom_WalkAboveAssistant::
	lockall
	goto_if_unset FLAG_SET_SEED_ENABLED, Rogue_ConfigRoom_WalkAboveAssistant_2
Rogue_ConfigRoom_WalkAboveAssistant_1:
	releaseall
	return

Rogue_ConfigRoom_WalkAboveAssistant_2:
	lockall
	applymovement 1, ConfigRoom_LookUp
	applymovement OBJ_EVENT_ID_PLAYER, ConfigRoom_LookDown
	waitmovement 0
	setflag FLAG_TEMP_2
	call Rogue_ConfigRoom_TalkAssistantInternal
	goto Rogue_ConfigRoom_WalkAboveAssistant_1


Rogue_ConfigRoom_WalkBelowAssistant::
	lockall
	goto_if_unset FLAG_SET_SEED_ENABLED, Rogue_ConfigRoom_WalkBelowAssistant_2
Rogue_ConfigRoom_WalkBelowAssistant_1:
	releaseall
	return

Rogue_ConfigRoom_WalkBelowAssistant_2:
	applymovement 1, ConfigRoom_LookDown
	applymovement OBJ_EVENT_ID_PLAYER, ConfigRoom_LookUp
	waitmovement 0
	setflag FLAG_TEMP_2
	call Rogue_ConfigRoom_TalkAssistantInternal
	goto Rogue_ConfigRoom_WalkBelowAssistant_1


ConfigRoom_LookUp:
	face_up
	step_end

ConfigRoom_LookDown:
	face_down
	step_end

ConfigRoom_LookLeft:
	face_left
	step_end

ConfigRoom_WalkLeft:
	walk_left
	step_end

Rogue_ConfigRoom_TalkHiddenEnterance::
	goto_if_unset FLAG_ROGUE_UNCOVERRED_POKABBIE, Rogue_ConfigRoom_TalkHiddenEnterance_1
	return

Rogue_ConfigRoom_TalkHiddenEnterance_1:
	lock
	msgbox Rogue_ConfigRoom_TalkHiddenEnterance_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_TalkHiddenEnterance_4
Rogue_ConfigRoom_TalkHiddenEnterance_3:
	release
	return

Rogue_ConfigRoom_TalkHiddenEnterance_4:
	setflag FLAG_ROGUE_UNCOVERRED_POKABBIE
	waitse
	playse SE_TRUCK_DOOR
	call Rogue_ConfigRoom_RedrawMap
	goto Rogue_ConfigRoom_TalkHiddenEnterance_3


Rogue_ConfigRoom_MeetPokabbie::
	lockall
	playse SE_PIN
	applymovement 4, ConfigRoom_Pokabbie_NoticePlayer
	waitmovement 0
	goto_if_set FLAG_IS_CHAMPION, Rogue_ConfigRoom_MeetPokabbie_2
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_2
	applymovement 4, ConfigRoom_Pokabbie_FaceUp
	waitmovement 0
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_3
	applymovement 4, ConfigRoom_Pokabbie_FaceDown
	waitmovement 0
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_4
	applymovement 4, ConfigRoom_Pokabbie_Spin
	waitmovement 0
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_5
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_6
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_1
Rogue_ConfigRoom_MeetPokabbie_1:
	completequest QUEST_MeetPokabbie
	setflag FLAG_ROGUE_MET_POKABBIE
	setvar VAR_TEMP_1, 1
	releaseall
	return

Rogue_ConfigRoom_MeetPokabbie_2:
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_0
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_1
	goto Rogue_ConfigRoom_MeetPokabbie_1


Rogue_ConfigRoom_TalkPokabbie::
	lockall
	faceplayer
	msgbox Rogue_ConfigRoom_TalkPokabbie_Text_0, MSGBOX_DEFAULT
	applymovement 4, ConfigRoom_Pokabbie_Spin
	waitmovement 0
	releaseall
	return


Rogue_ConfigRoom_TalkDitto::
	lock
	playmoncry SPECIES_DITTO, CRY_MODE_NORMAL
	msgbox Rogue_ConfigRoom_TalkDitto_Text_0, MSGBOX_DEFAULT
	release
	return


ConfigRoom_Pokabbie_FaceDown:
	face_down
	step_end

ConfigRoom_Pokabbie_FaceUp:
	face_up
	step_end

ConfigRoom_Pokabbie_NoticePlayer:
	face_down
	emote_exclamation_mark
	step_end

ConfigRoom_Pokabbie_Spin:
	face_right
	delay_1
	face_up
	delay_1
	face_left
	delay_1
	face_down
	delay_1
	face_right
	delay_1
	face_up
	delay_1
	face_left
	delay_1
	face_down
	delay_1
	step_end

Rogue_ConfigRoom_ItemBag::
	bufferitemname STR_VAR_1, VAR_0x8000
	checkitem VAR_0x8000
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ConfigRoom_ItemBag_2
	msgbox Rogue_ConfigRoom_ItemBag_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_ItemBag_7
Rogue_ConfigRoom_ItemBag_1:
	return

Rogue_ConfigRoom_ItemBag_2:
	msgbox Rogue_ConfigRoom_ItemBag_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_ItemBag_5
	goto Rogue_ConfigRoom_ItemBag_1

Rogue_ConfigRoom_ItemBag_5:
	giveitem VAR_0x8000
	goto Rogue_ConfigRoom_ItemBag_1

Rogue_ConfigRoom_ItemBag_7:
	msgbox Rogue_ConfigRoom_ItemBag_Text_2, MSGBOX_DEFAULT
	removeitem VAR_0x8000
	goto Rogue_ConfigRoom_ItemBag_1


Rogue_ConfigRoom_MegaBag::
	lock
	goto_if_set FLAG_ROGUE_EXPANSION_ACTIVE, Rogue_ConfigRoom_MegaBag_2
Rogue_ConfigRoom_MegaBag_1:
	release
	return

Rogue_ConfigRoom_MegaBag_2:
	setvar VAR_0x8000, 703
	call Rogue_ConfigRoom_ItemBag
	goto Rogue_ConfigRoom_MegaBag_1


Rogue_ConfigRoom_ZMoveBag::
	lock
	goto_if_set FLAG_ROGUE_EXPANSION_ACTIVE, Rogue_ConfigRoom_ZMoveBag_2
Rogue_ConfigRoom_ZMoveBag_1:
	release
	return

Rogue_ConfigRoom_ZMoveBag_2:
	setvar VAR_0x8000, 704
	call Rogue_ConfigRoom_ItemBag
	goto Rogue_ConfigRoom_ZMoveBag_1


Rogue_ConfigRoom_DynamaxBag::
	lock
	goto_if_set FLAG_ROGUE_EXPANSION_ACTIVE, Rogue_ConfigRoom_DynamaxBag_2
Rogue_ConfigRoom_DynamaxBag_1:
	release
	return

Rogue_ConfigRoom_DynamaxBag_2:
	setvar VAR_0x8000, 705
	call Rogue_ConfigRoom_ItemBag
	goto Rogue_ConfigRoom_DynamaxBag_1


Rogue_ConfigRoom_AdjustWildGens::
	lock
	faceplayer
	msgbox Rogue_ConfigRoom_AdjustWildGens_Text_0, MSGBOX_DEFAULT
	buffernumberstring STR_VAR_1, VAR_ROGUE_ENABLED_GEN_LIMIT
	message Rogue_ConfigRoom_AdjustWildGens_Text_1
	waitmessage
	goto_if_set FLAG_ROGUE_EXPANSION_ACTIVE, Rogue_ConfigRoom_AdjustWildGens_2
	multichoice 20, 6, MULTI_GENERATIONS_UPTO3, NO
Rogue_ConfigRoom_AdjustWildGens_1:
	switch VAR_RESULT
	case 0, Rogue_ConfigRoom_AdjustWildGens_7
	case 1, Rogue_ConfigRoom_AdjustWildGens_7
	case 2, Rogue_ConfigRoom_AdjustWildGens_7
	case 3, Rogue_ConfigRoom_AdjustWildGens_7
	case 4, Rogue_ConfigRoom_AdjustWildGens_7
	case 5, Rogue_ConfigRoom_AdjustWildGens_7
	case 6, Rogue_ConfigRoom_AdjustWildGens_7
	case 7, Rogue_ConfigRoom_AdjustWildGens_7
	case MULTI_B_PRESSED, Rogue_ConfigRoom_AdjustWildGens_8
Rogue_ConfigRoom_AdjustWildGens_5:
	release
	return

Rogue_ConfigRoom_AdjustWildGens_2:
	multichoice 20, 0, MULTI_GENERATIONS_UPTO8, NO
	goto Rogue_ConfigRoom_AdjustWildGens_1

Rogue_ConfigRoom_AdjustWildGens_7:
	waitse
	playse SE_PIN
	addvar VAR_RESULT, 1
	copyvar VAR_ROGUE_ENABLED_GEN_LIMIT, VAR_RESULT
	buffernumberstring STR_VAR_1, VAR_ROGUE_ENABLED_GEN_LIMIT
	msgbox Rogue_ConfigRoom_AdjustWildGens_Text_2, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_AdjustWildGens_5

Rogue_ConfigRoom_AdjustWildGens_8:
	msgbox Rogue_ConfigRoom_AdjustWildGens_Text_3, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_AdjustWildGens_5


Rogue_ConfigRoom_PreButtonInternal::
	lockall
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_PreButtonInternal_3
	case 1, Rogue_ConfigRoom_PreButtonInternal_4
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreButtonInternal_Text_2
Rogue_ConfigRoom_PreButtonInternal_1:
	message Rogue_ConfigRoom_PreButtonInternal_Text_3
	waitmessage
	multichoice 20, 6, MULTI_YESNOINFO_2, NO
	switch VAR_RESULT
	case 0, Rogue_ConfigRoom_PreButtonInternal_7
	case 1, Rogue_ConfigRoom_PreButtonInternal_8
	case 2, Rogue_ConfigRoom_PreButtonInternal_9
	case MULTI_B_PRESSED, Rogue_ConfigRoom_PreButtonInternal_10
	return

Rogue_ConfigRoom_PreButtonInternal_3:
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreButtonInternal_Text_0
	goto Rogue_ConfigRoom_PreButtonInternal_1

Rogue_ConfigRoom_PreButtonInternal_4:
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreButtonInternal_Text_1
	goto Rogue_ConfigRoom_PreButtonInternal_1

Rogue_ConfigRoom_PreButtonInternal_7:
	waitse
	playse SE_PIN
	setvar VAR_0x8000, 0
	setvar VAR_RESULT, YES
	return

Rogue_ConfigRoom_PreButtonInternal_8:
	waitse
	playse SE_PIN
	setvar VAR_0x8000, 1
	setvar VAR_RESULT, YES
	return

Rogue_ConfigRoom_PreButtonInternal_9:
	setvar VAR_RESULT, NO
	return

Rogue_ConfigRoom_PreButtonInternal_10:
	setvar VAR_RESULT, YES
	return


Rogue_ConfigRoom_PreEAHButtonInternal::
	lockall
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_PreEAHButtonInternal_3
	case 1, Rogue_ConfigRoom_PreEAHButtonInternal_4
	case 2, Rogue_ConfigRoom_PreEAHButtonInternal_5
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreButtonInternal_Text_2
Rogue_ConfigRoom_PreEAHButtonInternal_1:
	message Rogue_ConfigRoom_PreButtonInternal_Text_3
	waitmessage
	multichoice 20, 4, MULTI_EASY_AVERAGE_HARD_INFO, NO
	switch VAR_RESULT
	case 0, Rogue_ConfigRoom_PreEAHButtonInternal_8
	case 1, Rogue_ConfigRoom_PreEAHButtonInternal_9
	case 2, Rogue_ConfigRoom_PreEAHButtonInternal_10
	case 3, Rogue_ConfigRoom_PreEAHButtonInternal_11
	case MULTI_B_PRESSED, Rogue_ConfigRoom_PreEAHButtonInternal_12
	return

Rogue_ConfigRoom_PreEAHButtonInternal_3:
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreEAHButtonInternal_Text_0
	goto Rogue_ConfigRoom_PreEAHButtonInternal_1

Rogue_ConfigRoom_PreEAHButtonInternal_4:
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreEAHButtonInternal_Text_1
	goto Rogue_ConfigRoom_PreEAHButtonInternal_1

Rogue_ConfigRoom_PreEAHButtonInternal_5:
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreEAHButtonInternal_Text_2
	goto Rogue_ConfigRoom_PreEAHButtonInternal_1

Rogue_ConfigRoom_PreEAHButtonInternal_8:
	waitse
	playse SE_PIN
	setvar VAR_0x8000, 0
	setvar VAR_RESULT, YES
	return

Rogue_ConfigRoom_PreEAHButtonInternal_9:
	waitse
	playse SE_PIN
	setvar VAR_0x8000, 1
	setvar VAR_RESULT, YES
	return

Rogue_ConfigRoom_PreEAHButtonInternal_10:
	waitse
	playse SE_PIN
	setvar VAR_0x8000, 2
	setvar VAR_RESULT, YES
	return

Rogue_ConfigRoom_PreEAHButtonInternal_11:
	setvar VAR_RESULT, NO
	return

Rogue_ConfigRoom_PreEAHButtonInternal_12:
	setvar VAR_RESULT, YES
	return


Rogue_ConfigRoom_PostButtonInternal::
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_PostButtonInternal_2
Rogue_ConfigRoom_PostButtonInternal_1:
	releaseall
	return

Rogue_ConfigRoom_PostButtonInternal_2:
	call Rogue_ConfigRoom_RedrawMap
	goto Rogue_ConfigRoom_PostButtonInternal_1


Rogue_ConfigRoom_RedrawMap::
	goto_if_set FLAG_ROGUE_UNCOVERRED_POKABBIE, Rogue_ConfigRoom_RedrawMap_2
Rogue_ConfigRoom_RedrawMap_1:
	goto_if_set FLAG_ROGUE_EXPANSION_ACTIVE, Rogue_ConfigRoom_RedrawMap_5
	setvar VAR_TEMP_2, 1
	setflag FLAG_TEMP_1
Rogue_ConfigRoom_RedrawMap_4:
	goto_if_set FLAG_ROGUE_MET_POKABBIE, Rogue_ConfigRoom_RedrawMap_9
	setvar VAR_TEMP_1, 0
Rogue_ConfigRoom_RedrawMap_8:
	goto_if_set FLAG_ROGUE_EXP_ALL, Rogue_ConfigRoom_RedrawMap_13
	setmetatile 1, 3, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_12:
	goto_if_set FLAG_ROGUE_CAN_OVERLVL, Rogue_ConfigRoom_RedrawMap_17
	setmetatile 3, 3, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_16:
	goto_if_set FLAG_ROGUE_EV_GAIN_ENABLED, Rogue_ConfigRoom_RedrawMap_21
	setmetatile 5, 3, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_20:
	goto_if_set FLAG_ROGUE_HARD_TRAINERS, Rogue_ConfigRoom_RedrawMap_25
	goto_if_set FLAG_ROGUE_EASY_TRAINERS, Rogue_ConfigRoom_RedrawMap_26
	setmetatile 1, 5, METATILE_BikeShop_Button_Green, FALSE
Rogue_ConfigRoom_RedrawMap_24:
	goto_if_set FLAG_ROGUE_HARD_ITEMS, Rogue_ConfigRoom_RedrawMap_31
	goto_if_set FLAG_ROGUE_EASY_ITEMS, Rogue_ConfigRoom_RedrawMap_32
	setmetatile 3, 5, METATILE_BikeShop_Button_Green, FALSE
Rogue_ConfigRoom_RedrawMap_30:
	goto_if_set FLAG_SET_SEED_ITEMS, Rogue_ConfigRoom_RedrawMap_37
	setmetatile 14, 6, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_36:
	goto_if_set FLAG_SET_SEED_WILDMONS, Rogue_ConfigRoom_RedrawMap_41
	setmetatile 16, 6, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_40:
	goto_if_set FLAG_SET_SEED_TRAINERS, Rogue_ConfigRoom_RedrawMap_45
	setmetatile 14, 8, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_44:
	goto_if_set FLAG_SET_SEED_BOSSES, Rogue_ConfigRoom_RedrawMap_49
	setmetatile 16, 8, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_48:
	special DrawWholeMapView
	return

Rogue_ConfigRoom_RedrawMap_2:
	setmetatile 8, 0, 0x25C, TRUE
	setmetatile 8, 1, 0x293, TRUE
	setmetatile 9, 0, 0x26E, TRUE
	setmetatile 9, 1, 0x276, FALSE
	setmetatile 10, 0, 0x25D, TRUE
	setmetatile 10, 1, 0x294, TRUE
	goto Rogue_ConfigRoom_RedrawMap_1

Rogue_ConfigRoom_RedrawMap_5:
	setvar VAR_TEMP_2, 1
	clearflag FLAG_TEMP_1
	goto Rogue_ConfigRoom_RedrawMap_4

Rogue_ConfigRoom_RedrawMap_9:
	setvar VAR_TEMP_1, 1
	goto Rogue_ConfigRoom_RedrawMap_8

Rogue_ConfigRoom_RedrawMap_13:
	setmetatile 1, 3, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_12

Rogue_ConfigRoom_RedrawMap_17:
	setmetatile 3, 3, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_16

Rogue_ConfigRoom_RedrawMap_21:
	setmetatile 5, 3, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_20

Rogue_ConfigRoom_RedrawMap_25:
	setmetatile 1, 5, 0x23E, FALSE
	goto Rogue_ConfigRoom_RedrawMap_24

Rogue_ConfigRoom_RedrawMap_26:
	setmetatile 1, 5, METATILE_BikeShop_Button_Blue, FALSE
	goto Rogue_ConfigRoom_RedrawMap_24

Rogue_ConfigRoom_RedrawMap_31:
	setmetatile 3, 5, 0x23E, FALSE
	goto Rogue_ConfigRoom_RedrawMap_30

Rogue_ConfigRoom_RedrawMap_32:
	setmetatile 3, 5, METATILE_BikeShop_Button_Blue, FALSE
	goto Rogue_ConfigRoom_RedrawMap_30

Rogue_ConfigRoom_RedrawMap_37:
	setmetatile 14, 6, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_36

Rogue_ConfigRoom_RedrawMap_41:
	setmetatile 16, 6, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_40

Rogue_ConfigRoom_RedrawMap_45:
	setmetatile 14, 8, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_44

Rogue_ConfigRoom_RedrawMap_49:
	setmetatile 16, 8, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_48


Rogue_ConfigRoom_Button_ExpAll::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_ExpAll_Text_0
	goto_if_set FLAG_ROGUE_EXP_ALL, Rogue_ConfigRoom_Button_ExpAll_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_ExpAll_1:
	call Rogue_ConfigRoom_PreButtonInternal
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_Button_ExpAll_7
	case 1, Rogue_ConfigRoom_Button_ExpAll_8
Rogue_ConfigRoom_Button_ExpAll_5:
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ConfigRoom_Button_ExpAll_10
Rogue_ConfigRoom_Button_ExpAll_9:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_ExpAll_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_ExpAll_1

Rogue_ConfigRoom_Button_ExpAll_7:
	setflag FLAG_ROGUE_EXP_ALL
	goto Rogue_ConfigRoom_Button_ExpAll_5

Rogue_ConfigRoom_Button_ExpAll_8:
	clearflag FLAG_ROGUE_EXP_ALL
	goto Rogue_ConfigRoom_Button_ExpAll_5

Rogue_ConfigRoom_Button_ExpAll_10:
	msgbox Rogue_ConfigRoom_Button_ExpAll_Text_1, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_Button_ExpAll_9


Rogue_ConfigRoom_Button_CanOverLvl::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_CanOverLvl_Text_0
	goto_if_set FLAG_ROGUE_CAN_OVERLVL, Rogue_ConfigRoom_Button_CanOverLvl_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_CanOverLvl_1:
	call Rogue_ConfigRoom_PreButtonInternal
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_Button_CanOverLvl_7
	case 1, Rogue_ConfigRoom_Button_CanOverLvl_8
Rogue_ConfigRoom_Button_CanOverLvl_5:
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ConfigRoom_Button_CanOverLvl_10
Rogue_ConfigRoom_Button_CanOverLvl_9:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_CanOverLvl_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_CanOverLvl_1

Rogue_ConfigRoom_Button_CanOverLvl_7:
	setflag FLAG_ROGUE_CAN_OVERLVL
	goto Rogue_ConfigRoom_Button_CanOverLvl_5

Rogue_ConfigRoom_Button_CanOverLvl_8:
	clearflag FLAG_ROGUE_CAN_OVERLVL
	goto Rogue_ConfigRoom_Button_CanOverLvl_5

Rogue_ConfigRoom_Button_CanOverLvl_10:
	msgbox Rogue_ConfigRoom_Button_CanOverLvl_Text_1, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_Button_CanOverLvl_9


Rogue_ConfigRoom_Button_EVGainEnabled::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_EVGainEnabled_Text_0
	goto_if_set FLAG_ROGUE_EV_GAIN_ENABLED, Rogue_ConfigRoom_Button_EVGainEnabled_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_EVGainEnabled_1:
	call Rogue_ConfigRoom_PreButtonInternal
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_Button_EVGainEnabled_7
	case 1, Rogue_ConfigRoom_Button_EVGainEnabled_8
Rogue_ConfigRoom_Button_EVGainEnabled_5:
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ConfigRoom_Button_EVGainEnabled_10
Rogue_ConfigRoom_Button_EVGainEnabled_9:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_EVGainEnabled_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_EVGainEnabled_1

Rogue_ConfigRoom_Button_EVGainEnabled_7:
	setflag FLAG_ROGUE_EV_GAIN_ENABLED
	goto Rogue_ConfigRoom_Button_EVGainEnabled_5

Rogue_ConfigRoom_Button_EVGainEnabled_8:
	clearflag FLAG_ROGUE_EV_GAIN_ENABLED
	goto Rogue_ConfigRoom_Button_EVGainEnabled_5

Rogue_ConfigRoom_Button_EVGainEnabled_10:
	msgbox Rogue_ConfigRoom_Button_EVGainEnabled_Text_1, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_Button_EVGainEnabled_9


Rogue_ConfigRoom_Button_TrainerDifficulty::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_TrainerDifficulty_Text_0
	goto_if_set FLAG_ROGUE_EASY_TRAINERS, Rogue_ConfigRoom_Button_TrainerDifficulty_2
	goto_if_set FLAG_ROGUE_HARD_TRAINERS, Rogue_ConfigRoom_Button_TrainerDifficulty_3
	setvar VAR_0x8000, 1
Rogue_ConfigRoom_Button_TrainerDifficulty_1:
	call Rogue_ConfigRoom_PreEAHButtonInternal
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_Button_TrainerDifficulty_9
	case 1, Rogue_ConfigRoom_Button_TrainerDifficulty_10
	case 2, Rogue_ConfigRoom_Button_TrainerDifficulty_11
Rogue_ConfigRoom_Button_TrainerDifficulty_7:
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ConfigRoom_Button_TrainerDifficulty_13
Rogue_ConfigRoom_Button_TrainerDifficulty_12:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_TrainerDifficulty_2:
	setvar VAR_0x8000, 0
	goto Rogue_ConfigRoom_Button_TrainerDifficulty_1

Rogue_ConfigRoom_Button_TrainerDifficulty_3:
	setvar VAR_0x8000, 2
	goto Rogue_ConfigRoom_Button_TrainerDifficulty_1

Rogue_ConfigRoom_Button_TrainerDifficulty_9:
	setflag FLAG_ROGUE_EASY_TRAINERS
	clearflag FLAG_ROGUE_HARD_TRAINERS
	goto Rogue_ConfigRoom_Button_TrainerDifficulty_7

Rogue_ConfigRoom_Button_TrainerDifficulty_10:
	clearflag FLAG_ROGUE_EASY_TRAINERS
	clearflag FLAG_ROGUE_HARD_TRAINERS
	goto Rogue_ConfigRoom_Button_TrainerDifficulty_7

Rogue_ConfigRoom_Button_TrainerDifficulty_11:
	clearflag FLAG_ROGUE_EASY_TRAINERS
	setflag FLAG_ROGUE_HARD_TRAINERS
	goto Rogue_ConfigRoom_Button_TrainerDifficulty_7

Rogue_ConfigRoom_Button_TrainerDifficulty_13:
	msgbox Rogue_ConfigRoom_Button_TrainerDifficulty_Text_1, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_Button_TrainerDifficulty_12


Rogue_ConfigRoom_Button_ItemDifficulty::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_ItemDifficulty_Text_0
	goto_if_set FLAG_ROGUE_EASY_ITEMS, Rogue_ConfigRoom_Button_ItemDifficulty_2
	goto_if_set FLAG_ROGUE_HARD_ITEMS, Rogue_ConfigRoom_Button_ItemDifficulty_3
	setvar VAR_0x8000, 1
Rogue_ConfigRoom_Button_ItemDifficulty_1:
	call Rogue_ConfigRoom_PreEAHButtonInternal
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_Button_ItemDifficulty_9
	case 1, Rogue_ConfigRoom_Button_ItemDifficulty_10
	case 2, Rogue_ConfigRoom_Button_ItemDifficulty_11
Rogue_ConfigRoom_Button_ItemDifficulty_7:
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ConfigRoom_Button_ItemDifficulty_13
Rogue_ConfigRoom_Button_ItemDifficulty_12:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_ItemDifficulty_2:
	setvar VAR_0x8000, 0
	goto Rogue_ConfigRoom_Button_ItemDifficulty_1

Rogue_ConfigRoom_Button_ItemDifficulty_3:
	setvar VAR_0x8000, 2
	goto Rogue_ConfigRoom_Button_ItemDifficulty_1

Rogue_ConfigRoom_Button_ItemDifficulty_9:
	setflag FLAG_ROGUE_EASY_ITEMS
	clearflag FLAG_ROGUE_HARD_ITEMS
	goto Rogue_ConfigRoom_Button_ItemDifficulty_7

Rogue_ConfigRoom_Button_ItemDifficulty_10:
	clearflag FLAG_ROGUE_EASY_ITEMS
	clearflag FLAG_ROGUE_HARD_ITEMS
	goto Rogue_ConfigRoom_Button_ItemDifficulty_7

Rogue_ConfigRoom_Button_ItemDifficulty_11:
	clearflag FLAG_ROGUE_EASY_ITEMS
	setflag FLAG_ROGUE_HARD_ITEMS
	goto Rogue_ConfigRoom_Button_ItemDifficulty_7

Rogue_ConfigRoom_Button_ItemDifficulty_13:
	msgbox Rogue_ConfigRoom_Button_ItemDifficulty_Text_1, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_Button_ItemDifficulty_12


Rogue_ConfigRoom_Button_SeedItems::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_SeedItems_Text_0
	goto_if_set FLAG_SET_SEED_ITEMS, Rogue_ConfigRoom_Button_SeedItems_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_SeedItems_1:
	call Rogue_ConfigRoom_PreButtonInternal
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_Button_SeedItems_7
	case 1, Rogue_ConfigRoom_Button_SeedItems_8
Rogue_ConfigRoom_Button_SeedItems_5:
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ConfigRoom_Button_SeedItems_10
Rogue_ConfigRoom_Button_SeedItems_9:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_SeedItems_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_SeedItems_1

Rogue_ConfigRoom_Button_SeedItems_7:
	setflag FLAG_SET_SEED_ITEMS
	goto Rogue_ConfigRoom_Button_SeedItems_5

Rogue_ConfigRoom_Button_SeedItems_8:
	clearflag FLAG_SET_SEED_ITEMS
	goto Rogue_ConfigRoom_Button_SeedItems_5

Rogue_ConfigRoom_Button_SeedItems_10:
	msgbox Rogue_ConfigRoom_Button_SeedItems_Text_1, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_Button_SeedItems_9


Rogue_ConfigRoom_Button_SeedWildMons::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_SeedWildMons_Text_0
	goto_if_set FLAG_SET_SEED_WILDMONS, Rogue_ConfigRoom_Button_SeedWildMons_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_SeedWildMons_1:
	call Rogue_ConfigRoom_PreButtonInternal
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_Button_SeedWildMons_7
	case 1, Rogue_ConfigRoom_Button_SeedWildMons_8
Rogue_ConfigRoom_Button_SeedWildMons_5:
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ConfigRoom_Button_SeedWildMons_10
Rogue_ConfigRoom_Button_SeedWildMons_9:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_SeedWildMons_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_SeedWildMons_1

Rogue_ConfigRoom_Button_SeedWildMons_7:
	setflag FLAG_SET_SEED_WILDMONS
	goto Rogue_ConfigRoom_Button_SeedWildMons_5

Rogue_ConfigRoom_Button_SeedWildMons_8:
	clearflag FLAG_SET_SEED_WILDMONS
	goto Rogue_ConfigRoom_Button_SeedWildMons_5

Rogue_ConfigRoom_Button_SeedWildMons_10:
	msgbox Rogue_ConfigRoom_Button_SeedWildMons_Text_1, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_Button_SeedWildMons_9


Rogue_ConfigRoom_Button_SeedTrainers::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_SeedTrainers_Text_0
	goto_if_set FLAG_SET_SEED_TRAINERS, Rogue_ConfigRoom_Button_SeedTrainers_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_SeedTrainers_1:
	call Rogue_ConfigRoom_PreButtonInternal
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_Button_SeedTrainers_7
	case 1, Rogue_ConfigRoom_Button_SeedTrainers_8
Rogue_ConfigRoom_Button_SeedTrainers_5:
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ConfigRoom_Button_SeedTrainers_10
Rogue_ConfigRoom_Button_SeedTrainers_9:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_SeedTrainers_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_SeedTrainers_1

Rogue_ConfigRoom_Button_SeedTrainers_7:
	setflag FLAG_SET_SEED_TRAINERS
	goto Rogue_ConfigRoom_Button_SeedTrainers_5

Rogue_ConfigRoom_Button_SeedTrainers_8:
	clearflag FLAG_SET_SEED_TRAINERS
	goto Rogue_ConfigRoom_Button_SeedTrainers_5

Rogue_ConfigRoom_Button_SeedTrainers_10:
	msgbox Rogue_ConfigRoom_Button_SeedTrainers_Text_1, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_Button_SeedTrainers_9


Rogue_ConfigRoom_Button_SeedBosses::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_SeedBosses_Text_0
	goto_if_set FLAG_SET_SEED_BOSSES, Rogue_ConfigRoom_Button_SeedBosses_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_SeedBosses_1:
	call Rogue_ConfigRoom_PreButtonInternal
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_Button_SeedBosses_7
	case 1, Rogue_ConfigRoom_Button_SeedBosses_8
Rogue_ConfigRoom_Button_SeedBosses_5:
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ConfigRoom_Button_SeedBosses_10
Rogue_ConfigRoom_Button_SeedBosses_9:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_SeedBosses_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_SeedBosses_1

Rogue_ConfigRoom_Button_SeedBosses_7:
	setflag FLAG_SET_SEED_BOSSES
	goto Rogue_ConfigRoom_Button_SeedBosses_5

Rogue_ConfigRoom_Button_SeedBosses_8:
	clearflag FLAG_SET_SEED_BOSSES
	goto Rogue_ConfigRoom_Button_SeedBosses_5

Rogue_ConfigRoom_Button_SeedBosses_10:
	msgbox Rogue_ConfigRoom_Button_SeedBosses_Text_1, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_Button_SeedBosses_9


Rogue_ConfigRoom_TalkLanette_Text_0:
	.string "LANETTE: Hi {PLAYER}!\p"
	.string "This is the CONFIG LAB!\p"
	.string "In here you can change settings for\n"
	.string "the next time you go on an adventure.\p"
	.string "Just step onto a switch to adjust the\n"
	.string "setting.$"

Rogue_ConfigRoom_TalkAssistant_Text_0:
	.string "ASSISTANT: The settings back here are\n"
	.string "for SEEDED mode!\p"
	.string "In SEEDED mode you will always\n"
	.string "encounter the same ROUTES, TRAINERS\l"
	.string "and POKéMON.\p"
	.string "I can enabled SEEDED mode for your\n"
	.string "next adventure, if you'd like?$"

Rogue_ConfigRoom_SpeakSeedMode_Text_0:
	.string "ASSISTANT: I can enabled SEEDED mode\n"
	.string "for your next adventure, if you'd like?$"

Rogue_ConfigRoom_SpeakBasicBag_Text_0:
	.string "Would you like to disable the BASIC\n"
	.string "BAG?$"

Rogue_ConfigRoom_SpeakBasicBag_Text_1:
	.string "OK. You'll no longer have the BASIC BAG\n"
	.string "when going on Adventures.$"

Rogue_ConfigRoom_SpeakBasicBag_Text_2:
	.string "Hi {PLAYER}!\p"
	.string "I can give you the BASIC BAG instead of\n"
	.string "your current one if you'd like.\p"
	.string "When you leave on an Adventure with a\n"
	.string "BASIC BAG you will have your BAG and\l"
	.string "MONEY reset so you will have none of\l"
	.string "the new upgrades you've gotten in the\l"
	.string "HUB.\p"
	.string "This isn't permanent of course...\n"
	.string "You will be given back you current\l"
	.string "contents once you return back to the\l"
	.string "HUB as per usual.$"

Rogue_ConfigRoom_SpeakBasicBag_Text_3:
	.string "Would you like to enable the BASIC BAG?$"

Rogue_ConfigRoom_SpeakBasicBag_Text_4:
	.string "OK. You'll be given the BASIC BAG when\n"
	.string "going on Adventures.$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_0:
	.string "ASSISTANT: Do you want to disable\n"
	.string "SEEDED mode?$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_1:
	.string "ASSISTANT: OK.\n"
	.string "I'll disable SEEDED mode$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_2:
	.string "ASSISTANT: Do you want to enable\n"
	.string "SEEDED mode for your next adventure?$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_3:
	.string "ASSISTANT: Please select the SEED\n"
	.string "phrases$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_4:
	.string "ASSISTANT: OK.\n"
	.string "I'll enable SEEDED mode using that\l"
	.string "phrase!$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_5:
	.string "ASSISTANT: Oh.\n"
	.string "Speak to me if you ever want to enable\l"
	.string "SEEDED mode$"

Rogue_ConfigRoom_TalkHiddenEnterance_Text_0:
	.string "What's this?\p"
	.string "A hidden switch!\p"
	.string "Press the switch?$"

Rogue_ConfigRoom_MeetPokabbie_Text_0:
	.string "POKABBIE: Ah {PLAYER}!\p"
	.string "Welcome to my LAB!\n"
	.string "Or what I like to dub...\p"
	.string "The EXPANSION ROOM!$"

Rogue_ConfigRoom_MeetPokabbie_Text_1:
	.string "You can use this ROOM to enable\n"
	.string "SPECIAL MODES and although there's\l"
	.string "not too many now, I'll be sure to add\l"
	.string "more in future!\p"
	.string "Feel free to look around!$"

Rogue_ConfigRoom_MeetPokabbie_Text_2:
	.string "???: What are you doing here!\p"
	.string "You discovered my SECRET LAB already!$"

Rogue_ConfigRoom_MeetPokabbie_Text_3:
	.string "I was gonna wait until you had beat the\n"
	.string "CHAMPION at least...$"

Rogue_ConfigRoom_MeetPokabbie_Text_4:
	.string "Anyway!\p"
	.string "Where are my manners!\n"
	.string "Allow me to introduce myself!$"

Rogue_ConfigRoom_MeetPokabbie_Text_5:
	.string "The name's Abbie!\n"
	.string "But you can call me Pokabbie!$"

Rogue_ConfigRoom_MeetPokabbie_Text_6:
	.string "POKABBIE: I'm the DEV behind EMERALD\n"
	.string "ROGUE!\p"
	.string "Welcome to my LAB!\n"
	.string "Or what I like to dub...\p"
	.string "The EXPANSION ROOM!$"

Rogue_ConfigRoom_TalkPokabbie_Text_0:
	.string "POKABBIE: I hope you're enjoying\n"
	.string "EMERALD ROGUE!\p"
	.string "Feel free to enable any SPECIAL MODES\n"
	.string "from this room!\p"
	.string "I'll be sure to add some more in future!$"

Rogue_ConfigRoom_TalkDitto_Text_0:
	.string "A Ditto!\p"
	.string "Why is it here?$"

Rogue_ConfigRoom_ItemBag_Text_0:
	.string "Would you like to take the\n"
	.string "{STR_VAR_1}?$"

Rogue_ConfigRoom_ItemBag_Text_1:
	.string "Would you like to put back the\n"
	.string "{STR_VAR_1}?$"

Rogue_ConfigRoom_ItemBag_Text_2:
	.string "The {STR_VAR_1} was placed back\n"
	.string "into the bag.$"

Rogue_ConfigRoom_AdjustWildGens_Text_0:
	.string "Hi there {PLAYER}!\p"
	.string "I can adjust the enabled GENERATIONs\n"
	.string "if you'd like to expand or shrink the\l"
	.string "types of POKéMON you'll encounter.$"

Rogue_ConfigRoom_AdjustWildGens_Text_1:
	.string "GEN {STR_VAR_1} is the current max.\n"
	.string "What should it be changed to?$"

Rogue_ConfigRoom_AdjustWildGens_Text_2:
	.string "Now you will encounter POKéMON upto\n"
	.string "GENERATION {STR_VAR_1}.$"

Rogue_ConfigRoom_AdjustWildGens_Text_3:
	.string "No worries!\n"
	.string "Come chat to me if you ever change your\l"
	.string "mind.$"

Rogue_ConfigRoom_PreButtonInternal_Text_0:
	.string "OFF$"

Rogue_ConfigRoom_PreButtonInternal_Text_1:
	.string "ON$"

Rogue_ConfigRoom_PreButtonInternal_Text_2:
	.string "???$"

Rogue_ConfigRoom_PreButtonInternal_Text_3:
	.string "{STR_VAR_1} is currently\n"
	.string "'{STR_VAR_2}'$"

Rogue_ConfigRoom_PreEAHButtonInternal_Text_0:
	.string "EASY$"

Rogue_ConfigRoom_PreEAHButtonInternal_Text_1:
	.string "AVERAGE$"

Rogue_ConfigRoom_PreEAHButtonInternal_Text_2:
	.string "HARD$"

Rogue_ConfigRoom_Button_ExpAll_Text_0:
	.string "EXP. ALL$"

Rogue_ConfigRoom_Button_ExpAll_Text_1:
	.string "When enabled all POKéMON in the party\n"
	.string "will gain EXP.\n"
	.string "It is heavily recommended that you\l"
	.string "keep this ON as the game has been\l"
	.string "balanced around it.$"

Rogue_ConfigRoom_Button_CanOverLvl_Text_0:
	.string "ALLOW OVERLEVLING$"

Rogue_ConfigRoom_Button_CanOverLvl_Text_1:
	.string "When disabled will prevent POKéMON from\n"
	.string "levelling past the hight level POKéMON\l"
	.string "of the next GYM LEADER/BOSS.$"

Rogue_ConfigRoom_Button_EVGainEnabled_Text_0:
	.string "ENABLE EVs$"

Rogue_ConfigRoom_Button_EVGainEnabled_Text_1:
	.string "When enabled POKéMON will gain EVs at\n"
	.string "an accelerated rate.\p"
	.string "When disabled POKéMON will gain no EVs.$"

Rogue_ConfigRoom_Button_TrainerDifficulty_Text_0:
	.string "TRAINER DIFFICULTY$"

Rogue_ConfigRoom_Button_TrainerDifficulty_Text_1:
	.string "Adjust teams/moves from very casual to\n"
	.string "very competitive.\p"
	.string "Average mode will start casual but get\n"
	.string "more competitive as you progress.\p"
	.string "The higher this difficulty the more\n"
	.string "rewards you get when returning from an\l"
	.string "adventure.$"

Rogue_ConfigRoom_Button_ItemDifficulty_Text_0:
	.string "ITEM RATES$"

Rogue_ConfigRoom_Button_ItemDifficulty_Text_1:
	.string "Increases/Decrease the rate at which\n"
	.string "items will appear and money trainers\l"
	.string "give$"

Rogue_ConfigRoom_Button_SeedItems_Text_0:
	.string "SEED ITEMS$"

Rogue_ConfigRoom_Button_SeedItems_Text_1:
	.string "If disabled items will be completely\n"
	.string "RANDOM and not linked to the SEED.$"

Rogue_ConfigRoom_Button_SeedWildMons_Text_0:
	.string "SEED WILD POKéMON$"

Rogue_ConfigRoom_Button_SeedWildMons_Text_1:
	.string "If disabled WILD POKéMON encounters\n"
	.string "will be completely RANDOM and not\l"
	.string "linked to the SEED.\p"
	.string "Note this does not affect LEGENDARY\n"
	.string "encounters.$"

Rogue_ConfigRoom_Button_SeedTrainers_Text_0:
	.string "SEED TRAINERS$"

Rogue_ConfigRoom_Button_SeedTrainers_Text_1:
	.string "If disabled TRAINER spawns and teams\n"
	.string "will be completely RANDOM and not\l"
	.string "linked to the SEED.$"

Rogue_ConfigRoom_Button_SeedBosses_Text_0:
	.string "SEED BOSSES$"

Rogue_ConfigRoom_Button_SeedBosses_Text_1:
	.string "If disabled GYM LEADER teams will be\n"
	.string "completely RANDOM and not linked to\l"
	.string "the SEED.$"
