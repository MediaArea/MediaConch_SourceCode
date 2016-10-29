/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef GENERATEDCSVVIDEOS_HPP
#define GENERATEDCSVVIDEOS_HPP

#include <map>
#include <vector>
#include <string>

/* This file is generated, do not modify it without the script. */
int get_generated_values_from_csv(std::map<std::string, std::map<std::string, std::vector<std::string> > >& values)
{
    {
        std::map<std::string, std::vector<std::string> > tmp;
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Aligned");
            tmp_vec.push_back("Split");
            tmp["Alignment"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0");
            tmp_vec.push_back("1");
            tmp_vec.push_back("2");
            tmp_vec.push_back("4");
            tmp_vec.push_back("8");
            tmp_vec.push_back("12");
            tmp_vec.push_back("13");
            tmp_vec.push_back("16");
            tmp_vec.push_back("16 / 20");
            tmp_vec.push_back("16 / 24");
            tmp_vec.push_back("20");
            tmp_vec.push_back("20 / 24");
            tmp_vec.push_back("24");
            tmp_vec.push_back("32");
            tmp["BitDepth"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("20");
            tmp_vec.push_back("24");
            tmp["BitDepth_Stored"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("CBR");
            tmp_vec.push_back("VBR");
            tmp_vec.push_back("VBR / CBR");
            tmp["BitRate_Mode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1");
            tmp_vec.push_back("10");
            tmp_vec.push_back("11");
            tmp_vec.push_back("16");
            tmp_vec.push_back("18");
            tmp_vec.push_back("2");
            tmp_vec.push_back("2 / 1 / 1");
            tmp_vec.push_back("2 / 2 / 2");
            tmp_vec.push_back("3");
            tmp_vec.push_back("3+");
            tmp_vec.push_back("4");
            tmp_vec.push_back("5");
            tmp_vec.push_back("6");
            tmp_vec.push_back("6 / 5");
            tmp_vec.push_back("6 / 6");
            tmp_vec.push_back("7");
            tmp_vec.push_back("7 / 5");
            tmp_vec.push_back("7 / 6");
            tmp_vec.push_back("8");
            tmp_vec.push_back("8 / 6");
            tmp["Channel(s)"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1+1");
            tmp_vec.push_back("C");
            tmp_vec.push_back("C L R");
            tmp_vec.push_back("C L R Cs");
            tmp_vec.push_back("C L R Cs LFE");
            tmp_vec.push_back("C L R LFE");
            tmp_vec.push_back("C L R Ls Rs");
            tmp_vec.push_back("C L R Ls Rs LFE");
            tmp_vec.push_back("C X X X");
            tmp_vec.push_back("C X X X X X");
            tmp_vec.push_back("C X X X X X X X");
            tmp_vec.push_back("Cs");
            tmp_vec.push_back("Discrete-0");
            tmp_vec.push_back("Discrete-1");
            tmp_vec.push_back("Discrete-10");
            tmp_vec.push_back("Discrete-11");
            tmp_vec.push_back("Discrete-2");
            tmp_vec.push_back("Discrete-3");
            tmp_vec.push_back("Discrete-4");
            tmp_vec.push_back("Discrete-5");
            tmp_vec.push_back("Discrete-6");
            tmp_vec.push_back("Discrete-7");
            tmp_vec.push_back("Discrete-8");
            tmp_vec.push_back("Discrete-9");
            tmp_vec.push_back("EBU R 123: 4b");
            tmp_vec.push_back("L");
            tmp_vec.push_back("L C Ls Lc R LFE Rs Rc");
            tmp_vec.push_back("L C Ls Lrs R LFE Rs Rrs");
            tmp_vec.push_back("L C Ls R LFE Rs");
            tmp_vec.push_back("L C Ls X R LFE Rs X");
            tmp_vec.push_back("L C R");
            tmp_vec.push_back("L C R Cs");
            tmp_vec.push_back("L C R LFE");
            tmp_vec.push_back("L C R LFE Cs");
            tmp_vec.push_back("L C R LFE Ls Rs");
            tmp_vec.push_back("L C R LFE Ls Rs / L C R LFE Ls Rs");
            tmp_vec.push_back("L C R LFE Ls Rs Cs / L C R LFE Ls Rs");
            tmp_vec.push_back("L C R LFE Ls Rs Lrs Rrs / L C R LFE Ls Rs");
            tmp_vec.push_back("L C R Ls Rs");
            tmp_vec.push_back("L C R Ls Rs Cs / L C R Ls Rs");
            tmp_vec.push_back("L C R Ls Rs LFE");
            tmp_vec.push_back("L C R Ls Rs Lrs Rrs / L C R Ls Rs");
            tmp_vec.push_back("L C R S");
            tmp_vec.push_back("L C X R S X");
            tmp_vec.push_back("L C X X R S X X");
            tmp_vec.push_back("L R");
            tmp_vec.push_back("L R C LFE Ls Rs");
            tmp_vec.push_back("L R Cs");
            tmp_vec.push_back("L R Cs LFE");
            tmp_vec.push_back("L R LFE");
            tmp_vec.push_back("L R LFE Ls Rs");
            tmp_vec.push_back("L R Ls Rs");
            tmp_vec.push_back("L R Ls Rs LFE");
            tmp_vec.push_back("L R S");
            tmp_vec.push_back("L R S LFE");
            tmp_vec.push_back("L X R X");
            tmp_vec.push_back("L X X R X X");
            tmp_vec.push_back("L X X X R X X X");
            tmp_vec.push_back("LFE");
            tmp_vec.push_back("Ls");
            tmp_vec.push_back("Lt");
            tmp_vec.push_back("Lt Rt");
            tmp_vec.push_back("M1 M2");
            tmp_vec.push_back("R");
            tmp_vec.push_back("R C LFE Ls Rs ?");
            tmp_vec.push_back("Rs");
            tmp_vec.push_back("Rt");
            tmp_vec.push_back("X C X X");
            tmp_vec.push_back("X C X X X X");
            tmp_vec.push_back("X C X X X X X X");
            tmp_vec.push_back("X L X R");
            tmp_vec.push_back("X L X R X");
            tmp_vec.push_back("X L X X R X");
            tmp_vec.push_back("X L X X X R X X");
            tmp_vec.push_back("X X C X");
            tmp_vec.push_back("X X C X X X");
            tmp_vec.push_back("X X C X X X X X");
            tmp_vec.push_back("X X L C X X R S");
            tmp_vec.push_back("X X L X X R");
            tmp_vec.push_back("X X L X X X R X");
            tmp_vec.push_back("X X X C");
            tmp_vec.push_back("X X X C X X");
            tmp_vec.push_back("X X X C X X X X");
            tmp_vec.push_back("X X X L X X X R");
            tmp_vec.push_back("X X X X C X");
            tmp_vec.push_back("X X X X C X X X");
            tmp_vec.push_back("X X X X X C");
            tmp_vec.push_back("X X X X X C X X");
            tmp_vec.push_back("X X X X X X C X");
            tmp_vec.push_back("X X X X X X X C");
            tmp["ChannelLayout"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Back: C");
            tmp_vec.push_back("Back: L");
            tmp_vec.push_back("Back: R");
            tmp_vec.push_back("Discrete-0");
            tmp_vec.push_back("Discrete-1");
            tmp_vec.push_back("Discrete-10");
            tmp_vec.push_back("Discrete-11");
            tmp_vec.push_back("Discrete-2");
            tmp_vec.push_back("Discrete-3");
            tmp_vec.push_back("Discrete-4");
            tmp_vec.push_back("Discrete-5");
            tmp_vec.push_back("Discrete-6");
            tmp_vec.push_back("Discrete-7");
            tmp_vec.push_back("Discrete-8");
            tmp_vec.push_back("Discrete-9");
            tmp_vec.push_back("Dual mono");
            tmp_vec.push_back("EBU R 123: 4b");
            tmp_vec.push_back("Front: C");
            tmp_vec.push_back("Front: C, Side: L R");
            tmp_vec.push_back("Front: C, Side: L R, LFE");
            tmp_vec.push_back("Front: L");
            tmp_vec.push_back("Front: L (Matrix)");
            tmp_vec.push_back("Front: L C C C R, Side: L R, LFE");
            tmp_vec.push_back("Front: L C R");
            tmp_vec.push_back("Front: L C R, Back: C");
            tmp_vec.push_back("Front: L C R, Back: C, LFE");
            tmp_vec.push_back("Front: L C R, Back: L R");
            tmp_vec.push_back("Front: L C R, Back: L R, LFE");
            tmp_vec.push_back("Front: L C R, LFE");
            tmp_vec.push_back("Front: L C R, Rear: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R");
            tmp_vec.push_back("Front: L C R, Side: L R, Back: C / Front: L C R, Side: L R");
            tmp_vec.push_back("Front: L C R, Side: L R, Back: C, High: C, LFE / Front: L C R, Side: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R, Back: C, LFE / Front: L C R, Side: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R, Back: C, TopCtrSrrd, LFE / Front: L C R, Side: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R, Back: L R");
            tmp_vec.push_back("Front: L C R, Side: L R, Back: L R / Front: L C R, Side: L R");
            tmp_vec.push_back("Front: L C R, Side: L R, Back: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R, Back: L R, LFE / Front: L C R, Side: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R, HiSide: L R, LFE / Front: L C R, Side: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R, High: L R, LFE / Front: L C R, Side: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R, LFE / Front: L C R, Side: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R, Rear: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R, Wide: L R, LFE / Front: L C R, Side: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R, c: L R, LFE");
            tmp_vec.push_back("Front: L C R, Side: L R, vh: L R, LFE");
            tmp_vec.push_back("Front: L C R, vh: L R, LFE");
            tmp_vec.push_back("Front: L R");
            tmp_vec.push_back("Front: L R (Matrix)");
            tmp_vec.push_back("Front: L R / Front: C / Front: C");
            tmp_vec.push_back("Front: L R / Front: L R / Front: L R");
            tmp_vec.push_back("Front: L R,   Back: C");
            tmp_vec.push_back("Front: L R,   Back: C, LFE");
            tmp_vec.push_back("Front: L R,   Side: L R");
            tmp_vec.push_back("Front: L R,   Side: L R, LFE");
            tmp_vec.push_back("Front: L R, Back: C");
            tmp_vec.push_back("Front: L R, Back: C, LFE");
            tmp_vec.push_back("Front: L R, LFE");
            tmp_vec.push_back("Front: L R, Side: L R");
            tmp_vec.push_back("Front: L R, Side: L R, LFE");
            tmp_vec.push_back("Front: L, R");
            tmp_vec.push_back("Front: Lt");
            tmp_vec.push_back("Front: Lt Rt");
            tmp_vec.push_back("Front: M1 M2");
            tmp_vec.push_back("Front: R");
            tmp_vec.push_back("Front: R (Matrix)");
            tmp_vec.push_back("Front: Rt");
            tmp_vec.push_back("LFE");
            tmp_vec.push_back("Side: L");
            tmp_vec.push_back("Side: R");
            tmp["ChannelPositions"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back(".mp3");
            tmp_vec.push_back("0");
            tmp_vec.push_back("0 / 0");
            tmp_vec.push_back("00001000-0000-0100-8000-00AA00389B71");
            tmp_vec.push_back("0D01030102010101");
            tmp_vec.push_back("0D01030102010101-040202017F000000");
            tmp_vec.push_back("0D01030102010201");
            tmp_vec.push_back("0D01030102010301");
            tmp_vec.push_back("0D01030102010501");
            tmp_vec.push_back("0D01030102010501-040202017F000000");
            tmp_vec.push_back("0D01030102010601");
            tmp_vec.push_back("0D01030102024101");
            tmp_vec.push_back("0D01030102060000-0402020100000000 / 0D01030102060200-0402020100000000");
            tmp_vec.push_back("0D01030102060100");
            tmp_vec.push_back("0D01030102060100 / 0D01030102060200");
            tmp_vec.push_back("0D01030102060100 / 1");
            tmp_vec.push_back("0D01030102060100-0402020100000000");
            tmp_vec.push_back("0D01030102060100-0402020100000000 / 0D01030102060200-0402020100000000");
            tmp_vec.push_back("0D01030102060100-0402020101000000");
            tmp_vec.push_back("0D01030102060100-0402020101000000 / 0D01030102060200-0402020101000000");
            tmp_vec.push_back("0D01030102060100-040202017F000000");
            tmp_vec.push_back("0D01030102060100-0402020203010100");
            tmp_vec.push_back("0D01030102060100-0402020203021C00");
            tmp_vec.push_back("0D01030102060200");
            tmp_vec.push_back("0D01030102060200-0000000000000000");
            tmp_vec.push_back("0D01030102060200-0402020100000000");
            tmp_vec.push_back("0D01030102060200-0402020101000000");
            tmp_vec.push_back("0D01030102060200-040202017F000000");
            tmp_vec.push_back("0D01030102060300");
            tmp_vec.push_back("0D01030102060300-0402020101000000");
            tmp_vec.push_back("0D01030102060300-040202017F000000");
            tmp_vec.push_back("0D01030102060300-0402020203020100");
            tmp_vec.push_back("0D01030102060300-0402020203021C00");
            tmp_vec.push_back("0D01030102060400");
            tmp_vec.push_back("0D01030102060B00");
            tmp_vec.push_back("0D010301020A0300-0402020203010100");
            tmp_vec.push_back("0D01130101040100-040202017E000000");
            tmp_vec.push_back("1");
            tmp_vec.push_back("10");
            tmp_vec.push_back("11");
            tmp_vec.push_back("128");
            tmp_vec.push_back("129");
            tmp_vec.push_back("130");
            tmp_vec.push_back("131");
            tmp_vec.push_back("133");
            tmp_vec.push_back("134");
            tmp_vec.push_back("135");
            tmp_vec.push_back("15");
            tmp_vec.push_back("16-bit signed linear");
            tmp_vec.push_back("161");
            tmp_vec.push_back("162");
            tmp_vec.push_back("163");
            tmp_vec.push_back("17");
            tmp_vec.push_back("18");
            tmp_vec.push_back("2");
            tmp_vec.push_back("2000");
            tmp_vec.push_back("2001");
            tmp_vec.push_back("22");
            tmp_vec.push_back("3");
            tmp_vec.push_back("3 / 3");
            tmp_vec.push_back("31");
            tmp_vec.push_back("4");
            tmp_vec.push_back("40");
            tmp_vec.push_back("4B464141000D4D4F");
            tmp_vec.push_back("50");
            tmp_vec.push_back("500");
            tmp_vec.push_back("501");
            tmp_vec.push_back("55");
            tmp_vec.push_back("566F");
            tmp_vec.push_back("6");
            tmp_vec.push_back("67");
            tmp_vec.push_back("674F");
            tmp_vec.push_back("6750");
            tmp_vec.push_back("6751");
            tmp_vec.push_back("6771");
            tmp_vec.push_back("69");
            tmp_vec.push_back("6B");
            tmp_vec.push_back("7");
            tmp_vec.push_back("75");
            tmp_vec.push_back("8-bit mu-law");
            tmp_vec.push_back("9");
            tmp_vec.push_back("A");
            tmp_vec.push_back("A9");
            tmp_vec.push_back("A_AAC");
            tmp_vec.push_back("A_AAC/MPEG2/LC/SBR");
            tmp_vec.push_back("A_AAC/MPEG4/LC");
            tmp_vec.push_back("A_AAC/MPEG4/LC/SBR");
            tmp_vec.push_back("A_AC3");
            tmp_vec.push_back("A_AC3/BSID10");
            tmp_vec.push_back("A_DTS");
            tmp_vec.push_back("A_EAC3");
            tmp_vec.push_back("A_FLAC");
            tmp_vec.push_back("A_MLP");
            tmp_vec.push_back("A_MPEG/L3");
            tmp_vec.push_back("A_MS/ACM / 161");
            tmp_vec.push_back("A_MS/ACM / 2000");
            tmp_vec.push_back("A_MS/ACM / FF");
            tmp_vec.push_back("A_OPUS");
            tmp_vec.push_back("A_PCM/INT/LIT");
            tmp_vec.push_back("A_REAL/COOK");
            tmp_vec.push_back("A_TRUEHD");
            tmp_vec.push_back("A_TTA1");
            tmp_vec.push_back("A_VORBIS");
            tmp_vec.push_back("A_WAVPACK4");
            tmp_vec.push_back("D3");
            tmp_vec.push_back("D4");
            tmp_vec.push_back("DE");
            tmp_vec.push_back("E1");
            tmp_vec.push_back("FF");
            tmp_vec.push_back("MAC6");
            tmp_vec.push_back("NONE");
            tmp_vec.push_back("QDM2");
            tmp_vec.push_back("QDMC");
            tmp_vec.push_back("Qclp");
            tmp_vec.push_back("WMA2");
            tmp_vec.push_back("aac ");
            tmp_vec.push_back("ac-3");
            tmp_vec.push_back("alac");
            tmp_vec.push_back("alaw");
            tmp_vec.push_back("audio/x-ralf-mpeg4-generic");
            tmp_vec.push_back("cook");
            tmp_vec.push_back("dnet");
            tmp_vec.push_back("dtsc");
            tmp_vec.push_back("dvca");
            tmp_vec.push_back("ec-3");
            tmp_vec.push_back("enca");
            tmp_vec.push_back("fl32");
            tmp_vec.push_back("ima4");
            tmp_vec.push_back("in24");
            tmp_vec.push_back("in24 / 1");
            tmp_vec.push_back("in32");
            tmp_vec.push_back("lpcm");
            tmp_vec.push_back("ni24 / 00001000-0000-0100-8000-00AA00389B71");
            tmp_vec.push_back("nmos");
            tmp_vec.push_back("owma / 161");
            tmp_vec.push_back("owma / 162");
            tmp_vec.push_back("raw ");
            tmp_vec.push_back("sac3");
            tmp_vec.push_back("samr");
            tmp_vec.push_back("sevc");
            tmp_vec.push_back("sipr");
            tmp_vec.push_back("sowt");
            tmp_vec.push_back("sowt / 00001000-0000-0100-8000-00AA00389B71");
            tmp_vec.push_back("sowt / 1");
            tmp_vec.push_back("sowt / 4B464141000D4D4F");
            tmp_vec.push_back("twos");
            tmp_vec.push_back("ulaw");
            tmp_vec.push_back("vdva");
            tmp["CodecID"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Lossless");
            tmp_vec.push_back("Lossless / Lossy");
            tmp_vec.push_back("Lossy");
            tmp["Compression_Mode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("No");
            tmp_vec.push_back("Yes");
            tmp["Default"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Cryptoworks");
            tmp_vec.push_back("Encrypted");
            tmp_vec.push_back("NDS Videoguard 1/2");
            tmp_vec.push_back("Nagravision");
            tmp_vec.push_back("PlayReady");
            tmp_vec.push_back("SDMI");
            tmp_vec.push_back("Seca Mediaguard 1/2");
            tmp_vec.push_back("TPS-Crypt  or Viaccess");
            tmp_vec.push_back("iTunes");
            tmp["Encryption"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("No");
            tmp_vec.push_back("Yes");
            tmp["Forced"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("(Encrypted)");
            tmp_vec.push_back("0");
            tmp_vec.push_back("A-law");
            tmp_vec.push_back("AAC");
            tmp_vec.push_back("AC-3");
            tmp_vec.push_back("ACELP");
            tmp_vec.push_back("ADPCM");
            tmp_vec.push_back("ALAC");
            tmp_vec.push_back("ALS");
            tmp_vec.push_back("AMR");
            tmp_vec.push_back("Atrac3");
            tmp_vec.push_back("CELT");
            tmp_vec.push_back("Cooker");
            tmp_vec.push_back("DTS");
            tmp_vec.push_back("Dolby E");
            tmp_vec.push_back("E-AC-3");
            tmp_vec.push_back("EVRC");
            tmp_vec.push_back("FLAC");
            tmp_vec.push_back("GSM 6.10");
            tmp_vec.push_back("LA");
            tmp_vec.push_back("MACE 6");
            tmp_vec.push_back("MIDI");
            tmp_vec.push_back("MLP");
            tmp_vec.push_back("MPEG Audio");
            tmp_vec.push_back("Monkey's Audio");
            tmp_vec.push_back("Musepack SV7");
            tmp_vec.push_back("Musepack SV8");
            tmp_vec.push_back("Nellymoser");
            tmp_vec.push_back("Opus");
            tmp_vec.push_back("PCM");
            tmp_vec.push_back("PCM ");
            tmp_vec.push_back("QCELP");
            tmp_vec.push_back("Qdesign 2");
            tmp_vec.push_back("RK Audio");
            tmp_vec.push_back("RT29");
            tmp_vec.push_back("RealAudio Lossless");
            tmp_vec.push_back("SLS");
            tmp_vec.push_back("Shorten");
            tmp_vec.push_back("Speex");
            tmp_vec.push_back("TAK");
            tmp_vec.push_back("TTA");
            tmp_vec.push_back("TrueHD");
            tmp_vec.push_back("TrueHD / AC-3");
            tmp_vec.push_back("Truespeech");
            tmp_vec.push_back("TwinVQ");
            tmp_vec.push_back("VP6");
            tmp_vec.push_back("VP7");
            tmp_vec.push_back("Vorbis");
            tmp_vec.push_back("WMA");
            tmp_vec.push_back("WavPack");
            tmp_vec.push_back("ZWF");
            tmp["Format"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("4.03");
            tmp_vec.push_back("4.04");
            tmp_vec.push_back("4.05");
            tmp_vec.push_back("4.06");
            tmp_vec.push_back("4.07");
            tmp_vec.push_back("4.16");
            tmp_vec.push_back("96/24 / Core");
            tmp_vec.push_back("Dolby Digital");
            tmp_vec.push_back("Dual Mono");
            tmp_vec.push_back("ES");
            tmp_vec.push_back("Express");
            tmp_vec.push_back("Float");
            tmp_vec.push_back("HE-AAC / LC");
            tmp_vec.push_back("HE-AACv2 / HE-AAC / LC");
            tmp_vec.push_back("HRA / Core");
            tmp_vec.push_back("LC");
            tmp_vec.push_back("Layer 1");
            tmp_vec.push_back("Layer 2");
            tmp_vec.push_back("Layer 3");
            tmp_vec.push_back("Lossless");
            tmp_vec.push_back("MA / Core");
            tmp_vec.push_back("Main");
            tmp_vec.push_back("Narrow band");
            tmp_vec.push_back("Pro");
            tmp_vec.push_back("U-Law");
            tmp_vec.push_back("Version 1");
            tmp_vec.push_back("Voice");
            tmp_vec.push_back("non-core");
            tmp["Format_Profile"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("50/15ms");
            tmp_vec.push_back("CCITT");
            tmp["Format_Settings_Emphasis"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Big");
            tmp_vec.push_back("Little");
            tmp["Format_Settings_Endianness"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("IMA");
            tmp_vec.push_back("ShockWave");
            tmp["Format_Settings_Firm"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0");
            tmp_vec.push_back("1");
            tmp["Format_Settings_Floor"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("14");
            tmp_vec.push_back("16");
            tmp_vec.push_back("20");
            tmp_vec.push_back("24");
            tmp_vec.push_back("32");
            tmp_vec.push_back("Dual Mono");
            tmp_vec.push_back("Dual mono");
            tmp_vec.push_back("Joint stereo");
            tmp["Format_Settings_Mode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("C (commentary)");
            tmp_vec.push_back("CM (complete main)");
            tmp_vec.push_back("D (dialogue)");
            tmp_vec.push_back("E (emergency)");
            tmp_vec.push_back("HI (hearing impaired)");
            tmp_vec.push_back("Intensity Stereo");
            tmp_vec.push_back("Intensity Stereo + MS Stereo");
            tmp_vec.push_back("ME (music and effects)");
            tmp_vec.push_back("MS Stereo");
            tmp_vec.push_back("VI (visually impaired)");
            tmp_vec.push_back("VO (voice over)");
            tmp["Format_Settings_ModeExtension"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("No (Explicit)");
            tmp_vec.push_back("Yes (Implicit)");
            tmp["Format_Settings_PS"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("No");
            tmp_vec.push_back("No (Explicit)");
            tmp_vec.push_back("Yes");
            tmp_vec.push_back("Yes (Implicit)");
            tmp["Format_Settings_SBR"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Signed");
            tmp_vec.push_back("Unsigned");
            tmp["Format_Settings_Sign"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("?");
            tmp_vec.push_back("Clip");
            tmp_vec.push_back("Clip (AES)");
            tmp_vec.push_back("Clip (BWF)");
            tmp_vec.push_back("Frame");
            tmp_vec.push_back("Frame (AES)");
            tmp_vec.push_back("Frame (BWF)");
            tmp_vec.push_back("Frame (D-10)");
            tmp["Format_Settings_Wrapping"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0.4");
            tmp_vec.push_back("Mode 1");
            tmp_vec.push_back("Mode 2");
            tmp_vec.push_back("Mode 3");
            tmp_vec.push_back("Mode 3+");
            tmp_vec.push_back("Version 1");
            tmp_vec.push_back("Version 2");
            tmp_vec.push_back("Version 2.5");
            tmp_vec.push_back("Version 4");
            tmp["Format_Version"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("25.000");
            tmp_vec.push_back("29.970");
            tmp_vec.push_back("30.000");
            tmp_vec.push_back("50.000");
            tmp_vec.push_back("59.940");
            tmp_vec.push_back("60.000");
            tmp["FrameRate"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1000");
            tmp_vec.push_back("1001");
            tmp["FrameRate_Den"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("29970");
            tmp_vec.push_back("30000");
            tmp_vec.push_back("59940");
            tmp_vec.push_back("60000");
            tmp["FrameRate_Num"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("17");
            tmp_vec.push_back("26");
            tmp_vec.push_back("ENG L (1-1)");
            tmp_vec.push_back("ENG R (1-2)");
            tmp_vec.push_back("Espanol");
            tmp_vec.push_back("Silence");
            tmp_vec.push_back("aaa");
            tmp_vec.push_back("af");
            tmp_vec.push_back("ar");
            tmp_vec.push_back("cs");
            tmp_vec.push_back("cy");
            tmp_vec.push_back("da");
            tmp_vec.push_back("de");
            tmp_vec.push_back("de-ch");
            tmp_vec.push_back("dos");
            tmp_vec.push_back("ehs");
            tmp_vec.push_back("en");
            tmp_vec.push_back("en-Sajid790");
            tmp_vec.push_back("en-US");
            tmp_vec.push_back("en-gb");
            tmp_vec.push_back("en-us");
            tmp_vec.push_back("enm");
            tmp_vec.push_back("es");
            tmp_vec.push_back("es-ES");
            tmp_vec.push_back("esl");
            tmp_vec.push_back("fi");
            tmp_vec.push_back("fr");
            tmp_vec.push_back("fr-FR");
            tmp_vec.push_back("fr-ch");
            tmp_vec.push_back("hi-Sajid790");
            tmp_vec.push_back("hr");
            tmp_vec.push_back("is");
            tmp_vec.push_back("it");
            tmp_vec.push_back("ja");
            tmp_vec.push_back("ko");
            tmp_vec.push_back("nar");
            tmp_vec.push_back("no");
            tmp_vec.push_back("or");
            tmp_vec.push_back("pt");
            tmp_vec.push_back("pt-br");
            tmp_vec.push_back("qaa");
            tmp_vec.push_back("r01");
            tmp_vec.push_back("r02");
            tmp_vec.push_back("r03");
            tmp_vec.push_back("r04");
            tmp_vec.push_back("r05");
            tmp_vec.push_back("r06");
            tmp_vec.push_back("r07");
            tmp_vec.push_back("r08");
            tmp_vec.push_back("r09");
            tmp_vec.push_back("r10");
            tmp_vec.push_back("r11");
            tmp_vec.push_back("r12");
            tmp_vec.push_back("r13");
            tmp_vec.push_back("r14");
            tmp_vec.push_back("rl1");
            tmp_vec.push_back("rl2");
            tmp_vec.push_back("rl3");
            tmp_vec.push_back("rl4");
            tmp_vec.push_back("rl5");
            tmp_vec.push_back("rl6");
            tmp_vec.push_back("ru");
            tmp_vec.push_back("sl");
            tmp_vec.push_back("sv");
            tmp_vec.push_back("uk");
            tmp_vec.push_back("zh");
            tmp_vec.push_back("zh-cn");
            tmp["Language"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Clean effects");
            tmp_vec.push_back("Director's Comments");
            tmp_vec.push_back("Director's comments");
            tmp_vec.push_back("Hearing impaired");
            tmp_vec.push_back("Visual impaired commentary");
            tmp["Language_More"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("6");
            tmp["Matrix_Channel(s)"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Front: L C R, Side: L R, LFE");
            tmp["Matrix_ChannelPositions"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("ADTS");
            tmp_vec.push_back("AES3");
            tmp_vec.push_back("AIFF");
            tmp_vec.push_back("After core data");
            tmp_vec.push_back("Blu-ray");
            tmp_vec.push_back("DCP CPL / MXF");
            tmp_vec.push_back("DV");
            tmp_vec.push_back("DVD-Video");
            tmp_vec.push_back("HLS / ADTS");
            tmp_vec.push_back("HLS / MPEG-TS");
            tmp_vec.push_back("HLS / MPEG-TS / ADTS");
            tmp_vec.push_back("Header stripping");
            tmp_vec.push_back("IMF CPL");
            tmp_vec.push_back("IMF CPL / MXF");
            tmp_vec.push_back("LATM");
            tmp_vec.push_back("MPEG-4");
            tmp_vec.push_back("MPEG-TS");
            tmp_vec.push_back("MPEG-TS / ADTS");
            tmp_vec.push_back("MXF");
            tmp_vec.push_back("MXF / AES3");
            tmp_vec.push_back("MXF / DV");
            tmp_vec.push_back("PS2");
            tmp_vec.push_back("SL");
            tmp_vec.push_back("Stream extension");
            tmp_vec.push_back("Wave");
            tmp_vec.push_back("pre-FLAC 1.1.1");
            tmp["MuxingMode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("3");
            tmp_vec.push_back("7978");
            tmp_vec.push_back("8000");
            tmp_vec.push_back("10000");
            tmp_vec.push_back("11024");
            tmp_vec.push_back("11025");
            tmp_vec.push_back("11127");
            tmp_vec.push_back("12000");
            tmp_vec.push_back("16000");
            tmp_vec.push_back("22000");
            tmp_vec.push_back("22001");
            tmp_vec.push_back("22050");
            tmp_vec.push_back("32000 / 16000");
            tmp_vec.push_back("22050 / 22050 / 11025");
            tmp_vec.push_back("22050.000");
            tmp_vec.push_back("22254");
            tmp_vec.push_back("22500");
            tmp_vec.push_back("24000");
            tmp_vec.push_back("32000");
            tmp_vec.push_back("44100 / 22050");
            tmp_vec.push_back("48000 / 24000");
            tmp_vec.push_back("32000 / 32000 / 16000");
            tmp_vec.push_back("44000");
            tmp_vec.push_back("44100");
            tmp_vec.push_back("44100 / 44100");
            tmp_vec.push_back("44100 / 44100 / 22050");
            tmp_vec.push_back("44100.000");
            tmp_vec.push_back("48000");
            tmp_vec.push_back("48000 / 48000");
            tmp_vec.push_back("48000 / 48000 / 24000");
            tmp_vec.push_back("48000.000");
            tmp_vec.push_back("88200");
            tmp_vec.push_back("96000");
            tmp_vec.push_back("96000 / 48000");
            tmp_vec.push_back("176400");
            tmp_vec.push_back("192000");
            tmp_vec.push_back("352800");
            tmp["SamplingRate"] = tmp_vec;
        }
        values["Audio"] = tmp;
    }
    {
        std::map<std::string, std::vector<std::string> > tmp;
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("3dv ");
            tmp_vec.push_back("3g2a");
            tmp_vec.push_back("3gp4");
            tmp_vec.push_back("3gp5");
            tmp_vec.push_back("3gp6");
            tmp_vec.push_back("3gs6");
            tmp_vec.push_back("JUNK");
            tmp_vec.push_back("M4A");
            tmp_vec.push_back("M4A ");
            tmp_vec.push_back("M4V ");
            tmp_vec.push_back("M4VH");
            tmp_vec.push_back("M4VP");
            tmp_vec.push_back("MSNV");
            tmp_vec.push_back("avc1");
            tmp_vec.push_back("ccff");
            tmp_vec.push_back("dby1");
            tmp_vec.push_back("f4v ");
            tmp_vec.push_back("isml");
            tmp_vec.push_back("iso4");
            tmp_vec.push_back("isom");
            tmp_vec.push_back("jp2 ");
            tmp_vec.push_back("mp41");
            tmp_vec.push_back("mp42");
            tmp_vec.push_back("ndxh");
            tmp_vec.push_back("piff");
            tmp_vec.push_back("qt  ");
            tmp["CodecID"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("3dv ");
            tmp_vec.push_back("3gs6");
            tmp_vec.push_back("7-Zip");
            tmp_vec.push_back("AAF");
            tmp_vec.push_back("AC-3");
            tmp_vec.push_back("ADIF");
            tmp_vec.push_back("ADTS");
            tmp_vec.push_back("AES3");
            tmp_vec.push_back("AIFF");
            tmp_vec.push_back("ALS");
            tmp_vec.push_back("AMR");
            tmp_vec.push_back("AMV");
            tmp_vec.push_back("AQTitle");
            tmp_vec.push_back("ASS");
            tmp_vec.push_back("AU");
            tmp_vec.push_back("AVC");
            tmp_vec.push_back("AVI");
            tmp_vec.push_back("AVS Video");
            tmp_vec.push_back("Adobe encore DVD");
            tmp_vec.push_back("Arri Raw");
            tmp_vec.push_back("BDAV");
            tmp_vec.push_back("BPG");
            tmp_vec.push_back("BZip2");
            tmp_vec.push_back("Bitmap");
            tmp_vec.push_back("Blender");
            tmp_vec.push_back("Blu-ray Clip info");
            tmp_vec.push_back("Blu-ray Index");
            tmp_vec.push_back("Blu-ray Movie object");
            tmp_vec.push_back("Blu-ray Playlist");
            tmp_vec.push_back("Blu-ray movie");
            tmp_vec.push_back("CAF");
            tmp_vec.push_back("CDDA");
            tmp_vec.push_back("CDXA/MPEG-PS");
            tmp_vec.push_back("CMP4");
            tmp_vec.push_back("CPC Captioning");
            tmp_vec.push_back("Captions Inc");
            tmp_vec.push_back("Cheeta");
            tmp_vec.push_back("DASH MPD");
            tmp_vec.push_back("DCP AM");
            tmp_vec.push_back("DCP CPL");
            tmp_vec.push_back("DCP PKL");
            tmp_vec.push_back("DDS");
            tmp_vec.push_back("DPG");
            tmp_vec.push_back("DPX");
            tmp_vec.push_back("DTS");
            tmp_vec.push_back("DV");
            tmp_vec.push_back("DVD Video");
            tmp_vec.push_back("DXW");
            tmp_vec.push_back("Dirac");
            tmp_vec.push_back("DivX");
            tmp_vec.push_back("E-AC-3");
            tmp_vec.push_back("ELF");
            tmp_vec.push_back("EXR");
            tmp_vec.push_back("Extended Module");
            tmp_vec.push_back("FLAC");
            tmp_vec.push_back("FLIC");
            tmp_vec.push_back("Flash Video");
            tmp_vec.push_back("GIF");
            tmp_vec.push_back("GXF");
            tmp_vec.push_back("GZip");
            tmp_vec.push_back("Google Video");
            tmp_vec.push_back("HDS F4M");
            tmp_vec.push_back("HEVC");
            tmp_vec.push_back("HLS");
            tmp_vec.push_back("ICO");
            tmp_vec.push_back("IMF AM");
            tmp_vec.push_back("IMF CPL");
            tmp_vec.push_back("IMF PKL");
            tmp_vec.push_back("ISM");
            tmp_vec.push_back("IVF");
            tmp_vec.push_back("Impulse Tracker");
            tmp_vec.push_back("JNG");
            tmp_vec.push_back("JPEG");
            tmp_vec.push_back("JPEG 2000");
            tmp_vec.push_back("JUNK");
            tmp_vec.push_back("LA");
            tmp_vec.push_back("LATM");
            tmp_vec.push_back("LXF");
            tmp_vec.push_back("M4A");
            tmp_vec.push_back("MIDI");
            tmp_vec.push_back("MLP");
            tmp_vec.push_back("MPEG Audio");
            tmp_vec.push_back("MPEG Video");
            tmp_vec.push_back("MPEG-4");
            tmp_vec.push_back("MPEG-4 Visual");
            tmp_vec.push_back("MPEG-PS");
            tmp_vec.push_back("MPEG-TS");
            tmp_vec.push_back("MPEG-TS 188+16");
            tmp_vec.push_back("MTV");
            tmp_vec.push_back("MXF");
            tmp_vec.push_back("MZ");
            tmp_vec.push_back("Matroska");
            tmp_vec.push_back("Module");
            tmp_vec.push_back("Monkey's Audio");
            tmp_vec.push_back("Musepack SV7");
            tmp_vec.push_back("Musepack SV8");
            tmp_vec.push_back("N19");
            tmp_vec.push_back("OGG");
            tmp_vec.push_back("OpenMG");
            tmp_vec.push_back("P2 Clip");
            tmp_vec.push_back("PCX");
            tmp_vec.push_back("PDF");
            tmp_vec.push_back("PNG");
            tmp_vec.push_back("PSD");
            tmp_vec.push_back("Pro Tools Session");
            tmp_vec.push_back("QLCM");
            tmp_vec.push_back("QuickTime");
            tmp_vec.push_back("R3D");
            tmp_vec.push_back("RK Audio");
            tmp_vec.push_back("RMP3");
            tmp_vec.push_back("RealMedia");
            tmp_vec.push_back("SAMI");
            tmp_vec.push_back("SCC");
            tmp_vec.push_back("SKM");
            tmp_vec.push_back("SSA");
            tmp_vec.push_back("Scream Tracker 3");
            tmp_vec.push_back("SequenceInfo");
            tmp_vec.push_back("ShockWave");
            tmp_vec.push_back("Shorten");
            tmp_vec.push_back("SubRip");
            tmp_vec.push_back("TAK");
            tmp_vec.push_back("TGA");
            tmp_vec.push_back("TIFF");
            tmp_vec.push_back("TTA");
            tmp_vec.push_back("TTML");
            tmp_vec.push_back("TrueHD");
            tmp_vec.push_back("TrueHD / AC-3");
            tmp_vec.push_back("TwinVQ");
            tmp_vec.push_back("VC-1");
            tmp_vec.push_back("VC-3");
            tmp_vec.push_back("WavPack");
            tmp_vec.push_back("Wave");
            tmp_vec.push_back("Wave64");
            tmp_vec.push_back("WebM");
            tmp_vec.push_back("Windows Media");
            tmp_vec.push_back("XDCAM Clip");
            tmp_vec.push_back("YUV4MPEG2");
            tmp_vec.push_back("ZIP");
            tmp_vec.push_back("ZWF");
            tmp_vec.push_back("ccff");
            tmp_vec.push_back("dby1");
            tmp["Format"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("AVC-Intra 100");
            tmp_vec.push_back("DNxHD 116");
            tmp_vec.push_back("DNxHD 120");
            tmp_vec.push_back("DNxHD 145");
            tmp_vec.push_back("DNxHD 175");
            tmp_vec.push_back("DNxHD 175x");
            tmp_vec.push_back("DNxHD 185");
            tmp_vec.push_back("DNxHD 185x");
            tmp_vec.push_back("DNxHD 220x");
            tmp_vec.push_back("DNxHD 90x");
            tmp_vec.push_back("DV");
            tmp_vec.push_back("DVCPRO");
            tmp_vec.push_back("DVCPRO 50");
            tmp_vec.push_back("DVCPRO HD");
            tmp_vec.push_back("HDV 1080i");
            tmp_vec.push_back("HDV 1080p");
            tmp_vec.push_back("HDV 720p");
            tmp_vec.push_back("IMX 30");
            tmp_vec.push_back("IMX 40");
            tmp_vec.push_back("IMX 50");
            tmp_vec.push_back("XDCAM EX 18");
            tmp_vec.push_back("XDCAM EX 35");
            tmp_vec.push_back("XDCAM HD 18");
            tmp_vec.push_back("XDCAM HD 25");
            tmp_vec.push_back("XDCAM HD 35");
            tmp_vec.push_back("XDCAM HD422");
            tmp["Format_Commercial_IfAny"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("3GPP Media Release 4");
            tmp_vec.push_back("3GPP Media Release 5");
            tmp_vec.push_back("3GPP Media Release 6 Basic");
            tmp_vec.push_back("3GPP2 Media");
            tmp_vec.push_back("96/24 / Core");
            tmp_vec.push_back("A-1b");
            tmp_vec.push_back("Adobe Flash");
            tmp_vec.push_back("Apple TV");
            tmp_vec.push_back("Apple audio with iTunes info");
            tmp_vec.push_back("Apple iPhone");
            tmp_vec.push_back("Base Media");
            tmp_vec.push_back("Base Media / Version 1");
            tmp_vec.push_back("Base Media / Version 2");
            tmp_vec.push_back("DLL / Intel i386");
            tmp_vec.push_back("ES");
            tmp_vec.push_back("Executable / Intel i386");
            tmp_vec.push_back("Express");
            tmp_vec.push_back("HRA / Core");
            tmp_vec.push_back("ISML");
            tmp_vec.push_back("JVT");
            tmp_vec.push_back("MA / Core");
            tmp_vec.push_back("MPEG-4");
            tmp_vec.push_back("Master");
            tmp_vec.push_back("Media");
            tmp_vec.push_back("Menu");
            tmp_vec.push_back("Nero Digital AVC HDTV Profile");
            tmp_vec.push_back("No PAT/PMT");
            tmp_vec.push_back("OP-1a");
            tmp_vec.push_back("OP-1b");
            tmp_vec.push_back("OP-2b");
            tmp_vec.push_back("OP-Atom");
            tmp_vec.push_back("OpenDML");
            tmp_vec.push_back("PIFF");
            tmp_vec.push_back("Program");
            tmp_vec.push_back("QuickTime");
            tmp_vec.push_back("RF64");
            tmp_vec.push_back("SMV v1");
            tmp_vec.push_back("SMV v2");
            tmp_vec.push_back("Sony PSP");
            tmp_vec.push_back("deflate");
            tmp["Format_Profile"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Closed / Complete");
            tmp_vec.push_back("Closed / Incomplete");
            tmp_vec.push_back("Compressed header");
            tmp_vec.push_back("Open / Complete");
            tmp_vec.push_back("Open / Incomplete");
            tmp_vec.push_back("rec");
            tmp["Format_Settings"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0.4");
            tmp_vec.push_back("1.04");
            tmp_vec.push_back("1.2");
            tmp_vec.push_back("1.2 ");
            tmp_vec.push_back("1.3");
            tmp_vec.push_back("1.4");
            tmp_vec.push_back("1.5");
            tmp_vec.push_back("1.6");
            tmp_vec.push_back("1.7");
            tmp_vec.push_back("2");
            tmp_vec.push_back("Interop");
            tmp_vec.push_back("SMPTE");
            tmp_vec.push_back("Version 0");
            tmp_vec.push_back("Version 1");
            tmp_vec.push_back("Version 1 / Version 2");
            tmp_vec.push_back("Version 10");
            tmp_vec.push_back("Version 2");
            tmp_vec.push_back("Version 2.00");
            tmp_vec.push_back("Version 3 / Version 2");
            tmp_vec.push_back("Version 4 / Version 2");
            tmp["Format_Version"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("25.000");
            tmp_vec.push_back("29.970");
            tmp_vec.push_back("30.000");
            tmp_vec.push_back("50.000");
            tmp_vec.push_back("59.940");
            tmp_vec.push_back("60.000");
            tmp["FrameRate"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1000");
            tmp_vec.push_back("1001");
            tmp["FrameRate_Den"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("29970");
            tmp_vec.push_back("30000");
            tmp_vec.push_back("59940");
            tmp_vec.push_back("60000");
            tmp["FrameRate_Num"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("No");
            tmp_vec.push_back("Yes");
            tmp["Interleaved"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("No");
            tmp_vec.push_back("Yes");
            tmp["IsStreamable"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("CBR");
            tmp_vec.push_back("VBR");
            tmp["OverallBitRate_Mode"] = tmp_vec;
        }
        values["General"] = tmp;
    }
    {
        std::map<std::string, std::vector<std::string> > tmp;
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1");
            tmp_vec.push_back("2");
            tmp_vec.push_back("4");
            tmp_vec.push_back("6");
            tmp_vec.push_back("8");
            tmp_vec.push_back("10");
            tmp_vec.push_back("12");
            tmp_vec.push_back("14");
            tmp_vec.push_back("16");
            tmp_vec.push_back("24");
            tmp_vec.push_back("32");
            tmp_vec.push_back("64");
            tmp["BitDepth"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("4:2:0");
            tmp_vec.push_back("4:2:2");
            tmp_vec.push_back("4:4:4");
            tmp_vec.push_back("4:4:4:4");
            tmp["ChromaSubsampling"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("9");
            tmp_vec.push_back("DXT1");
            tmp["CodecID"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("CMYK");
            tmp_vec.push_back("RGB");
            tmp_vec.push_back("RGBA");
            tmp_vec.push_back("Y");
            tmp_vec.push_back("YCCB");
            tmp_vec.push_back("YUV");
            tmp["ColorSpace"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Lossless");
            tmp_vec.push_back("Lossy");
            tmp["Compression_Mode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1.333");
            tmp_vec.push_back("1.778");
            tmp["DisplayAspectRatio"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("BPG");
            tmp_vec.push_back("Bit field");
            tmp_vec.push_back("CCITT T.4");
            tmp_vec.push_back("Color-mapped + RLE");
            tmp_vec.push_back("DPX");
            tmp_vec.push_back("DirectX TC");
            tmp_vec.push_back("EXR");
            tmp_vec.push_back("GIF");
            tmp_vec.push_back("JNG");
            tmp_vec.push_back("JPEG");
            tmp_vec.push_back("JPEG 2000");
            tmp_vec.push_back("LZW");
            tmp_vec.push_back("PCX");
            tmp_vec.push_back("PNG");
            tmp_vec.push_back("PSD");
            tmp_vec.push_back("PackBits");
            tmp_vec.push_back("RGB");
            tmp_vec.push_back("Raw");
            tmp["Format"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("B44");
            tmp_vec.push_back("B44A");
            tmp_vec.push_back("LZ77");
            tmp_vec.push_back("PIZ");
            tmp_vec.push_back("PXR24");
            tmp_vec.push_back("ZIP");
            tmp["Format_Compression"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("87a");
            tmp_vec.push_back("89a");
            tmp_vec.push_back("Line");
            tmp_vec.push_back("No restrictions");
            tmp_vec.push_back("Profile-0");
            tmp_vec.push_back("Tile");
            tmp["Format_Profile"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1");
            tmp_vec.push_back("Paintbrush v3.0+");
            tmp_vec.push_back("Version 2");
            tmp["Format_Version"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("480");
            tmp_vec.push_back("576");
            tmp_vec.push_back("720");
            tmp_vec.push_back("1080");
            tmp_vec.push_back("2160");
            tmp_vec.push_back("4320");
            tmp["Height"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("16");
            tmp_vec.push_back("32");
            tmp["Height_Offset"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("480");
            tmp_vec.push_back("576");
            tmp_vec.push_back("720");
            tmp_vec.push_back("1080");
            tmp_vec.push_back("2160");
            tmp_vec.push_back("4320");
            tmp["Height_Original"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1.000");
            tmp_vec.push_back("1.067");
            tmp_vec.push_back("1.126");
            tmp_vec.push_back("1.422");
            tmp_vec.push_back("1.500");
            tmp_vec.push_back("1.501");
            tmp["PixelAspectRatio"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("720");
            tmp_vec.push_back("1080");
            tmp_vec.push_back("1920");
            tmp_vec.push_back("3840");
            tmp_vec.push_back("7680");
            tmp["Width"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("720");
            tmp_vec.push_back("1080");
            tmp_vec.push_back("1920");
            tmp_vec.push_back("3840");
            tmp_vec.push_back("7680");
            tmp["Width_Original"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Yes");
            tmp["colour_description_present"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("BT.601");
            tmp_vec.push_back("Linear");
            tmp_vec.push_back("Printing density");
            tmp["colour_primaries"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("BT.601 PAL");
            tmp_vec.push_back("Printing density");
            tmp["transfer_characteristics"] = tmp_vec;
        }
        values["Image"] = tmp;
    }
    {
        std::map<std::string, std::vector<std::string> > tmp;
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("14+");
            tmp_vec.push_back("C8+");
            tmp_vec.push_back("E");
            tmp_vec.push_back("G");
            tmp_vec.push_back("N/A");
            tmp_vec.push_back("None");
            tmp_vec.push_back("PG");
            tmp_vec.push_back("R");
            tmp_vec.push_back("TV-14");
            tmp_vec.push_back("TV-14 (DL)");
            tmp_vec.push_back("TV-14 (DLS)");
            tmp_vec.push_back("TV-14 (L)");
            tmp_vec.push_back("TV-14 (S)");
            tmp_vec.push_back("TV-14 (SV)");
            tmp_vec.push_back("TV-14 (V)");
            tmp_vec.push_back("TV-G");
            tmp_vec.push_back("TV-PG");
            tmp_vec.push_back("TV-PG (D)");
            tmp_vec.push_back("TV-PG (DL)");
            tmp_vec.push_back("TV-PG (DS)");
            tmp_vec.push_back("TV-Y");
            tmp["LawRating"] = tmp_vec;
        }
        values["Menu"] = tmp;
    }
    {
        std::map<std::string, std::vector<std::string> > tmp;
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0");
            tmp_vec.push_back("0x00000000");
            tmp_vec.push_back("0x00000000 / 0x00000000 / 0x00000000 / 0x00000000 / 0x00000000 / 0x00000000 / 0x00000000 / 0x00000000 / 0x00000000");
            tmp_vec.push_back("1");
            tmp_vec.push_back("2");
            tmp_vec.push_back("21");
            tmp_vec.push_back("24");
            tmp_vec.push_back("7");
            tmp_vec.push_back("8");
            tmp_vec.push_back("amf0");
            tmp_vec.push_back("rtp ");
            tmp["CodecID"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0D-0C");
            tmp_vec.push_back("0x00000000");
            tmp_vec.push_back("6D-10");
            tmp_vec.push_back("80-01");
            tmp_vec.push_back("AS-11 Core");
            tmp_vec.push_back("AS-11 Segmentation");
            tmp_vec.push_back("AS-11 UKDPP");
            tmp_vec.push_back("Acquisition Metadata");
            tmp_vec.push_back("Audio Metadata");
            tmp_vec.push_back("CPL TC");
            tmp_vec.push_back("MXF TC");
            tmp_vec.push_back("QuickTime TC");
            tmp_vec.push_back("RTP");
            tmp_vec.push_back("SMPTE ATC");
            tmp_vec.push_back("SMPTE TC");
            tmp_vec.push_back("System");
            tmp_vec.push_back("System Core");
            tmp_vec.push_back("UMID");
            tmp_vec.push_back("WM TC");
            tmp_vec.push_back("WSS");
            tmp_vec.push_back("amf0");
            tmp_vec.push_back("mp4s");
            tmp["Format"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("25.000");
            tmp_vec.push_back("29.970");
            tmp_vec.push_back("30.000");
            tmp_vec.push_back("50.000");
            tmp_vec.push_back("59.940");
            tmp_vec.push_back("60.000");
            tmp["FrameRate"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1000");
            tmp_vec.push_back("1001");
            tmp["FrameRate_Den"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("29970");
            tmp_vec.push_back("30000");
            tmp_vec.push_back("59940");
            tmp_vec.push_back("60000");
            tmp["FrameRate_Num"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("en");
            tmp_vec.push_back("fr");
            tmp["Language"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Ancillary data / RDD 18");
            tmp_vec.push_back("Ancillary data / RDD 8");
            tmp_vec.push_back("Ancillary data / SMPTE RP 188");
            tmp_vec.push_back("Ancillary data / SMPTE RP 223");
            tmp_vec.push_back("Ancillary data / SMPTE ST 2020");
            tmp_vec.push_back("IMF CPL");
            tmp_vec.push_back("MXF");
            tmp_vec.push_back("SDTI");
            tmp_vec.push_back("System scheme 1");
            tmp["MuxingMode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("00:00:00:000");
            tmp_vec.push_back("00:00:00;000");
            tmp_vec.push_back("01:00:00:000");
            tmp_vec.push_back("01:00:00;000");
            tmp["TimeCode_FirstFrame"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("ATC_LTC");
            tmp_vec.push_back("ATC_VITC1");
            tmp_vec.push_back("ATC_VITC2");
            tmp_vec.push_back("Material Package");
            tmp_vec.push_back("Source Package");
            tmp["TimeCode_Settings"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Yes");
            tmp["TimeCode_Striped"] = tmp_vec;
        }
        values["Other"] = tmp;
    }
    {
        std::map<std::string, std::vector<std::string> > tmp;
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("2");
            tmp_vec.push_back("4");
            tmp["BitDepth"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("CBR");
            tmp_vec.push_back("VBR");
            tmp["BitRate_Mode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0x00000000");
            tmp_vec.push_back("144");
            tmp_vec.push_back("6");
            tmp_vec.push_back("DXSB");
            tmp_vec.push_back("E0");
            tmp_vec.push_back("S_ASS");
            tmp_vec.push_back("S_HDMV/PGS");
            tmp_vec.push_back("S_SSA");
            tmp_vec.push_back("S_TEXT/ASS");
            tmp_vec.push_back("S_TEXT/SSA");
            tmp_vec.push_back("S_TEXT/UTF8");
            tmp_vec.push_back("S_VOBSUB");
            tmp_vec.push_back("c608");
            tmp_vec.push_back("c708");
            tmp_vec.push_back("dfxp");
            tmp_vec.push_back("stpp");
            tmp_vec.push_back("text");
            tmp_vec.push_back("text / text / text / text");
            tmp_vec.push_back("tx3g");
            tmp["CodecID"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Lossless");
            tmp["Compression_Mode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("No");
            tmp_vec.push_back("Yes");
            tmp["Default"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1.333");
            tmp_vec.push_back("1.778");
            tmp["DisplayAspectRatio"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Encrypted");
            tmp["Encryption"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("No");
            tmp_vec.push_back("Yes");
            tmp["Forced"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0x00000000");
            tmp_vec.push_back("AQTitle");
            tmp_vec.push_back("ARIB STD B24/B37");
            tmp_vec.push_back("ASS");
            tmp_vec.push_back("Adobe encore DVD");
            tmp_vec.push_back("CEA-608");
            tmp_vec.push_back("CMML");
            tmp_vec.push_back("CPC Captioning");
            tmp_vec.push_back("Captions Inc");
            tmp_vec.push_back("Cheeta");
            tmp_vec.push_back("DVB Subtitle");
            tmp_vec.push_back("DivX Subtitle");
            tmp_vec.push_back("EIA-608");
            tmp_vec.push_back("EIA-708");
            tmp_vec.push_back("Kate");
            tmp_vec.push_back("N19");
            tmp_vec.push_back("PDF");
            tmp_vec.push_back("PGS");
            tmp_vec.push_back("RLE");
            tmp_vec.push_back("SAMI");
            tmp_vec.push_back("SSA");
            tmp_vec.push_back("SalmonSoft");
            tmp_vec.push_back("SubRip");
            tmp_vec.push_back("Subrip");
            tmp_vec.push_back("TTML");
            tmp_vec.push_back("Teletext");
            tmp_vec.push_back("Teletext Subtitle");
            tmp_vec.push_back("Timed Text");
            tmp_vec.push_back("UTF-8");
            tmp_vec.push_back("VobSub");
            tmp_vec.push_back("mp4s");
            tmp["Format"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("HD side panel");
            tmp_vec.push_back("Mobile closed caption");
            tmp["Format_Profile"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("25.000");
            tmp_vec.push_back("29.970");
            tmp_vec.push_back("30.000");
            tmp_vec.push_back("50.000");
            tmp_vec.push_back("59.940");
            tmp_vec.push_back("60.000");
            tmp["FrameRate"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1000");
            tmp_vec.push_back("1001");
            tmp["FrameRate_Den"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("29970");
            tmp_vec.push_back("30000");
            tmp_vec.push_back("59940");
            tmp_vec.push_back("60000");
            tmp["FrameRate_Num"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("480");
            tmp_vec.push_back("576");
            tmp_vec.push_back("720");
            tmp_vec.push_back("1080");
            tmp_vec.push_back("2160");
            tmp_vec.push_back("4320");
            tmp["Height"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("ar");
            tmp_vec.push_back("ca");
            tmp_vec.push_back("cs");
            tmp_vec.push_back("da");
            tmp_vec.push_back("de");
            tmp_vec.push_back("el");
            tmp_vec.push_back("en");
            tmp_vec.push_back("en-Sajid790");
            tmp_vec.push_back("es");
            tmp_vec.push_back("fi");
            tmp_vec.push_back("fr");
            tmp_vec.push_back("he");
            tmp_vec.push_back("hu");
            tmp_vec.push_back("is");
            tmp_vec.push_back("it");
            tmp_vec.push_back("ja");
            tmp_vec.push_back("ko");
            tmp_vec.push_back("mul");
            tmp_vec.push_back("nl");
            tmp_vec.push_back("no");
            tmp_vec.push_back("pl");
            tmp_vec.push_back("pt");
            tmp_vec.push_back("ru");
            tmp_vec.push_back("sl");
            tmp_vec.push_back("spn");
            tmp_vec.push_back("sv");
            tmp_vec.push_back("tr");
            tmp_vec.push_back("uk");
            tmp_vec.push_back("zh");
            tmp["Language"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Director comments");
            tmp_vec.push_back("For hearing impaired people");
            tmp_vec.push_back("Large");
            tmp_vec.push_back("Normal");
            tmp_vec.push_back("Subtitles");
            tmp_vec.push_back("Transcript");
            tmp["Language_More"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("A/53 / DTVCC Transport");
            tmp_vec.push_back("Ancillary data / CCIS");
            tmp_vec.push_back("Ancillary data / CDP");
            tmp_vec.push_back("Ancillary data / OP-47 / SDP");
            tmp_vec.push_back("Ancillary data / SMPTE 334");
            tmp_vec.push_back("CCIS");
            tmp_vec.push_back("DV");
            tmp_vec.push_back("DVD-Video");
            tmp_vec.push_back("Final Cut");
            tmp_vec.push_back("Final Cut / CDP");
            tmp_vec.push_back("HLS / MPEG-TS / SCTE 128 / DTVCC Transport");
            tmp_vec.push_back("MPEG-4");
            tmp_vec.push_back("MPEG-4 / SCTE 128 / DTVCC Transport");
            tmp_vec.push_back("MPEG-TS / SCTE 128 / DTVCC Transport");
            tmp_vec.push_back("MXF");
            tmp_vec.push_back("MXF / A/53 / DTVCC Transport");
            tmp_vec.push_back("SCTE 128 / DTVCC Transport");
            tmp_vec.push_back("SCTE 20");
            tmp_vec.push_back("sbtl");
            tmp_vec.push_back("zlib");
            tmp["MuxingMode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1.000");
            tmp_vec.push_back("1.067");
            tmp_vec.push_back("1.126");
            tmp_vec.push_back("1.422");
            tmp_vec.push_back("1.500");
            tmp_vec.push_back("1.501");
            tmp["PixelAspectRatio"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("720");
            tmp_vec.push_back("1080");
            tmp_vec.push_back("1920");
            tmp_vec.push_back("3840");
            tmp_vec.push_back("7680");
            tmp["Width"] = tmp_vec;
        }
        values["Text"] = tmp;
    }
    {
        std::map<std::string, std::vector<std::string> > tmp;
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0");
            tmp_vec.push_back("5");
            tmp_vec.push_back("8");
            tmp_vec.push_back("9");
            tmp_vec.push_back("10");
            tmp_vec.push_back("14");
            tmp_vec.push_back("15");
            tmp["ActiveFormatDescription"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1");
            tmp_vec.push_back("2");
            tmp_vec.push_back("3");
            tmp_vec.push_back("4");
            tmp_vec.push_back("5");
            tmp_vec.push_back("6");
            tmp_vec.push_back("7");
            tmp_vec.push_back("8");
            tmp_vec.push_back("8 / 8");
            tmp_vec.push_back("10");
            tmp_vec.push_back("12");
            tmp_vec.push_back("14");
            tmp_vec.push_back("15");
            tmp_vec.push_back("16");
            tmp_vec.push_back("24");
            tmp["BitDepth"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("CBR");
            tmp_vec.push_back("VBR");
            tmp["BitRate_Mode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("4:1:0 (4x4)");
            tmp_vec.push_back("4:1:1");
            tmp_vec.push_back("4:2:0");
            tmp_vec.push_back("4:2:0 / 4:2:0");
            tmp_vec.push_back("4:2:2");
            tmp_vec.push_back("4:4:4");
            tmp["ChromaSubsampling"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0D01030102010101");
            tmp_vec.push_back("0D01030102010101-0401020201020101");
            tmp_vec.push_back("0D01030102010201-0401020201020102");
            tmp_vec.push_back("0D01030102010301-0401020201020103");
            tmp_vec.push_back("0D01030102010501-0401020201020105");
            tmp_vec.push_back("0D01030102010601-0401020201020106");
            tmp_vec.push_back("0D01030102020201-0401020202010200");
            tmp_vec.push_back("0D01030102024002-0401020202020100");
            tmp_vec.push_back("0D01030102024101-0401020202020200");
            tmp_vec.push_back("0D01030102024102-0401020202020200");
            tmp_vec.push_back("0D01030102025001-0401020202020300");
            tmp_vec.push_back("0D01030102025002-0401020202020300");
            tmp_vec.push_back("0D01030102025101-0401020202020400");
            tmp_vec.push_back("0D01030102025102-0401020202020400");
            tmp_vec.push_back("0D01030102026001-0401020202020500");
            tmp_vec.push_back("0D01030102026002-0401020202020500");
            tmp_vec.push_back("0D01030102026102-0401020202020600");
            tmp_vec.push_back("0D01030102026202-0401020202020700");
            tmp_vec.push_back("0D01030102027F02");
            tmp_vec.push_back("0D01030102040000-0401020201000000");
            tmp_vec.push_back("0D01030102040000-0401020201000000 / 0D01030102046001-0401020201020200");
            tmp_vec.push_back("0D01030102046001");
            tmp_vec.push_back("0D01030102046001-0401020201000000");
            tmp_vec.push_back("0D01030102046001-0401020201000000 / 0D01030102046001-0401020201040300");
            tmp_vec.push_back("0D01030102046001-0401020201011100");
            tmp_vec.push_back("0D01030102046001-0401020201020200");
            tmp_vec.push_back("0D01030102046001-0401020201020300");
            tmp_vec.push_back("0D01030102046001-0401020201030300");
            tmp_vec.push_back("0D01030102046001-0401020201040000");
            tmp_vec.push_back("0D01030102046001-0401020201040200");
            tmp_vec.push_back("0D01030102046001-0401020201040300");
            tmp_vec.push_back("0D01030102046001-0401020201050300");
            tmp_vec.push_back("0D01030102046001-0401020201070300");
            tmp_vec.push_back("0D01030102046101-0401020201011100");
            tmp_vec.push_back("0D01030102046101-0401020201020101");
            tmp_vec.push_back("0D01030102046101-0401020201020105");
            tmp_vec.push_back("0D01030102046101-0401020201020200");
            tmp_vec.push_back("0D01030102046101-0401020201020300");
            tmp_vec.push_back("0D01030102046101-0401020201030300");
            tmp_vec.push_back("0D01030102046101-0401020201040200");
            tmp_vec.push_back("0D01030102046101-0401020201040300");
            tmp_vec.push_back("0D01030102046107-0401020201200203");
            tmp_vec.push_back("0D01030102050001-040102017F000000");
            tmp_vec.push_back("0D01030102050105-0401020101020201");
            tmp_vec.push_back("0D01030102050200-0401020101020201");
            tmp_vec.push_back("0D010301020C0000-0401020203010100");
            tmp_vec.push_back("0D010301020C0000-0401020203010100 / 0D010301020C0100-0401020203010100");
            tmp_vec.push_back("0D010301020C0100-0401020203010100");
            tmp_vec.push_back("0D010301020C0100-0401020203010101");
            tmp_vec.push_back("0D010301020C0100-0401020203010102");
            tmp_vec.push_back("0D010301020C0100-0401020203010103");
            tmp_vec.push_back("0D010301020C0100-0401020203010104");
            tmp_vec.push_back("0D010301020C0100-0401020203010112");
            tmp_vec.push_back("0D010301020C0100-0401020203010113");
            tmp_vec.push_back("0D010301020C0100-0401020203010114");
            tmp_vec.push_back("0D010301020C0100-0401020203010115");
            tmp_vec.push_back("0D010301020C0100-0401020203010117");
            tmp_vec.push_back("0D010301020C0100-040102020301017F");
            tmp_vec.push_back("0D01030102106001-0401020201316001");
            tmp_vec.push_back("0D01030102106001-0401020201323101");
            tmp_vec.push_back("0D01030102106001-0401020201323102");
            tmp_vec.push_back("0D01030102106001-0401020201323103");
            tmp_vec.push_back("0D01030102106002-0401020201322101");
            tmp_vec.push_back("0D01030102106002-0401020201322109");
            tmp_vec.push_back("0D01030102106002-0401020201323101");
            tmp_vec.push_back("0D01030102106002-0401020201323102");
            tmp_vec.push_back("0D01030102106002-0401020201323104");
            tmp_vec.push_back("0D01030102106101-0401020201323101");
            tmp_vec.push_back("0D01030102110100-0401020271010000");
            tmp_vec.push_back("0D01030102110100-0401020271030000");
            tmp_vec.push_back("0D01030102110100-0401020271040000");
            tmp_vec.push_back("0D01030102110100-0401020271070000");
            tmp_vec.push_back("0D01030102110100-0401020271080000");
            tmp_vec.push_back("0D01030102110100-0401020271100000");
            tmp_vec.push_back("0D01030102110100-0401020271160000");
            tmp_vec.push_back("0E060D0302010100-0E06040102040201");
            tmp_vec.push_back("0x00000000");
            tmp_vec.push_back("0x01000000");
            tmp_vec.push_back("0x02000010");
            tmp_vec.push_back("10");
            tmp_vec.push_back("11");
            tmp_vec.push_back("12");
            tmp_vec.push_back("128");
            tmp_vec.push_back("13");
            tmp_vec.push_back("14");
            tmp_vec.push_back("15");
            tmp_vec.push_back("16");
            tmp_vec.push_back("2");
            tmp_vec.push_back("20");
            tmp_vec.push_back("209");
            tmp_vec.push_back("234");
            tmp_vec.push_back("25");
            tmp_vec.push_back("27");
            tmp_vec.push_back("2Vuy");
            tmp_vec.push_back("2vuy");
            tmp_vec.push_back("3");
            tmp_vec.push_back("32");
            tmp_vec.push_back("36");
            tmp_vec.push_back("4");
            tmp_vec.push_back("4B464141000D4D4F-0401020201020101");
            tmp_vec.push_back("4B464141000D4D4F-0401020201040300");
            tmp_vec.push_back("4B464141000D4D4F-0401020202010200");
            tmp_vec.push_back("4B464141000D4D4F-0401020202020600");
            tmp_vec.push_back("4B464141000D4D4F-0401020202020800");
            tmp_vec.push_back("4B464141000D4D4F-0E04020102040100");
            tmp_vec.push_back("5");
            tmp_vec.push_back("6");
            tmp_vec.push_back("61");
            tmp_vec.push_back("6C");
            tmp_vec.push_back("6D");
            tmp_vec.push_back("7");
            tmp_vec.push_back("9");
            tmp_vec.push_back("AV1x");
            tmp_vec.push_back("AVC1");
            tmp_vec.push_back("AVDJ");
            tmp_vec.push_back("AVd1");
            tmp_vec.push_back("AVdn");
            tmp_vec.push_back("AVdv");
            tmp_vec.push_back("AVmp / 4B464141000D4D4F-0401020201020101");
            tmp_vec.push_back("CFHD");
            tmp_vec.push_back("CHQX");
            tmp_vec.push_back("CLLC");
            tmp_vec.push_back("CRAM");
            tmp_vec.push_back("CUVC");
            tmp_vec.push_back("DIV3");
            tmp_vec.push_back("DIVX");
            tmp_vec.push_back("DVR ");
            tmp_vec.push_back("DX50");
            tmp_vec.push_back("FFV1");
            tmp_vec.push_back("FFVH");
            tmp_vec.push_back("FMP4");
            tmp_vec.push_back("FPS1");
            tmp_vec.push_back("GMP4");
            tmp_vec.push_back("H263");
            tmp_vec.push_back("H264");
            tmp_vec.push_back("HFYU");
            tmp_vec.push_back("Hap1");
            tmp_vec.push_back("I420");
            tmp_vec.push_back("IV32");
            tmp_vec.push_back("KW-DIRAC");
            tmp_vec.push_back("LAGS");
            tmp_vec.push_back("M4S2");
            tmp_vec.push_back("MJPG");
            tmp_vec.push_back("MMES");
            tmp_vec.push_back("MP42");
            tmp_vec.push_back("MP43");
            tmp_vec.push_back("ONYX");
            tmp_vec.push_back("REDC");
            tmp_vec.push_back("RV20");
            tmp_vec.push_back("RV30");
            tmp_vec.push_back("RV40");
            tmp_vec.push_back("SEDG");
            tmp_vec.push_back("SVQ1");
            tmp_vec.push_back("SVQ3");
            tmp_vec.push_back("ULH0");
            tmp_vec.push_back("ULH2");
            tmp_vec.push_back("UYVY");
            tmp_vec.push_back("VP50");
            tmp_vec.push_back("VP80");
            tmp_vec.push_back("V_DIRAC");
            tmp_vec.push_back("V_MPEG2");
            tmp_vec.push_back("V_MPEG4/IS0/ASP");
            tmp_vec.push_back("V_MPEG4/ISO/AVC");
            tmp_vec.push_back("V_MPEGH/ISO/HEVC");
            tmp_vec.push_back("V_MS/VFW/FOURCC / DIV3");
            tmp_vec.push_back("V_MS/VFW/FOURCC / DX50");
            tmp_vec.push_back("V_MS/VFW/FOURCC / FFV1");
            tmp_vec.push_back("V_MS/VFW/FOURCC / H264");
            tmp_vec.push_back("V_MS/VFW/FOURCC / VP62");
            tmp_vec.push_back("V_MS/VFW/FOURCC / WMV3");
            tmp_vec.push_back("V_MS/VFW/FOURCC / WVC1");
            tmp_vec.push_back("V_MS/VFW/FOURCC / XVID");
            tmp_vec.push_back("V_MS/VFW/FOURCC / apch");
            tmp_vec.push_back("V_REAL/RV10");
            tmp_vec.push_back("V_REAL/RV20");
            tmp_vec.push_back("V_REAL/RV30");
            tmp_vec.push_back("V_REAL/RV40");
            tmp_vec.push_back("V_THEORA");
            tmp_vec.push_back("V_VP8");
            tmp_vec.push_back("V_VP9");
            tmp_vec.push_back("WMV1");
            tmp_vec.push_back("WMV2");
            tmp_vec.push_back("WMV3");
            tmp_vec.push_back("WMVA");
            tmp_vec.push_back("WRLE");
            tmp_vec.push_back("WVC1");
            tmp_vec.push_back("WVP2");
            tmp_vec.push_back("XVID");
            tmp_vec.push_back("YV12");
            tmp_vec.push_back("ai12");
            tmp_vec.push_back("ai13");
            tmp_vec.push_back("ai15");
            tmp_vec.push_back("ai5q");
            tmp_vec.push_back("ap4h");
            tmp_vec.push_back("apch");
            tmp_vec.push_back("apcn");
            tmp_vec.push_back("apco");
            tmp_vec.push_back("apcs");
            tmp_vec.push_back("avc1");
            tmp_vec.push_back("cvid");
            tmp_vec.push_back("divx");
            tmp_vec.push_back("dslv");
            tmp_vec.push_back("dv25");
            tmp_vec.push_back("dv5n");
            tmp_vec.push_back("dvc ");
            tmp_vec.push_back("dvcp");
            tmp_vec.push_back("dvh2");
            tmp_vec.push_back("dvh3");
            tmp_vec.push_back("dvh5");
            tmp_vec.push_back("dvh6");
            tmp_vec.push_back("dvhp");
            tmp_vec.push_back("dvhq");
            tmp_vec.push_back("dvpp");
            tmp_vec.push_back("dvsd");
            tmp_vec.push_back("encv / avc1");
            tmp_vec.push_back("encv / avc1 / mp4a");
            tmp_vec.push_back("h263");
            tmp_vec.push_back("hdv3");
            tmp_vec.push_back("hdv6");
            tmp_vec.push_back("hvc1");
            tmp_vec.push_back("icod");
            tmp_vec.push_back("jpeg");
            tmp_vec.push_back("mjp2");
            tmp_vec.push_back("mjpa");
            tmp_vec.push_back("mpeg");
            tmp_vec.push_back("mpg1");
            tmp_vec.push_back("mpg2");
            tmp_vec.push_back("mx3n");
            tmp_vec.push_back("mx3p");
            tmp_vec.push_back("mx4p");
            tmp_vec.push_back("mx5n");
            tmp_vec.push_back("mx5p");
            tmp_vec.push_back("ovc1");
            tmp_vec.push_back("png ");
            tmp_vec.push_back("raw ");
            tmp_vec.push_back("rle ");
            tmp_vec.push_back("rpza");
            tmp_vec.push_back("s263");
            tmp_vec.push_back("tscc");
            tmp_vec.push_back("v210");
            tmp_vec.push_back("vc-1");
            tmp_vec.push_back("xd59");
            tmp_vec.push_back("xd5a");
            tmp_vec.push_back("xd5b");
            tmp_vec.push_back("xd5c");
            tmp_vec.push_back("xd5d");
            tmp_vec.push_back("xd5e");
            tmp_vec.push_back("xd5f");
            tmp_vec.push_back("xdv1");
            tmp_vec.push_back("xdv2");
            tmp_vec.push_back("xdv3");
            tmp_vec.push_back("xdv4");
            tmp_vec.push_back("xdv5");
            tmp_vec.push_back("xdv6");
            tmp_vec.push_back("xdv7");
            tmp_vec.push_back("xdv8");
            tmp_vec.push_back("xdv9");
            tmp_vec.push_back("xdva");
            tmp_vec.push_back("xdvb");
            tmp_vec.push_back("xdvc");
            tmp_vec.push_back("xdvd");
            tmp_vec.push_back("xdve");
            tmp_vec.push_back("xdvf");
            tmp_vec.push_back("yuv2");
            tmp["CodecID"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("RGB");
            tmp_vec.push_back("RGBA");
            tmp_vec.push_back("XYZ");
            tmp_vec.push_back("Y");
            tmp_vec.push_back("YUV");
            tmp_vec.push_back("YUV / YUV");
            tmp["ColorSpace"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Lossless");
            tmp_vec.push_back("Lossy");
            tmp["Compression_Mode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("No");
            tmp_vec.push_back("Yes");
            tmp["Default"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1.333");
            tmp_vec.push_back("1.778");
            tmp["DisplayAspectRatio"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Cryptoworks");
            tmp_vec.push_back("Encrypted");
            tmp_vec.push_back("NDS Videoguard 1/2");
            tmp_vec.push_back("Nagravision");
            tmp_vec.push_back("PlayReady");
            tmp_vec.push_back("Seca Mediaguard 1/2");
            tmp_vec.push_back("TPS-Crypt  or Viaccess");
            tmp["Encryption"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("No");
            tmp_vec.push_back("Yes");
            tmp["Forced"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("AIC");
            tmp_vec.push_back("AVC");
            tmp_vec.push_back("AVS Video");
            tmp_vec.push_back("Bitmap");
            tmp_vec.push_back("Canopus HQ");
            tmp_vec.push_back("Canopus HQX");
            tmp_vec.push_back("Canopus Lossless");
            tmp_vec.push_back("CineForm");
            tmp_vec.push_back("Cinepack");
            tmp_vec.push_back("Cross Fade");
            tmp_vec.push_back("DPX");
            tmp_vec.push_back("DV");
            tmp_vec.push_back("Dirac");
            tmp_vec.push_back("EXR");
            tmp_vec.push_back("FFV1");
            tmp_vec.push_back("FLC");
            tmp_vec.push_back("FLI");
            tmp_vec.push_back("Fraps");
            tmp_vec.push_back("GeoVision Advanced MPEG-4");
            tmp_vec.push_back("H.263");
            tmp_vec.push_back("HEVC");
            tmp_vec.push_back("Hap");
            tmp_vec.push_back("HuffYUV");
            tmp_vec.push_back("Indeo 3");
            tmp_vec.push_back("JPEG");
            tmp_vec.push_back("JPEG 2000");
            tmp_vec.push_back("Lagarith");
            tmp_vec.push_back("MPEG Video");
            tmp_vec.push_back("MPEG-4 Visual");
            tmp_vec.push_back("MS Video");
            tmp_vec.push_back("PNG");
            tmp_vec.push_back("ProRes");
            tmp_vec.push_back("REDC");
            tmp_vec.push_back("RGB");
            tmp_vec.push_back("RGBA");
            tmp_vec.push_back("RLE");
            tmp_vec.push_back("RV24");
            tmp_vec.push_back("RealVideo 1");
            tmp_vec.push_back("RealVideo 2");
            tmp_vec.push_back("RealVideo 3");
            tmp_vec.push_back("RealVideo 4");
            tmp_vec.push_back("Road Pizza");
            tmp_vec.push_back("Screen video");
            tmp_vec.push_back("Sony RAW SQ");
            tmp_vec.push_back("Sorenson 1");
            tmp_vec.push_back("Sorenson 3");
            tmp_vec.push_back("Sorenson Spark");
            tmp_vec.push_back("TechSmith");
            tmp_vec.push_back("Theora");
            tmp_vec.push_back("VC-1");
            tmp_vec.push_back("VC-3");
            tmp_vec.push_back("VP5");
            tmp_vec.push_back("VP6");
            tmp_vec.push_back("VP7");
            tmp_vec.push_back("VP8");
            tmp_vec.push_back("VP9");
            tmp_vec.push_back("WMV1");
            tmp_vec.push_back("WMV2");
            tmp_vec.push_back("WVP2");
            tmp_vec.push_back("YUV");
            tmp["Format"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("AVC-Intra 100");
            tmp_vec.push_back("AVC-Intra 50");
            tmp_vec.push_back("DNxHD 116");
            tmp_vec.push_back("DNxHD 120");
            tmp_vec.push_back("DNxHD 145");
            tmp_vec.push_back("DNxHD 175");
            tmp_vec.push_back("DNxHD 175x");
            tmp_vec.push_back("DNxHD 185");
            tmp_vec.push_back("DNxHD 185x");
            tmp_vec.push_back("DNxHD 220x");
            tmp_vec.push_back("DNxHD 90x");
            tmp_vec.push_back("DVCPRO");
            tmp_vec.push_back("DVCPRO 50");
            tmp_vec.push_back("DVCPRO HD");
            tmp_vec.push_back("HDV 1080i");
            tmp_vec.push_back("HDV 1080p");
            tmp_vec.push_back("HDV 720p");
            tmp_vec.push_back("IMX 30");
            tmp_vec.push_back("IMX 40");
            tmp_vec.push_back("IMX 50");
            tmp_vec.push_back("XDCAM EX 18");
            tmp_vec.push_back("XDCAM EX 35");
            tmp_vec.push_back("XDCAM HD 18");
            tmp_vec.push_back("XDCAM HD 25");
            tmp_vec.push_back("XDCAM HD 35");
            tmp_vec.push_back("XDCAM HD422");
            tmp["Format_Commercial_IfAny"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("LZ77");
            tmp_vec.push_back("PIZ");
            tmp_vec.push_back("ZIP");
            tmp["Format_Compression"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("4:2:2@High");
            tmp_vec.push_back("4:2:2@Main");
            tmp_vec.push_back("Advanced Simple@L1");
            tmp_vec.push_back("Advanced Simple@L2");
            tmp_vec.push_back("Advanced Simple@L3");
            tmp_vec.push_back("Advanced Simple@L4");
            tmp_vec.push_back("Advanced Simple@L5");
            tmp_vec.push_back("Advanced@L0");
            tmp_vec.push_back("Advanced@L1");
            tmp_vec.push_back("Advanced@L2");
            tmp_vec.push_back("Advanced@L3");
            tmp_vec.push_back("Alpha channel");
            tmp_vec.push_back("BCMR@L7");
            tmp_vec.push_back("BCS@L2");
            tmp_vec.push_back("BCS@L3");
            tmp_vec.push_back("BCS@L4");
            tmp_vec.push_back("BCS@L5");
            tmp_vec.push_back("Base@2.0");
            tmp_vec.push_back("Base@6.0");
            tmp_vec.push_back("Base@6.2");
            tmp_vec.push_back("BaseLine@1.0");
            tmp_vec.push_back("BaseLine@4.0");
            tmp_vec.push_back("Baseline@L1.0");
            tmp_vec.push_back("Baseline@L1.1");
            tmp_vec.push_back("Baseline@L1.2");
            tmp_vec.push_back("Baseline@L1.3");
            tmp_vec.push_back("Baseline@L2.1");
            tmp_vec.push_back("Baseline@L2.2");
            tmp_vec.push_back("Baseline@L3.0");
            tmp_vec.push_back("Baseline@L3.1");
            tmp_vec.push_back("Baseline@L3.2");
            tmp_vec.push_back("Baseline@L4.0");
            tmp_vec.push_back("Baseline@L4.1");
            tmp_vec.push_back("Baseline@L5.0");
            tmp_vec.push_back("D-Cinema 2k");
            tmp_vec.push_back("D-Cinema 4k");
            tmp_vec.push_back("Heightened Sharpness");
            tmp_vec.push_back("High");
            tmp_vec.push_back("High 10 Intra@L3.2");
            tmp_vec.push_back("High 10 Intra@L4.0");
            tmp_vec.push_back("High 10@L4.1");
            tmp_vec.push_back("High 4:2:2 Intra@L4.1");
            tmp_vec.push_back("High 4:2:2@L4.0");
            tmp_vec.push_back("High 4:2:2@L4.1");
            tmp_vec.push_back("High 4:2:2@L5.1");
            tmp_vec.push_back("High@High");
            tmp_vec.push_back("High@High 1440");
            tmp_vec.push_back("High@L1.2");
            tmp_vec.push_back("High@L1.3");
            tmp_vec.push_back("High@L2.0");
            tmp_vec.push_back("High@L2.1");
            tmp_vec.push_back("High@L2.2");
            tmp_vec.push_back("High@L3.0");
            tmp_vec.push_back("High@L3.1");
            tmp_vec.push_back("High@L3.2");
            tmp_vec.push_back("High@L4.0");
            tmp_vec.push_back("High@L4.1");
            tmp_vec.push_back("High@L4.2");
            tmp_vec.push_back("High@L5.0");
            tmp_vec.push_back("High@L5.1");
            tmp_vec.push_back("LT");
            tmp_vec.push_back("Line");
            tmp_vec.push_back("MP@HL");
            tmp_vec.push_back("MP@LL");
            tmp_vec.push_back("MP@ML");
            tmp_vec.push_back("Main");
            tmp_vec.push_back("Main 10@L5.1");
            tmp_vec.push_back("Main@High");
            tmp_vec.push_back("Main@High 1440");
            tmp_vec.push_back("Main@L1.1");
            tmp_vec.push_back("Main@L1.2");
            tmp_vec.push_back("Main@L1.3");
            tmp_vec.push_back("Main@L2.0");
            tmp_vec.push_back("Main@L2.1");
            tmp_vec.push_back("Main@L2.2");
            tmp_vec.push_back("Main@L3.0");
            tmp_vec.push_back("Main@L3.1");
            tmp_vec.push_back("Main@L3.2");
            tmp_vec.push_back("Main@L4.0");
            tmp_vec.push_back("Main@L4.1");
            tmp_vec.push_back("Main@L5.0");
            tmp_vec.push_back("Main@L5.1");
            tmp_vec.push_back("Main@L6.2");
            tmp_vec.push_back("Main@Low");
            tmp_vec.push_back("Main@Main");
            tmp_vec.push_back("Multiview High@L3.1 / High@L3.1");
            tmp_vec.push_back("No restrictions");
            tmp_vec.push_back("Profile-0");
            tmp_vec.push_back("Proxy");
            tmp_vec.push_back("Simple");
            tmp_vec.push_back("Simple@L0");
            tmp_vec.push_back("Simple@L0b");
            tmp_vec.push_back("Simple@L1");
            tmp_vec.push_back("Simple@L3");
            tmp_vec.push_back("Simple@L4a");
            tmp_vec.push_back("Simple@Main");
            tmp_vec.push_back("Stereo High@L4.1");
            tmp_vec.push_back("Stereo High@L4.1 / High@L4.0");
            tmp_vec.push_back("Stereo High@L4.1 / High@L4.1");
            tmp_vec.push_back("Unknown@L25.5 / Main@L4.0");
            tmp["Format_Profile"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1");
            tmp_vec.push_back("2");
            tmp_vec.push_back("3");
            tmp_vec.push_back("No");
            tmp_vec.push_back("Yes");
            tmp["Format_Settings_BVOP"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("No");
            tmp_vec.push_back("Yes");
            tmp["Format_Settings_CABAC"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Frame doubling");
            tmp["Format_Settings_FrameMode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0");
            tmp_vec.push_back("2");
            tmp_vec.push_back("3");
            tmp["Format_Settings_GMC"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("M=1, N=10");
            tmp_vec.push_back("M=1, N=12");
            tmp_vec.push_back("M=1, N=120");
            tmp_vec.push_back("M=1, N=13");
            tmp_vec.push_back("M=1, N=15");
            tmp_vec.push_back("M=1, N=18");
            tmp_vec.push_back("M=1, N=22");
            tmp_vec.push_back("M=1, N=24");
            tmp_vec.push_back("M=1, N=25");
            tmp_vec.push_back("M=1, N=30");
            tmp_vec.push_back("M=1, N=31");
            tmp_vec.push_back("M=1, N=32");
            tmp_vec.push_back("M=1, N=33");
            tmp_vec.push_back("M=1, N=45");
            tmp_vec.push_back("M=1, N=48");
            tmp_vec.push_back("M=1, N=60");
            tmp_vec.push_back("M=1, N=90");
            tmp_vec.push_back("M=16, N=32");
            tmp_vec.push_back("M=2, N=10");
            tmp_vec.push_back("M=2, N=12");
            tmp_vec.push_back("M=2, N=120");
            tmp_vec.push_back("M=2, N=13");
            tmp_vec.push_back("M=2, N=14");
            tmp_vec.push_back("M=2, N=15");
            tmp_vec.push_back("M=2, N=16");
            tmp_vec.push_back("M=2, N=24");
            tmp_vec.push_back("M=2, N=30");
            tmp_vec.push_back("M=2, N=33");
            tmp_vec.push_back("M=2, N=4");
            tmp_vec.push_back("M=2, N=48");
            tmp_vec.push_back("M=2, N=5");
            tmp_vec.push_back("M=2, N=8");
            tmp_vec.push_back("M=3, N=10");
            tmp_vec.push_back("M=3, N=12");
            tmp_vec.push_back("M=3, N=13");
            tmp_vec.push_back("M=3, N=15");
            tmp_vec.push_back("M=3, N=16");
            tmp_vec.push_back("M=3, N=18");
            tmp_vec.push_back("M=3, N=24");
            tmp_vec.push_back("M=3, N=25");
            tmp_vec.push_back("M=3, N=27");
            tmp_vec.push_back("M=3, N=30");
            tmp_vec.push_back("M=3, N=33");
            tmp_vec.push_back("M=3, N=4");
            tmp_vec.push_back("M=3, N=45");
            tmp_vec.push_back("M=3, N=48");
            tmp_vec.push_back("M=3, N=6");
            tmp_vec.push_back("M=3, N=60");
            tmp_vec.push_back("M=3, N=9");
            tmp_vec.push_back("M=4, N=12");
            tmp_vec.push_back("M=4, N=120");
            tmp_vec.push_back("M=4, N=15");
            tmp_vec.push_back("M=4, N=16");
            tmp_vec.push_back("M=4, N=24");
            tmp_vec.push_back("M=4, N=26");
            tmp_vec.push_back("M=4, N=32");
            tmp_vec.push_back("M=4, N=33");
            tmp_vec.push_back("M=4, N=60");
            tmp_vec.push_back("M=4, N=90");
            tmp_vec.push_back("M=5, N=15");
            tmp_vec.push_back("M=5, N=26");
            tmp_vec.push_back("N=1");
            tmp_vec.push_back("N=10");
            tmp_vec.push_back("N=12");
            tmp_vec.push_back("N=15");
            tmp_vec.push_back("N=299");
            tmp_vec.push_back("N=3");
            tmp_vec.push_back("N=30");
            tmp_vec.push_back("N=60");
            tmp_vec.push_back("N=8");
            tmp_vec.push_back("Variable");
            tmp["Format_Settings_GOP"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Custom");
            tmp_vec.push_back("Default");
            tmp_vec.push_back("Default (H.263)");
            tmp_vec.push_back("Default (MPEG)");
            tmp["Format_Settings_Matrix"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Default");
            tmp["Format_Settings_Matrix_Data"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Field");
            tmp_vec.push_back("Frame");
            tmp["Format_Settings_PictureStructure"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("No");
            tmp_vec.push_back("Yes");
            tmp["Format_Settings_QPel"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1");
            tmp_vec.push_back("2");
            tmp_vec.push_back("3");
            tmp_vec.push_back("4");
            tmp_vec.push_back("5");
            tmp_vec.push_back("6");
            tmp_vec.push_back("7");
            tmp_vec.push_back("8");
            tmp_vec.push_back("9");
            tmp_vec.push_back("10");
            tmp_vec.push_back("11");
            tmp_vec.push_back("13");
            tmp_vec.push_back("15");
            tmp_vec.push_back("16");
            tmp["Format_Settings_RefFrames"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Clip");
            tmp_vec.push_back("Custom: Closed GOP");
            tmp_vec.push_back("Frame");
            tmp_vec.push_back("Frame (D-10)");
            tmp["Format_Settings_Wrapping"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0");
            tmp_vec.push_back("1");
            tmp_vec.push_back("2");
            tmp_vec.push_back("3");
            tmp_vec.push_back("3.4");
            tmp["Format_Version"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("25.000");
            tmp_vec.push_back("29.970");
            tmp_vec.push_back("30.000");
            tmp_vec.push_back("50.000");
            tmp_vec.push_back("59.940");
            tmp_vec.push_back("60.000");
            tmp["FrameRate"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1000");
            tmp_vec.push_back("1001");
            tmp["FrameRate_Den"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("CFR");
            tmp_vec.push_back("VFR");
            tmp["FrameRate_Mode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("29970");
            tmp_vec.push_back("30000");
            tmp_vec.push_back("59940");
            tmp_vec.push_back("60000");
            tmp["FrameRate_Num"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Closed");
            tmp_vec.push_back("Open");
            tmp["Gop_OpenClosed"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("480");
            tmp_vec.push_back("576");
            tmp_vec.push_back("720");
            tmp_vec.push_back("1080");
            tmp_vec.push_back("2160");
            tmp_vec.push_back("4320");
            tmp["Height"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("16");
            tmp_vec.push_back("32");
            tmp["Height_Offset"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("480");
            tmp_vec.push_back("576");
            tmp_vec.push_back("720");
            tmp_vec.push_back("1080");
            tmp_vec.push_back("2160");
            tmp_vec.push_back("4320");
            tmp["Height_Original"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("de");
            tmp_vec.push_back("ehs");
            tmp_vec.push_back("en");
            tmp_vec.push_back("en-S");
            tmp_vec.push_back("en-au");
            tmp_vec.push_back("en-gb");
            tmp_vec.push_back("en-us");
            tmp_vec.push_back("es");
            tmp_vec.push_back("fr");
            tmp_vec.push_back("it");
            tmp_vec.push_back("ja");
            tmp_vec.push_back("ko");
            tmp_vec.push_back("mul");
            tmp_vec.push_back("pt-br");
            tmp_vec.push_back("sv");
            tmp_vec.push_back("zh-cn");
            tmp["Language"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Clean effects");
            tmp["Language_More"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1");
            tmp_vec.push_back("2");
            tmp_vec.push_back("3");
            tmp["MultiView_Count"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Anaglyph (cyan/red)");
            tmp_vec.push_back("Side by Side (left eye first)");
            tmp["MultiView_Layout"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Container profile=Unknown@1.3");
            tmp_vec.push_back("DCP CPL / MXF");
            tmp_vec.push_back("HLS / MPEG-TS");
            tmp_vec.push_back("Header stripping");
            tmp_vec.push_back("IMF CPL");
            tmp_vec.push_back("IMF CPL / MXF");
            tmp_vec.push_back("MPEG-4");
            tmp_vec.push_back("MPEG-TS");
            tmp_vec.push_back("MXF");
            tmp_vec.push_back("Packed bitstream");
            tmp_vec.push_back("R3D");
            tmp_vec.push_back("SMV v1");
            tmp_vec.push_back("SMV v2");
            tmp["MuxingMode"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("1.000");
            tmp_vec.push_back("1.067");
            tmp_vec.push_back("1.126");
            tmp_vec.push_back("1.422");
            tmp_vec.push_back("1.500");
            tmp_vec.push_back("1.501");
            tmp["PixelAspectRatio"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("0.000");
            tmp_vec.push_back("90.000");
            tmp_vec.push_back("180.000");
            tmp["Rotation"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("2:3 Pulldown");
            tmp_vec.push_back("BFF");
            tmp_vec.push_back("TFF");
            tmp["ScanOrder"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Interlaced");
            tmp_vec.push_back("MBAFF");
            tmp_vec.push_back("Mixed");
            tmp_vec.push_back("Progressive");
            tmp["ScanType"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Component");
            tmp_vec.push_back("NTSC");
            tmp_vec.push_back("PAL");
            tmp["Standard"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("00:00:00:000");
            tmp_vec.push_back("00:00:00;000");
            tmp_vec.push_back("01:00:00:000");
            tmp_vec.push_back("01:00:00;000");
            tmp["TimeCode_FirstFrame"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Adobe tc_A / Adobe tc_O");
            tmp_vec.push_back("Group of pictures header");
            tmp_vec.push_back("ISMP");
            tmp_vec.push_back("Subcode time code");
            tmp["TimeCode_Source"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("720");
            tmp_vec.push_back("1080");
            tmp_vec.push_back("1920");
            tmp_vec.push_back("3840");
            tmp_vec.push_back("7680");
            tmp["Width"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("720");
            tmp_vec.push_back("1080");
            tmp_vec.push_back("1920");
            tmp_vec.push_back("3840");
            tmp_vec.push_back("7680");
            tmp["Width_Original"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Yes");
            tmp["colour_description_present"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Yes");
            tmp["colour_description_present_Original"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("BT.470 System B");
            tmp_vec.push_back("BT.470 System M");
            tmp_vec.push_back("BT.601 NTSC");
            tmp_vec.push_back("BT.601 PAL");
            tmp_vec.push_back("BT.709");
            tmp_vec.push_back("Linear");
            tmp_vec.push_back("Printing density");
            tmp_vec.push_back("SMPTE 240M");
            tmp["colour_primaries"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("BT.601 PAL");
            tmp_vec.push_back("BT.709");
            tmp["colour_primaries_Original"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("Full");
            tmp_vec.push_back("Limited");
            tmp["colour_range"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("BT.601");
            tmp_vec.push_back("BT.709");
            tmp_vec.push_back("FCC 73.682");
            tmp_vec.push_back("RGB");
            tmp_vec.push_back("SMPTE 240M");
            tmp["matrix_coefficients"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("BT.601");
            tmp_vec.push_back("BT.709");
            tmp_vec.push_back("RGB");
            tmp["matrix_coefficients_Original"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("BT.470");
            tmp_vec.push_back("BT.470 System B, BT.470 System G");
            tmp_vec.push_back("BT.470 System M");
            tmp_vec.push_back("BT.601");
            tmp_vec.push_back("BT.709");
            tmp_vec.push_back("Printing density");
            tmp_vec.push_back("SMPTE 240M");
            tmp["transfer_characteristics"] = tmp_vec;
        }
        {
            std::vector<std::string> tmp_vec;
            tmp_vec.push_back("BT.470 System B, BT.470 System G");
            tmp_vec.push_back("BT.601");
            tmp_vec.push_back("BT.709");
            tmp["transfer_characteristics_Original"] = tmp_vec;
        }
        values["Video"] = tmp;
    }
    return 0;
}

#endif // ! GENERATEDCSVVIDEOS_HPP
