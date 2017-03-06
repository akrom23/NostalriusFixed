/*
 * 
 */

#include "TransactionMgr.h"

TransactionMgr::TransactionMgr(bool confEnabled, uint32 confPeak, uint32 confPeakFreq, uint32 confPackFreq) :
    enabled(confEnabled), peak(confPeak), peakFrequency(confPeakFreq), packFrequency(confPackFreq)
{
}

void TransactionMgr::Update()
{
    if (!enabled)
        return;

    // TODO
}

void TransactionMgr::Enable(bool enable)
{
    enabled = enable;
}
