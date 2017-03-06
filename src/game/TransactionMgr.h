/*
 * 
 */

#ifndef TRANSACTION_MGR_H
#define TRANSACTION_MGR_H

enum TransactionType
{
    TRANS_LOOT      = 0,
    TRANS_TRADE     = 1,
    TRANS_QUEST     = 2,
    TRANS_MAIL      = 3,
    TRANS_MAIL_COD  = 4,
    TRANS_MAIL_AUC  = 5,

    TRANS_UNK       = 6
};

struct Transaction
{
    ObjectGuid source, target;
    TransactionType type;
    uint32 amount;
    // time
    // quest entry, creature entry ?
};

enum
{
    DEF_PEAK        = 50000,
    DEF_PEAK_FREQ   = 1,
    DEF_PACK_FREQ   = 3000
};

class TransactionMgr
{
public:
    explicit TransactionMgr(bool confEnabled, uint32 confPeak = DEF_PEAK, uint32 confPeakFreq = DEF_PEAK_FREQ, uint32 confPackFreq = DEF_PACK_FREQ);

    void Update();
    void Enable(bool enable);

private:
    bool enabled;               // transaction system status
    uint32 peak;                // minimum peak to trigger
    uint32 peakFrequency;       // world updates between peak acquision
    uint32 packFrequency;       // world updates between full dump
};

#endif
