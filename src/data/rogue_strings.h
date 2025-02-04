

// Make sure to bump ROGUE_COMPAT_VERSION in rogue_controller.c when changing this
const u8 gText_RogueVersion[] = _("v1.2.1");

#ifdef ROGUE_EXPANSION
const u8 gText_RogueVersionPrefix[] = _("EX ");
#else
const u8 gText_RogueVersionPrefix[] = _("Vanilla ");
#endif

#ifdef ROGUE_DEBUG
const u8 gText_RogueVersionSuffix[] = _(" (DEBUG)");
#else
const u8 gText_RogueVersionSuffix[] = _(" - The Questing Update");
#endif


const u8 gText_TrainerNameChallenger[] = _("CHALLENGER");
const u8 gText_TrainerNameGrunt[] = _("GRUNT");

#ifdef ROGUE_EXPANSION
const u8 gText_ItemLinkCable[] = _("Link Cable");
const u8 gText_ItemQuestLog[] = _("Quest Log");
#else
const u8 gText_ItemLinkCable[] = _("LINK CABLE");
const u8 gText_ItemQuestLog[] = _("QUEST LOG");
#endif
const u8 gText_ItemQuestLogDesc[] = _("A digital log for\ntracking Quests\nand their rewards.");
const u8 gText_QuestLogTitleOverview[] = _("QUESTS");
const u8 gText_QuestLogTitleDesc[] = _("DESCRIPTION");
const u8 gText_QuestLogTitleRewards[] = _("REWARDS");
const u8 gText_QuestLogTitlePinned[] = _("Pinned");
const u8 gText_QuestLogTitleActive[] = _("In progress");
const u8 gText_QuestLogTitleInactive[] = _("Inactive");
const u8 gText_QuestLogTitleComplete[] = _("Complete");
const u8 gText_QuestLogTitleTodo[] = _("To do");
const u8 gText_QuestLogTitleRepeatable[] = _("Repeatable");
const u8 gText_QuestLogTitleNew[] = _("New!");
const u8 gText_QuestLogTitleStatus[] = _("Status:");
const u8 gText_QuestLogMarkerRepeatable[] = _("REPEATABLE");
const u8 gText_QuestLogMarkerInactive[] = _("INACTIVE");
const u8 gText_QuestLogStatusIncomplete[] = _("INCOMPLETE");
const u8 gText_QuestLogStatusComplete[] = _("COMPLETE");
const u8 gText_QuestLogStatusCollection[] = _("READY TO COLLECT");
const u8 gText_QuestLogStatusCollected[] = _("ALREADY COLLECTED");
const u8 gText_QuestLogTitleRewardMoney[] = _("¥{STR_VAR_1}");
const u8 gText_QuestLogTitleRewardPokemon[] = _("{STR_VAR_1}");
const u8 gText_QuestLogTitleRewardShinyPokemon[] = _("Shiny {STR_VAR_1}");
const u8 gText_QuestLogTitleQuestUnlocks[] = _("Unlocks new Quests");
const u8 gText_QuestLogPromptOverview[] = _("Select a Category");
const u8 gText_QuestLogPromptCategory[] = _("Select a Quest");
const u8 gText_QuestLogPromptPinQuest[] = _("Pin this Quest?");
const u8 gText_QuestLogPromptUnpinQuest[] = _("Unpin this Quest?");
const u8 gText_QuestLogOverviewCompleted[] = _("Completed:");
const u8 gText_QuestLogOverviewUnlocked[] = _("Unlocked:");
const u8 gText_QuestLogOverviewRewardsToCollect[] = _("PENDING REWARDS");
const u8 gText_QuestLogBack[] = _("Back");

const u8 gText_QuestRewardGive[] = _("Recieved {STR_VAR_1}!");
const u8 gText_QuestRewardGiveMon[] = _("Recieved a {STR_VAR_1}!");
const u8 gText_QuestRewardGiveShinyMon[] = _("Recieved a Shiny {STR_VAR_1}!");
const u8 gText_QuestRewardGiveMoney[] = _("Recieved ¥{STR_VAR_1}!");

#ifdef ROGUE_DEBUG
const u8 gText_RogueDebug_Header[] = _("ROGUE DEBUG");
const u8 gText_RogueDebug_Save[] = _("\nSave: ");
const u8 gText_RogueDebug_Room[] = _("\nRoom: ");
const u8 gText_RogueDebug_BossRoom[] = _("\nBossRoom: ");
const u8 gText_RogueDebug_Difficulty[] = _("\nDfcy: ");
const u8 gText_RogueDebug_PlayerLvl[] = _("\nPly lvl: ");
const u8 gText_RogueDebug_WildLvl[] = _("\nWld lvl: ");
const u8 gText_RogueDebug_WildCount[] = _("\nWld Opt: ");
const u8 gText_RogueDebug_ItemCount[] = _("\nItm Opt: ");
const u8 gText_RogueDebug_TrainerCount[] = _("\nTrn Opt: ");
const u8 gText_RogueDebug_Seed[] = _("\nSeed: ");

const u8 gText_RogueDebug_AdvHeader[] = _("ROGUE ADVPATH");
const u8 gText_RogueDebug_AdvCount[] = _("\nCount: ");
const u8 gText_RogueDebug_X[] = _("\nX: ");
const u8 gText_RogueDebug_Y[] = _("\nY: ");
#endif