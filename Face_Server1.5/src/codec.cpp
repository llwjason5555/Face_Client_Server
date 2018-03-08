#include "codec.h"

using namespace std;

void encode_data(std::string id,std::string code,std::string& data_str)
{
    data::Data d;
    d.set_id(id);
    d.set_code(code);
    d.SerializeToString(&data_str);
}

data::Data decode_data(std::string recv_str)
{
    data::Data d;
    d.ParseFromString(recv_str);
    return d;
}
