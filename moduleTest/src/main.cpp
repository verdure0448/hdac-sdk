#include <iostream>
#include <rpc/rpccaller.h>
#include <structs/hashes.h>
#include <rpc/cif_rpccall.h>
#include <keys/key.h>
#include <utils/utilstrencodings.h>
#include <keys/bitcoinaddress.h>
#include <utils/utilsfront.h>
#include <rpc/rpcresult.h>
#include <rpc/rpcclient.h>
#include <keys/bitcoinsecret.h>
#include <keys/keyshelper.h>
#include <json_spirit/json_spirit_reader_template.h>
#include <json_spirit/json_spirit_writer_template.h>
#include <keys/transactions.h>
#include <rpc/rpcapis.h>

using namespace std;
using namespace json_spirit;

void testCalcSHA256()
{
    //using namespace std::placeholders;
    //obtainHash("This is test message", displayHashValue);
    // data from https://passwordsgenerator.net/sha256-hash-generator/
    auto compareHash = bind(compareHashValue, placeholders::_1, "DDDBDC2845C9D80DC288710D9B2CF2D6C4F613D0DC4C048A9EA0E8674C2C5E73");
    obtainHash("This is test message", compareHash);
//    auto compareHash1 = bind(compareHashValue, _1, "CDDBDC2845C9D80DC288710D9B2CF2D6C4F613D0DC4C048A9EA0E8674C2C5E73");
//    obtainHash("This is test message", compareHash1);
}

// TODO : make class

// TODO : should add the logging function
// TODO : parameters from config-file.

class KeysHelper {
public:
    KeysHelper(const RpcClient& client) {
        const Object reply = blockChainParams(client);

        string resultStr;
        int nRet = result(reply, resultStr);
        if (nRet) {
            cerr << "rpc error : " << resultStr;
            return;
        }

        std::vector<string> keys{
            "address-pubkeyhash-version",
            "address-scripthash-version",
            "address-checksum-value",
            "private-key-version"
        };

        _resultMap = mapFromRpcResult(resultStr, keys);
        _addrHelper.reset(new WalletAddrHelper(_resultMap));
        _privHelper.reset(new PrivateKeyHelper(_resultMap));
    }

    IWalletAddrHelper& addrHelper() {
        return *_addrHelper;
    }

    IPrivateKeyHelper& privHelper() {
        return *_privHelper;
    }

private:
    map<string, string> _resultMap;

    class WalletAddrHelper : public IWalletAddrHelper {
    public:
        WalletAddrHelper(const map<string, string> &result) :
            _resultMap(result)  { }

        const std::vector<unsigned char> pubkeyAddrPrefix() const override  {
            return ParseHex(_resultMap.at("address-pubkeyhash-version"));
        }
        const std::vector<unsigned char> scriptAddrPrefix() const override  {
            return ParseHex(_resultMap.at("address-scripthash-version"));
        }

        int32_t addrChecksumValue() const override {
            return parseHexToInt32Le(_resultMap.at("address-checksum-value"));
        }
    private:
        const map<string, string> & _resultMap;

    };
    std::unique_ptr<WalletAddrHelper> _addrHelper;

    class PrivateKeyHelper : public IPrivateKeyHelper {
    public:
        PrivateKeyHelper(const map<string, string> &result) :
            _resultMap(result)  { }

        const std::vector<unsigned char> privkeyPrefix() const override {
            return ParseHex(_resultMap.at("private-key-version"));
        }

        int32_t addrChecksumValue() const override  {
            return parseHexToInt32Le(_resultMap.at("address-checksum-value"));
        }
    private:
        const map<string, string> &_resultMap;
    };
    std::unique_ptr<PrivateKeyHelper> _privHelper;
};


void createKeyPairs()
{
    using namespace std;

    ECC_Start();
    unique_ptr<ECCVerifyHandle> handle(new ECCVerifyHandle);

    if(!ECC_InitSanityCheck()) {
        cerr << "Elliptic curve cryptography sanity check failure. Aborting." << endl;
        //InitError("Elliptic curve cryptography sanity check failure. Aborting.");
        ECC_Stop();
        return;
    }

    CKey secret;
    //secret.MakeNewKey(fCompressed);
    secret.MakeNewKey(true);

    CPubKey pubkey = secret.GetPubKey();

    RpcClient client{"13.125.145.98", 4260, "hdacrpc", "1234", "kcc"};
    KeysHelper helper(client);

    CBitcoinAddress addr(pubkey.GetID(), helper.addrHelper());

    //signmessage with private key, and compare pubkeys each other

    cout << "address : " << addr.ToString() << endl;
    cout << "pubkey ID: " << HexStr(pubkey.GetID()) << endl;
    cout << "pubKey: " << HexStr(pubkey) << endl;
    cout << "privkey: "
         << CBitcoinSecret(secret, helper.privHelper()).ToString() << endl;

    ECC_Stop();
}


