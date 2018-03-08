#ifndef FACERECOGNIZE_H_INCLUDED
#define FACERECOGNIZE_H_INCLUDED

#include "wcdCFacerecognize.h"
#include "codec.h"

#include <muduo/base/Types.h>
#include <muduo/base/StringPiece.h>

using namespace cv;

void Face_Server_Init();

std::vector<faceData> solveFacerec(const muduo::StringPiece& s);

std::string process_res(std::vector<faceData> result);

void Face_Server_Close();

#endif // FACERECOGNIZE_H_INCLUDED
