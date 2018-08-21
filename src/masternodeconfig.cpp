
#include "netbase.h"
#include "masternodeconfig.h"
#include "util.h"
#include "chainparams.h"

#include "coins.h"
#include "main.h"


#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

CMasternodeConfig masternodeConfig;

void CMasternodeConfig::add(std::string alias, std::string ip, std::string privKey, std::string txHash, std::string outputIndex) {
    CMasternodeEntry cme(alias, ip, privKey, txHash, outputIndex);
    entries.push_back(cme);
}

bool CMasternodeConfig::read(std::string& strErr) {
#if 0
    int linenumber = 1;
    boost::filesystem::path pathMasternodeConfigFile = GetMasternodeConfigFile();
    boost::filesystem::ifstream streamConfig(pathMasternodeConfigFile);

    if (!streamConfig.good()) {
        FILE* configFile = fopen(pathMasternodeConfigFile.string().c_str(), "a");
        if (configFile != NULL) {
            std::string strHeader = "# Masternode config file\n"
                          "# Format: alias IP:port masternodeprivkey collateral_output_txid collateral_output_index\n"
                          "# Example: mn1 127.0.0.2:19888 93HaYBVUCYjEMeeH1Y4sBGLALQZE1Yc1K64xiqgX37tGBDQL8Xg 2bcd3c84c84f87eaa86e4e56834c92927a07f9e18718810b92e0d0324456a67c 0\n";
            fwrite(strHeader.c_str(), std::strlen(strHeader.c_str()), 1, configFile);
            fclose(configFile);
        }
        return true; // Nothing to read, so just return
    }

    for(std::string line; std::getline(streamConfig, line); linenumber++)
    {
        if(line.empty()) continue;

        std::istringstream iss(line);
        std::string comment, alias, ip, privKey, txHash, outputIndex;

        if (iss >> comment) {
            if(comment.at(0) == '#') continue;
            iss.str(line);
            iss.clear();
        }

        if (!(iss >> alias >> ip >> privKey >> txHash >> outputIndex)) {
            iss.str(line);
            iss.clear();
            if (!(iss >> alias >> ip >> privKey >> txHash >> outputIndex)) {
                strErr = _("Could not parse masternode.conf") + "\n" +
                        strprintf(_("Line: %d"), linenumber) + "\n\"" + line + "\"";
                streamConfig.close();
                return false;
            }
        }

        int port = 0;
        std::string hostname = "";
        SplitHostPort(ip, port, hostname);
        if(port == 0 || hostname == "") {
            strErr = _("Failed to parse host:port string") + "\n"+
                    strprintf(_("Line: %d"), linenumber) + "\n\"" + line + "\"";
            streamConfig.close();
            return false;
        }
        int mainnetDefaultPort = Params(CBaseChainParams::MAIN).GetDefaultPort();
        if(Params().NetworkIDString() == CBaseChainParams::MAIN) {
            if(port != mainnetDefaultPort) {
                strErr = _("Invalid port detected in masternode.conf") + "\n" +
                        strprintf(_("Port: %d"), port) + "\n" +
                        strprintf(_("Line: %d"), linenumber) + "\n\"" + line + "\"" + "\n" +
                        strprintf(_("(must be %d for mainnet)"), mainnetDefaultPort);
                streamConfig.close();
                return false;
            }
        } else if(port == mainnetDefaultPort) {
            strErr = _("Invalid port detected in masternode.conf") + "\n" +
                    strprintf(_("Line: %d"), linenumber) + "\n\"" + line + "\"" + "\n" +
                    strprintf(_("(%d could be used only on mainnet)"), mainnetDefaultPort);
            streamConfig.close();
            return false;
        }


        add(alias, ip, privKey, txHash, outputIndex);
    }
    streamConfig.close();
#endif

    boost masternodeflag = GetBoolArg("-masternode", false);
    if(masternodeflag)
    {
        std::string alias, ip, privKey, txHash, outputIndex;
        alias = GetArg("-alias", "");
        if(alias.empty())
        {
            strErr = _("please add your masternode name into ulord.conf; for example: alias=mynode\n");
            return false;
        }
        ip = GetArg("-externalip", "");
        if(ip.empty())
        {
            strErr = _("Invalid masternode ip, please add your ip into ulord.conf; for example: externalip=0.0.0.0\n");
            return false;
        }
        ip = ip + ":" + std::to_string(Params().GetDefaultPort());
        
        privKey = GetArg("-masternodeprivkey", "");
        if(privKey.empty())
        {
            strErr = _("Invalid masternode privKey, please add your privKey into ulord.conf; for example: masternodeprivkey=***\n");
            return false;
        }
        txHash = GetArg("-collateraloutputtxid", "");
        if(txHash.empty())
        {
            strErr = _("Invalid masternode collateral txid, please add your collateral txid into ulord.conf; for example: collateraloutputtxid=***\n");
            return false;
        }

        outputIndex = GetArg("-collateraloutputindex", "");
        if(outputIndex.empty())
        {
            strErr = _("Invalid masternode collateral Index, please add your collateral Index into ulord.conf; for example: collateraloutputindex=0\n");
            return false;
        }
        
        int port = 0;
        std::string hostname = "";
        SplitHostPort(ip, port, hostname);
        if(port == 0 || hostname == "") {
            strErr = _("Failed to parse host:port string") + "\n";
            return false;
        }
        int mainnetDefaultPort = Params(CBaseChainParams::MAIN).GetDefaultPort();
        if(Params().NetworkIDString() == CBaseChainParams::MAIN) {
            if(port != mainnetDefaultPort) {
                strErr = _("Invalid port detected in ulord.conf") + "\n" +
                        strprintf(_("Port: %d"), port) + "\n" +
                        strprintf(_("(must be %d for mainnet)"), mainnetDefaultPort);
                return false;
            }
        } else if(port == mainnetDefaultPort) {
            strErr = _("Invalid port detected in ulord.conf") + "\n" +
                    strprintf(_("(%d could be used only on mainnet)"), mainnetDefaultPort);
            return false;
        }
            
        add(alias, ip, privKey, txHash, outputIndex);
    }
    return true;
}