string resultWithRPC(const RpcClient& client, const string &method, const Array &params = Array())
{
    //RpcClient client{"13.125.145.98", 4260, "hdacrpc", "1234"};
    const Object reply = client.CallRPC(method, params);

    string resultStr;
    int nRet = result(reply, resultStr);
    if (!nRet)
        return resultStr;
    return "";
}

void testPubkeyToAddrAfterGettingParams()
{
    // from cli, getaddresses true
    CPubKey pubkey(ParseHex("027e75736b41474547b7e2443d7235f4030cbb378093bbd2e98ea36ded6d703c2b"));
    cout << "pubKey: " << HexStr(pubkey) << endl;

    RpcClient client{"13.125.145.98", 4260, "hdacrpc", "1234", "kcc"};
    KeysHelper helper(client);
    //cout << hex << checksum << endl;
    CBitcoinAddress addr(pubkey.GetID(), helper.addrHelper());

    cout << "address : " << addr.ToString() << endl;
}

bool rpcResult(const Object& reply, string &resultStr)
{
    int nRet = result(reply, resultStr);
    if (nRet) {
        cerr << "rpc error : " << resultStr;
        return false;
    }
    return true;
}

void testgetinfo()
{
    string resultStr;
    RpcClient client{"13.125.145.98", 4260, "hdacrpc", "1234", "kcc"};
    if (!rpcResult(getinfo(client), resultStr))  {
        return;
    }
    //cout <<
}

void testRawTransactionForStreamPublish()
{
    vector<tuple<string, string>> addrNPrivates = {
        {"1WCRNaPb3jAjb4GE9t34uLiLtPseA8JKEvdtg5", "V6X4NaaDQSTgXdAcCzUrSxWqAuFcd53TRXRqmSafUYEbY5DgGMitPEzk"},
        {"18wD7MBodeTYRAvN5bRuWYB11jwHdkGVCBLSnB", "VHXjccrTPdRXG8asyos5oqvw6mhWtqASkbFsVuBnkpi4WXn2jr8eMwwp"},
        {"1EpVCtEHe61hVdgQLKSzM8ZyeFJGdsey29sMQi", "V9ugoEazm16SKbvj7DVxMUcXQnvKpBPaeZ3KEUxTUWoChXQTuHKyzbKx"}
    };
    int selected = 0;
    string walletAddr;
    string privateKey;
    tie(walletAddr, privateKey) = addrNPrivates[selected];

    RpcClient client{"13.125.145.98", 4260, "hdacrpc", "1234", "kcc"};
    string resultStr;
    if (!rpcResult(listunspent(client, walletAddr), resultStr))  {
        return;
    }

    Value resultValue;
    string txid;
    int vout;
    string scriptPubKey;

    cout << "listunspent result: " << endl;
    if (read_string(resultStr, resultValue))   {
        if (resultValue.type() != array_type) {
            throw "wrong result format";
        }
        Array unspents = resultValue.get_array();
        Object selected;
        for (int i = 0; i < unspents.size(); i++) {
            Value value = find_value(unspents[i].get_obj(), "assets");
            if (value.type() == array_type && value.get_array().size() == 0) {
                selected = unspents[i].get_obj();
                break;
            }
        }

        txid = find_value(selected, "txid").get_str();
        vout = find_value(selected, "vout").get_int();
        scriptPubKey = find_value(selected, "scriptPubKey").get_str();
        cout << "txid = " << txid << endl;
        cout << "vout = " << vout << endl;
        cout << "scriptPubKey = " << scriptPubKey << endl;
    }

    if (!rpcResult(lockunspent(client, false, txid, vout), resultStr))  {
        return;
    }

    if (!rpcResult(listlockunspent(client), resultStr))  {
        return;
    }

    //Value resultValue;
    //string txid;
    //int vout;
    cout << "listlockunspent result: " << endl;
    if (read_string(resultStr, resultValue))   {
        if (resultValue.type() != array_type) {
            throw "wrong result format";
        }
        Array unspents = resultValue.get_array();
        txid = find_value(unspents[0].get_obj(), "txid").get_str();
        vout = find_value(unspents[0].get_obj(), "vout").get_int();
        cout << "txid = " << txid << endl;
        cout << "vout = " << vout << endl;
    }

    if (!rpcResult(lockunspent(client, true, txid, vout), resultStr))  {
        return;
    }

    if (!rpcResult(liststreams(client, "stream9"), resultStr))  {
        return;
    }

    string createTxid;
    if (read_string(resultStr, resultValue))   {
        if (resultValue.type() != array_type) {
            throw "wrong result format";
        }
        Array unspents = resultValue.get_array();
        createTxid = find_value(unspents[0].get_obj(), "createtxid").get_str();
        cout << "createtxid = " << createTxid << endl;
    }

    string txHex = createStreamPublishTx("key1", "first programmed version", createTxid,
                                         scriptPubKey, txid, vout, "", privateKey, KeysHelper(client).privHelper());
    cout << "raw transaction: " << txHex << endl;
}

