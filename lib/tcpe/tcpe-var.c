/*
 * Copyright (c) 2012 The Board of Trustees of the University of Illinois,
 *                    Carnegie Mellon University.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 */
#include <estats/estats-int.h>

#define TCPEVAR(_name_,_type_) (struct estats_var) { .name = #_name_, .type = TCPE_##_type_ }

int max_index[MAX_TABLE] = { PERF_INDEX_MAX, PATH_INDEX_MAX, STACK_INDEX_MAX, APP_INDEX_MAX, TUNE_INDEX_MAX };

struct estats_var estats_var_array[TOTAL_INDEX_MAX] = {
        TCPEVAR(SegsOut,UNSIGNED32),
        TCPEVAR(DataSegsOut,UNSIGNED32),
        TCPEVAR(DataOctetsOut,UNSIGNED32),
        TCPEVAR(HCDataOctetsOut,UNSIGNED64),
        TCPEVAR(SegsRetrans,UNSIGNED32),
        TCPEVAR(OctetsRetrans,UNSIGNED32),
        TCPEVAR(SegsIn,UNSIGNED32),
        TCPEVAR(DataSegsIn,UNSIGNED32),
        TCPEVAR(DataOctetsIn,UNSIGNED32),
        TCPEVAR(HCDataOctetsIn,UNSIGNED64),
        TCPEVAR(ElapsedSecs,UNSIGNED32),
        TCPEVAR(ElapsedMicroSecs,UNSIGNED32),
        TCPEVAR(StartTimeStamp,UNSIGNED8),
        TCPEVAR(CurMSS,UNSIGNED32),
        TCPEVAR(PipeSize,UNSIGNED32),
        TCPEVAR(MaxPipeSize,UNSIGNED32),
        TCPEVAR(SmoothedRTT,UNSIGNED32),
        TCPEVAR(CurRTO,UNSIGNED32),
        TCPEVAR(CongSignals,UNSIGNED32),
        TCPEVAR(CurCwnd,UNSIGNED32),
        TCPEVAR(CurSsthresh,UNSIGNED32),
        TCPEVAR(Timeouts,UNSIGNED32),
        TCPEVAR(CurRwinSent,UNSIGNED32),
        TCPEVAR(MaxRwinSent,UNSIGNED32),
        TCPEVAR(ZeroRwinSent,UNSIGNED32),
        TCPEVAR(CurRwinRcvd,UNSIGNED32),
        TCPEVAR(MaxRwinRcvd,UNSIGNED32),
        TCPEVAR(ZeroRwinRcvd,UNSIGNED32),
        TCPEVAR(SndLimTransRwin,UNSIGNED32),
        TCPEVAR(SndLimTransCwnd,UNSIGNED32),
        TCPEVAR(SndLimTransSnd,UNSIGNED32),
        TCPEVAR(SndLimTimeRwin,UNSIGNED32),
        TCPEVAR(SndLimTimeCwnd,UNSIGNED32),
        TCPEVAR(SndLimTimeSnd,UNSIGNED32),

        TCPEVAR(RetranThresh,UNSIGNED32),
        TCPEVAR(NonRecovDAEpisodes,UNSIGNED32),
        TCPEVAR(SumOctetsReordered,UNSIGNED32),
        TCPEVAR(NonRecovDA,UNSIGNED32),
        TCPEVAR(SampleRTT,UNSIGNED32),
        TCPEVAR(RTTVar,UNSIGNED32),
        TCPEVAR(MaxRTT,UNSIGNED32),
        TCPEVAR(MinRTT,UNSIGNED32),
        TCPEVAR(SumRTT,UNSIGNED32),
        TCPEVAR(HCSumRTT,UNSIGNED64),
        TCPEVAR(CountRTT,UNSIGNED32),
        TCPEVAR(MaxRTO,UNSIGNED32),
        TCPEVAR(MinRTO,UNSIGNED32),
        TCPEVAR(IpTtl,UNSIGNED32),
        TCPEVAR(IpTosIn,UNSIGNED8),
        TCPEVAR(IpTosOut,UNSIGNED8),
        TCPEVAR(PreCongSumCwnd,UNSIGNED32),
        TCPEVAR(PreCongSumRTT,UNSIGNED32),
        TCPEVAR(PostCongSumRTT,UNSIGNED32),
        TCPEVAR(PostCongCountRTT,UNSIGNED32),
        TCPEVAR(ECNsignals,UNSIGNED32),
        TCPEVAR(DupAckEpisodes,UNSIGNED32),
        TCPEVAR(RcvRTT,UNSIGNED32),
        TCPEVAR(DupAcksOut,UNSIGNED32),
        TCPEVAR(CERcvd,UNSIGNED32),
        TCPEVAR(ECESent,UNSIGNED32),