CMasternodeConfig::CMasternodeEntry CMasternodeConfig::GetLocalEntry()
{
	if(fMasterNode)
	{
		for(auto & mn : entries)
		{
			if(mn.getPrivKey() == GetArg("-masternodeprivkey", ""))
				return mn;
		}
	}
	return CMasternodeEntry();
}

bool CMasternodeConfig::IsLocalEntry()
{
	if(fMasterNode)
	{
		for(auto & mn : entries)
		{
			if(mn.getPrivKey() == GetArg("-masternodeprivkey", "") && GetArg("-collateraloutputtxid", "") != "" 
				&& GetArg("-broadcastsign", "") != "")
				return true;
		}
	}
	return false;
}

bool CMasternodeConfig::InMempool(uint256 txHash) const
{
    LOCK(mempool.cs);
    if (mempool.exists(txHash)) {
        return true;
    }
    return false;
}

bool CMasternodeConfig::HaveInputs(const CTransaction& tx) const
{
    if (!tx.IsCoinBase()) {
        for (unsigned int i = 0; i < tx.vin.size(); i++) {
            const COutPoint &prevout = tx.vin[i].prevout;
            const CCoins* coins = AccessCoins(prevout.hash);
            if (!coins || !coins->IsAvailable(prevout.n)) {
                return false;
            }
        }
    }
    return true;
}

bool CMasternodeConfig::AvailableCoins(uint256 txHash, unsigned int index)
{
    // Find the block it claims to be in
    BlockMap::iterator mi = mapBlockIndex.find(txHash);
    if (mi == mapBlockIndex.end())
    {
        return false;
    }
    else {
        if(!InMempool(txHash))
            return false;
    }

    CTransaction tx;
    uint256 hashBlock;
    if(GetTransaction(txHash, tx, Params().GetConsensus(), hashBlock, true))
    {
        LogPrintf("CMasternodeBroadcast::AvailableCoins -- masternode collateraloutputtxid or collateraloutputindex is error,please check it\n");
        return false;
    }
    if (!CheckFinalTx(*tx) || tx.IsCoinBase()) {
        return false;
    }

    if(!HaveInputs(tx))
    {
        return false;
    }
    
    CCoins coins;
    if(!pcoinsTip->GetCoins(txHash, coins) || index >=coins.vout.size() || coins.vout[index].IsNull())
    {
        LogPrintf("CMasternodeBroadcast::GetMasternodeVin -- masternode collateraloutputtxid or collateraloutputindex is error,please check it\n");
        return false;
    }

    const int64_t ct = Params().GetConsensus().colleteral;     // colleteral amount
    if(coins->vout[index].nValue != ct)
        return false;

    return true;
}

bool CMasternodeConfig::GetMasternodeVin(CTxIn& txinRet,  std::string strTxHash, std::string strOutputIndex)
{
    // wait for reindex and/or import to finish
    if (fImporting || fReindex) return false;


    if(strTxHash.empty()) // No output specified, select the one specified by masternodeConfig
    {
        CMasternodeConfig::CMasternodeEntry mne = masternodeConfig.GetLocalEntry();
        unsigned int index = atoi(mne.getOutputIndex().c_str());
        uint256 txHash = uint256S(mne.getTxHash());
        txinRet = CTxIn(txHash, index);
        
        int nInputAge = GetInputAge(txinRet);
        if(nInputAge <= 0)
        {
            LogPrintf("CMasternodeBroadcast::GetMasternodeVin -- collateraloutputtxid or collateraloutputindex is not exist,please check it\n");
            return false;
        }

        if(!masternodeConfig.AvailableCoins(txHash, index))
        {
            LogPrintf("CMasternodeBroadcast::GetMasternodeVin -- collateraloutputtxid or collateraloutputindex is AvailableCoins,please check it\n");
            return false;
        }
        
        return true;
    }

    // Find specific vin
    uint256 txHash = uint256S(strTxHash);
    int nOutputIndex = atoi(strOutputIndex.c_str());

    txinRet = CTxIn(txHash,nOutputIndex);
    CCoins coins;
    if(pcoinsTip->GetCoins(txHash, coins))	
    {
        return true;
    }
    
    LogPrintf("CMasternodeConfig::GetMasternodeVin -- Could not locate specified masternode vin\n");
    return false;    
}