void testRawTransactionForAssetSend()
{
    vector<tuple<string, string>> addrNPrivates = {
        {"1WCRNaPb3jAjb4GE9t34uLiLtPseA8JKEvdtg5", "V6X4NaaDQSTgXdAcCzUrSxWqAuFcd53TRXRqmSafUYEbY5DgGMitPEzk"},
        {"18wD7MBodeTYRAvN5bRuWYB11jwHdkGVCBLSnB", "VHXjccrTPdRXG8asyos5oqvw6mhWtqASkbFsVuBnkpi4WXn2jr8eMwwp"},
        {"1EpVCtEHe61hVdgQLKSzM8ZyeFJGdsey29sMQi", "V9ugoEazm16SKbvj7DVxMUcXQnvKpBPaeZ3KEUxTUWoChXQTuHKyzbKx"}
    };

    int selected = 1;
    string walletAddr;
    string privateKey;
    tie(walletAddr, privateKey) = addrNPrivates[selected];

    RpcClient client{"13.125.145.98", 4260, "hdacrpc", "1234", "kcc"};
    string resultStr;
    if (!rpcResult(listunspent(client, walletAddr), resultStr))  {
        return;
    }

    Value resultValue;
    string txid;
    int vout;
    string scriptPubKey;
    double unspentQty;

    string assetName = "ass1";
    cout << "listunspent result: " << endl;
    if (read_string(resultStr, resultValue))   {
        if (resultValue.type() != array_type) {
            throw "wrong result format";
        }

        Array unspents = resultValue.get_array();
        Object selected;
        for (int i = 0; i < unspents.size(); i++) {
            Value value = find_value(unspents[i].get_obj(), "assets");
            if (value.type() == array_type && value.get_array().size() != 0) {
                bool found = false;
                Array assetInfo = value.get_array();
                for (int i = 0; i < assetInfo.size(); i++) {
                //for (const Object& assetInfo : value.get_array())   {
                    if (assetInfo[i].type() == obj_type)    {
                        Value name = find_value(assetInfo[i].get_obj(), "name");
                        if (name.get_str() == assetName)   {
                            found = true;
                            Value qty = find_value(assetInfo[i].get_obj(), "qty");
                            unspentQty = qty.get_real();
                            break;
                        }
                    }
                }
                if (found == true)  {
                    selected = unspents[i].get_obj();
                    break;
                }
            }
        }

        txid = find_value(selected, "txid").get_str();
        vout = find_value(selected, "vout").get_int();
        scriptPubKey = find_value(selected, "scriptPubKey").get_str();
        cout << "txid = " << txid << endl;
        cout << "vout = " << vout << endl;
        cout << "scriptPubKey = " << scriptPubKey << endl;
    }

    if (!rpcResult(lockunspent(client, false, txid, vout), resultStr))  {
        return;
    }

    if (!rpcResult(listlockunspent(client), resultStr))  {
        return;
    }

    //Value resultValue;
    //string txid;
    //int vout;
    cout << "listlockunspent result: " << endl;
    if (read_string(resultStr, resultValue))   {
        if (resultValue.type() != array_type) {
            throw "wrong result format";
        }
        Array unspents = resultValue.get_array();
        txid = find_value(unspents[0].get_obj(), "txid").get_str();
        vout = find_value(unspents[0].get_obj(), "vout").get_int();
        cout << "txid = " << txid << endl;
        cout << "vout = " << vout << endl;
    }

    if (!rpcResult(lockunspent(client, true, txid, vout), resultStr))  {
        return;
    }

    if (!rpcResult(listassets(client, assetName), resultStr))  {
        return;
    }

    string issueTxid;
    int multiple;
    if (read_string(resultStr, resultValue))   {
        if (resultValue.type() != array_type) {
            throw "wrong result format";
        }
        Array assets = resultValue.get_array();
        issueTxid = find_value(assets[0].get_obj(), "issuetxid").get_str();
        cout << "issuetxid = " << issueTxid << endl;
        multiple = find_value(assets[0].get_obj(), "multiple").get_int();
        cout << "multiple = " << multiple << endl;
    }

    string txHex = createAssetSendTx("1WCRNaPb3jAjb4GE9t34uLiLtPseA8JKEvdtg5",
                10, issueTxid, multiple,
                scriptPubKey, txid, vout,
                unspentQty, "", privateKey,
                KeysHelper(client).privHelper(),
                KeysHelper(client).addrHelper());
    cout << "raw transaction: " << txHex << endl;
}

int main()
{
    cout << "1. test sha256 of \"This is test message\"" << endl;
    testCalcSHA256();

    cout << "2. test sha256 of file, video1.mp4" << endl;
    hashFromFile("video1.mp4");

    //cout << "3. temp test" << endl;
    //getinfo();

    cout << "4. create key pairs" << endl;
    createKeyPairs();

    cout << "5. pubkey test" << endl;
    testPubkeyToAddrAfterGettingParams();

    cout << "6. stream publish test" << endl;
    try {
        testRawTransactionForStreamPublish();
    } catch(std::exception &e) {
        cout << e.what() << endl;
    }

    cout << "7. asset send test" << endl;
    try {
        testRawTransactionForAssetSend();
    } catch(std::exception &e) {
        cout << e.what() << endl;
    }

    return 0;
}