	TCPEVAR(ActiveOpen,SIGNED32),
	TCPEVAR(MSSSent,UNSIGNED32),
	TCPEVAR(MSSRcvd,UNSIGNED32),
	TCPEVAR(WinScaleSent,SIGNED32),
	TCPEVAR(WinScaleRcvd,SIGNED32),
	TCPEVAR(TimeStamps,SIGNED32),
	TCPEVAR(ECN,SIGNED32),
	TCPEVAR(WillSendSACK,SIGNED32),
	TCPEVAR(WillUseSACK,SIGNED32),
	TCPEVAR(State,SIGNED32),
	TCPEVAR(Nagle,SIGNED32),
	TCPEVAR(MaxSsCwnd,UNSIGNED32),
	TCPEVAR(MaxCaCwnd,UNSIGNED32),
	TCPEVAR(MaxSsthresh,UNSIGNED32),
	TCPEVAR(MinSsthresh,UNSIGNED32),
	TCPEVAR(InRecovery,SIGNED32),
	TCPEVAR(DupAcksIn,UNSIGNED32),
	TCPEVAR(SpuriousFrDetected,UNSIGNED32),
	TCPEVAR(SpuriousRtoDetected,UNSIGNED32),
	TCPEVAR(SoftErrors,UNSIGNED32),
	TCPEVAR(SoftErrorReason,SIGNED32),
	TCPEVAR(SlowStart,UNSIGNED32),
	TCPEVAR(CongAvoid,UNSIGNED32),
	TCPEVAR(OtherReductions,UNSIGNED32),
	TCPEVAR(CongOverCount,UNSIGNED32),
	TCPEVAR(FastRetran,UNSIGNED32),
	TCPEVAR(SubsequentTimeouts,UNSIGNED32),
	TCPEVAR(CurTimeoutCount,UNSIGNED32),
	TCPEVAR(AbruptTimeouts,UNSIGNED32),
	TCPEVAR(SACKsRcvd,UNSIGNED32),
	TCPEVAR(SACKBlocksRcvd,UNSIGNED32),
	TCPEVAR(SendStall,UNSIGNED32),
	TCPEVAR(DSACKDups,UNSIGNED32),
	TCPEVAR(MaxMSS,UNSIGNED32),
	TCPEVAR(MinMSS,UNSIGNED32),
	TCPEVAR(SndInitial,UNSIGNED32),
	TCPEVAR(RecInitial,UNSIGNED32),
	TCPEVAR(CurRetxQueue,UNSIGNED32),
	TCPEVAR(MaxRetxQueue,UNSIGNED32),
	TCPEVAR(CurReasmQueue,UNSIGNED32),
	TCPEVAR(MaxReasmQueue,UNSIGNED32),

        TCPEVAR(SndUna,UNSIGNED32),
        TCPEVAR(SndNxt,UNSIGNED32),
        TCPEVAR(SndMax,UNSIGNED32),
        TCPEVAR(ThruOctetsAcked,UNSIGNED32),
        TCPEVAR(HCThruOctetsAcked,UNSIGNED64),
        TCPEVAR(RcvNxt,UNSIGNED32),
        TCPEVAR(ThruOctetsReceived,UNSIGNED32),
        TCPEVAR(HCThruOctetsReceived,UNSIGNED64),
        TCPEVAR(CurAppWQueue,UNSIGNED32),
        TCPEVAR(MaxAppWQueue,UNSIGNED32),
        TCPEVAR(CurAppRQueue,UNSIGNED32),
        TCPEVAR(MaxAppRQueue,UNSIGNED32),

        TCPEVAR(LimCwnd,UNSIGNED32),
        TCPEVAR(LimSsthresh,UNSIGNED32),
        TCPEVAR(LimRwin,UNSIGNED32),
        TCPEVAR(LimMSS,UNSIGNED32),
};
