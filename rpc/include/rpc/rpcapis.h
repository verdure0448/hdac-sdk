#ifndef RPCAPIS_H
#define RPCAPIS_H

#include <json_spirit/json_spirit.h>

class RpcClient;

json_spirit::Object getinfo(const RpcClient& client);
json_spirit::Object blockChainParams(const RpcClient& client);
json_spirit::Object listunspent(const RpcClient& client, int minConf = 1, int maxConf = 9999999,
                                const std::vector<std::string>& addresses = std::vector<std::string>{});
json_spirit::Object listunspent(const RpcClient& client, const std::vector<std::string>& addresses);
json_spirit::Object listunspent(const RpcClient& client, const std::string& address);
json_spirit::Object lockunspent(const RpcClient& client, bool unlock, std::string txid, int vout);
json_spirit::Object listlockunspent(const RpcClient& client);
json_spirit::Object liststreams(const RpcClient& client, const std::vector<std::string> &streamNames);
json_spirit::Object liststreams(const RpcClient& client, const std::string &streamName = "all");
json_spirit::Object listassets(const RpcClient& client, const std::vector<std::string> &assetNames);
json_spirit::Object listassets(const RpcClient& client, const std::string &name = "");


#endif // RPCAPIS_H
