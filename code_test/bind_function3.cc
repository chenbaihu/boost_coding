#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <tr1/functional>
#include <tr1/memory>

class DataMgn {
public:
    enum KeyType { 
        kDid, 
        kOid,
    };
public:
    typedef std::tr1::function<void(size_t id, std::string value)> OnGetKey;
    typedef std::tr1::function<void(std::vector<size_t> ids, std::vector<std::string> value)> OnMGetKey;
    typedef std::tr1::function<void(std::vector<std::string> value)> OnMGetKey2;

public:
    DataMgn() {
        datas_.clear();
    }

public:
    void Init();

    void Get(KeyType type, size_t id, const OnGetKey& onGetKey);
    
    void MGet(KeyType type, std::vector<size_t> ids, const OnMGetKey& onGetKey);
    void MGet2(KeyType type, std::vector<size_t> ids, const OnMGetKey2& onGetKey);
private:   
    typedef std::map<size_t/*id*/, std::string> ISMap;
    typedef std::map<KeyType, ISMap> TISMap;
    TISMap datas_;
};

void DataMgn::Init() { 
    //TODO初始化，redis，memcache，codis，ckv等存储信息
    ISMap& did_values_ = datas_[kDid];
    did_values_.insert(std::make_pair(1, "dids 11111"));
    did_values_.insert(std::make_pair(2, "dids k1=v1&k2=v2&k3=v3"));
    did_values_.insert(std::make_pair(3, "dids {k1:v1,k2,v2,k3:v3}"));

    ISMap& oid_values_ = datas_[kOid];
    oid_values_.insert(std::make_pair(1, "oids 11111"));
    oid_values_.insert(std::make_pair(2, "oids k1=v1&k2=v2&k3=v3"));
    oid_values_.insert(std::make_pair(3, "oids {k1:v1,k2,v2,k3:v3}"));
}

void DataMgn::Get(KeyType type, size_t id, const OnGetKey& onGetKey)
{
    ISMap& d = datas_[type]; 
    
    //TODO 从 redis，memcache，codis，ckv等获取信息
    ISMap::const_iterator it = d.find(id);
    if (it == d.end()) {
        onGetKey(id, std::string("NULL")); // not find
    } 
    onGetKey(id, it->second);
}

void DataMgn::MGet(KeyType type, std::vector<size_t> ids, const OnMGetKey& onMGetKey)
{
    ISMap& d = datas_[type]; 
    
    std::vector<std::string> values;
    values.resize(ids.size());

    //TODO 从 redis，memcache，codis，ckv等获取信息
    for (size_t i=0; i<ids.size(); i++) {
        ISMap::const_iterator it = d.find(ids[i]);
        if (it == d.end()) {
            values[i] = std::string("NULL"); // not find
        } 
        values[i] = it->second;
    }
    onMGetKey(ids, values);
}

void DataMgn::MGet2(KeyType type, std::vector<size_t> ids, const OnMGetKey2& onMGetKey)
{
    ISMap& d = datas_[type]; 
    
    std::vector<std::string> values;
    values.resize(ids.size());

    //TODO 从 redis，memcache，codis，ckv等获取信息
    for (size_t i=0; i<ids.size(); i++) {
        ISMap::const_iterator it = d.find(ids[i]);
        if (it == d.end()) {
            values[i] = std::string("NULL"); // not find
        } 
        values[i] = it->second;
    }
    onMGetKey(values);
}

typedef struct OrderFeatrue { 
    int oid;
    //....
} OrderFeatrue;

typedef struct DriverFeatrue { 
    int did;
    //....
} DriverFeatrue;

class Featrue {
public: 
    Featrue(DataMgn& dc):dc_(dc) {};
    ~Featrue() {};

public:
    bool Exec(std::vector<size_t>dids, std::vector<size_t> oids);

public:
    void ParseDriversFeature(std::vector<size_t> dids, std::vector<std::string> values);
    void ParseDriverFeature(std::string key, std::string value);

    void ParseOrdersFeature(std::vector<size_t> oids, std::vector<std::string> values);
    void ParseOrderFeature(std::string key, std::string value);

private:
    DataMgn& dc_; 
    std::vector<OrderFeatrue>  order_feature_;
    std::vector<DriverFeatrue> driver_feature_;
}; 

bool Featrue::Exec(std::vector<size_t> dids, std::vector<size_t> oids) { 
    dc_.MGet(DataMgn::kDid, dids, std::tr1::bind(&Featrue::ParseDriversFeature, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    dc_.MGet(DataMgn::kOid, oids, std::tr1::bind(&Featrue::ParseOrdersFeature, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    dc_.MGet2(DataMgn::kOid, oids, std::tr1::bind(&Featrue::ParseOrdersFeature, this, oids, std::tr1::placeholders::_1));
}

void Featrue::ParseDriversFeature(std::vector<size_t> dids, std::vector<std::string> values) 
{
    for (size_t i=0; i<dids.size(); i++) {
        std::cout << "ParseDriversFeature\t" << dids[i] << "\t" << values[i] << std::endl;
    }
    //TODO....
}

void Featrue::ParseDriverFeature(std::string key, std::string value) 
{ 
    //TODO...
}

void Featrue::ParseOrdersFeature(std::vector<size_t> oids, std::vector<std::string> values) 
{
    for (size_t i=0; i<oids.size(); i++) {
        std::cout << "ParseOrdersFeature\t" << oids[i] << "\t" << values[i] << std::endl;
    }
    //TODO....
}

void Featrue::ParseOrderFeature(std::string key, std::string value) 
{
    //TODO....
}

int main(int argc,char* argv[])
{
    DataMgn dc;
    dc.Init();

    Featrue fe(dc);

    std::vector<size_t> dids;
    dids.push_back(1);
    dids.push_back(2);
    dids.push_back(3);

    std::vector<size_t> oids;
    oids.push_back(1);
    oids.push_back(2);
    oids.push_back(3);

    fe.Exec(dids, oids);
    return 0;
}


