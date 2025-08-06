#ifndef Z64_AUDIO_SEQPLAYER_H
#define Z64_AUDIO_SEQPLAYER_H

/**
 * @file audio_seqplayer.c
 *
 * Manages audio sequence players, interprets and executes sequence instructions used to write .seq files
 *
 * Sequence Instructions:
 *   - A customized assembly language based on MIDI
 *   - All sequences are written using these instructions
 *   - There are 3 different sets of instructions
 *        1) Sequence Instructions
 *        2) Channel Instructions
 *        3) Layer Instruction
 *   - All three sets share a common pool of control flow instructions (>= 0xF2).
 *     Otherwise, each set of instructions has its own command interpreter
 */
#include "ultra64.h"
#include "global.h"

#define PORTAMENTO_IS_SPECIAL(x) ((x).mode & 0x80)
#define PORTAMENTO_MODE(x) ((x).mode & ~0x80)

#define PROCESS_SCRIPT_END -1

typedef enum {
    /* 0 */ PORTAMENTO_MODE_OFF,
    /* 1 */ PORTAMENTO_MODE_1,
    /* 2 */ PORTAMENTO_MODE_2,
    /* 3 */ PORTAMENTO_MODE_3,
    /* 4 */ PORTAMENTO_MODE_4,
    /* 5 */ PORTAMENTO_MODE_5
} PortamentoMode;

u8 AudioSeq_ScriptReadU8(SeqScriptState* state);
s16 AudioSeq_ScriptReadS16(SeqScriptState* state);
u16 AudioSeq_ScriptReadCompressedU16(SeqScriptState* state);

void AudioSeq_SeqLayerProcessScriptStep1(SequenceLayer* layer);
s32 AudioSeq_SeqLayerProcessScriptStep2(SequenceLayer* layer);
s32 AudioSeq_SeqLayerProcessScriptStep3(SequenceLayer* layer, s32 cmd);
s32 AudioSeq_SeqLayerProcessScriptStep4(SequenceLayer* layer, s32 cmd);
s32 AudioSeq_SeqLayerProcessScriptStep5(SequenceLayer* layer, s32 sameTunedSample);

u8 AudioSeq_GetInstrument(SequenceChannel* channel, u8 instId, Instrument** instOut, AdsrSettings* adsr);

/**
 * sSeqInstructionArgsTable is a table for each sequence instruction
 * that contains both how many arguments an instruction takes, as well
 * as the type of each argument
 *
 * sSeqInstructionArgsTable is bitpacked as follows:
 * abcUUUnn
 *
 * n - number of arguments that the sequence instruction takes
 *
 * a - bitFlag for the type of arg0 if it exists
 * b - bitFlag for the type of arg1 if it exists
 * c - bitFlag for the type of arg2 if it exists
 *
 * bitFlag on - argument is s16
 * bitFlag off - argument is u8
 *
 * U - Unused
 */

// CMD_ARGS_(NUMBER_OF_ARGS)
#define CMD_ARGS_0() 0
#define CMD_ARGS_1(arg0Type) (((sizeof(arg0Type) - 1) << 7) | 1)
#define CMD_ARGS_2(arg0Type, arg1Type) (((sizeof(arg0Type) - 1) << 7) | ((sizeof(arg1Type) - 1) << 6) | 2)
#define CMD_ARGS_3(arg0Type, arg1Type, arg2Type) \
    (((sizeof(arg0Type) - 1) << 7) | ((sizeof(arg1Type) - 1) << 6) | ((sizeof(arg2Type) - 1) << 5) | 3)

extern u8 sSeqInstructionArgsTable[];

/**
 * Read and return the argument from the sequence script for a control flow instruction.
 * Control flow instructions (>= 0xF2) can only have 0 or 1 args.
 * @return the argument value for a control flow instruction, or 0 if there is no argument
 */
u16 AudioSeq_GetScriptControlFlowArgument(SeqScriptState* state, u8 cmd);

/**
 * Read and execute the control flow sequence instructions
 * @return number of frames until next instruction. -1 signals termination
 */
s32 AudioSeq_HandleScriptFlowControl(SequencePlayer* seqPlayer, SeqScriptState* state, s32 cmd, s32 cmdArg);

void AudioSeq_InitSequenceChannel(SequenceChannel* channel);

s32 AudioSeq_SeqChannelSetLayer(SequenceChannel* channel, s32 layerIndex);

void AudioSeq_SeqLayerDisable(SequenceLayer* layer);

void AudioSeq_SeqLayerFree(SequenceChannel* channel, s32 layerIndex);

void AudioSeq_SequenceChannelDisable(SequenceChannel* channel);

void AudioSeq_SequencePlayerSetupChannels(SequencePlayer* seqPlayer, u16 channelBits);

void AudioSeq_SequencePlayerDisableChannels(SequencePlayer* seqPlayer, u16 channelBitsUnused);

void AudioSeq_SequenceChannelEnable(SequencePlayer* seqPlayer, u8 channelIndex, void* script);

void AudioSeq_SequencePlayerDisableAsFinished(SequencePlayer* seqPlayer);

void AudioSeq_SequencePlayerDisable(SequencePlayer* seqPlayer);

void AudioSeq_AudioListPushBack(AudioListItem* list, AudioListItem* item);

void* AudioSeq_AudioListPopBack(AudioListItem* list);

void AudioSeq_InitLayerFreelist(void);

u8 AudioSeq_ScriptReadU8(SeqScriptState* state);

s16 AudioSeq_ScriptReadS16(SeqScriptState* state);

u16 AudioSeq_ScriptReadCompressedU16(SeqScriptState* state);

void AudioSeq_SeqLayerProcessScript(SequenceLayer* layer);

void AudioSeq_SeqLayerProcessScriptStep1(SequenceLayer* layer);

s32 AudioSeq_SeqLayerProcessScriptStep5(SequenceLayer* layer, s32 sameTunedSample);

s32 AudioSeq_SeqLayerProcessScriptStep2(SequenceLayer* layer);

s32 AudioSeq_SeqLayerProcessScriptStep4(SequenceLayer* layer, s32 cmd);

s32 AudioSeq_SeqLayerProcessScriptStep3(SequenceLayer* layer, s32 cmd);

void AudioSeq_SetChannelPriorities(SequenceChannel* channel, u8 priority);

u8 AudioSeq_GetInstrument(SequenceChannel* channel, u8 instId, Instrument** instOut, AdsrSettings* adsr);

void AudioSeq_SetInstrument(SequenceChannel* channel, u8 instId);

void AudioSeq_SequenceChannelSetVolume(SequenceChannel* channel, u8 volume);

void AudioSeq_SequenceChannelProcessScript(SequenceChannel* channel);

void AudioSeq_SequencePlayerProcessSequence(SequencePlayer* seqPlayer);

void AudioSeq_ProcessSequences(s32 arg0);

void AudioSeq_SkipForwardSequence(SequencePlayer* seqPlayer);

void AudioSeq_ResetSequencePlayer(SequencePlayer* seqPlayer);

void AudioSeq_InitSequencePlayerChannels(s32 playerIdx);

void AudioSeq_InitSequencePlayer(SequencePlayer* seqPlayer);

void AudioSeq_InitSequencePlayers(void);

#endif
