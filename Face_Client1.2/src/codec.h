#ifndef CODEC_H_INCLUDED
#define CODEC_H_INCLUDED

#include "data.pb.h"
#include<string>

void encode_data(std::string id,std::string code,std::string& data_str);

data::Data decode_data(std::string recv_str);

#endif // CODEC_H_INCLUDED
